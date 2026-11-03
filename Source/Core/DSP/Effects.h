#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class Effects
{
public:
    Effects();
    ~Effects();

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    // Reverb
    void setReverbEnabled (bool enabled);
    void setReverbRoomSize (float size); // 0.0 to 1.0
    void setReverbDamping (float damping); // 0.0 to 1.0
    void setReverbWetLevel (float level); // 0.0 to 1.0
    void setReverbDryLevel (float level); // 0.0 to 1.0
    void setReverbWidth (float width); // 0.0 to 1.0
    void setReverbFreezeMode (bool freeze);

    // Delay
    void setDelayEnabled (bool enabled);
    void setDelayTime (float timeMs);
    void setDelayFeedback (float feedback); // 0.0 to 1.0
    void setDelayWetLevel (float level);
    void setDelayTempoSync (bool sync, float bpm = 120.0f);

    // Distortion
    void setDistortionEnabled (bool enabled);
    void setDistortionType (int type); // 0=soft clip, 1=hard clip, 2=wavefold, 3=bitcrush
    void setDistortionDrive (float drive); // 1.0 to 100.0
    void setDistortionMix (float mix); // 0.0 to 1.0

    // Chorus
    void setChorusEnabled (bool enabled);
    void setChorusRate (float rateHz);
    void setChorusDepth (float depth);
    void setChorusFeedback (float feedback);
    void setChorusMix (float mix);

    // Master
    void setMasterVolume (float volume);
    void setMasterPan (float pan); // -1.0 to 1.0

    void processBlock (juce::AudioBuffer<float>& buffer);

private:
    double sampleRate = 44100.0;

    // Reverb (using JUCE's Reverb)
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    bool reverbEnabled = false;

    // Delay
    juce::AudioBuffer<float> delayBuffer;
    int delayWritePos = 0;
    int delayLength = 0;
    float delayFeedback = 0.0f;
    float delayWetLevel = 0.0f;
    bool delayEnabled = false;
    bool delayTempoSync = false;
    float delayBpm = 120.0f;

    // Distortion
    bool distortionEnabled = false;
    int distortionType = 0;
    float distortionDrive = 1.0f;
    float distortionMix = 0.0f;

    // Chorus
    juce::dsp::Chorus<float> chorus;
    bool chorusEnabled = false;

    // Master
    float masterVolume = 1.0f;
    float masterPan = 0.0f;

    // Distortion functions
    float applySoftClip (float x, float drive) const;
    float applyHardClip (float x, float drive) const;
    float applyWavefold (float x, float drive) const;
    float applyBitcrush (float x, float drive) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Effects)
};

} // namespace zalaxy