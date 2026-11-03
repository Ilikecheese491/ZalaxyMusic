#pragma once

#include <JuceHeader.h>

namespace zalaxy
{

class ExplanationResponse
{
public:
    static juce::String getResponse (const juce::var& response);
};

} // namespace zalaxy