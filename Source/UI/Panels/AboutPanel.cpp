#include "AboutPanel.h"
#include "Core/Licensing/SubscriptionManager.h"

namespace zalaxy
{

AboutPanel::AboutPanel()
{
    // Title
    titleLabel.setText ("ZalaxyMusic", juce::dontSendNotification);
    titleLabel.setFont (theme.getFont ("heading1"));
    titleLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    titleLabel.setJustificationType (juce::Justification::centred);

    // Version
    versionLabel.setText ("Version 0.1.0", juce::dontSendNotification);
    versionLabel.setFont (theme.getFont ("body"));
    versionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
    versionLabel.setJustificationType (juce::Justification::centred);

    // Company
    companyLabel.setText ("ZalaxyMusic — AI-Powered Synthesizer", juce::dontSendNotification);
    companyLabel.setFont (theme.getFont ("bodySmall"));
    companyLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    companyLabel.setJustificationType (juce::Justification::centred);

    // Website
    websiteLabel.setText ("https://zalaxymusic.com", juce::dontSendNotification);
    websiteLabel.setFont (theme.getFont ("bodySmall"));
    websiteLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    websiteLabel.setJustificationType (juce::Justification::centred);

    // Description
    descriptionLabel.setText ("A free-first, AI-powered VST3 synthesizer.\nText-to-sound generation • WAV reference analysis • Production assistant",
                              juce::dontSendNotification);
    descriptionLabel.setFont (theme.getFont ("body"));
    descriptionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
    descriptionLabel.setJustificationType (juce::Justification::centred);

    // Buttons
    eulaButton.setButtonText ("View EULA");
    eulaButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    eulaButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    eulaButton.addListener (this);

    privacyButton.setButtonText ("Privacy Policy");
    privacyButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    privacyButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    privacyButton.addListener (this);

    websiteButton.setButtonText ("Visit Website");
    websiteButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    websiteButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
    websiteButton.addListener (this);

    licenseButton.setButtonText ("Manage License");
    licenseButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    licenseButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    licenseButton.addListener (this);

    // License status
    licenseStatusLabel.setFont (theme.getFont ("caption"));
    licenseStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    licenseStatusLabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible (titleLabel);
    addAndMakeVisible (versionLabel);
    addAndMakeVisible (companyLabel);
    addAndMakeVisible (websiteLabel);
    addAndMakeVisible (descriptionLabel);
    addAndMakeVisible (eulaButton);
    addAndMakeVisible (privacyButton);
    addAndMakeVisible (websiteButton);
    addAndMakeVisible (licenseButton);
    addAndMakeVisible (licenseStatusLabel);

    updateLicenseStatus();
}

AboutPanel::~AboutPanel()
{
    eulaButton.removeListener (this);
    privacyButton.removeListener (this);
    websiteButton.removeListener (this);
    licenseButton.removeListener (this);
}

void AboutPanel::initialize()
{
    updateLicenseStatus();
}

void AboutPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    g.fillRect (bounds.getX() + 8, bounds.getY() + 2, bounds.getWidth() - 16, 2);
}

void AboutPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (3));
    int spacing = theme.getSpace (2);

    // Title area
    titleLabel.setBounds (bounds.removeFromTop (40));
    versionLabel.setBounds (bounds.removeFromTop (24));
    companyLabel.setBounds (bounds.removeFromTop (20));
    websiteLabel.setBounds (bounds.removeFromTop (20));
    bounds.removeFromTop (spacing);

    // Description
    descriptionLabel.setBounds (bounds.removeFromTop (60));
    bounds.removeFromTop (spacing);

    // Buttons row
    auto buttonRow = bounds.removeFromTop (36);
    int btnWidth = (buttonRow.getWidth() - spacing * 3) / 4;
    eulaButton.setBounds (buttonRow.removeFromLeft (btnWidth + spacing));
    privacyButton.setBounds (buttonRow.removeFromLeft (btnWidth + spacing));
    websiteButton.setBounds (buttonRow.removeFromLeft (btnWidth + spacing));
    licenseButton.setBounds (buttonRow);

    bounds.removeFromTop (spacing);

    // License status at bottom
    licenseStatusLabel.setBounds (bounds.removeFromBottom (24));
}

void AboutPanel::buttonClicked (juce::Button* button)
{
    if (button == &eulaButton)
    {
        openURL ("https://zalaxymusic.com/eula");
    }
    else if (button == &privacyButton)
    {
        openURL ("https://zalaxymusic.com/privacy");
    }
    else if (button == &websiteButton)
    {
        openURL ("https://zalaxymusic.com");
    }
    else if (button == &licenseButton)
    {
        showLicenseDialog();
    }
}

void AboutPanel::openURL (const juce::String& url)
{
    juce::URL u (url);
    u.launchInDefaultBrowser();
}

void AboutPanel::showLicenseDialog()
{
    // This will be implemented with SubscriptionManager
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon,
                                             "License Management",
                                             "License management dialog coming soon.\n\nCurrent status: " + licenseStatusLabel.getText(),
                                             "OK");
}

void AboutPanel::updateLicenseStatus()
{
    // Will be connected to SubscriptionManager
    licenseStatusLabel.setText ("License: Free Tier (AI features limited)", juce::dontSendNotification);
}

} // namespace zalaxy