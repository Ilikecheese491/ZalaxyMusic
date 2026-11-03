#pragma once

#include "AIProvider.h"
#include "JuceModules.h"

namespace zalaxy
{

class LocalAIProvider : public AIProvider
{
public:
    LocalAIProvider();
    ~LocalAIProvider() override;

    void requestTextToPatch (const juce::String& prompt, std::function<void(const juce::var&)> callback) override;
    void requestAudioAnalysis (const juce::File& wavFile, double startTime, double endTime, std::function<void(const juce::var&)> callback) override;
    void requestAssistantResponse (const juce::String& question, std::function<void(const juce::var&)> callback) override;

    juce::String getProviderName() const override { return "Local (Stub)"; }
    bool isAvailable() const override { return true; }

private:
    juce::var createStubPatchResponse (const juce::String& prompt);
    juce::var createStubAnalysisResponse();
    juce::var createStubAssistantResponse (const juce::String& question);
};

} // namespace zalaxy