#include "FFTAnalyzer.h"

namespace zalaxy
{

FFTAnalyzer::FFTAnalyzer (int fftSize_)
    : fftSize (fftSize_)
    , hopSize (fftSize_ / 4)
    , fft (std::make_unique<juce::dsp::FFT> (static_cast<int> (std::log2 (fftSize_))))
    , fftData (fftSize_ * 2)
    , magnitude (fftSize_ / 2)
    , phase (fftSize_ / 2)
    , prevMagnitude (fftSize_ / 2)
    , averageMagnitude (fftSize_ / 2)
{
    window.resize (fftSize_);
    for (int i = 0; i < fftSize_; ++i)
        window[i] = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi * i / (fftSize_ - 1)));

    magnitude.clear (fftSize_ / 2);
    phase.clear (fftSize_ / 2);
    prevMagnitude.clear (fftSize_ / 2);
    averageMagnitude.clear (fftSize_ / 2);
}

FFTAnalyzer::~FFTAnalyzer() {}

void FFTAnalyzer::process (const float* input, int numSamples)
{
    magnitude.clear (fftSize / 2);
    phase.clear (fftSize / 2);
    prevMagnitude.clear (fftSize / 2);
    averageMagnitude.clear (fftSize / 2);
    spectralFlux = 0.0f;

    if (input == nullptr || numSamples < fftSize || hopSize <= 0)
        return;

    const int numFrames = (numSamples - fftSize) / hopSize + 1;
    float accumulatedFlux = 0.0f;

    for (int frame = 0; frame < numFrames; ++frame)
    {
        const int offset = frame * hopSize;

        for (int i = 0; i < fftSize; ++i)
        {
            fftData[i * 2] = input[offset + i] * window[static_cast<size_t> (i)];
            fftData[i * 2 + 1] = 0.0f;
        }

        fft->performRealOnlyForwardTransform (fftData);

        for (int i = 0; i < fftSize / 2; ++i)
        {
            const float re = fftData[i * 2];
            const float im = fftData[i * 2 + 1];
            const float frameMagnitude = std::sqrt (re * re + im * im);
            const float difference = frameMagnitude - magnitude[i];

            if (difference > 0.0f)
                accumulatedFlux += difference;

            prevMagnitude[i] = magnitude[i];
            magnitude[i] = frameMagnitude;
            averageMagnitude[i] += frameMagnitude;
            phase[i] = std::atan2 (im, re);
        }
    }

    for (int i = 0; i < fftSize / 2; ++i)
        magnitude[i] = averageMagnitude[i] / static_cast<float> (numFrames);

    spectralFlux = accumulatedFlux / static_cast<float> (numFrames);
}

void FFTAnalyzer::getMagnitudeSpectrum (float* output, int numBins) const
{
    if (output == nullptr || numBins <= 0)
        return;

    const int bins = juce::jmin (numBins, fftSize / 2);
    for (int i = 0; i < bins; ++i)
        output[i] = magnitude[i];
}

void FFTAnalyzer::getPhaseSpectrum (float* output, int numBins) const
{
    if (output == nullptr || numBins <= 0)
        return;

    const int bins = juce::jmin (numBins, fftSize / 2);
    for (int i = 0; i < bins; ++i)
        output[i] = phase[i];
}

float FFTAnalyzer::getFundamentalFrequency (float sampleRate) const
{
    float maxMag = 0.0f;
    int maxBin = 0;

    for (int i = 1; i < fftSize / 2; ++i)
    {
        if (magnitude[i] > maxMag)
        {
            maxMag = magnitude[i];
            maxBin = i;
        }
    }

    return maxBin * sampleRate / fftSize;
}

float FFTAnalyzer::getSpectralCentroid (float sampleRate) const
{
    float sumMag = 0.0f;
    float sumFreqMag = 0.0f;

    for (int i = 1; i < fftSize / 2; ++i)
    {
        const float freq = i * sampleRate / fftSize;
        sumMag += magnitude[i];
        sumFreqMag += freq * magnitude[i];
    }

    return sumMag > 0.0f ? sumFreqMag / sumMag : 0.0f;
}

float FFTAnalyzer::getSpectralFlatness() const
{
    float logMagnitudeSum = 0.0f;
    float magnitudeSum = 0.0f;
    int count = 0;

    for (int i = 1; i < fftSize / 2; ++i)
    {
        const float value = magnitude[i];
        if (value > 0.0f)
        {
            logMagnitudeSum += std::log (value);
            magnitudeSum += value;
            ++count;
        }
    }

    if (count == 0 || magnitudeSum <= 0.0f)
        return 0.0f;

    const float geometricMean = std::exp (logMagnitudeSum / static_cast<float> (count));
    const float arithmeticMean = magnitudeSum / static_cast<float> (count);
    return juce::jlimit (0.0f, 1.0f, geometricMean / arithmeticMean);
}

float FFTAnalyzer::getSpectralFlux() const
{
    return spectralFlux;
}

void FFTAnalyzer::applyWindow()
{
    for (int i = 0; i < fftSize; ++i)
        fftData[i * 2] *= window[static_cast<size_t> (i)];
}

} // namespace zalaxy
