#pragma once

#include "JuceModules.h"
#include "ParameterSchema.h"

namespace zalaxy
{

class ParameterManager
{
public:
    ParameterManager (juce::AudioProcessor& processor);
    ~ParameterManager();

    void initializeParameters();
    void prepare (double sampleRate);

    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    const juce::AudioProcessorValueTreeState& getAPVTS() const { return apvts; }

    juce::RangedAudioParameter* getParameter (const juce::String& paramID);
    const juce::RangedAudioParameter* getParameter (const juce::String& paramID) const;

    float getParameterValue (const juce::String& paramID) const;
    void setParameterValue (const juce::String& paramID, float newValue);

    // Parameter smoothing
    void addParameterListener (const juce::String& paramID, juce::AudioProcessorValueTreeState::Listener* listener);
    void removeParameterListener (const juce::String& paramID, juce::AudioProcessorValueTreeState::Listener* listener);

    // State management
    void saveState (juce::MemoryOutputStream& stream);
    void loadState (juce::MemoryInputStream& stream);

    // Reset to defaults
    void resetToDefaults();

    // Get all parameter IDs for a category
    juce::StringArray getParameterIDsForCategory (const juce::String& category) const;

    // Modulation source/destination enums (for modulation matrix)
    enum class ModSource
    {
        None = 0,
        LFO1 = 1,
        LFO2 = 2,
        AmpEnvelope = 3,
        FilterEnvelope = 4,
        ModEnvelope = 5,
        Velocity = 6,
        KeyTrack = 7,
        ModWheel = 8,
        PitchBend = 9,
        Aftertouch = 10,
        Expression = 11,
        Breath = 12,
        Random = 13,
        StepSequencer = 14,
        Macro1 = 15
    };

    enum class ModDestination
    {
        None = 0,
        Osc1Pitch = 1,
        Osc2Pitch = 2,
        Osc3Pitch = 3,
        Osc4Pitch = 4,
        AllOscPitch = 5,
        Osc1Level = 6,
        Osc2Level = 7,
        Osc3Level = 8,
        Osc4Level = 9,
        Filter1Cutoff = 10,
        Filter2Cutoff = 11,
        Filter1Resonance = 12,
        Filter2Resonance = 13,
        AmpLevel = 14,
        Pan = 15,
        LFO1Rate = 16,
        LFO2Rate = 17,
        LFO1Depth = 18,
        LFO2Depth = 19,
        Filter1Drive = 20,
        Filter2Drive = 21,
        ReverbMix = 22,
        DelayMix = 23,
        DistortionDrive = 24,
        MasterVolume = 25
    };

    static juce::String modSourceToString (ModSource source);
    static juce::String modDestToString (ModDestination dest);
    static ModSource stringToModSource (const juce::String& str);
    static ModDestination stringToModDest (const juce::String& str);

private:
    juce::AudioProcessor& processor;
    juce::AudioProcessorValueTreeState apvts;

    // Parameter layout creation
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterManager)
};

} // namespace zalaxy