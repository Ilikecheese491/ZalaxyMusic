#pragma once

#include <JuceHeader.h>
#include "Analysis/FFT/FFTAnalyzer.h"

namespace zalaxy
{

struct SpectralFeatures
{
    float centroid = 0.0f;
    float flatness = 0.0f;
    float flux = 0.0f;
    float rolloff = 0.0f;
    float fundamental = 0.0f;
    float brightness = 0.0f;
    float noisiness = 0.0f;
};

class SpectralAnalyzer
{
public:
    SpectralAnalyzer (int fftSize = 2048);
    ~SpectralAnalyzer();

    void processBlock (const float* input, int numSamples, float sampleRate);
    SpectralFeatures getFeatures() const { return features; }
    void getMagnitudeSpectrum (float* output, int numBins) const;

private:
    std::unique_ptr<FFTAnalyzer> fftAnalyzer;
    SpectralFeatures features;
    float sampleRate = 44100.0f;

    float computeRolloff (float threshold = 0.85f) const;
    float computeBrightness() const;
    float computeNoisiness() const;
};

} // namespace zalaxy