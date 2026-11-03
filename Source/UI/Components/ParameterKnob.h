#pragma once

#include "Common/JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"

namespace zalaxy
{

class ParameterKnob : public juce::Slider
{
public:
    ParameterKnob (const juce::String& paramID, ZalaxyTheme& theme, juce::AudioProcessorValueTreeState& apvts);
    ~ParameterKnob() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

    void setParameterID (const juce::String& newParamID);

    juce::String getTooltip() const override { return tooltip; }

private:
    juce::String paramID;
    ZalaxyTheme& theme;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    juce::String tooltip;
    bool isHovering = false;
    bool isDragging = false;
    float startAngle = -juce::MathConstants<float>::halfPi * 1.25f;
    float endAngle = juce::MathConstants<float>::halfPi * 1.25f;

    juce::Colour trackColor;
    juce::Colour valueColor;
    juce::Colour thumbColor;

    void updateColors();
    void drawKnob (juce::Graphics& g, float x, float y, float diameter, float value, bool isHover, bool isDown);
    void drawValueArc (juce::Graphics& g, float cx, float cy, float radius, float value);
    void drawTickMarks (juce::Graphics& g, float cx, float cy, float radius);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterKnob)
};

} // namespace zalaxy