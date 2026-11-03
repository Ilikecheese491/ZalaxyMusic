#include "Oscillator.h"
#include <cmath>

namespace zalaxy
{

Oscillator::Oscillator() = default;
Oscillator::~Oscillator() = default;

void Oscillator::prepare (double sr)
{
    sampleRate = sr;
    // Initialize default wavetable (single cycle sine)
    wavetable.setSize (1, 2048);
    wavetableSize = 2048;
    for (int i = 0; i < wavetableSize; ++i)
    {
        float phase = static_cast<float>(i) / wavetableSize * 2.0f * juce::MathConstants<float>::pi;
        wavetable.setSample (0, i, std::sin (phase));
    }
}

void Oscillator::reset()
{
    phase = 0.0;
}

void Oscillator::setWaveform (Waveform wf)
{
    currentWaveform = wf;
}

void Oscillator::setFrequency (float freqHz)
{
    phaseIncrement = freqHz / sampleRate;
}

void Oscillator::setLevel (float lvl)
{
    level = lvl;
}

void Oscillator::setPulseWidth (float pw)
{
    pulseWidth = juce::jlimit (0.01f, 0.99f, pw);
}

void Oscillator::setWavetable (const juce::AudioBuffer<float>& table)
{
    if (table.getNumSamples() > 0)
    {
        wavetable.makeCopyOf (table);
        wavetableSize = wavetable.getNumSamples();
    }
}

float Oscillator::getNextSample()
{
    float sample = 0.0f;
    double modulatedPhase = phase + phaseMod;

    switch (currentWaveform)
    {
        case Waveform::Sine:
            sample = naiveSine (modulatedPhase);
            break;
        case Waveform::Triangle:
            sample = naiveTriangle (modulatedPhase);
            break;
        case Waveform::Sawtooth:
            sample = naiveSawtooth (modulatedPhase);
            break;
        case Waveform::Square:
            sample = naiveSquare (modulatedPhase);
            break;
        case Waveform::Pulse:
            sample = naivePulse (modulatedPhase);
            break;
        case Waveform::Wavetable:
            sample = interpolateWavetable (modulatedPhase);
            break;
        case Waveform::Noise:
            sample = static_cast<float> (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);
            break;
    }

    phase += phaseIncrement;
    if (phase >= 1.0) phase -= 1.0;

    return sample * level;
}

void Oscillator::processBlock (float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = getNextSample();
    }
}

void Oscillator::setPhaseModulation (float amount)
{
    phaseMod = amount;
}

float Oscillator::naiveSine (double ph) const
{
    return static_cast<float> (std::sin (ph * 2.0 * juce::MathConstants<double>::pi));
}

float Oscillator::naiveTriangle (double ph) const
{
    float p = static_cast<float> (ph);
    return 2.0f * std::abs (2.0f * (p - std::floor (p + 0.5f))) - 1.0f;
}

float Oscillator::naiveSawtooth (double ph) const
{
    float p = static_cast<float> (ph);
    return 2.0f * (p - std::floor (p + 0.5f));
}

float Oscillator::naiveSquare (double ph) const
{
    return (ph < 0.5) ? 1.0f : -1.0f;
}

float Oscillator::naivePulse (double ph) const
{
    return (ph < pulseWidth) ? 1.0f : -1.0f;
}

float Oscillator::interpolateWavetable (double ph) const
{
    if (wavetableSize == 0) return 0.0f;

    float p = static_cast<float> (ph - std::floor (ph));
    float index = p * (wavetableSize - 1);
    int idx1 = static_cast<int> (index);
    int idx2 = (idx1 + 1) % wavetableSize;
    float frac = index - idx1;

    float s1 = wavetable.getSample (0, idx1);
    float s2 = wavetable.getSample (0, idx2);

    return s1 + frac * (s2 - s1);
}

} // namespace zalaxy