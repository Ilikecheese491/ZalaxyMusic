#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class TextToPatchRequest
{
public:
    static juce::var createRequest (const juce::String& prompt);
    static bool validateResponse (const juce::var& response);
    static juce::String getSchema();
};

} // namespace zalaxy