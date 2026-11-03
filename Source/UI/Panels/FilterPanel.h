#pragma once

#include "Common/JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterKnob.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class FilterPanel : public juce::Component
{
public:
    FilterPanel (ParameterManager& params, AudioEngine& engine);
    ~FilterPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    ParameterManager& parameterManager;
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    // Filter 1
    std::unique_ptr<ParameterKnob> filter1Type;
    std::unique_ptr<ParameterKnob> filter1Cutoff;
    std::unique_ptr<ParameterKnob> filter1Resonance;
    std::unique_ptr<ParameterKnob> filter1Drive;
    std::unique_ptr<ParameterKnob> filter1Keytrack;

    // Filter 2
    std::unique_ptr<ParameterKnob> filter2Type;
    std::unique_ptr<ParameterKnob> filter2Cutoff;
    std::unique_ptr<ParameterKnob> filter2Resonance;
    std::unique_ptr<ParameterKnob> filter2Drive;
    std::unique_ptr<ParameterKnob> filter2Keytrack;

    // Routing
    std::unique_ptr<ParameterKnob> filterRouting;
    std::unique_ptr<ParameterKnob> filterBlend;

    juce::Label panelTitle;
    juce::Label filterLabels[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterPanel)
};

} // namespace zalaxy