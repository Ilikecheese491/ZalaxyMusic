#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"

namespace zalaxy
{

class AboutPanel : public juce::Component,
                   public juce::Button::Listener
{
public:
    AboutPanel();
    ~AboutPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;

private:
    ZalaxyTheme theme;

    juce::Label titleLabel;
    juce::Label versionLabel;
    juce::Label companyLabel;
    juce::Label websiteLabel;
    juce::Label descriptionLabel;

    juce::TextButton eulaButton;
    juce::TextButton privacyButton;
    juce::TextButton websiteButton;
    juce::TextButton licenseButton;

    juce::Label licenseStatusLabel;

    void openURL (const juce::String& url);
    void showLicenseDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutPanel)
};

} // namespace zalaxy