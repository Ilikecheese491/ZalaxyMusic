#pragma once

#include "JuceModules.h"

namespace zalaxy
{

class SubscriptionManager
{
public:
    enum class LicenseTier
    {
        Free = 0,
        Pro = 1,
        Lifetime = 2
    };

    SubscriptionManager();
    ~SubscriptionManager();

    void initialize();

    // License validation
    bool validateLicenseKey (const juce::String& key);
    void applyLicenseKey (const juce::String& key);
    void clearLicense();

    LicenseTier getCurrentTier() const { return currentTier; }
    juce::String getCurrentTierName() const;
    juce::String getLicenseKey() const { return licenseKey; }
    bool isLicenseValid() const { return currentTier != LicenseTier::Free || isTrialActive(); }

    // Feature gates
    bool canUseCloudAI() const;
    bool canUseLocalAI() const { return true; } // Always available
    int getMonthlyTokenLimit() const;
    bool hasFeature (const juce::String& feature) const;

    // Trial
    bool isTrialActive() const;
    void startTrial (int days = 14);
    int getTrialDaysRemaining() const;

    // Expiration
    bool isExpired() const;
    juce::String getExpirationDate() const;

    // Persistence
    void save();
    void load();

    // License key format: ZALX-XXXX-XXXX-XXXX (base64 encoded signed data)
    static juce::String generateLicenseKey (LicenseTier tier, const juce::String& userEmail, int validityDays);
    static bool verifyLicenseSignature (const juce::String& key, const juce::String& publicKey);

private:
    LicenseTier currentTier = LicenseTier::Free;
    juce::String licenseKey;
    juce::String userEmail;
    int64_t trialStartTime = 0;
    int64_t licenseExpiryTime = 0;
    int trialDays = 14;

    // RSA public key for license verification (embedded)
    static constexpr const char* PUBLIC_KEY_PEM =
        "-----BEGIN PUBLIC KEY-----\n"
        "YOUR_RSA_PUBLIC_KEY_HERE\n"
        "-----END PUBLIC KEY-----";

    juce::File getDataFile() const;
    bool parseLicenseKey (const juce::String& key);
    bool checkExpiration();
    int64_t getCurrentTimeMs() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubscriptionManager)
};

} // namespace zalaxy