#include "FilterPanel.h"

namespace zalaxy
{

FilterPanel::FilterPanel (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    addAndMakeVisible (panelTitle);
    panelTitle.setText ("FILTERS", juce::dontSendNotification);
    panelTitle.setFont (theme.getFont ("heading3"));
    panelTitle.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::FilterColor));
    panelTitle.setJustificationType (juce::Justification::centredLeft);

    auto& apvts = parameterManager.getAPVTS();

    // Filter 1
    filter1Type = std::make_unique<ParameterKnob> ("filter1Type", theme, apvts);
    filter1Cutoff = std::make_unique<ParameterKnob> ("filter1Cutoff", theme, apvts);
    filter1Resonance = std::make_unique<ParameterKnob> ("filter1Resonance", theme, apvts);
    filter1Drive = std::make_unique<ParameterKnob> ("filter1Drive", theme, apvts);
    filter1Keytrack = std::make_unique<ParameterKnob> ("filter1Keytrack", theme, apvts);

    // Filter 2
    filter2Type = std::make_unique<ParameterKnob> ("filter2Type", theme, apvts);
    filter2Cutoff = std::make_unique<ParameterKnob> ("filter2Cutoff", theme, apvts);
    filter2Resonance = std::make_unique<ParameterKnob> ("filter2Resonance", theme, apvts);
    filter2Drive = std::make_unique<ParameterKnob> ("filter2Drive", theme, apvts);
    filter2Keytrack = std::make_unique<ParameterKnob> ("filter2Keytrack", theme, apvts);

    // Routing
    filterRouting = std::make_unique<ParameterKnob> ("filterRouting", theme, apvts);
    filterBlend = std::make_unique<ParameterKnob> ("filterBlend", theme, apvts);

    addAndMakeVisible (filter1Type.get());
    addAndMakeVisible (filter1Cutoff.get());
    addAndMakeVisible (filter1Resonance.get());
    addAndMakeVisible (filter1Drive.get());
    addAndMakeVisible (filter1Keytrack.get());
    addAndMakeVisible (filter2Type.get());
    addAndMakeVisible (filter2Cutoff.get());
    addAndMakeVisible (filter2Resonance.get());
    addAndMakeVisible (filter2Drive.get());
    addAndMakeVisible (filter2Keytrack.get());
    addAndMakeVisible (filterRouting.get());
    addAndMakeVisible (filterBlend.get());

    addAndMakeVisible (filterLabels[0]);
    filterLabels[0].setText ("FILTER 1", juce::dontSendNotification);
    filterLabels[0].setFont (theme.getFont ("caption"));
    filterLabels[0].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    filterLabels[0].setJustificationType (juce::Justification::centred);

    addAndMakeVisible (filterLabels[1]);
    filterLabels[1].setText ("FILTER 2", juce::dontSendNotification);
    filterLabels[1].setFont (theme.getFont ("caption"));
    filterLabels[1].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    filterLabels[1].setJustificationType (juce::Justification::centred);
}

FilterPanel::~FilterPanel()
{
}

void FilterPanel::initialize() {}

void FilterPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::FilterColor));
    g.fillRect (juce::Rectangle<float> (bounds.getX() + 8.0f,
                                    bounds.getY() + 2.0f,
                                    bounds.getWidth() - 16.0f,
                                    2.0f));
}

void FilterPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    panelTitle.setBounds (bounds.removeFromTop (titleHeight));

    int knobSize = ColorScheme::knobMedium;
    int spacing = theme.getSpace (1);
    int labelHeight = 16;
    int currentY = bounds.getY();

    // Filter 1
    filterLabels[0].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto row1 = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    filter1Type->setBounds (row1.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter1Cutoff->setBounds (row1.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter1Resonance->setBounds (row1.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter1Drive->setBounds (row1.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter1Keytrack->setBounds (row1.withSizeKeepingCentre (knobSize, knobSize));

    currentY += knobSize + theme.getSpace (2);

    // Filter 2
    filterLabels[1].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto row2 = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    filter2Type->setBounds (row2.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter2Cutoff->setBounds (row2.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter2Resonance->setBounds (row2.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter2Drive->setBounds (row2.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filter2Keytrack->setBounds (row2.withSizeKeepingCentre (knobSize, knobSize));

    currentY += knobSize + theme.getSpace (2);

    // Routing
    auto routeRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    filterRouting->setBounds (routeRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    filterBlend->setBounds (routeRow.withSizeKeepingCentre (knobSize, knobSize));
}

} // namespace zalaxy