#include "ParameterValidator.h"

namespace zalaxy
{

bool ParameterValidator::validate (const juce::String& paramID, float value)
{
    const auto* info = ParameterSchema::getParameterInfo (paramID);
    if (info == nullptr)
        return false;

    return value >= info->minValue && value <= info->maxValue;
}

float ParameterValidator::clamp (const juce::String& paramID, float value)
{
    const auto* info = ParameterSchema::getParameterInfo (paramID);
    if (info == nullptr)
        return value;

    return juce::jlimit (info->minValue, info->maxValue, value);
}

juce::String ParameterValidator::getErrorMessage (const juce::String& paramID, float value)
{
    const auto* info = ParameterSchema::getParameterInfo (paramID);
    if (info == nullptr)
        return paramID + " value " + juce::String (value) + " is invalid for an unknown parameter";

    if (value < info->minValue || value > info->maxValue)
        return paramID + " value " + juce::String (value) + " out of range [" +
               juce::String (info->minValue) + ", " + juce::String (info->maxValue) + "]";
    return "";
}

} // namespace zalaxy
