#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class PatchResponse
{
public:
    static juce::String getExplanation (const juce::var& response);
    static juce::Array<std::pair<juce::String, float>> getParameters (const juce::var& response);
    static bool hasVariations (const juce::var& response);
};

} // namespace zalaxy