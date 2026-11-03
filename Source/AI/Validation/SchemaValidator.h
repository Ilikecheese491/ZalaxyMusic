#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class SchemaValidator
{
public:
    static bool validateJSON (const juce::String& json, const juce::String& schema);
    static juce::String getTextToPatchSchema();
    static juce::String getAudioAnalysisSchema();
    static juce::String getAssistantResponseSchema();
};

} // namespace zalaxy