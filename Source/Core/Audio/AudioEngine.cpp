#include "AudioEngine.h"
#include "Core/Parameters/ParameterManager.h"
#include <algorithm>

namespace zalaxy
{

AudioEngine::AudioEngine (ParameterManager& params)
    : parameters (params)
{
    for (auto& sample : visualizationBuffer)
        sample.store (0.0f, std::memory_order_relaxed);

    for (int i = 0; i < MAX_VOICES; ++i)
    {
        voices[i].ampEnvelope.prepare (44100.0);
        voices[i].filterEnvelope.prepare (44100.0);
        voices[i].modEnvelope.prepare (44100.0);
        voices[i].lfo1.prepare (44100.0);
        voices[i].lfo2.prepare (44100.0);
        voices[i].filter1.prepare (44100.0);
        voices[i].filter2.prepare (44100.0);
        for (int o = 0; o < 4; ++o)
        {
            voices[i].osc[o].prepare (44100.0);
        }
    }

    masterVolume.reset (44100.0, 0.05);
    masterVolume.setCurrentAndTargetValue (1.0f);
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::prepare (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;

    for (int i = 0; i < MAX_VOICES; ++i)
    {
        voices[i].ampEnvelope.prepare (sampleRate);
        voices[i].filterEnvelope.prepare (sampleRate);
        voices[i].modEnvelope.prepare (sampleRate);
        voices[i].lfo1.prepare (sampleRate);
        voices[i].lfo2.prepare (sampleRate);
        voices[i].filter1.prepare (sampleRate);
        voices[i].filter2.prepare (sampleRate);
        for (int o = 0; o < 4; ++o)
        {
            voices[i].osc[o].prepare (sampleRate);
        }
    }

    effects.prepare (sampleRate, samplesPerBlock);

    masterVolume.reset (sampleRate, 0.05);
}

void AudioEngine::release()
{
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        if (voices[i].active)
        {
            releaseVoice (&voices[i]);
        }
    }
    effects.reset();
}

void AudioEngine::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        const auto& msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            handleNoteOn (nullptr, msg.getChannel(), msg.getNoteNumber(), msg.getVelocity() / 127.0f);
        }
        else if (msg.isNoteOff())
        {
            handleNoteOff (nullptr, msg.getChannel(), msg.getNoteNumber(), msg.getVelocity() / 127.0f);
        }
    }

    for (int i = 0; i < MAX_VOICES; ++i)
    {
        if (voices[i].active)
        {
            processVoice (voices[i], buffer, 0, numSamples);
        }
    }

    effects.processBlock (buffer);

    float vol = masterVolume.getNextValue();
    buffer.applyGain (vol);

    // Publish a mono snapshot for the UI visualizer without locking the audio thread.
    const auto writeStart = visualizationWritePosition.load (std::memory_order_relaxed);
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;

        for (int channel = 0; channel < numChannels; ++channel)
            monoSample += buffer.getSample (channel, sample);

        if (numChannels > 0)
            monoSample /= static_cast<float> (numChannels);

        const auto index = (writeStart + static_cast<std::uint64_t> (sample)) % visualizationBufferSize;
        visualizationBuffer[static_cast<std::size_t> (index)].store (monoSample, std::memory_order_relaxed);
    }

    visualizationWritePosition.store (writeStart + static_cast<std::uint64_t> (numSamples),
                                       std::memory_order_release);
}

void AudioEngine::copyWaveformData (float* destination, int numSamples) const
{
    if (destination == nullptr || numSamples <= 0)
        return;

    const auto writeEnd = visualizationWritePosition.load (std::memory_order_acquire);
    const auto available = std::min<std::uint64_t> (writeEnd, visualizationBufferSize);

    if (available == 0)
    {
        std::fill_n (destination, numSamples, 0.0f);
        return;
    }

    const auto firstSample = writeEnd - available;

    for (int i = 0; i < numSamples; ++i)
    {
        const auto sourceOffset = (numSamples == 1)
            ? available - 1
            : (static_cast<std::uint64_t> (i) * (available - 1)) / static_cast<std::uint64_t> (numSamples - 1);

        const auto sourceIndex = (firstSample + sourceOffset) % visualizationBufferSize;
        destination[i] = visualizationBuffer[static_cast<std::size_t> (sourceIndex)].load (std::memory_order_relaxed);
    }
}

void AudioEngine::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (velocity <= 0.0f)
    {
        handleNoteOff (nullptr, midiChannel, midiNoteNumber, 0.0f);
        return;
    }

    Voice* voice = allocateVoice (midiNoteNumber, velocity);
    if (voice)
    {
        voice->note = midiNoteNumber;
        voice->velocity = velocity;
        voice->active = true;
        activeVoices++;

        voice->ampEnvelope.noteOn (velocity);
        voice->filterEnvelope.noteOn (velocity);
        voice->modEnvelope.noteOn (velocity);

        voice->lfo1.noteOn();
        voice->lfo2.noteOn();

        float freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        for (int o = 0; o < 4; ++o)
        {
            voice->osc[o].setFrequency (freq);
        }
    }
}

void AudioEngine::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    Voice* voice = findVoice (midiNoteNumber);
    if (voice)
    {
        voice->ampEnvelope.noteOff();
        voice->filterEnvelope.noteOff();
        voice->modEnvelope.noteOff();
    }
}

void AudioEngine::onParameterChanged (const juce::String& paramID, float newValue)
{
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        if (voices[i].active)
        {
            updateVoiceParameters (voices[i]);
        }
    }

    if (paramID.startsWith ("reverb"))
    {
        if (paramID == "reverbEnabled") effects.setReverbEnabled (newValue > 0.5f);
        else if (paramID == "reverbRoomSize") effects.setReverbRoomSize (newValue);
        else if (paramID == "reverbDamping") effects.setReverbDamping (newValue);
        else if (paramID == "reverbWetLevel") effects.setReverbWetLevel (newValue);
        else if (paramID == "reverbDryLevel") effects.setReverbDryLevel (newValue);
        else if (paramID == "reverbWidth") effects.setReverbWidth (newValue);
    }
    else if (paramID.startsWith ("delay"))
    {
        if (paramID == "delayEnabled") effects.setDelayEnabled (newValue > 0.5f);
        else if (paramID == "delayTime") effects.setDelayTime (newValue);
        else if (paramID == "delayFeedback") effects.setDelayFeedback (newValue);
        else if (paramID == "delayWetLevel") effects.setDelayWetLevel (newValue);
    }
    else if (paramID.startsWith ("distortion"))
    {
        if (paramID == "distortionEnabled") effects.setDistortionEnabled (newValue > 0.5f);
        else if (paramID == "distortionType") effects.setDistortionType (static_cast<int> (newValue));
        else if (paramID == "distortionDrive") effects.setDistortionDrive (newValue);
        else if (paramID == "distortionMix") effects.setDistortionMix (newValue);
    }
    else if (paramID.startsWith ("chorus"))
    {
        if (paramID == "chorusEnabled") effects.setChorusEnabled (newValue > 0.5f);
        else if (paramID == "chorusRate") effects.setChorusRate (newValue);
        else if (paramID == "chorusDepth") effects.setChorusDepth (newValue);
        else if (paramID == "chorusFeedback") effects.setChorusFeedback (newValue);
        else if (paramID == "chorusMix") effects.setChorusMix (newValue);
    }
    else if (paramID == "masterVolume")
    {
        masterVolume.setTargetValue (newValue);
    }
    else if (paramID == "masterPan")
    {
        effects.setMasterPan (newValue);
    }
}

AudioEngine::Voice* AudioEngine::allocateVoice (int note, float velocity)
{
    Voice* oldestReleaseVoice = nullptr;

    for (int i = 0; i < MAX_VOICES; ++i)
    {
        if (!voices[i].active)
        {
            return &voices[i];
        }

        if (voices[i].ampEnvelope.getCurrentStage() == EnvelopeStage::Release)
        {
            oldestReleaseVoice = &voices[i];
        }
    }

    if (oldestReleaseVoice)
    {
        releaseVoice (oldestReleaseVoice);
        return oldestReleaseVoice;
    }

    Voice* voice = &voices[nextVoiceIndex];
    nextVoiceIndex = (nextVoiceIndex + 1) % MAX_VOICES;
    releaseVoice (voice);
    return voice;
}

void AudioEngine::releaseVoice (Voice* voice)
{
    if (voice->active)
    {
        voice->active = false;
        voice->note = -1;
        voice->velocity = 0.0f;
        activeVoices = juce::jmax (0, activeVoices - 1);
    }
}

AudioEngine::Voice* AudioEngine::findVoice (int note)
{
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        if (voices[i].active && voices[i].note == note)
        {
            return &voices[i];
        }
    }
    return nullptr;
}

void AudioEngine::processVoice (Voice& voice, juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    const int numChannels = buffer.getNumChannels();

    float envBuffer[512];
    voice.ampEnvelope.processBlock (envBuffer, numSamples);

    float filterEnvBuffer[512];
    voice.filterEnvelope.processBlock (filterEnvBuffer, numSamples);

    float modEnvBuffer[512];
    voice.modEnvelope.processBlock (modEnvBuffer, numSamples);

    float lfo1Buffer[512];
    float lfo2Buffer[512];
    voice.lfo1.processBlock (lfo1Buffer, numSamples);
    voice.lfo2.processBlock (lfo2Buffer, numSamples);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer (ch, startSample);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = 0.0f;

            for (int o = 0; o < 4; ++o)
            {
                float pitchMod = lfo1Buffer[i] * 0.02f;
                voice.osc[o].setFrequency (juce::MidiMessage::getMidiNoteInHertz (voice.note) * std::pow (2.0f, pitchMod / 12.0f));

                sample += voice.osc[o].getNextSample();
            }

            float filterCutoff = 1000.0f + filterEnvBuffer[i] * 5000.0f;
            voice.filter1.setCutoff (filterCutoff);
            voice.filter2.setCutoff (filterCutoff);

            sample = voice.filter1.processSample (sample);
            sample = voice.filter2.processSample (sample);

            sample *= envBuffer[i];

            channelData[i] += sample;
        }
    }

    if (voice.ampEnvelope.getCurrentStage() == EnvelopeStage::Idle)
    {
        releaseVoice (&voice);
    }
}

void AudioEngine::updateVoiceParameters (Voice& voice)
{
    for (int o = 0; o < 4; ++o)
    {
        juce::String paramID = "osc" + juce::String (o + 1) + "Waveform";
        if (auto* param = parameters.getParameter (paramID))
        {
            voice.osc[o].setWaveform (static_cast<Waveform> (static_cast<int> (param->getValue())));
        }

        paramID = "osc" + juce::String (o + 1) + "Level";
        if (auto* param = parameters.getParameter (paramID))
        {
            voice.osc[o].setLevel (param->getValue());
        }

        paramID = "osc" + juce::String (o + 1) + "PulseWidth";
        if (auto* param = parameters.getParameter (paramID))
        {
            voice.osc[o].setPulseWidth (param->getValue());
        }
    }

    if (auto* param = parameters.getParameter ("filter1Type"))
        voice.filter1.setType (static_cast<FilterType> (static_cast<int> (param->getValue())));
    if (auto* param = parameters.getParameter ("filter1Cutoff"))
        voice.filter1.setCutoff (param->getValue());
    if (auto* param = parameters.getParameter ("filter1Resonance"))
        voice.filter1.setResonance (param->getValue());

    if (auto* param = parameters.getParameter ("filter2Type"))
        voice.filter2.setType (static_cast<FilterType> (static_cast<int> (param->getValue())));
    if (auto* param = parameters.getParameter ("filter2Cutoff"))
        voice.filter2.setCutoff (param->getValue());
    if (auto* param = parameters.getParameter ("filter2Resonance"))
        voice.filter2.setResonance (param->getValue());

    if (auto* param = parameters.getParameter ("ampAttack"))
        voice.ampEnvelope.setAttack (param->getValue());
    if (auto* param = parameters.getParameter ("ampDecay"))
        voice.ampEnvelope.setDecay (param->getValue());
    if (auto* param = parameters.getParameter ("ampSustain"))
        voice.ampEnvelope.setSustain (param->getValue());
    if (auto* param = parameters.getParameter ("ampRelease"))
        voice.ampEnvelope.setRelease (param->getValue());

    if (auto* param = parameters.getParameter ("filterAttack"))
        voice.filterEnvelope.setAttack (param->getValue());
    if (auto* param = parameters.getParameter ("filterDecay"))
        voice.filterEnvelope.setDecay (param->getValue());
    if (auto* param = parameters.getParameter ("filterSustain"))
        voice.filterEnvelope.setSustain (param->getValue());
    if (auto* param = parameters.getParameter ("filterRelease"))
        voice.filterEnvelope.setRelease (param->getValue());

    if (auto* param = parameters.getParameter ("lfo1Rate"))
        voice.lfo1.setRate (param->getValue());
    if (auto* param = parameters.getParameter ("lfo1Depth"))
        voice.lfo1.setDepth (param->getValue());
    if (auto* param = parameters.getParameter ("lfo1Waveform"))
        voice.lfo1.setWaveform (static_cast<LFOWaveform> (static_cast<int> (param->getValue())));

    if (auto* param = parameters.getParameter ("lfo2Rate"))
        voice.lfo2.setRate (param->getValue());
    if (auto* param = parameters.getParameter ("lfo2Depth"))
        voice.lfo2.setDepth (param->getValue());
    if (auto* param = parameters.getParameter ("lfo2Waveform"))
        voice.lfo2.setWaveform (static_cast<LFOWaveform> (static_cast<int> (param->getValue())));
}

} // namespace zalaxy