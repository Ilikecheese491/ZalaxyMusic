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

    // Estimate cost (rough average) - update both total and per-provider
    const double cost = (promptTokens + completionTokens) / 1000.0 * 0.001; // $0.001 per 1k tokens average
    estimatedCost += cost;
    providerUsage.cost += cost;

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
        breakdownObj->setProperty (provider, juce::var (providerObj.get()));
    }
    obj->setProperty ("providerBreakdown", juce::var (breakdownObj.get()));

    juce::File file = getDataFile();
    file.getParentDirectory().createDirectory();
    juce::FileOutputStream stream (file);
    if (stream.openedOk())
    {
        juce::var json (obj.get());
        juce::JSON::writeToStream (stream, json, false, 0);
    }
}

void UsageTracker::load()
{
    juce::File file = getDataFile();
    if (!file.existsAsFile())
        return;

    juce::String jsonText = file.loadFileAsString();
    juce::var parsed = juce::JSON::parse (jsonText);
    if (!parsed.isObject())
        return;

    auto* obj = parsed.getDynamicObject();
    if (!obj)
        return;

    auto getInt = [&] (const juce::String& key, int defaultVal) -> int {
        auto v = obj->getProperty (key);
        return v.isVoid() ? defaultVal : (int)v;
    };
    auto getDouble = [&] (const juce::String& key, double defaultVal) -> double {
        auto v = obj->getProperty (key);
        return v.isVoid() ? defaultVal : (double)v;
    };

    totalTokensUsed = getInt ("totalTokensUsed", 0);
    promptTokensUsed = getInt ("promptTokensUsed", 0);
    completionTokensUsed = getInt ("completionTokensUsed", 0);
    estimatedCost = getDouble ("estimatedCost", 0.0);
    monthlyTokenLimit = getInt ("monthlyTokenLimit", 1000000);

    if (auto* breakdownObj = obj->getProperty ("providerBreakdown").getDynamicObject())
    {
        auto getIntFromObj = [&] (juce::DynamicObject* o, const juce::String& key, int defaultVal) -> int {
            auto v = o->getProperty (key);
            return v.isVoid() ? defaultVal : (int)v;
        };
        auto getDoubleFromObj = [&] (juce::DynamicObject* o, const juce::String& key, double defaultVal) -> double {
            auto v = o->getProperty (key);
            return v.isVoid() ? defaultVal : (double)v;
        };

        for (auto& prop : breakdownObj->getProperties())
        {
            auto* usageObj = prop.value.getDynamicObject();
            if (usageObj)
            {
                ProviderUsage usage;
                usage.promptTokens = getIntFromObj (usageObj, "promptTokens", 0);
                usage.completionTokens = getIntFromObj (usageObj, "completionTokens", 0);
                usage.cost = getDoubleFromObj (usageObj, "cost", 0.0);
                providerBreakdown[prop.name.toString()] = usage;
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