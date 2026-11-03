#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterSlider.h"
#include "UI/Components/ParameterKnob.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class EnvelopePanel : public juce::Component
{
public:
    EnvelopePanel (ParameterManager& params, AudioEngine& engine);
    ~EnvelopePanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    ParameterManager& parameterManager;
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    // Amp Envelope
    std::unique_ptr<ParameterSlider> ampAttack;
    std::unique_ptr<ParameterSlider> ampDecay;
    std::unique_ptr<ParameterSlider> ampSustain;
    std::unique_ptr<ParameterSlider> ampRelease;
    std::unique_ptr<ParameterKnob> ampVelocity;

    // Filter Envelope
    std::unique_ptr<ParameterSlider> filterAttack;
    std::unique_ptr<ParameterSlider> filterDecay;
    std::unique_ptr<ParameterSlider> filterSustain;
    std::unique_ptr<ParameterSlider> filterRelease;
    std::unique_ptr<ParameterKnob> filterEnvAmount;
    std::unique_ptr<ParameterKnob> filterVelocity;

    // Mod Envelope
    std::unique_ptr<ParameterSlider> modAttack;
    std::unique_ptr<ParameterSlider> modDecay;
    std::unique_ptr<ParameterSlider> modSustain;
    std::unique_ptr<ParameterSlider> modRelease;
    std::unique_ptr<ParameterKnob> modAmount;

    juce::Label panelTitle;
    juce::Label envLabels[3];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopePanel)
};

} // namespace zalaxy