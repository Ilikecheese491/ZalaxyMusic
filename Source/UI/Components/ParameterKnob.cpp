#include "ParameterKnob.h"
#include "Core/Parameters/ParameterSchema.h"

namespace zalaxy
{

ParameterKnob::ParameterKnob (const juce::String& pid, ZalaxyTheme& t, juce::AudioProcessorValueTreeState& apvts)
    : Slider (Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow)
    , paramID (pid)
    , theme (t)
{
    setTextBoxStyle (Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 18);
    setRange (0.0, 1.0, 0.001);
    setVelocityModeParameters (2.0, 1, false, false);
    setScrollWheelEnabled (true);

    // Ensure minimum hit area
    setSize (theme.getRadius("small") * 2 + ColorScheme::knobMedium, ColorScheme::knobMedium + 30);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, *this);

    // Use the parameter's schema default as its double-click reset value.
    const auto* parameterInfo = ParameterSchema::getParameterInfo (paramID);
    setDoubleClickReturnValue (true, parameterInfo != nullptr ? parameterInfo->defaultValue : getValue());

    // Tooltip from parameter name (provided by JUCE's SettableTooltipClient).
    setTooltip (paramID);

    updateColors();
}

ParameterKnob::~ParameterKnob()
{
}

void ParameterKnob::updateColors()
{
    trackColor = theme.getColor (ZalaxyTheme::ColorId::BorderSubtle);
    valueColor = theme.getParameterColor (paramID);
    thumbColor = theme.getColor (ZalaxyTheme::ColorId::TextPrimary);
}

void ParameterKnob::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float diameter = juce::jmin (bounds.getWidth(), bounds.getHeight() - 22); // Space for text box
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY() - 11;

    float normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());

    bool hover = isHovering || isMouseOverOrDragging();
    bool down = isDragging || isMouseButtonDown();

    drawKnob (g, cx - diameter * 0.5f, cy - diameter * 0.5f, diameter, normalizedValue, hover, down);
}

void ParameterKnob::drawKnob (juce::Graphics& g, float x, float y, float diameter, float value, bool isHover, bool isDown)
{
    float cx = x + diameter * 0.5f;
    float cy = y + diameter * 0.5f;
    float radius = diameter * 0.5f;

    // Outer glow when hovered/pressed
    if (isHover || isDown)
    {
        juce::DropShadow glow (valueColor.withAlpha (isDown ? 0.3f : 0.15f), 8, {0, 0});
        juce::Path glowPath;
        glowPath.addEllipse (x + 2, y + 2, diameter - 4, diameter - 4);
        glow.drawForPath (g, glowPath);
    }

    // Track background
    g.setColour (trackColor);
    g.drawEllipse (x + 2, y + 2, diameter - 4, diameter - 4, 2.0f);

    // Value arc
    drawValueArc (g, cx, cy, radius - 4, value);

    // Tick marks for discrete parameters
    if (getInterval() > 0.0f)
        drawTickMarks (g, cx, cy, radius - 8);

    // Center indicator dot
    g.setColour (isDown ? valueColor.brighter() : valueColor);
    float dotRadius = 3.0f;
    g.fillEllipse (cx - dotRadius, cy - dotRadius, dotRadius * 2, dotRadius * 2);

    // Thumb indicator (line from center to edge)
    float angle = startAngle + value * (endAngle - startAngle);
    float thumbLength = radius - 10;
    g.setColour (thumbColor);
    g.drawLine (cx, cy,
                cx + std::cos (angle) * thumbLength,
                cy + std::sin (angle) * thumbLength, 2.0f);
}

void ParameterKnob::drawValueArc (juce::Graphics& g, float cx, float cy, float radius, float value)
{
    juce::Path arc;
    float angle = startAngle + value * (endAngle - startAngle);

    arc.addCentredArc (cx, cy, radius, radius, 0.0f, startAngle, angle, true);

    g.setColour (valueColor);
    g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void ParameterKnob::drawTickMarks (juce::Graphics& g, float cx, float cy, float radius)
{
    int numSteps = static_cast<int> ((getMaximum() - getMinimum()) / getInterval()) + 1;
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextMuted).withAlpha (0.5f));

    for (int i = 0; i < numSteps; ++i)
    {
        float stepValue = static_cast<float> (i) / (numSteps - 1);
        float angle = startAngle + stepValue * (endAngle - startAngle);

        float x1 = cx + std::cos (angle) * (radius - 2);
        float y1 = cy + std::sin (angle) * (radius - 2);
        float x2 = cx + std::cos (angle) * radius;
        float y2 = cy + std::sin (angle) * radius;

        g.drawLine (x1, y1, x2, y2, 1.0f);
    }
}

void ParameterKnob::resized()
{
    // Ensure minimum hit area
    int minSize = ColorScheme::minHitSize;
    if (getWidth() < minSize || getHeight() < minSize)
        setSize (juce::jmax (getWidth(), minSize), juce::jmax (getHeight(), minSize));
}

void ParameterKnob::mouseDown (const juce::MouseEvent& e)
{
    isDragging = true;
    Slider::mouseDown (e);
    repaint();
}

void ParameterKnob::mouseDrag (const juce::MouseEvent& e)
{
    Slider::mouseDrag (e);
    repaint();
}

void ParameterKnob::mouseUp (const juce::MouseEvent& e)
{
    isDragging = false;
    Slider::mouseUp (e);
    repaint();
}

void ParameterKnob::mouseEnter (const juce::MouseEvent&)
{
    isHovering = true;
    repaint();
}

void ParameterKnob::mouseExit (const juce::MouseEvent&)
{
    isHovering = false;
    repaint();
}

void ParameterKnob::setParameterID (const juce::String& newParamID)
{
    paramID = newParamID;
    setTooltip (newParamID);
    attachment.reset();
    updateColors();
    repaint();
}

} // namespace zalaxy