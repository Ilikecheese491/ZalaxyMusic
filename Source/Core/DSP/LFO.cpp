#include "LFO.h"
#include <cmath>

namespace zalaxy
{

LFO::LFO() = default;
LFO::~LFO() = default;

void LFO::prepare (double sr)
{
    sampleRate = sr;
    updatePhaseIncrement();
}

void LFO::reset()
{
    phase = 0.0;
    delaySamplesRemaining = delayTime * static_cast<float> (sampleRate);
    fadeInSamplesRemaining = fadeInTime * static_cast<float> (sampleRate);
    currentDepth = 0.0f;
    sampleAndHoldValue = 0.0f;
    sampleAndHoldPhase = 0.0;
}

void LFO::setWaveform (LFOWaveform wf)
{
    currentWaveform = wf;
}

void LFO::setRate (float rate)
{
    rateHz = juce::jmax (0.01f, rate);
    updatePhaseIncrement();
}

void LFO::setDepth (float d)
{
    depth = juce::jlimit (0.0f, 1.0f, d);
}

void LFO::setDelay (float seconds)
{
    delayTime = juce::jmax (0.0f, seconds);
}

void LFO::setFadeIn (float seconds)
{
    fadeInTime = juce::jmax (0.0f, seconds);
}

void LFO::setTempoSync (bool sync, float bpmValue)
{
    tempoSync = sync;
    bpm = juce::jlimit (20.0f, 300.0f, bpmValue);
    if (tempoSync)
    {
        // Convert BPM to Hz for common note values
        // Default to 1/4 note
        rateHz = bpm / 60.0f;
    }
    updatePhaseIncrement();
}

void LFO::noteOn()
{
    if (keySync)
    {
        phase = 0.0;
        delaySamplesRemaining = delayTime * static_cast<float> (sampleRate);
        fadeInSamplesRemaining = fadeInTime * static_cast<float> (sampleRate);
        currentDepth = 0.0f;
        sampleAndHoldValue = 0.0f;
        sampleAndHoldPhase = 0.0;
    }
}

void LFO::noteOff()
{
    // LFO continues running unless keySync is false and we want it to stop
}

float LFO::getNextSample()
{
    // Handle delay
    if (delaySamplesRemaining > 0)
    {
        delaySamplesRemaining -= 1.0f;
        return 0.0f;
    }

    // Handle fade-in
    if (fadeInSamplesRemaining > 0)
    {
        fadeInSamplesRemaining -= 1.0f;
        currentDepth = depth * (1.0f - fadeInSamplesRemaining / (fadeInTime * static_cast<float> (sampleRate)));
    }
    else
    {
        currentDepth = depth;
    }

    float output = 0.0f;

    switch (currentWaveform)
    {
        case LFOWaveform::Sine:
            output = static_cast<float> (std::sin (phase * 2.0 * juce::MathConstants<double>::pi));
            break;

        case LFOWaveform::Triangle:
        {
            float p = static_cast<float> (phase);
            output = 2.0f * std::abs (2.0f * (p - std::floor (p + 0.5f))) - 1.0f;
            break;
        }

        case LFOWaveform::Sawtooth:
        {
            float p = static_cast<float> (phase);
            output = 2.0f * (p - std::floor (p + 0.5f));
            break;
        }

        case LFOWaveform::Square:
            output = (phase < 0.5) ? 1.0f : -1.0f;
            break;

        case LFOWaveform::SampleAndHold:
        {
            double newPhase = phase + phaseIncrement;
            if (newPhase >= 1.0)
            {
                sampleAndHoldValue = static_cast<float> (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);
            }
            output = sampleAndHoldValue;
            break;
        }

        case LFOWaveform::Random:
            output = static_cast<float> (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);
            break;
    }

    phase += phaseIncrement;
    if (phase >= 1.0) phase -= 1.0;

    return output * currentDepth;
}

void LFO::processBlock (float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = getNextSample();
    }
}

void LFO::getShapePoints (juce::Array<juce::Point<float>>& points, float width, float height) const
{
    points.clear();
    if (width <= 0 || height <= 0) return;

    const int numPoints = 100;
    for (int i = 0; i <= numPoints; ++i)
    {
        float ph = static_cast<float> (i) / numPoints;
        float val = 0.0f;

        switch (currentWaveform)
        {
            case LFOWaveform::Sine:
                val = std::sin (ph * 2.0f * juce::MathConstants<float>::pi);
                break;
            case LFOWaveform::Triangle:
                val = 2.0f * std::abs (2.0f * (ph - std::floor (ph + 0.5f))) - 1.0f;
                break;
            case LFOWaveform::Sawtooth:
                val = 2.0f * (ph - std::floor (ph + 0.5f));
                break;
            case LFOWaveform::Square:
                val = (ph < 0.5f) ? 1.0f : -1.0f;
                break;
            case LFOWaveform::SampleAndHold:
            case LFOWaveform::Random:
                val = 0.0f; // Can't predict
                break;
        }

        float x = ph * width;
        float y = height * 0.5f * (1.0f - val); // Center vertically
        points.add ({x, y});
    }
}

void LFO::updatePhaseIncrement()
{
    if (tempoSync)
    {
        // Already set in setTempoSync
    }
    else
    {
        phaseIncrement = rateHz / sampleRate;
    }
}

double LFO::timeToSamples (float seconds) const
{
    return seconds * sampleRate;
}

} // namespace zalaxy