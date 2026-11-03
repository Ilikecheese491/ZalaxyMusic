#include "SpectralAnalyzer.h"

#include <vector>

namespace zalaxy
{

SpectralAnalyzer::SpectralAnalyzer (int fftSize)
    : fftAnalyzer (std::make_unique<FFTAnalyzer> (fftSize))
{}

SpectralAnalyzer::~SpectralAnalyzer() {}

void SpectralAnalyzer::processBlock (const float* input, int numSamples, float sampleRate_)
{
    sampleRate = sampleRate_;
    fftAnalyzer->process (input, numSamples);

    features.centroid = fftAnalyzer->getSpectralCentroid (sampleRate);
    features.flatness = fftAnalyzer->getSpectralFlatness();
    features.flux = fftAnalyzer->getSpectralFlux();
    features.fundamental = fftAnalyzer->getFundamentalFrequency (sampleRate);
    features.rolloff = computeRolloff();
    features.brightness = computeBrightness();
    features.noisiness = computeNoisiness();
}

void SpectralAnalyzer::getMagnitudeSpectrum (float* output, int numBins) const
{
    fftAnalyzer->getMagnitudeSpectrum (output, numBins);
}

float SpectralAnalyzer::computeRolloff (float threshold) const
{
    const int fftSize = fftAnalyzer->getFFTSize();
    const int numBins = fftSize / 2;

    if (numBins <= 1)
        return 0.0f;

    std::vector<float> magnitudes (static_cast<size_t> (numBins));
    fftAnalyzer->getMagnitudeSpectrum (magnitudes.data(), numBins);

    float totalEnergy = 0.0f;
    for (int i = 1; i < numBins; ++i)
    {
        const float mag = magnitudes[static_cast<size_t> (i)];
        totalEnergy += mag * mag;
    }

    if (totalEnergy <= 0.0f)
        return 0.0f;

    threshold = juce::jlimit (0.0f, 1.0f, threshold);
    const float targetEnergy = threshold * totalEnergy;
    float cumulativeEnergy = 0.0f;

    for (int i = 1; i < numBins; ++i)
    {
        const float mag = magnitudes[static_cast<size_t> (i)];
        cumulativeEnergy += mag * mag;
        if (cumulativeEnergy >= targetEnergy)
            return static_cast<float> (i) * sampleRate / static_cast<float> (fftSize);
    }

    return static_cast<float> (numBins - 1) * sampleRate / static_cast<float> (fftSize);
}

float SpectralAnalyzer::computeBrightness() const
{
    const int fftSize = fftAnalyzer->getFFTSize();
    const int numBins = fftSize / 2;

    if (numBins <= 1)
        return 0.0f;

    std::vector<float> magnitudes (static_cast<size_t> (numBins));
    fftAnalyzer->getMagnitudeSpectrum (magnitudes.data(), numBins);

    float totalEnergy = 0.0f;
    float highFrequencyEnergy = 0.0f;

    for (int i = 1; i < numBins; ++i)
    {
        const float mag = magnitudes[static_cast<size_t> (i)];
        const float energy = mag * mag;
        totalEnergy += energy;

        if (i >= numBins / 2)
            highFrequencyEnergy += energy;
    }

    return totalEnergy > 0.0f ? highFrequencyEnergy / totalEnergy : 0.0f;
}

float SpectralAnalyzer::computeNoisiness() const
{
    return features.flatness;
}

} // namespace zalaxy
