#pragma once

#include <JuceHeader.h>
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterKnob.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel (ParameterManager& params, AudioEngine& engine);
    ~OscillatorPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    ParameterManager& parameterManager;
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    struct OscillatorControls
    {
        std::unique_ptr<ParameterKnob> waveform;
        std::unique_ptr<ParameterKnob> level;
        std::unique_ptr<ParameterKnob> pitch;
        std::unique_ptr<ParameterKnob> fineTune;
        std::unique_ptr<ParameterKnob> pulseWidth;
        std::unique_ptr<ParameterKnob> phaseMod;
    };

    std::array<OscillatorControls, 4> oscControls;

    std::unique_ptr<ParameterKnob> subLevel;
    std::unique_ptr<ParameterKnob> subWaveform;
    std::unique_ptr<ParameterKnob> noiseLevel;
    std::unique_ptr<ParameterKnob> noiseColor;

    juce::Label panelTitle;
    juce::Label oscLabels[4];

    void createOscillatorControls (int index, const juce::String& prefix);
    void layoutOscillator (int index, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorPanel)
};

} // namespace zalaxy