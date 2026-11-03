#pragma once

#include "Common/JuceModules.h"

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
    juce::Array<float> window;

    void applyWindow();
};

} // namespace zalaxy