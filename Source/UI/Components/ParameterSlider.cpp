#include "ParameterSlider.h"

namespace zalaxy
{

ParameterSlider::ParameterSlider (const juce::String& pid, ZalaxyTheme& t, juce::AudioProcessorValueTreeState& apvts, bool horizontal)
    : Slider (horizontal ? Slider::SliderStyle::LinearHorizontal : Slider::SliderStyle::LinearVertical,
              Slider::TextEntryBoxPosition::TextBoxBelow)
    , paramID (pid)
    , theme (t)
    , isHorizontal (horizontal)
{
    setTextBoxStyle (Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 18);
    setRange (0.0, 1.0, 0.001);
    setVelocityModeParameters (2.0, 1, false, false);

    if (horizontal)
        setSize (120, 60);
    else
        setSize (30, 120);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, *this);
    tooltip = paramID;
    updateColors();
}

ParameterSlider::~ParameterSlider()
{
}

void ParameterSlider::updateColors()
{
    trackColor = theme.getColor (ZalaxyTheme::ColorId::BorderSubtle);
    fillColor = theme.getParameterColor (paramID);
    thumbColor = theme.getColor (ZalaxyTheme::ColorId::TextPrimary);
}

void ParameterSlider::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float textBoxHeight = 20;
    float trackHeight = bounds.getHeight() - textBoxHeight - 8;

    if (isHorizontal)
    {
        float trackWidth = bounds.getWidth();
        float trackX = bounds.getX();
        float trackY = bounds.getCentreY();

        // Track background
        g.setColour (trackColor);
        g.fillRoundedRectangle (trackX + 4, trackY - 3, trackWidth - 8, 6, 3.0f);

        // Fill
        float normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        float fillWidth = (trackWidth - 8) * normalizedValue;
        g.setColour (fillColor);
        g.fillRoundedRectangle (trackX + 4, trackY - 3, fillWidth, 6, 3.0f);

        // Thumb
        float thumbX = trackX + 4 + fillWidth;
        g.setColour (isHovering ? fillColor.brighter() : fillColor);
        g.fillEllipse (thumbX - 6, trackY - 6, 12, 12);

        // Center line on thumb
        g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        g.drawLine (thumbX - 2, trackY, thumbX + 2, trackY, 2.0f);
    }
    else
    {
        float trackX = bounds.getCentreX();
        float trackY = bounds.getY() + 4;
        float trackHeight = bounds.getHeight() - textBoxHeight - 12;

        // Track background
        g.setColour (trackColor);
        g.fillRoundedRectangle (trackX - 3, trackY, 6, trackHeight, 3.0f);

        // Fill (from bottom)
        float normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        float fillHeight = trackHeight * normalizedValue;
        g.setColour (fillColor);
        g.fillRoundedRectangle (trackX - 3, trackY + trackHeight - fillHeight, 6, fillHeight, 3.0f);

        // Thumb
        float thumbY = trackY + trackHeight - fillHeight;
        g.setColour (isHovering ? fillColor.brighter() : fillColor);
        g.fillEllipse (trackX - 6, thumbY - 6, 12, 12);

        // Center line
        g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        g.drawLine (trackX, thumbY - 2, trackX, thumbY + 2, 2.0f);
    }
}

void ParameterSlider::resized()
{
    int minSize = ColorScheme::minHitSize;
    if (isHorizontal)
    {
        if (getWidth() < minSize * 2) setSize (minSize * 2, getHeight());
    }
    else
    {
        if (getHeight() < minSize * 2) setSize (getWidth(), minSize * 2);
    }
}

void ParameterSlider::mouseEnter (const juce::MouseEvent&)
{
    isHovering = true;
    repaint();
}

void ParameterSlider::mouseExit (const juce::MouseEvent&)
{
    isHovering = false;
    repaint();
}

void ParameterSlider::setParameterID (const juce::String& newParamID)
{
    paramID = newParamID;
    attachment.reset();
    updateColors();
    repaint();
}

} // namespace zalaxy