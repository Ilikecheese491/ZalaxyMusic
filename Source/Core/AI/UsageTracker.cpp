#include "UsageTracker.h"
#include <algorithm>

namespace zalaxy
{

UsageTracker::UsageTracker()
{
    load();
}

UsageTracker::~UsageTracker()
{
    save();
}

void UsageTracker::initialize()
{
    load();
}

void UsageTracker::shutdown()
{
    save();
}

void UsageTracker::recordUsage (int promptTokens, int completionTokens, const juce::String& provider, const juce::String& model)
{
    const juce::ScopedLock lockGuard (lock);

    totalTokensUsed += promptTokens + completionTokens;
    promptTokensUsed += promptTokens;
    completionTokensUsed += completionTokens;

    auto& providerUsage = providerBreakdown[provider];
    providerUsage.promptTokens += promptTokens;
    providerUsage.completionTokens += completionTokens;

    // Estimate cost (rough average)
    estimatedCost += (promptTokens + completionTokens) / 1000.0 * 0.001; // $0.001 per 1k tokens average

    checkThresholds();
}

void UsageTracker::resetMonthlyUsage()
{
    const juce::ScopedLock lockGuard (lock);

    totalTokensUsed = 0;
    promptTokensUsed = 0;
    completionTokensUsed = 0;
    estimatedCost = 0.0;
    providerBreakdown.clear();

    save();
}

void UsageTracker::checkThresholds()
{
    double usagePct = getUsagePercentage();

    if (usagePct >= 1.0 && onLimitExceeded)
    {
        onLimitExceeded();
    }
    else if (usagePct >= 0.8 && onLimitWarning)
    {
        onLimitWarning();
    }
}

void UsageTracker::save()
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty ("totalTokensUsed", totalTokensUsed);
    obj->setProperty ("promptTokensUsed", promptTokensUsed);
    obj->setProperty ("completionTokensUsed", completionTokensUsed);
    obj->setProperty ("estimatedCost", estimatedCost);
    obj->setProperty ("monthlyTokenLimit", monthlyTokenLimit);

    juce::DynamicObject::Ptr breakdownObj = new juce::DynamicObject();
    for (auto& [provider, usage] : providerBreakdown)
    {
        juce::DynamicObject::Ptr providerObj = new juce::DynamicObject();
        providerObj->setProperty ("promptTokens", usage.promptTokens);
        providerObj->setProperty ("completionTokens", usage.completionTokens);
        providerObj->setProperty ("cost", usage.cost);
        breakdownObj->setProperty (provider, providerObj);
    }
    obj->setProperty ("providerBreakdown", breakdownObj);

    juce::File file = getDataFile();
    file.getParentDirectory().createDirectory();
    juce::FileOutputStream stream (file);
    if (stream.openedOk())
    {
        juce::JSON::writeToStream (stream, obj);
    }
}

void UsageTracker::load()
{
    juce::File file = getDataFile();
    if (!file.existsAsFile())
        return;

    juce::var parsed = juce::JSON::parse (file);
    if (!parsed.isObject())
        return;

    auto* obj = parsed.getDynamicObject();
    if (!obj)
        return;

    totalTokensUsed = obj->getProperty ("totalTokensUsed", 0);
    promptTokensUsed = obj->getProperty ("promptTokensUsed", 0);
    completionTokensUsed = obj->getProperty ("completionTokensUsed", 0);
    estimatedCost = obj->getProperty ("estimatedCost", 0.0);
    monthlyTokenLimit = obj->getProperty ("monthlyTokenLimit", 1000000);

    if (auto* breakdownObj = obj->getProperty ("providerBreakdown").getDynamicObject())
    {
        for (auto& prop : breakdownObj->getProperties())
        {
            auto* usageObj = prop.value.getDynamicObject();
            if (usageObj)
            {
                ProviderUsage usage;
                usage.promptTokens = usageObj->getProperty ("promptTokens", 0);
                usage.completionTokens = usageObj->getProperty ("completionTokens", 0);
                usage.cost = usageObj->getProperty ("cost", 0.0);
                providerBreakdown[prop.key.toString()] = usage;
            }
        }
    }
}

juce::File UsageTracker::getDataFile() const
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("ZalaxyMusic")
        .getChildFile ("usage_tracker.json");
}

} // namespace zalaxy