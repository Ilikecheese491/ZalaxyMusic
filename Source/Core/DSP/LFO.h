#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

enum class LFOWaveform
{
    Sine,
    Triangle,
    Sawtooth,
    Square,
    SampleAndHold,
    Random
};

class LFO
{
public:
    LFO();
    ~LFO();

    void prepare (double sampleRate);
    void reset();

    void setWaveform (LFOWaveform waveform);
    void setRate (float rateHz);
    void setDepth (float depth); // 0.0 to 1.0
    void setDelay (float seconds);
    void setFadeIn (float seconds);
    void setKeySync (bool sync) { keySync = sync; }
    void setTempoSync (bool sync, float bpm = 120.0f);

    void noteOn();
    void noteOff();

    float getNextSample();
    void processBlock (float* buffer, int numSamples);

    // For visualization
    void getShapePoints (juce::Array<juce::Point<float>>& points, float width, float height) const;

private:
    LFOWaveform currentWaveform = LFOWaveform::Sine;
    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;

    float rateHz = 1.0f;
    float depth = 1.0f;
    float delayTime = 0.0f;
    float fadeInTime = 0.0f;

    bool keySync = true;
    bool tempoSync = false;
    float bpm = 120.0f;

    float delaySamplesRemaining = 0;
    float fadeInSamplesRemaining = 0;
    float currentDepth = 0.0f;

    float sampleAndHoldValue = 0.0f;
    double sampleAndHoldPhase = 0.0;

    void updatePhaseIncrement();
    double timeToSamples (float seconds) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFO)
};

} // namespace zalaxy