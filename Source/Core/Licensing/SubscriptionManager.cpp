#include "SubscriptionManager.h"

namespace zalaxy
{

void SubscriptionManager::load()
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

    currentTier = static_cast<LicenseTier> (static_cast<int> (obj->getProperty ("tier")));
    licenseKey = obj->getProperty ("licenseKey").toString();
    userEmail = obj->getProperty ("userEmail").toString();
    trialStartTime = (int64_t) obj->getProperty ("trialStartTime");
    licenseExpiryTime = (int64_t) obj->getProperty ("licenseExpiryTime");
    trialDays = (int) obj->getProperty ("trialDays");

    if (!licenseKey.isEmpty())
    {
        parseLicenseKey (licenseKey);
    }
}

} // namespace zalaxy
