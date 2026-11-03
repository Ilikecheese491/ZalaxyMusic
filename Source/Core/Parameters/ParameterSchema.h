#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

struct ParameterInfo
{
    juce::String id;
    juce::String name;
    juce::String category;
    float minValue;
    float maxValue;
    float defaultValue;
    float interval;
    juce::String unit;
    bool isAutomatable = true;
    bool isMetaParameter = false;
};

class ParameterSchema
{
public:
    static const juce::Array<ParameterInfo>& getAllParameters();
    static const ParameterInfo* getParameterInfo (const juce::String& paramID);

    struct IDs
    {
        static constexpr const char* masterVolume = "masterVolume";
        static constexpr const char* masterPan = "masterPan";
        static constexpr const char* polyphony = "polyphony";

        static constexpr const char* osc1Waveform = "osc1Waveform";
        static constexpr const char* osc1Level = "osc1Level";
        static constexpr const char* osc1Pitch = "osc1Pitch";
        static constexpr const char* osc1FineTune = "osc1FineTune";
        static constexpr const char* osc1PulseWidth = "osc1PulseWidth";
        static constexpr const char* osc1PhaseMod = "osc1PhaseMod";

        static constexpr const char* osc2Waveform = "osc2Waveform";
        static constexpr const char* osc2Level = "osc2Level";
        static constexpr const char* osc2Pitch = "osc2Pitch";
        static constexpr const char* osc2FineTune = "osc2FineTune";
        static constexpr const char* osc2PulseWidth = "osc2PulseWidth";
        static constexpr const char* osc2PhaseMod = "osc2PhaseMod";

        static constexpr const char* osc3Waveform = "osc3Waveform";
        static constexpr const char* osc3Level = "osc3Level";
        static constexpr const char* osc3Pitch = "osc3Pitch";
        static constexpr const char* osc3FineTune = "osc3FineTune";
        static constexpr const char* osc3PulseWidth = "osc3PulseWidth";
        static constexpr const char* osc3PhaseMod = "osc3PhaseMod";

        static constexpr const char* osc4Waveform = "osc4Waveform";
        static constexpr const char* osc4Level = "osc4Level";
        static constexpr const char* osc4Pitch = "osc4Pitch";
        static constexpr const char* osc4FineTune = "osc4FineTune";
        static constexpr const char* osc4PulseWidth = "osc4PulseWidth";
        static constexpr const char* osc4PhaseMod = "osc4PhaseMod";

        static constexpr const char* subOscLevel = "subOscLevel";
        static constexpr const char* subOscWaveform = "subOscWaveform";

        static constexpr const char* noiseLevel = "noiseLevel";
        static constexpr const char* noiseColor = "noiseColor";

        static constexpr const char* filter1Type = "filter1Type";
        static constexpr const char* filter1Cutoff = "filter1Cutoff";
        static constexpr const char* filter1Resonance = "filter1Resonance";
        static constexpr const char* filter1Drive = "filter1Drive";
        static constexpr const char* filter1Keytrack = "filter1Keytrack";

        static constexpr const char* filter2Type = "filter2Type";
        static constexpr const char* filter2Cutoff = "filter2Cutoff";
        static constexpr const char* filter2Resonance = "filter2Resonance";
        static constexpr const char* filter2Drive = "filter2Drive";
        static constexpr const char* filter2Keytrack = "filter2Keytrack";

        static constexpr const char* filterRouting = "filterRouting";
        static constexpr const char* filterBlend = "filterBlend";

        static constexpr const char* ampAttack = "ampAttack";
        static constexpr const char* ampDecay = "ampDecay";
        static constexpr const char* ampSustain = "ampSustain";
        static constexpr const char* ampRelease = "ampRelease";
        static constexpr const char* ampVelocity = "ampVelocity";

        static constexpr const char* filterAttack = "filterAttack";
        static constexpr const char* filterDecay = "filterDecay";
        static constexpr const char* filterSustain = "filterSustain";
        static constexpr const char* filterRelease = "filterRelease";
        static constexpr const char* filterEnvAmount = "filterEnvAmount";
        static constexpr const char* filterVelocity = "filterVelocity";

        static constexpr const char* modAttack = "modAttack";
        static constexpr const char* modDecay = "modDecay";
        static constexpr const char* modSustain = "modSustain";
        static constexpr const char* modRelease = "modRelease";
        static constexpr const char* modAmount = "modAmount";

        static constexpr const char* lfo1Waveform = "lfo1Waveform";
        static constexpr const char* lfo1Rate = "lfo1Rate";
        static constexpr const char* lfo1Depth = "lfo1Depth";
        static constexpr const char* lfo1Delay = "lfo1Delay";
        static constexpr const char* lfo1FadeIn = "lfo1FadeIn";
        static constexpr const char* lfo1KeySync = "lfo1KeySync";
        static constexpr const char* lfo1TempoSync = "lfo1TempoSync";

        static constexpr const char* lfo2Waveform = "lfo2Waveform";
        static constexpr const char* lfo2Rate = "lfo2Rate";
        static constexpr const char* lfo2Depth = "lfo2Depth";
        static constexpr const char* lfo2Delay = "lfo2Delay";
        static constexpr const char* lfo2FadeIn = "lfo2FadeIn";
        static constexpr const char* lfo2KeySync = "lfo2KeySync";
        static constexpr const char* lfo2TempoSync = "lfo2TempoSync";

        static constexpr const char* modSlot1Source = "modSlot1Source";
        static constexpr const char* modSlot1Dest = "modSlot1Dest";
        static constexpr const char* modSlot1Amount = "modSlot1Amount";

        static constexpr const char* modSlot2Source = "modSlot2Source";
        static constexpr const char* modSlot2Dest = "modSlot2Dest";
        static constexpr const char* modSlot2Amount = "modSlot2Amount";

        static constexpr const char* modSlot3Source = "modSlot3Source";
        static constexpr const char* modSlot3Dest = "modSlot3Dest";
        static constexpr const char* modSlot3Amount = "modSlot3Amount";

        static constexpr const char* modSlot4Source = "modSlot4Source";
        static constexpr const char* modSlot4Dest = "modSlot4Dest";
        static constexpr const char* modSlot4Amount = "modSlot4Amount";

        static constexpr const char* modSlot5Source = "modSlot5Source";
        static constexpr const char* modSlot5Dest = "modSlot5Dest";
        static constexpr const char* modSlot5Amount = "modSlot5Amount";

        static constexpr const char* modSlot6Source = "modSlot6Source";
        static constexpr const char* modSlot6Dest = "modSlot6Dest";
        static constexpr const char* modSlot6Amount = "modSlot6Amount";

        static constexpr const char* modSlot7Source = "modSlot7Source";
        static constexpr const char* modSlot7Dest = "modSlot7Dest";
        static constexpr const char* modSlot7Amount = "modSlot7Amount";

        static constexpr const char* modSlot8Source = "modSlot8Source";
        static constexpr const char* modSlot8Dest = "modSlot8Dest";
        static constexpr const char* modSlot8Amount = "modSlot8Amount";

        static constexpr const char* reverbEnabled = "reverbEnabled";
        static constexpr const char* reverbRoomSize = "reverbRoomSize";
        static constexpr const char* reverbDamping = "reverbDamping";
        static constexpr const char* reverbWetLevel = "reverbWetLevel";
        static constexpr const char* reverbDryLevel = "reverbDryLevel";
        static constexpr const char* reverbWidth = "reverbWidth";
        static constexpr const char* reverbPreDelay = "reverbPreDelay";

        static constexpr const char* delayEnabled = "delayEnabled";
        static constexpr const char* delayTime = "delayTime";
        static constexpr const char* delayFeedback = "delayFeedback";
        static constexpr const char* delayWetLevel = "delayWetLevel";
        static constexpr const char* delayTempoSync = "delayTempoSync";
        static constexpr const char* delayNoteValue = "delayNoteValue";

        static constexpr const char* distortionEnabled = "distortionEnabled";
        static constexpr const char* distortionType = "distortionType";
        static constexpr const char* distortionDrive = "distortionDrive";
        static constexpr const char* distortionMix = "distortionMix";
        static constexpr const char* distortionTone = "distortionTone";

        static constexpr const char* chorusEnabled = "chorusEnabled";
        static constexpr const char* chorusRate = "chorusRate";
        static constexpr const char* chorusDepth = "chorusDepth";
        static constexpr const char* chorusFeedback = "chorusFeedback";
        static constexpr const char* chorusMix = "chorusMix";

        static constexpr const char* arpEnabled = "arpEnabled";
        static constexpr const char* arpMode = "arpMode";
        static constexpr const char* arpRate = "arpRate";
        static constexpr const char* arpRange = "arpRange";
        static constexpr const char* arpGate = "arpGate";
    };

    struct Categories
    {
        static constexpr const char* master = "Master";
        static constexpr const char* oscillators = "Oscillators";
        static constexpr const char* filters = "Filters";
        static constexpr const char* envelopes = "Envelopes";
        static constexpr const char* lfos = "LFOs";
        static constexpr const char* modulation = "Modulation";
        static constexpr const char* effects = "Effects";
        static constexpr const char* arpeggiator = "Arpeggiator";
    };

private:
    static juce::Array<ParameterInfo> createParameterList();
    static juce::Array<ParameterInfo> allParameters;
    static bool initialized;
};

} // namespace zalaxy