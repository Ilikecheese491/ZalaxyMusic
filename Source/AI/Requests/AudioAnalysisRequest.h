#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

class AudioAnalysisRequest
{
public:
    static juce::var createRequest (const juce::File& wavFile, double startTime, double endTime);
    static bool validateResponse (const juce::var& response);
    static juce::String getSchema();
};

} // namespace zalaxy