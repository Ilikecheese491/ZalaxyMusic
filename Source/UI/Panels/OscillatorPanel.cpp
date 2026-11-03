#include "OscillatorPanel.h"

namespace zalaxy
{

OscillatorPanel::OscillatorPanel (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    addAndMakeVisible (panelTitle);
    panelTitle.setText ("OSCILLATORS", juce::dontSendNotification);
    panelTitle.setFont (theme.getFont ("heading3"));
    panelTitle.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::OscillatorColor));
    panelTitle.setJustificationType (juce::Justification::centredLeft);

    for (int i = 0; i < 4; ++i)
    {
        juce::String prefix = "osc" + juce::String (i + 1);
        createOscillatorControls (i, prefix);

        addAndMakeVisible (oscLabels[i]);
        oscLabels[i].setText ("OSC " + juce::String (i + 1), juce::dontSendNotification);
        oscLabels[i].setFont (theme.getFont ("caption"));
        oscLabels[i].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        oscLabels[i].setJustificationType (juce::Justification::centred);
    }

    // Sub oscillator
    subLevel = std::make_unique<ParameterKnob> ("subOscLevel", theme, parameterManager.getAPVTS());
    subWaveform = std::make_unique<ParameterKnob> ("subOscWaveform", theme, parameterManager.getAPVTS());
    addAndMakeVisible (subLevel.get());
    addAndMakeVisible (subWaveform.get());

    // Noise
    noiseLevel = std::make_unique<ParameterKnob> ("noiseLevel", theme, parameterManager.getAPVTS());
    noiseColor = std::make_unique<ParameterKnob> ("noiseColor", theme, parameterManager.getAPVTS());
    addAndMakeVisible (noiseLevel.get());
    addAndMakeVisible (noiseColor.get());
}

OscillatorPanel::~OscillatorPanel()
{
}

void OscillatorPanel::initialize()
{
    // Any post-construction setup
}

void OscillatorPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Panel background with subtle border
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));

    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);

    // Accent line at top
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::OscillatorColor));
    g.fillRect (juce::Rectangle<float> (bounds.getX() + 8.0f,
                                         bounds.getY() + 2.0f,
                                         bounds.getWidth() - 16.0f,
                                         2.0f));
}

void OscillatorPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));

    int titleHeight = 24;
    panelTitle.setBounds (bounds.removeFromTop (titleHeight));

    int sectionSpacing = theme.getSpace (2);
    int currentY = bounds.getY();

    // Layout 4 oscillators
    for (int i = 0; i < 4; ++i)
    {
        layoutOscillator (i, currentY);
        currentY += ColorScheme::controlHeight + sectionSpacing;
    }

    // Sub & Noise at bottom
    int knobSize = ColorScheme::knobMedium;
    int labelHeight = 16;

    auto subArea = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize + labelHeight * 2);
    subLabels:
    {
        subLevel->setBounds (subArea.removeFromLeft (knobSize + 10).withSizeKeepingCentre (knobSize, knobSize));
        subWaveform->setBounds (subArea.removeFromLeft (knobSize + 10).withSizeKeepingCentre (knobSize, knobSize));
    }

    currentY += knobSize + labelHeight * 2 + sectionSpacing;

    auto noiseArea = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize + labelHeight * 2);
    noiseLevel->setBounds (noiseArea.removeFromLeft (knobSize + 10).withSizeKeepingCentre (knobSize, knobSize));
    noiseColor->setBounds (noiseArea.removeFromLeft (knobSize + 10).withSizeKeepingCentre (knobSize, knobSize));
}

void OscillatorPanel::createOscillatorControls (int index, const juce::String& prefix)
{
    auto& apvts = parameterManager.getAPVTS();
    auto& osc = oscControls[index];

    osc.waveform = std::make_unique<ParameterKnob> (prefix + "Waveform", theme, apvts);
    osc.level = std::make_unique<ParameterKnob> (prefix + "Level", theme, apvts);
    osc.pitch = std::make_unique<ParameterKnob> (prefix + "Pitch", theme, apvts);
    osc.fineTune = std::make_unique<ParameterKnob> (prefix + "FineTune", theme, apvts);
    osc.pulseWidth = std::make_unique<ParameterKnob> (prefix + "PulseWidth", theme, apvts);
    osc.phaseMod = std::make_unique<ParameterKnob> (prefix + "PhaseMod", theme, apvts);

    addAndMakeVisible (osc.waveform.get());
    addAndMakeVisible (osc.level.get());
    addAndMakeVisible (osc.pitch.get());
    addAndMakeVisible (osc.fineTune.get());
    addAndMakeVisible (osc.pulseWidth.get());
    addAndMakeVisible (osc.phaseMod.get());
}

void OscillatorPanel::layoutOscillator (int index, int y)
{
    int knobSize = ColorScheme::knobMedium;
    int spacing = theme.getSpace (1);
    int labelHeight = 16;

    auto& osc = oscControls[index];
    auto area = juce::Rectangle<int> (getLocalBounds().getX() + theme.getSpace (2), y,
                                      getWidth() - theme.getSpace (4), knobSize + labelHeight * 2);

    oscLabels[index].setBounds (area.removeFromTop (labelHeight));

    auto knobRow = area.removeFromTop (knobSize);
    osc.waveform->setBounds (knobRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    osc.level->setBounds (knobRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    osc.pitch->setBounds (knobRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    osc.fineTune->setBounds (knobRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    osc.pulseWidth->setBounds (knobRow.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    osc.phaseMod->setBounds (knobRow.withSizeKeepingCentre (knobSize, knobSize));
}

} // namespace zalaxy