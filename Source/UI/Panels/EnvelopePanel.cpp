#include "EnvelopePanel.h"

namespace zalaxy
{

EnvelopePanel::EnvelopePanel (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    addAndMakeVisible (panelTitle);
    panelTitle.setText ("ENVELOPES", juce::dontSendNotification);
    panelTitle.setFont (theme.getFont ("heading3"));
    panelTitle.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::EnvelopeColor));
    panelTitle.setJustificationType (juce::Justification::centredLeft);

    auto& apvts = parameterManager.getAPVTS();

    // Amp Envelope - vertical sliders for time, knob for velocity
    ampAttack = std::make_unique<ParameterSlider> ("ampAttack", theme, apvts, false);
    ampDecay = std::make_unique<ParameterSlider> ("ampDecay", theme, apvts, false);
    ampSustain = std::make_unique<ParameterSlider> ("ampSustain", theme, apvts, false);
    ampRelease = std::make_unique<ParameterSlider> ("ampRelease", theme, apvts, false);
    ampVelocity = std::make_unique<ParameterKnob> ("ampVelocity", theme, apvts);

    // Filter Envelope
    filterAttack = std::make_unique<ParameterSlider> ("filterAttack", theme, apvts, false);
    filterDecay = std::make_unique<ParameterSlider> ("filterDecay", theme, apvts, false);
    filterSustain = std::make_unique<ParameterSlider> ("filterSustain", theme, apvts, false);
    filterRelease = std::make_unique<ParameterSlider> ("filterRelease", theme, apvts, false);
    filterEnvAmount = std::make_unique<ParameterKnob> ("filterEnvAmount", theme, apvts);
    filterVelocity = std::make_unique<ParameterKnob> ("filterVelocity", theme, apvts);

    // Mod Envelope
    modAttack = std::make_unique<ParameterSlider> ("modAttack", theme, apvts, false);
    modDecay = std::make_unique<ParameterSlider> ("modDecay", theme, apvts, false);
    modSustain = std::make_unique<ParameterSlider> ("modSustain", theme, apvts, false);
    modRelease = std::make_unique<ParameterSlider> ("modRelease", theme, apvts, false);
    modAmount = std::make_unique<ParameterKnob> ("modAmount", theme, apvts);

    // Add all to component
    addAndMakeVisible (ampAttack.get());
    addAndMakeVisible (ampDecay.get());
    addAndMakeVisible (ampSustain.get());
    addAndMakeVisible (ampRelease.get());
    addAndMakeVisible (ampVelocity.get());
    addAndMakeVisible (filterAttack.get());
    addAndMakeVisible (filterDecay.get());
    addAndMakeVisible (filterSustain.get());
    addAndMakeVisible (filterRelease.get());
    addAndMakeVisible (filterEnvAmount.get());
    addAndMakeVisible (filterVelocity.get());
    addAndMakeVisible (modAttack.get());
    addAndMakeVisible (modDecay.get());
    addAndMakeVisible (modSustain.get());
    addAndMakeVisible (modRelease.get());
    addAndMakeVisible (modAmount.get());

    addAndMakeVisible (envLabels[0]);
    envLabels[0].setText ("AMP", juce::dontSendNotification);
    envLabels[0].setFont (theme.getFont ("caption"));
    envLabels[0].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    envLabels[0].setJustificationType (juce::Justification::centred);

    addAndMakeVisible (envLabels[1]);
    envLabels[1].setText ("FILTER", juce::dontSendNotification);
    envLabels[1].setFont (theme.getFont ("caption"));
    envLabels[1].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    envLabels[1].setJustificationType (juce::Justification::centred);

    addAndMakeVisible (envLabels[2]);
    envLabels[2].setText ("MOD", juce::dontSendNotification);
    envLabels[2].setFont (theme.getFont ("caption"));
    envLabels[2].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    envLabels[2].setJustificationType (juce::Justification::centred);
}

EnvelopePanel::~EnvelopePanel() {}

void EnvelopePanel::initialize() {}

void EnvelopePanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::EnvelopeColor));
    g.fillRect (bounds.getX() + 8, bounds.getY() + 2, bounds.getWidth() - 16, 2);
}

void EnvelopePanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    panelTitle.setBounds (bounds.removeFromTop (titleHeight));

    int sliderWidth = 30;
    int sliderHeight = 120;
    int knobSize = ColorScheme::knobSmall;
    int spacing = theme.getSpace (1);
    int labelHeight = 16;
    int currentY = bounds.getY();
    int sectionWidth = (bounds.getWidth() - spacing * 2) / 3;

    // Amp Envelope
    envLabels[0].setBounds (bounds.getX(), currentY, sectionWidth, labelHeight);
    currentY += labelHeight;

    int x = bounds.getX();
    ampAttack->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    ampDecay->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    ampSustain->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    ampRelease->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    ampVelocity->setBounds (x, currentY + sliderHeight - knobSize, knobSize, knobSize);

    currentY += sliderHeight + theme.getSpace (2);

    // Filter Envelope
    envLabels[1].setBounds (bounds.getX(), currentY, sectionWidth, labelHeight);
    currentY += labelHeight;

    x = bounds.getX();
    filterAttack->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    filterDecay->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    filterSustain->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    filterRelease->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    filterEnvAmount->setBounds (x, currentY, knobSize, knobSize);
    x += knobSize + spacing;
    filterVelocity->setBounds (x, currentY, knobSize, knobSize);

    currentY += sliderHeight + theme.getSpace (2);

    // Mod Envelope
    envLabels[2].setBounds (bounds.getX(), currentY, sectionWidth, labelHeight);
    currentY += labelHeight;

    x = bounds.getX();
    modAttack->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    modDecay->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    modSustain->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    modRelease->setBounds (x, currentY, sliderWidth, sliderHeight);
    x += sliderWidth + spacing;
    modAmount->setBounds (x, currentY, knobSize, knobSize);
}

} // namespace zalaxy