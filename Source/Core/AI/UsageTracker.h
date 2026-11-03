#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

class UsageTracker
{
public:
    UsageTracker();
    ~UsageTracker();

    void initialize();
    void shutdown();

    // Token tracking
    void recordUsage (int promptTokens, int completionTokens, const juce::String& provider, const juce::String& model);
    int getTotalTokensUsed() const { return totalTokensUsed; }
    int getPromptTokensUsed() const { return promptTokensUsed; }
    int getCompletionTokensUsed() const { return completionTokensUsed; }
    double getEstimatedCost() const { return estimatedCost; }

    // Limits
    void setMonthlyLimit (int limit) { monthlyTokenLimit = limit; }
    int getMonthlyLimit() const { return monthlyTokenLimit; }
    int getRemainingTokens() const { return juce::jmax (0, monthlyTokenLimit - totalTokensUsed); }
    double getUsagePercentage() const { return monthlyTokenLimit > 0 ? (double)totalTokensUsed / monthlyTokenLimit : 0.0; }

    // Reset (monthly)
    void resetMonthlyUsage();
    bool isLimitExceeded() const { return totalTokensUsed >= monthlyTokenLimit; }

    // Per-provider breakdown
    struct ProviderUsage
    {
        int promptTokens = 0;
        int completionTokens = 0;
        double cost = 0.0;
    };
    std::map<juce::String, ProviderUsage> getProviderBreakdown() const { return providerBreakdown; }

    // Persistence
    void save();
    void load();

    // Callbacks
    std::function<void()> onLimitWarning;  // 80% threshold
    std::function<void()> onLimitExceeded;

private:
    juce::File getDataFile() const;
    void checkThresholds();

    int totalTokensUsed = 0;
    int promptTokensUsed = 0;
    int completionTokensUsed = 0;
    double estimatedCost = 0.0;
    int monthlyTokenLimit = 1000000; // 1M tokens default (OpenRouter free tier)

    std::map<juce::String, ProviderUsage> providerBreakdown;

    juce::CriticalSection lock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UsageTracker)
};

} // namespace zalaxy