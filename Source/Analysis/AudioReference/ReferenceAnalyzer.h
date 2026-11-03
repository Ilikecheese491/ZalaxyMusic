#pragma once

#include <JuceHeader.h>
#include "Analysis/Spectral/SpectralAnalyzer.h"
#include "Analysis/AudioReference/AudioReferenceProcessor.h"
#include "Core/Parameters/ParameterSchema.h"

namespace zalaxy
{

class ReferenceAnalyzer
{
public:
    ReferenceAnalyzer();
    ~ReferenceAnalyzer();

    juce::var analyzeReference (const juce::File& wavFile, double startTime, double endTime);

private:
    std::unique_ptr<AudioReferenceProcessor> processor;
    std::unique_ptr<SpectralAnalyzer> spectralAnalyzer;

    juce::Array<std::pair<juce::String, float>> mapFeaturesToParameters (const SpectralFeatures& features);
    juce::String generateExplanation (const SpectralFeatures& features);
};

} // namespace zalaxy