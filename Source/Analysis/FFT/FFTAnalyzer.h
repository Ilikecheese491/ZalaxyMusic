#pragma once

#include "JuceModules.h"
#include <vector>

namespace zalaxy
{

class FFTAnalyzer
{
public:
    FFTAnalyzer (int fftSize = 2048);
    ~FFTAnalyzer();

    void process (const float* input, int numSamples);
    void getMagnitudeSpectrum (float* output, int numBins) const;
    void getPhaseSpectrum (float* output, int numBins) const;
    float getFundamentalFrequency (float sampleRate) const;
    float getSpectralCentroid (float sampleRate) const;
    float getSpectralFlatness() const;
    float getSpectralFlux() const;

    int getFFTSize() const { return fftSize; }
    int getHopSize() const { return hopSize; }

private:
    int fftSize;
    int hopSize;
    std::unique_ptr<juce::dsp::FFT> fft;
    juce::HeapBlock<float> fftData;
    juce::HeapBlock<float> magnitude;
    juce::HeapBlock<float> phase;
    juce::HeapBlock<float> prevMagnitude;
    juce::HeapBlock<float> averageMagnitude;
    std::vector<float> window;
    float spectralFlux = 0.0f;

    void applyWindow();
};

} // namespace zalaxy
