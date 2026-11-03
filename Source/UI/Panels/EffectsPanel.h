#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterKnob.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class EffectsPanel : public juce::Component
{
public:
    EffectsPanel (ParameterManager& params, AudioEngine& engine);
    ~EffectsPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    ParameterManager& parameterManager;
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    // Reverb
    std::unique_ptr<ParameterKnob> reverbEnabled;
    std::unique_ptr<ParameterKnob> reverbRoomSize;
    std::unique_ptr<ParameterKnob> reverbDamping;
    std::unique_ptr<ParameterKnob> reverbWetLevel;
    std::unique_ptr<ParameterKnob> reverbDryLevel;
    std::unique_ptr<ParameterKnob> reverbWidth;
    std::unique_ptr<ParameterKnob> reverbPreDelay;

    // Delay
    std::unique_ptr<ParameterKnob> delayEnabled;
    std::unique_ptr<ParameterKnob> delayTime;
    std::unique_ptr<ParameterKnob> delayFeedback;
    std::unique_ptr<ParameterKnob> delayWetLevel;
    std::unique_ptr<ParameterKnob> delayTempoSync;
    std::unique_ptr<ParameterKnob> delayNoteValue;

    // Distortion
    std::unique_ptr<ParameterKnob> distortionEnabled;
    std::unique_ptr<ParameterKnob> distortionType;
    std::unique_ptr<ParameterKnob> distortionDrive;
    std::unique_ptr<ParameterKnob> distortionMix;
    std::unique_ptr<ParameterKnob> distortionTone;

    // Chorus
    std::unique_ptr<ParameterKnob> chorusEnabled;
    std::unique_ptr<ParameterKnob> chorusRate;
    std::unique_ptr<ParameterKnob> chorusDepth;
    std::unique_ptr<ParameterKnob> chorusFeedback;
    std::unique_ptr<ParameterKnob> chorusMix;

    // Master
    std::unique_ptr<ParameterKnob> masterVolume;
    std::unique_ptr<ParameterKnob> masterPan;

    juce::Label panelTitle;
    juce::Label fxLabels[5];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsPanel)
};

} // namespace zalaxy