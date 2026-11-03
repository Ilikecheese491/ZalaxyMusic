#include "Filter.h"
#include <cmath>

namespace zalaxy
{

Filter::Filter() = default;
Filter::~Filter() = default;

void Filter::prepare (double sr)
{
    sampleRate = sr;
    updateCoefficients();
}

void Filter::reset()
{
    x1 = x2 = y1 = y2 = 0.0f;
}

void Filter::setType (FilterType type)
{
    currentType = type;
    updateCoefficients();
}

void Filter::setCutoff (float freqHz)
{
    cutoff = juce::jlimit (20.0f, static_cast<float> (sampleRate * 0.45), freqHz);
    updateCoefficients();
}

void Filter::setResonance (float q)
{
    resonance = juce::jlimit (0.1f, 20.0f, q);
    updateCoefficients();
}

void Filter::setGain (float gain)
{
    gainDb = gain;
    updateCoefficients();
}

void Filter::updateCoefficients()
{
    const float wc = 2.0f * juce::MathConstants<float>::pi * cutoff / static_cast<float> (sampleRate);
    const float k = std::tan (wc * 0.5f);
    const float k2 = k * k;
    const float norm = 1.0f / (1.0f + k / resonance + k2);

    switch (currentType)
    {
        case FilterType::LowPass:
            a0 = k2 * norm;
            a1 = 2.0f * a0;
            a2 = a0;
            b1 = 2.0f * (k2 - 1.0f) * norm;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;

        case FilterType::HighPass:
            a0 = 1.0f * norm;
            a1 = -2.0f * a0;
            a2 = a0;
            b1 = 2.0f * (k2 - 1.0f) * norm;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;

        case FilterType::BandPass:
            a0 = k / resonance * norm;
            a1 = 0.0f;
            a2 = -a0;
            b1 = 2.0f * (k2 - 1.0f) * norm;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;

        case FilterType::Notch:
            a0 = (1.0f + k2) * norm;
            a1 = 2.0f * (k2 - 1.0f) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;

        case FilterType::Peak:
        {
            const float V0 = std::pow (10.0f, gainDb / 20.0f);
            a0 = (1.0f + k / resonance * V0 + k2) * norm;
            a1 = 2.0f * (k2 - 1.0f) * norm;
            a2 = (1.0f - k / resonance * V0 + k2) * norm;
            b1 = a1;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;
        }

        case FilterType::LowShelf:
        {
            const float V0 = std::pow (10.0f, gainDb / 20.0f);
            a0 = (1.0f + std::sqrt(2.0f) * k * std::sqrt(V0) + k2 * V0) * norm;
            a1 = 2.0f * (k2 * V0 - 1.0f) * norm;
            a2 = (1.0f - std::sqrt(2.0f) * k * std::sqrt(V0) + k2 * V0) * norm;
            b1 = 2.0f * (k2 - 1.0f) * norm;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;
        }

        case FilterType::HighShelf:
        {
            const float V0 = std::pow (10.0f, gainDb / 20.0f);
            a0 = (V0 + std::sqrt(2.0f) * k * std::sqrt(V0) + k2) * norm;
            a1 = 2.0f * (k2 - V0) * norm;
            a2 = (V0 - std::sqrt(2.0f) * k * std::sqrt(V0) + k2) * norm;
            b1 = 2.0f * (k2 - 1.0f) * norm;
            b2 = (1.0f - k / resonance + k2) * norm;
            break;
        }
    }
}

float Filter::processSample (float input)
{
    float output = a0 * input + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return output;
}

void Filter::processBlock (float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = processSample (buffer[i]);
    }
}

void Filter::getFrequencyResponse (float* magnitudes, float* phases, int numPoints, float minFreq, float maxFreq) const
{
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = minFreq * std::pow (maxFreq / minFreq, static_cast<float> (i) / (numPoints - 1));
        float w = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float> (sampleRate);
        float cosw = std::cos (w);
        float cos2w = std::cos (2.0f * w);
        float sinw = std::sin (w);
        float sin2w = std::sin (2.0f * w);

        float numRe = a0 + a1 * cosw + a2 * cos2w;
        float numIm = -(a1 * sinw + a2 * sin2w);
        float denRe = 1.0f + b1 * cosw + b2 * cos2w;
        float denIm = -(b1 * sinw + b2 * sin2w);

        float mag = std::sqrt (numRe * numRe + numIm * numIm) / std::sqrt (denRe * denRe + denIm * denIm);
        float phase = std::atan2 (numIm, numRe) - std::atan2 (denIm, denRe);

        magnitudes[i] = mag;
        phases[i] = phase;
    }
}

} // namespace zalaxy