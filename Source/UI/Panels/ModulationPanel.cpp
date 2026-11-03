#include "ModulationPanel.h"

namespace zalaxy
{

ModulationPanel::ModulationPanel (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    addAndMakeVisible (panelTitle);
    panelTitle.setText ("MODULATION", juce::dontSendNotification);
    panelTitle.setFont (theme.getFont ("heading3"));
    panelTitle.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::LFOColor));
    panelTitle.setJustificationType (juce::Justification::centredLeft);

    auto& apvts = parameterManager.getAPVTS();

    lfo1Waveform = std::make_unique<ParameterKnob> ("lfo1Waveform", theme, apvts);
    lfo1Rate = std::make_unique<ParameterKnob> ("lfo1Rate", theme, apvts);
    lfo1Depth = std::make_unique<ParameterKnob> ("lfo1Depth", theme, apvts);
    lfo1Delay = std::make_unique<ParameterKnob> ("lfo1Delay", theme, apvts);
    lfo1FadeIn = std::make_unique<ParameterKnob> ("lfo1FadeIn", theme, apvts);
    lfo1KeySync = std::make_unique<ParameterKnob> ("lfo1KeySync", theme, apvts);
    lfo1TempoSync = std::make_unique<ParameterKnob> ("lfo1TempoSync", theme, apvts);

    lfo2Waveform = std::make_unique<ParameterKnob> ("lfo2Waveform", theme, apvts);
    lfo2Rate = std::make_unique<ParameterKnob> ("lfo2Rate", theme, apvts);
    lfo2Depth = std::make_unique<ParameterKnob> ("lfo2Depth", theme, apvts);
    lfo2Delay = std::make_unique<ParameterKnob> ("lfo2Delay", theme, apvts);
    lfo2FadeIn = std::make_unique<ParameterKnob> ("lfo2FadeIn", theme, apvts);
    lfo2KeySync = std::make_unique<ParameterKnob> ("lfo2KeySync", theme, apvts);
    lfo2TempoSync = std::make_unique<ParameterKnob> ("lfo2TempoSync", theme, apvts);

    for (int i = 0; i < 8; ++i)
    {
        juce::String prefix = "modSlot" + juce::String (i + 1);
        modSlots[i].source = std::make_unique<ParameterKnob> (prefix + "Source", theme, apvts);
        modSlots[i].dest = std::make_unique<ParameterKnob> (prefix + "Dest", theme, apvts);
        modSlots[i].amount = std::make_unique<ParameterKnob> (prefix + "Amount", theme, apvts);
        addAndMakeVisible (modSlots[i].source.get());
        addAndMakeVisible (modSlots[i].dest.get());
        addAndMakeVisible (modSlots[i].amount.get());
    }

    addAndMakeVisible (lfo1Waveform.get());
    addAndMakeVisible (lfo1Rate.get());
    addAndMakeVisible (lfo1Depth.get());
    addAndMakeVisible (lfo1Delay.get());
    addAndMakeVisible (lfo1FadeIn.get());
    addAndMakeVisible (lfo1KeySync.get());
    addAndMakeVisible (lfo1TempoSync.get());

    addAndMakeVisible (lfo2Waveform.get());
    addAndMakeVisible (lfo2Rate.get());
    addAndMakeVisible (lfo2Depth.get());
    addAndMakeVisible (lfo2Delay.get());
    addAndMakeVisible (lfo2FadeIn.get());
    addAndMakeVisible (lfo2KeySync.get());
    addAndMakeVisible (lfo2TempoSync.get());

    addAndMakeVisible (lfoLabels[0]);
    lfoLabels[0].setText ("LFO 1", juce::dontSendNotification);
    lfoLabels[0].setFont (theme.getFont ("caption"));
    lfoLabels[0].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    lfoLabels[0].setJustificationType (juce::Justification::centred);

    addAndMakeVisible (lfoLabels[1]);
    lfoLabels[1].setText ("LFO 2", juce::dontSendNotification);
    lfoLabels[1].setFont (theme.getFont ("caption"));
    lfoLabels[1].setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    lfoLabels[1].setJustificationType (juce::Justification::centred);

    addAndMakeVisible (modMatrixLabel);
    modMatrixLabel.setText ("MOD MATRIX", juce::dontSendNotification);
    modMatrixLabel.setFont (theme.getFont ("caption"));
    modMatrixLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    modMatrixLabel.setJustificationType (juce::Justification::centredLeft);
}

ModulationPanel::~ModulationPanel() {}

void ModulationPanel::initialize() {}

void ModulationPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::LFOColor));
    g.fillRect (juce::Rectangle<float> (bounds.getX() + 8.0f,
                                    bounds.getY() + 2.0f,
                                    bounds.getWidth() - 16.0f,
                                    2.0f));
}

void ModulationPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    panelTitle.setBounds (bounds.removeFromTop (titleHeight));

    int knobSize = ColorScheme::knobMedium;
    int smallKnob = ColorScheme::knobSmall;
    int spacing = theme.getSpace (1);
    int labelHeight = 16;
    int currentY = bounds.getY();

    lfoLabels[0].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto lfo1Row = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    lfo1Waveform->setBounds (lfo1Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo1Rate->setBounds (lfo1Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo1Depth->setBounds (lfo1Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo1Delay->setBounds (lfo1Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo1FadeIn->setBounds (lfo1Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo1KeySync->setBounds (lfo1Row.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    lfo1TempoSync->setBounds (lfo1Row.withSizeKeepingCentre (smallKnob, smallKnob));

    currentY += knobSize + theme.getSpace (1);

    lfoLabels[1].setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    auto lfo2Row = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), knobSize);
    lfo2Waveform->setBounds (lfo2Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo2Rate->setBounds (lfo2Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo2Depth->setBounds (lfo2Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo2Delay->setBounds (lfo2Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo2FadeIn->setBounds (lfo2Row.removeFromLeft (knobSize + spacing).withSizeKeepingCentre (knobSize, knobSize));
    lfo2KeySync->setBounds (lfo2Row.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
    lfo2TempoSync->setBounds (lfo2Row.withSizeKeepingCentre (smallKnob, smallKnob));

    currentY += knobSize + theme.getSpace (2);

    modMatrixLabel.setBounds (bounds.getX(), currentY, bounds.getWidth(), labelHeight);
    currentY += labelHeight;

    int slotHeight = smallKnob + 4;
    for (int i = 0; i < 8; ++i)
    {
        auto slotRow = juce::Rectangle<int> (bounds.getX(), currentY, bounds.getWidth(), slotHeight);
        modSlots[i].source->setBounds (slotRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
        modSlots[i].dest->setBounds (slotRow.removeFromLeft (smallKnob + spacing).withSizeKeepingCentre (smallKnob, smallKnob));
        modSlots[i].amount->setBounds (slotRow.withSizeKeepingCentre (smallKnob, smallKnob));
        currentY += slotHeight;
    }
}

} // namespace zalaxy