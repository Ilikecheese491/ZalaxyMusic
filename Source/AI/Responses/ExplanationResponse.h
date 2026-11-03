#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

class ExplanationResponse
{
public:
    static juce::String getResponse (const juce::var& response);
};

} // namespace zalaxy