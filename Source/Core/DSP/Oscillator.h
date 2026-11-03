#pragma once

#include "JuceModules.h"

namespace zalaxy
{

enum class Waveform
{
    Sine,
    Triangle,
    Sawtooth,
    Square,
    Pulse,
    Wavetable,
    Noise
};

class Oscillator
{
public:
    Oscillator();
    ~Oscillator();

    void prepare (double sampleRate);
    void reset();

    void setWaveform (Waveform waveform);
    void setFrequency (float frequencyHz);
    void setLevel (float level);
    void setPulseWidth (float pw);
    void setWavetable (const juce::AudioBuffer<float>& table);

    float getNextSample();
    void processBlock (float* buffer, int numSamples);

    void setPhaseModulation (float amount);
    float getPhase() const { return static_cast<float>(phase); }

private:
    Waveform currentWaveform = Waveform::Sawtooth;
    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    float level = 1.0f;
    float pulseWidth = 0.5f;
    float phaseMod = 0.0f;

    juce::AudioBuffer<float> wavetable;
    int wavetableSize = 0;

    float naiveSine (double phase) const;
    float naiveTriangle (double phase) const;
    float naiveSawtooth (double phase) const;
    float naiveSquare (double phase) const;
    float naivePulse (double phase) const;
    float interpolateWavetable (double phase) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};

} // namespace zalaxy