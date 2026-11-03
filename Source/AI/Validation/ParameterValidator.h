#pragma once

#include "Common/JuceModules.h"
#include "Core/Parameters/ParameterSchema.h"

namespace zalaxy
{

class ParameterValidator
{
public:
    static bool validate (const juce::String& paramID, float value);
    static float clamp (const juce::String& paramID, float value);
    static juce::String getErrorMessage (const juce::String& paramID, float value);
};

} // namespace zalaxy