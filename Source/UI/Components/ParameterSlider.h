#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"

namespace zalaxy
{

class ParameterSlider : public juce::Slider
{
public:
    ParameterSlider (const juce::String& paramID, ZalaxyTheme& theme, juce::AudioProcessorValueTreeState& apvts,
                     bool horizontal = false);
    ~ParameterSlider() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

    void setParameterID (const juce::String& newParamID);


private:
    juce::String paramID;
    ZalaxyTheme& theme;
    bool isHorizontal;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    bool isHovering = false;

    juce::Colour trackColor;
    juce::Colour fillColor;
    juce::Colour thumbColor;

    void updateColors();
    void drawLinearSlider (juce::Graphics& g, float x, float y, float width, float height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSlider)
};

} // namespace zalaxy