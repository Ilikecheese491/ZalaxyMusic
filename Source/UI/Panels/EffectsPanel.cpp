#include "EffectsPanel.h"

namespace zalaxy
{

EffectsPanel::EffectsPanel (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    addAndMakeVisible (panelTitle);
    panelTitle.setText ("EFFECTS", juce::dontSendNotification);
    panelTitle.setFont (theme.getFont ("heading3"));
    panelTitle.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::EffectsColor));
    panelTitle.setJustificationType (juce::Justification::centredLeft);

    auto& apvts = parameterManager.getAPVTS();

    // Reverb
    reverbEnabled = std::make_unique<ParameterKnob> ("reverbEnabled", theme, apvts);
    reverbRoomSize = std::make_unique<ParameterKnob> ("reverbRoomSize", theme, apvts);
    reverbDamping = std::make_unique<ParameterKnob> ("reverbDamping", theme, apvts);
    reverbWetLevel = std::make_unique<ParameterKnob> ("reverbWetLevel", theme, apvts);
    reverbDryLevel = std::make_unique<ParameterKnob> ("reverbDryLevel", theme, apvts);
    reverbWidth = std::make_unique<ParameterKnob> ("reverbWidth", theme, apvts);
    reverbPreDelay = std::make_unique<ParameterKnob> ("reverbPreDelay", theme, apvts);

    // Delay
    delayEnabled = std::make_unique<ParameterKnob> ("delayEnabled", theme, apvts);
    delayTime = std::make_unique<ParameterKnob> ("delayTime", theme, apvts);
    delayFeedback = std::make_unique<ParameterKnob> ("delayFeedback", theme, apvts);
    delayWetLevel = std::make_unique<ParameterKnob> ("delayWetLevel", theme, apvts);
    delayTempoSync = std::make_unique<ParameterKnob> ("delayTempoSync", theme, apvts);
    delayNoteValue = std::make_unique<ParameterKnob> ("delayNoteValue", theme, apvts);

    // Distortion
    distortionEnabled = std::make_unique<ParameterKnob> ("distortionEnabled", theme, apvts);
    distortionType = std::make_unique<ParameterKnob> ("distortionType", theme, apvts);
    distortionDrive = std::make_unique<ParameterKnob> ("distortionDrive", theme, apvts);
    distortionMix = std::make_unique<ParameterKnob> ("distortionMix", theme, apvts);
    distortionTone = std::make_unique<ParameterKnob> ("distortionTone", theme, apvts);

    // Chorus
    chorusEnabled = std::make_unique<ParameterKnob> ("chorusEnabled", theme, apvts);
    chorusRate = std::make_unique<ParameterKnob> ("chorusRate", theme, apvts);
    chorusDepth = std::make_unique<ParameterKnob> ("chorusDepth", theme, apvts);
    chorusFeedback = std::make_unique<ParameterKnob> ("chorusFeedback", theme, apvts);
    chorusMix = std::make_unique<ParameterKnob> ("chorusMix", theme, apvts);

    // Master
    masterVolume = std::make_unique<ParameterKnob> ("masterVolume", theme, apvts);
    masterPan = std::make_unique<ParameterKnob> ("masterPan", theme, apvts);

    // Add all
    auto addAll = [this] (auto&... knobs) {
        (addAndMakeVisible (knobs.get()), ...);
    };

    addAll (reverbEnabled, reverbRoomSize, reverbDamping, reverbWetLevel, reverbDryLevel, reverbWidth, reverbPreDelay);
    addAll (delayEnabled, delayTime, delayFeedback, delayWetLevel, delayTempoSync, delayNoteValue);
    addAll (distortionEnabled, distortionType, distortionDrive, distortionMix, distortionTone);
    addAll (chorusEnabled, chorusRate, chorusDepth, chorusFeedback, chorusMix);
    addAll (masterVolume, masterPan);

    // Labels
    fxLabels[0].setText ("REVERB", juce::dontSendNotification);
    fxLabels[1].setText ("DELAY", juce::dontSendNotification);
    fxLabels[2].setText ("DISTORTION", juce::dontSendNotification);
    fxLabels[3].setText ("CHORUS", juce::dontSendNotification);
    fxLabels[4].setText ("MASTER", juce::dontSendNotification);

    for (int i = 0; i < 5; ++i)
    {
        addAndMakeVisible (fxLabels[i]);
        fxLabels[i].setFont (theme.getFont ("caption"));
        fxLabels[i].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        fxLabels[i].setJustificationType (juce::Justification::centred);
    }
}

EffectsPanel::~EffectsPanel() {}

void EffectsPanel::initialize() {}

void EffectsPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::EffectsColor));
    g.fillRect (bounds.getX() + 8, bounds.getY() + 2, bounds.getWidth() - 16, 2);
}

void EffectsPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    panelTitle.setBounds (bounds.removeFromTop (titleHeight));

    int knobSize = ColorScheme::knobMedium;
    int smallKnob = ColorScheme::knobSmall;
    int spacing = theme.getSpace (1);
    int labelHeight = 16;
    int currentY = bounds.getY();

    // Reverb (7 knobs)
    fxLabels[0].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto revRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    reverbEnabled->setBounds (revRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    reverbRoomSize->setBounds (revRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    reverbDamping->setBounds (revRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    reverbWetLevel->setBounds (revRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    reverbDryLevel->setBounds (revRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    reverbWidth->setBounds (revRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    reverbPreDelay->setBounds (revRow.withSizeKeepingCentre (knobSize, knobSize));

    currentY += knobSize + theme.getSpace (1);

    // Delay (6 knobs)
    fxLabels[1].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto delRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    delayEnabled->setBounds (delRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    delayTime->setBounds (delRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    delayFeedback->setBounds (delRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    delayWetLevel->setBounds (delRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    delayTempoSync->setBounds (delRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    delayNoteValue->setBounds (delRow.withSizeKeepingCentre (smallKnob, smallKnob));

    currentY += knobSize + theme.getSpace (1);

    // Distortion (5 knobs)
    fxLabels[2].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto distRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    distortionEnabled->setBounds (distRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    distortionType->setBounds (distRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    distortionDrive->setBounds (distRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    distortionMix->setBounds (distRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    distortionTone->setBounds (distRow.withSizeKeepingCentre (knobSize, knobSize));

    currentY += knobSize + theme.getSpace (1);

    // Chorus (5 knobs)
    fxLabels[3].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto chorRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    chorusEnabled->setBounds (chorRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    chorusRate->setBounds (chorRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    chorusDepth->setBounds (chorRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    chorusFeedback->setBounds (chorRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    chorusMix->setBounds (chorRow.withSizeKeepingCentre (knobSize, knobSize));

    currentY += knobSize + theme.getSpace (1);

    // Master (2 knobs)
    fxLabels[4].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto masterRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    masterVolume->setBounds (masterRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    masterPan->setBounds (masterRow.withSizeKeepingCentre (knobSize, knobSize));
}

} // namespace zalaxy