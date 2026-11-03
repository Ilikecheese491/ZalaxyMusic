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
{
    window.resize (fftSize_);
    for (int i = 0; i < fftSize_; ++i)
        window[i] = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi * i / (fftSize_ - 1)));
    prevMagnitude.clear (fftSize_ / 2);
}

FFTAnalyzer::~FFTAnalyzer() {}

void FFTAnalyzer::process (const float* input, int numSamples)
{
    int numFrames = (numSamples - fftSize) / hopSize + 1;
    for (int frame = 0; frame < numFrames; ++frame)
    {
        int offset = frame * hopSize;
        for (int i = 0; i < fftSize; ++i)
        {
            fftData[i * 2] = input[offset + i] * window[i];
            fftData[i * 2 + 1] = 0.0f;
        }

        fft->performRealOnlyForwardTransform (fftData);

        for (int i = 0; i < fftSize / 2; ++i)
        {
            float re = fftData[i * 2];
            float im = fftData[i * 2 + 1];
            magnitude[i] = std::sqrt (re * re + im * im);
            phase[i] = std::atan2 (im, re);
        }
    }
}

void FFTAnalyzer::getMagnitudeSpectrum (float* output, int numBins) const
{
    int bins = juce::jmin (numBins, fftSize / 2);
    for (int i = 0; i < bins; ++i)
        output[i] = magnitude[i];
}

void FFTAnalyzer::getPhaseSpectrum (float* output, int numBins) const
{
    int bins = juce::jmin (numBins, fftSize / 2);
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
    float sumMag = 0.0f, sumFreqMag = 0.0f;
    for (int i = 1; i < fftSize / 2; ++i)
    {
        float freq = i * sampleRate / fftSize;
        sumMag += magnitude[i];
        sumFreqMag += freq * magnitude[i];
    }
    return sumMag > 0 ? sumFreqMag / sumMag : 0.0f;
}

float FFTAnalyzer::getSpectralFlatness() const
{
    float geoMean = 0.0f, arithMean = 0.0f;
    int count = 0;
    for (int i = 1; i < fftSize / 2; ++i)
    {
        if (magnitude[i] > 0)
        {
            geoMean += std::log (magnitude[i]);
            arithMean += magnitude[i];
            ++count;
        }
    }
    if (count == 0) return 0.0f;
    geoMean = std::exp (geoMean / count);
    arithMean /= count;
    return arithMean > 0 ? geoMean / arithMean : 0.0f;
}

float FFTAnalyzer::getSpectralFlux() const
{
    float flux = 0.0f;
    for (int i = 0; i < fftSize / 2; ++i)
    {
        float diff = magnitude[i] - prevMagnitude[i];
        if (diff > 0) flux += diff;
    }
    // Note: prevMagnitude should be updated externally after each frame
    return flux;
}

void FFTAnalyzer::applyWindow()
{
    for (int i = 0; i < fftSize; ++i)
        fftData[i * 2] *= window[i];
}

} // namespace zalaxy