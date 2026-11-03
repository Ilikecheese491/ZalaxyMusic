#pragma once

#include "JuceModules.h"
#include "Providers/AIProvider.h"

namespace zalaxy
{

class UsageTracker;
class SubscriptionManager;

class CloudAIProvider : public AIProvider
{
public:
    struct ProviderConfig
    {
        juce::String name;
        juce::String baseURL;
        juce::String defaultModel;
        int maxTokens;
        double costPer1kTokens;
    };

    CloudAIProvider (UsageTracker& usage, SubscriptionManager& sub);
    ~CloudAIProvider() override;

    void initialize();

    void requestTextToPatch (const juce::String& prompt, std::function<void(const juce::var&)> callback) override;
    void requestAudioAnalysis (const juce::File& wavFile, double startTime, double endTime, std::function<void(const juce::var&)> callback) override;
    void requestAssistantResponse (const juce::String& question, std::function<void(const juce::var&)> callback) override;

    juce::String getProviderName() const override { return currentProviderConfig.name; }
    bool isAvailable() const override { return hasValidAPIKey(); }

    void setProvider (const juce::String& providerName);
    juce::String getCurrentProvider() const { return currentProviderName; }
    juce::StringArray getAvailableProviders() const;

    bool setAPIKey (const juce::String& provider, const juce::String& key);
    juce::String getAPIKey (const juce::String& provider) const;
    bool hasValidAPIKey() const;
    void clearAPIKey (const juce::String& provider);

    void setCloudMode (bool enabled);
    bool isCloudMode() const { return cloudModeEnabled; }

    void recordUsage (int promptTokens, int completionTokens);
    double getEstimatedCost() const;

    void testConnection (std::function<void(bool, const juce::String&)> callback);

private:
    UsageTracker& usageTracker;
    SubscriptionManager& subscriptionManager;

    juce::String currentProviderName;
    ProviderConfig currentProviderConfig;
    bool cloudModeEnabled = true;

    std::map<juce::String, juce::String> apiKeys;

    static const std::map<juce::String, ProviderConfig> providerConfigs;

    juce::String buildRequestBody (const juce::String& prompt, const juce::String& systemPrompt) const;
    void sendRequest (const juce::String& endpoint, const juce::String& body, std::function<void(const juce::var&)> callback);
    juce::String getSystemPromptForTask (const juce::String& task) const;
    bool validateResponse (const juce::var& response) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CloudAIProvider)
};

} // namespace zalaxy
