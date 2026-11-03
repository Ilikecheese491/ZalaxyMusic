#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class AIProvider
{
public:
    virtual ~AIProvider() = default;

    // Text-to-patch generation
    virtual void requestTextToPatch (const juce::String& prompt, std::function<void(const juce::var&)> callback) = 0;

    // Audio reference analysis
    virtual void requestAudioAnalysis (const juce::File& wavFile, double startTime, double endTime, std::function<void(const juce::var&)> callback) = 0;

    // Production assistant chat
    virtual void requestAssistantResponse (const juce::String& question, std::function<void(const juce::var&)> callback) = 0;

    // Provider info
    virtual juce::String getProviderName() const = 0;
    virtual bool isAvailable() const = 0;
};

} // namespace zalaxy