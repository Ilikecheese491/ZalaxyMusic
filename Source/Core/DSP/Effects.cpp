#include "Effects.h"
#include <cmath>

namespace zalaxy
{

Effects::Effects() = default;
Effects::~Effects() = default;

void Effects::prepare (double sr, int maxBlockSize)
{
    sampleRate = sr;

    // Prepare reverb
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.3f;
    reverbParams.dryLevel = 1.0f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters (reverbParams);

    // Prepare delay buffer (max 2 seconds)
    delayBuffer.setSize (2, static_cast<int> (sampleRate * 2.0));
    delayBuffer.clear();

    // Prepare chorus
    juce::dsp::ProcessSpec spec {sampleRate, static_cast<juce::uint32> (maxBlockSize), 2};
    chorus.prepare (spec);
    chorus.setRate (1.5f);
    chorus.setDepth (0.3f);
    chorus.setCentreDelay (7.0f);
    chorus.setFeedback (0.1f);
    chorus.setMix (0.5f);
}

void Effects::reset()
{
    reverb.reset();
    delayBuffer.clear();
    delayWritePos = 0;
    chorus.reset();
}

void Effects::setReverbEnabled (bool enabled)
{
    reverbEnabled = enabled;
}

void Effects::setReverbRoomSize (float size)
{
    reverbParams.roomSize = juce::jlimit (0.0f, 1.0f, size);
    reverb.setParameters (reverbParams);
}

void Effects::setReverbDamping (float damping)
{
    reverbParams.damping = juce::jlimit (0.0f, 1.0f, damping);
    reverb.setParameters (reverbParams);
}

void Effects::setReverbWetLevel (float level)
{
    reverbParams.wetLevel = juce::jlimit (0.0f, 1.0f, level);
    reverb.setParameters (reverbParams);
}

void Effects::setReverbDryLevel (float level)
{
    reverbParams.dryLevel = juce::jlimit (0.0f, 1.0f, level);
    reverb.setParameters (reverbParams);
}

void Effects::setReverbWidth (float width)
{
    reverbParams.width = juce::jlimit (0.0f, 1.0f, width);
    reverb.setParameters (reverbParams);
}

void Effects::setReverbFreezeMode (bool freeze)
{
    reverbParams.freezeMode = freeze ? 1.0f : 0.0f;
    reverb.setParameters (reverbParams);
}

void Effects::setDelayEnabled (bool enabled)
{
    delayEnabled = enabled;
}

void Effects::setDelayTime (float timeMs)
{
    delayLength = static_cast<int> (timeMs * 0.001f * sampleRate);
    delayLength = juce::jlimit (1, delayBuffer.getNumSamples() - 1, delayLength);
}

void Effects::setDelayFeedback (float feedback)
{
    delayFeedback = juce::jlimit (0.0f, 0.95f, feedback);
}

void Effects::setDelayWetLevel (float level)
{
    delayWetLevel = juce::jlimit (0.0f, 1.0f, level);
}

void Effects::setDelayTempoSync (bool sync, float bpm)
{
    delayTempoSync = sync;
    delayBpm = juce::jlimit (20.0f, 300.0f, bpm);
}

void Effects::setDistortionEnabled (bool enabled)
{
    distortionEnabled = enabled;
}

void Effects::setDistortionType (int type)
{
    distortionType = juce::jlimit (0, 3, type);
}

void Effects::setDistortionDrive (float drive)
{
    distortionDrive = juce::jlimit (1.0f, 100.0f, drive);
}

void Effects::setDistortionMix (float mix)
{
    distortionMix = juce::jlimit (0.0f, 1.0f, mix);
}

void Effects::setChorusEnabled (bool enabled)
{
    chorusEnabled = enabled;
}

void Effects::setChorusRate (float rate)
{
    chorus.setRate (juce::jlimit (0.1f, 10.0f, rate));
}

void Effects::setChorusDepth (float depth)
{
    chorus.setDepth (juce::jlimit (0.0f, 1.0f, depth));
}

void Effects::setChorusFeedback (float feedback)
{
    chorus.setFeedback (juce::jlimit (0.0f, 0.9f, feedback));
}

void Effects::setChorusMix (float mix)
{
    chorus.setMix (juce::jlimit (0.0f, 1.0f, mix));
}

void Effects::setMasterVolume (float vol)
{
    masterVolume = juce::jlimit (0.0f, 2.0f, vol);
}

void Effects::setMasterPan (float pan)
{
    masterPan = juce::jlimit (-1.0f, 1.0f, pan);
}

void Effects::processBlock (juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Apply distortion first (pre-effects)
    if (distortionEnabled && distortionMix > 0.0f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float dry = channelData[i];
                float wet = 0.0f;

                switch (distortionType)
                {
                    case 0: wet = applySoftClip (dry, distortionDrive); break;
                    case 1: wet = applyHardClip (dry, distortionDrive); break;
                    case 2: wet = applyWavefold (dry, distortionDrive); break;
                    case 3: wet = applyBitcrush (dry, distortionDrive); break;
                }

                channelData[i] = dry + (wet - dry) * distortionMix;
            }
        }
    }

    // Apply chorus
    if (chorusEnabled)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        chorus.process (context);
    }

    // Apply delay
    if (delayEnabled && delayWetLevel > 0.0f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float dry = channelData[i];
                float wet = delayBuffer.getSample (ch, delayWritePos);

                // Write input + feedback to delay buffer
                delayBuffer.setSample (ch, delayWritePos, dry + wet * delayFeedback);

                channelData[i] = dry + wet * delayWetLevel;
            }
        }

        delayWritePos++;
        if (delayWritePos >= delayBuffer.getNumSamples())
            delayWritePos = 0;
    }

    // Apply reverb (stereo only)
    if (reverbEnabled && numChannels >= 2)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        reverb.processStereo (block.getChannelPointer (0), block.getChannelPointer (1), numSamples);
    }

    // Apply master volume and pan
    if (masterVolume != 1.0f || masterPan != 0.0f)
    {
        float leftGain = masterVolume;
        float rightGain = masterVolume;

        if (masterPan < 0.0f)
            rightGain *= (1.0f + masterPan);
        else
            leftGain *= (1.0f - masterPan);

        if (numChannels >= 2)
        {
            buffer.applyGain (0, 0, numSamples, leftGain);
            buffer.applyGain (1, 0, numSamples, rightGain);
        }
        else
        {
            buffer.applyGain (0, 0, numSamples, masterVolume);
        }
    }
}

float Effects::applySoftClip (float x, float drive) const
{
    x *= drive;
    return std::tanh (x) / drive;
}

float Effects::applyHardClip (float x, float drive) const
{
    x *= drive;
    return juce::jlimit (-1.0f, 1.0f, x) / drive;
}

float Effects::applyWavefold (float x, float drive) const
{
    x *= drive;
    // Simple wavefolding: fold at +/-1
    while (x > 1.0f) x = 2.0f - x;
    while (x < -1.0f) x = -2.0f - x;
    return x / drive;
}

float Effects::applyBitcrush (float x, float drive) const
{
    // Bitcrush by quantizing
    float levels = 2.0f + drive * 62.0f; // 2 to 64 levels
    return std::round (x * levels) / levels;
}

} // namespace zalaxy