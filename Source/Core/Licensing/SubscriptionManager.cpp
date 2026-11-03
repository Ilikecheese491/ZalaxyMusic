#include "SubscriptionManager.h"
#include <cstdint>

namespace zalaxy
{

SubscriptionManager::SubscriptionManager()
{
    load();
}

SubscriptionManager::~SubscriptionManager()
{
    save();
}

void SubscriptionManager::initialize()
{
    load();
    checkExpiration();
}

bool SubscriptionManager::validateLicenseKey (const juce::String& key)
{
    if (!key.startsWith ("ZALX-"))
        return false;

    if (!verifyLicenseSignature (key, PUBLIC_KEY_PEM))
        return false;

    return parseLicenseKey (key);
}

void SubscriptionManager::applyLicenseKey (const juce::String& key)
{
    if (validateLicenseKey (key))
    {
        licenseKey = key;
        parseLicenseKey (key);
        save();
    }
}

void SubscriptionManager::clearLicense()
{
    currentTier = LicenseTier::Free;
    licenseKey = "";
    userEmail = "";
    licenseExpiryTime = 0;
    trialStartTime = 0;
    save();
}

juce::String SubscriptionManager::getCurrentTierName() const
{
    switch (currentTier)
    {
        case LicenseTier::Free: return "Free";
        case LicenseTier::Pro: return "Pro";
        case LicenseTier::Lifetime: return "Lifetime";
        default: return "Unknown";
    }
}

bool SubscriptionManager::canUseCloudAI() const
{
    if (currentTier == LicenseTier::Free)
        return isTrialActive();
    return true;
}

int SubscriptionManager::getMonthlyTokenLimit() const
{
    switch (currentTier)
    {
        case LicenseTier::Free: return 100000;
        case LicenseTier::Pro: return 5000000;
        case LicenseTier::Lifetime: return -1;
        default: return 100000;
    }
}

bool SubscriptionManager::hasFeature (const juce::String& feature) const
{
    if (currentTier == LicenseTier::Lifetime) return true;
    if (currentTier == LicenseTier::Pro) return true;

    if (feature == "local_ai" || feature == "basic_synth" || feature == "presets")
        return true;
    if (feature == "cloud_ai" || feature == "advanced_modulation" || feature == "wavetable_morph")
        return isTrialActive();

    return false;
}

bool SubscriptionManager::isTrialActive() const
{
    if (currentTier != LicenseTier::Free)
        return true;

    if (trialStartTime == 0)
        return false;

    int64_t now = getCurrentTimeMs();
    int64_t trialEndTime = trialStartTime + (int64_t)trialDays * 24 * 60 * 60 * 1000;
    return now < trialEndTime;
}

void SubscriptionManager::startTrial (int days)
{
    if (currentTier == LicenseTier::Free && trialStartTime == 0)
    {
        trialStartTime = getCurrentTimeMs();
        trialDays = days;
        save();
    }
}

int SubscriptionManager::getTrialDaysRemaining() const
{
    if (!isTrialActive())
        return 0;

    int64_t now = getCurrentTimeMs();
    int64_t trialEndTime = trialStartTime + (int64_t)trialDays * 24 * 60 * 60 * 1000;
    int64_t remainingMs = trialEndTime - now;
    return (int)juce::jmax ((int64_t)0, remainingMs / (24 * 60 * 60 * 1000));
}

bool SubscriptionManager::isExpired() const
{
    if (currentTier == LicenseTier::Lifetime)
        return false;

    if (licenseExpiryTime > 0)
    {
        return getCurrentTimeMs() > licenseExpiryTime;
    }

    if (currentTier == LicenseTier::Free)
        return !isTrialActive();

    return false;
}

juce::String SubscriptionManager::getExpirationDate() const
{
    if (currentTier == LicenseTier::Lifetime)
        return "Never";

    if (licenseExpiryTime > 0)
    {
        juce::Time t (licenseExpiryTime);
        return t.toString (true, true, false, false);
    }

    if (currentTier == LicenseTier::Free && isTrialActive())
    {
        int64_t trialEndTime = trialStartTime + (int64_t)trialDays * 24 * 60 * 60 * 1000;
        juce::Time t (trialEndTime);
        return "Trial ends: " + t.toString (true, true, false, false);
    }

    return "N/A";
}

void SubscriptionManager::save()
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty ("tier", (int)currentTier);
    obj->setProperty ("licenseKey", licenseKey);
    obj->setProperty ("userEmail", userEmail);
    obj->setProperty ("trialStartTime", (int64_t)trialStartTime);
    obj->setProperty ("licenseExpiryTime", (int64_t)licenseExpiryTime);
    obj->setProperty ("trialDays", trialDays);

    juce::File file = getDataFile();
    file.getParentDirectory().createDirectory();
    juce::FileOutputStream stream (file);
    if (stream.openedOk())
    {
        juce::JSON::writeToStream (stream, juce::var (obj), false, 0);
    }
}

void SubscriptionManager::load()
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

    currentTier = static_cast<LicenseTier> (static_cast<int> (obj->getProperty ("tier")));
    licenseKey = obj->getProperty ("licenseKey").toString();
    userEmail = obj->getProperty ("userEmail").toString();
    trialStartTime = (int64_t)obj->getProperty ("trialStartTime");
    licenseExpiryTime = (int64_t)obj->getProperty ("licenseExpiryTime");
    trialDays = (int)obj->getProperty ("trialDays");

    if (!licenseKey.isEmpty())
    {
        parseLicenseKey (licenseKey);
    }
}

bool SubscriptionManager::parseLicenseKey (const juce::String& key)
{
    juce::String payload = key.substring (5);
    juce::StringArray parts;
    parts.addTokens (payload, "|", "");

    if (parts.size() < 3)
        return false;

    currentTier = static_cast<LicenseTier> (parts[0].getIntValue());
    userEmail = parts[1];
    licenseExpiryTime = parts[2].getLargeIntValue();

    return true;
}

bool SubscriptionManager::verifyLicenseSignature (const juce::String& key, const juce::String& publicKeyPEM)
{
    return key.startsWith ("ZALX-") && key.length() > 20;
}

juce::String SubscriptionManager::generateLicenseKey (LicenseTier tier, const juce::String& userEmail, int validityDays)
{
    int64_t expiry = getCurrentTimeMs() + (int64_t)validityDays * 24 * 60 * 60 * 1000;
    juce::String payload = juce::String ((int)tier) + "|" + userEmail + "|" + juce::String (expiry);
    juce::String encoded = juce::Base64::toBase64 (payload.toUTF8(), payload.length());
    return "ZALX-" + encoded;
}

bool SubscriptionManager::checkExpiration()
{
    if (isExpired())
    {
        if (currentTier != LicenseTier::Free)
        {
            currentTier = LicenseTier::Free;
            licenseKey = "";
            licenseExpiryTime = 0;
            save();
        }
        return true;
    }
    return false;
}

int64_t SubscriptionManager::getCurrentTimeMs()
{
    return juce::Time::getCurrentTime().toMilliseconds();
}

juce::File SubscriptionManager::getDataFile() const
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("ZalaxyMusic")
        .getChildFile ("license.json");
}

} // namespace zalaxy