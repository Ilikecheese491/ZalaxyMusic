#include "SpectralAnalyzer.h"

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
    float totalEnergy = 0.0f;
    int numBins = fftAnalyzer->getFFTSize() / 2;

    for (int i = 0; i < numBins; ++i)
        totalEnergy += fftAnalyzer->getMagnitudeSpectrum(nullptr, 0)[i]; // Need access to magnitude

    // Stub implementation
    return 0.7f;
}

float SpectralAnalyzer::computeBrightness() const
{
    // Ratio of high-frequency energy to total energy
    return 0.5f;
}

float SpectralAnalyzer::computeNoisiness() const
{
    // Inverse of flatness (tonal vs noisy)
    return 1.0f - features.flatness;
}

} // namespace zalaxy