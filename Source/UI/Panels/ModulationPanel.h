#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterKnob.h"
#include "UI/Components/ParameterSlider.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class ModulationPanel : public juce::Component
{
public:
    ModulationPanel (ParameterManager& params, AudioEngine& engine);
    ~ModulationPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    ParameterManager& parameterManager;
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    // LFO 1
    std::unique_ptr<ParameterKnob> lfo1Waveform;
    std::unique_ptr<ParameterKnob> lfo1Rate;
    std::unique_ptr<ParameterKnob> lfo1Depth;
    std::unique_ptr<ParameterKnob> lfo1Delay;
    std::unique_ptr<ParameterKnob> lfo1FadeIn;
    std::unique_ptr<ParameterKnob> lfo1KeySync;
    std::unique_ptr<ParameterKnob> lfo1TempoSync;

    // LFO 2
    std::unique_ptr<ParameterKnob> lfo2Waveform;
    std::unique_ptr<ParameterKnob> lfo2Rate;
    std::unique_ptr<ParameterKnob> lfo2Depth;
    std::unique_ptr<ParameterKnob> lfo2Delay;
    std::unique_ptr<ParameterKnob> lfo2FadeIn;
    std::unique_ptr<ParameterKnob> lfo2KeySync;
    std::unique_ptr<ParameterKnob> lfo2TempoSync;

    // Mod Matrix (8 slots)
    struct ModSlot
    {
        std::unique_ptr<ParameterKnob> source;
        std::unique_ptr<ParameterKnob> dest;
        std::unique_ptr<ParameterKnob> amount;
    };
    std::array<ModSlot, 8> modSlots;

    juce::Label panelTitle;
    juce::Label lfoLabels[2];
    juce::Label modMatrixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationPanel)
};

} // namespace zalaxy