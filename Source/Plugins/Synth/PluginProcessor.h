#pragma once

#include <JuceHeader.h>
#include "Core/Audio/AudioEngine.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Presets/PresetManager.h"
#include "Core/Licensing/SubscriptionManager.h"
#include "Core/AI/CloudAIProvider.h"
#include "Core/AI/UsageTracker.h"
#include "Analysis/AudioReference/AudioReferenceProcessor.h"

namespace zalaxy
{

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    // AudioProcessor overrides
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // State management
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Plugin info
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ZalaxyMusic"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return presetManager.getNumPresets(); }
    int getCurrentProgram() override { return presetManager.getCurrentPresetIndex(); }
    void setCurrentProgram (int index) override { presetManager.loadPreset(index); }
    const juce::String getProgramName (int index) override { return presetManager.getPresetName(index); }
    void changeProgramName (int index, const juce::String& newName) override { presetManager.renamePreset(index, newName); }

    // Parameter access
    ParameterManager& getParameterManager() { return parameterManager; }
    AudioEngine& getAudioEngine() { return audioEngine; }
    PresetManager& getPresetManager() { return presetManager; }
    AIProvider& getAIProvider() { return cloudAIProvider; }
    AudioReferenceProcessor& getAudioReferenceProcessor() { return audioReferenceProcessor; }
    SubscriptionManager& getSubscriptionManager() { return subscriptionManager; }
    UsageTracker& getUsageTracker() { return usageTracker; }

    // AI callbacks (called from message thread)
    void onTextToPatchRequest (const juce::String& prompt);
    void onAudioAnalysisRequest (const juce::File& wavFile, double startTime, double endTime);

private:
    // Core components
    ParameterManager parameterManager;
    AudioEngine audioEngine;
    PresetManager presetManager;
    UsageTracker usageTracker;
    SubscriptionManager subscriptionManager;
    CloudAIProvider cloudAIProvider;
    AudioReferenceProcessor audioReferenceProcessor;

    // State
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

} // namespace zalaxy