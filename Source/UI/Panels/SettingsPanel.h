#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "Core/AI/CloudAIProvider.h"
#include "Core/Licensing/SubscriptionManager.h"

namespace zalaxy
{

class SettingsPanel : public juce::Component,
                      public juce::Button::Listener,
                      public juce::TextEditor::Listener,
                      public juce::ComboBox::Listener
{
public:
    SettingsPanel (CloudAIProvider& aiProvider, SubscriptionManager& subManager);
    ~SettingsPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

    void buttonClicked (juce::Button* button) override;
    void textEditorTextChanged (juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed (juce::TextEditor& editor) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;

private:
    CloudAIProvider& aiProvider;
    SubscriptionManager& subscriptionManager;
    ZalaxyTheme theme;

    // AI Provider Section
    juce::Label aiSectionLabel;
    juce::ComboBox providerSelector;
    juce::Label apiKeyLabel;
    juce::TextEditor apiKeyEditor;
    juce::TextButton saveKeyButton;
    juce::TextButton testKeyButton;
    juce::Label keyStatusLabel;

    // Cloud/Local Toggle
    juce::Label modeLabel;
    juce::ToggleButton cloudModeToggle;
    juce::Label modeDescriptionLabel;

    // Token Usage Section
    juce::Label usageSectionLabel;
    juce::Label tokensUsedLabel;
    juce::Label tokensRemainingLabel;
    double usageProgress = 0.0;
    juce::ProgressBar usageProgressBar;
    juce::Label limitWarningLabel;

    // Subscription Section
    juce::Label subSectionLabel;
    juce::Label subStatusLabel;
    juce::TextButton enterLicenseButton;
    juce::TextButton buyLicenseButton;

    // Setup Guide
    juce::TextButton setupGuideButton;

    void createAISection();
    void createModeSection();
    void createUsageSection();
    void createSubscriptionSection();
    void createGuideSection();
    void updateUI();
    void saveAPIKey();
    void testAPIKey();
    void showSetupGuide();
    void showLicenseDialog();
    void openOpenRouterSignup();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPanel)
};

} // namespace zalaxy