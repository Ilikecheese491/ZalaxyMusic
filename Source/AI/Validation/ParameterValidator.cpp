#include "ParameterValidator.h"

namespace zalaxy
{

bool ParameterValidator::validate (const juce::String& paramID, float value)
{
    auto info = ParameterSchema::getParameterInfo (paramID);
    return value >= info.minValue && value <= info.maxValue;
}

float ParameterValidator::clamp (const juce::String& paramID, float value)
{
    auto info = ParameterSchema::getParameterInfo (paramID);
    return juce::jlimit (info.minValue, info.maxValue, value);
}

juce::String ParameterValidator::getErrorMessage (const juce::String& paramID, float value)
{
    auto info = ParameterSchema::getParameterInfo (paramID);
    if (value < info.minValue || value > info.maxValue)
        return paramID + " value " + juce::String (value) + " out of range [" +
               juce::String (info.minValue) + ", " + juce::String (info.maxValue) + "]";
    return "";
}

} // namespace zalaxy