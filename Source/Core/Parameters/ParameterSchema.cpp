#include "ParameterSchema.h"

namespace zalaxy
{

juce::Array<ParameterInfo> ParameterSchema::allParameters;
bool ParameterSchema::initialized = false;

juce::Array<ParameterInfo> ParameterSchema::createParameterList()
{
    juce::Array<ParameterInfo> params;

    // Helper lambda for adding parameters
    auto addParam = [&] (const char* id, const char* name, const char* category,
                         float minV, float maxV, float defV, float interval,
                         const char* unit, bool automatable = true, bool meta = false)
    {
        ParameterInfo p;
        p.id = id;
        p.name = name;
        p.category = category;
        p.minValue = minV;
        p.maxValue = maxV;
        p.defaultValue = defV;
        p.interval = interval;
        p.unit = unit;
        p.isAutomatable = automatable;
        p.isMetaParameter = meta;
        params.add (p);
    };

    // ========== MASTER ==========
    addParam (IDs::masterVolume, "Master Volume", Categories::master,
              0.0f, 1.0f, 0.8f, 0.0f, "%");
    addParam (IDs::masterPan, "Master Pan", Categories::master,
              -1.0f, 1.0f, 0.0f, 0.0f, "");
    addParam (IDs::polyphony, "Polyphony", Categories::master,
              1.0f, 16.0f, 16.0f, 1.0f, "voices", true, true);

    // ========== OSCILLATORS (1-4) ==========
    for (int o = 1; o <= 4; ++o)
    {
        const juce::String prefix = "osc" + juce::String (o);
        const juce::String label = "Osc " + juce::String (o);

        addParam ((prefix + "Waveform").toRawUTF8(), (label + " Waveform").toRawUTF8(), Categories::oscillators,
                  0.0f, 6.0f, 2.0f, 1.0f, "", true, false);  // 0=Sine,1=Tri,2=Saw,3=Sq,4=Pulse,5=WT,6=Noise
        addParam ((prefix + "Level").toRawUTF8(), (label + " Level").toRawUTF8(), Categories::oscillators,
                  0.0f, 1.0f, (o == 1 ? 1.0f : 0.0f), 0.0f, "%");
        addParam ((prefix + "Pitch").toRawUTF8(), (label + " Pitch").toRawUTF8(), Categories::oscillators,
                  -24.0f, 24.0f, 0.0f, 1.0f, "semitones");
        addParam ((prefix + "FineTune").toRawUTF8(), (label + " Fine Tune").toRawUTF8(), Categories::oscillators,
                  -50.0f, 50.0f, 0.0f, 0.0f, "cents");
        addParam ((prefix + "PulseWidth").toRawUTF8(), (label + " Pulse Width").toRawUTF8(), Categories::oscillators,
                  0.01f, 0.99f, 0.5f, 0.0f, "%");
        addParam ((prefix + "PhaseMod").toRawUTF8(), (label + " Phase Mod").toRawUTF8(), Categories::oscillators,
                  0.0f, 1.0f, 0.0f, 0.0f, "%");
    }

    // Sub Oscillator
    addParam (IDs::subOscLevel, "Sub Osc Level", Categories::oscillators,
              0.0f, 1.0f, 0.0f, 0.0f, "%");
    addParam (IDs::subOscWaveform, "Sub Osc Waveform", Categories::oscillators,
              0.0f, 3.0f, 0.0f, 1.0f, "");  // Sine, Triangle, Saw, Square

    // Noise
    addParam (IDs::noiseLevel, "Noise Level", Categories::oscillators,
              0.0f, 1.0f, 0.0f, 0.0f, "%");
    addParam (IDs::noiseColor, "Noise Color", Categories::oscillators,
              0.0f, 1.0f, 0.5f, 0.0f, "");  // White to Pink

    // ========== FILTERS ==========
    for (int f = 1; f <= 2; ++f)
    {
        const juce::String prefix = "filter" + juce::String (f);
        const juce::String label = "Filter " + juce::String (f);

        addParam ((prefix + "Type").toRawUTF8(), (label + " Type").toRawUTF8(), Categories::filters,
                  0.0f, 6.0f, 0.0f, 1.0f, "");  // LP, HP, BP, Notch, LowShelf, HighShelf, Peak
        addParam ((prefix + "Cutoff").toRawUTF8(), (label + " Cutoff").toRawUTF8(), Categories::filters,
                  20.0f, 20000.0f, (f == 1 ? 5000.0f : 20000.0f), 0.0f, "Hz");
        addParam ((prefix + "Resonance").toRawUTF8(), (label + " Resonance").toRawUTF8(), Categories::filters,
                  0.1f, 20.0f, 1.0f, 0.0f, "");
        addParam ((prefix + "Drive").toRawUTF8(), (label + " Drive").toRawUTF8(), Categories::filters,
                  0.0f, 1.0f, 0.0f, 0.0f, "%");
        addParam ((prefix + "Keytrack").toRawUTF8(), (label + " Keytrack").toRawUTF8(), Categories::filters,
                  0.0f, 1.0f, (f == 1 ? 0.5f : 0.0f), 0.0f, "%");
    }

    addParam (IDs::filterRouting, "Filter Routing", Categories::filters,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);  // 0=Serial, 1=Parallel
    addParam (IDs::filterBlend, "Filter Blend", Categories::filters,
              0.0f, 1.0f, 0.5f, 0.0f, "%");

    // ========== ENVELOPES ==========
    // Amp Envelope
    addParam (IDs::ampAttack, "Amp Attack", Categories::envelopes,
              0.001f, 10.0f, 0.01f, 0.0f, "s");
    addParam (IDs::ampDecay, "Amp Decay", Categories::envelopes,
              0.001f, 10.0f, 0.1f, 0.0f, "s");
    addParam (IDs::ampSustain, "Amp Sustain", Categories::envelopes,
              0.0f, 1.0f, 0.7f, 0.0f, "%");
    addParam (IDs::ampRelease, "Amp Release", Categories::envelopes,
              0.001f, 10.0f, 0.3f, 0.0f, "s");
    addParam (IDs::ampVelocity, "Amp Velocity", Categories::envelopes,
              0.0f, 1.0f, 0.5f, 0.0f, "%");

    // Filter Envelope
    addParam (IDs::filterAttack, "Filter Attack", Categories::envelopes,
              0.001f, 10.0f, 0.01f, 0.0f, "s");
    addParam (IDs::filterDecay, "Filter Decay", Categories::envelopes,
              0.001f, 10.0f, 0.1f, 0.0f, "s");
    addParam (IDs::filterSustain, "Filter Sustain", Categories::envelopes,
              0.0f, 1.0f, 0.0f, 0.0f, "%");
    addParam (IDs::filterRelease, "Filter Release", Categories::envelopes,
              0.001f, 10.0f, 0.2f, 0.0f, "s");
    addParam (IDs::filterEnvAmount, "Filter Env Amount", Categories::envelopes,
              -1.0f, 1.0f, 0.5f, 0.0f, "%");
    addParam (IDs::filterVelocity, "Filter Velocity", Categories::envelopes,
              0.0f, 1.0f, 0.0f, 0.0f, "%");

    // Mod Envelope
    addParam (IDs::modAttack, "Mod Attack", Categories::envelopes,
              0.001f, 10.0f, 0.01f, 0.0f, "s");
    addParam (IDs::modDecay, "Mod Decay", Categories::envelopes,
              0.001f, 10.0f, 0.1f, 0.0f, "s");
    addParam (IDs::modSustain, "Mod Sustain", Categories::envelopes,
              0.0f, 1.0f, 0.0f, 0.0f, "%");
    addParam (IDs::modRelease, "Mod Release", Categories::envelopes,
              0.001f, 10.0f, 0.2f, 0.0f, "s");
    addParam (IDs::modAmount, "Mod Amount", Categories::envelopes,
              0.0f, 1.0f, 0.0f, 0.0f, "%");

    // ========== LFOs ==========
    for (int l = 1; l <= 2; ++l)
    {
        const juce::String prefix = "lfo" + juce::String (l);
        const juce::String label = "LFO " + juce::String (l);

        addParam ((prefix + "Waveform").toRawUTF8(), (label + " Waveform").toRawUTF8(), Categories::lfos,
                  0.0f, 5.0f, 0.0f, 1.0f, "");  // Sine, Tri, Saw, Sq, S&H, Random
        addParam ((prefix + "Rate").toRawUTF8(), (label + " Rate").toRawUTF8(), Categories::lfos,
                  0.01f, 30.0f, 1.0f, 0.0f, "Hz");
        addParam ((prefix + "Depth").toRawUTF8(), (label + " Depth").toRawUTF8(), Categories::lfos,
                  0.0f, 1.0f, 0.0f, 0.0f, "%");
        addParam ((prefix + "Delay").toRawUTF8(), (label + " Delay").toRawUTF8(), Categories::lfos,
                  0.0f, 10.0f, 0.0f, 0.0f, "s");
        addParam ((prefix + "FadeIn").toRawUTF8(), (label + " Fade In").toRawUTF8(), Categories::lfos,
                  0.0f, 10.0f, 0.0f, 0.0f, "s");
        addParam ((prefix + "KeySync").toRawUTF8(), (label + " Key Sync").toRawUTF8(), Categories::lfos,
                  0.0f, 1.0f, 1.0f, 1.0f, "", true, false);
        addParam ((prefix + "TempoSync").toRawUTF8(), (label + " Tempo Sync").toRawUTF8(), Categories::lfos,
                  0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    }

    // ========== MODULATION MATRIX (8 slots) ==========
    for (int s = 1; s <= 8; ++s)
    {
        const juce::String prefix = "modSlot" + juce::String (s);
        const juce::String label = "Mod Slot " + juce::String (s);

        addParam ((prefix + "Source").toRawUTF8(), (label + " Source").toRawUTF8(), Categories::modulation,
                  0.0f, 15.0f, 0.0f, 1.0f, "", true, false);
        addParam ((prefix + "Dest").toRawUTF8(), (label + " Dest").toRawUTF8(), Categories::modulation,
                  0.0f, 25.0f, 0.0f, 1.0f, "", true, false);
        addParam ((prefix + "Amount").toRawUTF8(), (label + " Amount").toRawUTF8(), Categories::modulation,
                  -1.0f, 1.0f, 0.0f, 0.0f, "%");
    }

    // ========== EFFECTS ==========
    // Reverb
    addParam (IDs::reverbEnabled, "Reverb On", Categories::effects,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::reverbRoomSize, "Reverb Room Size", Categories::effects,
              0.0f, 1.0f, 0.5f, 0.0f, "%");
    addParam (IDs::reverbDamping, "Reverb Damping", Categories::effects,
              0.0f, 1.0f, 0.5f, 0.0f, "%");
    addParam (IDs::reverbWetLevel, "Reverb Wet", Categories::effects,
              0.0f, 1.0f, 0.3f, 0.0f, "%");
    addParam (IDs::reverbDryLevel, "Reverb Dry", Categories::effects,
              0.0f, 1.0f, 1.0f, 0.0f, "%");
    addParam (IDs::reverbWidth, "Reverb Width", Categories::effects,
              0.0f, 1.0f, 1.0f, 0.0f, "%");
    addParam (IDs::reverbPreDelay, "Reverb Pre-Delay", Categories::effects,
              0.0f, 500.0f, 20.0f, 0.0f, "ms");

    // Delay
    addParam (IDs::delayEnabled, "Delay On", Categories::effects,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::delayTime, "Delay Time", Categories::effects,
              1.0f, 2000.0f, 375.0f, 0.0f, "ms");
    addParam (IDs::delayFeedback, "Delay Feedback", Categories::effects,
              0.0f, 0.95f, 0.3f, 0.0f, "%");
    addParam (IDs::delayWetLevel, "Delay Wet", Categories::effects,
              0.0f, 1.0f, 0.3f, 0.0f, "%");
    addParam (IDs::delayTempoSync, "Delay Tempo Sync", Categories::effects,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::delayNoteValue, "Delay Note Value", Categories::effects,
              0.0f, 10.0f, 4.0f, 1.0f, "", true, false);

    // Distortion
    addParam (IDs::distortionEnabled, "Distortion On", Categories::effects,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::distortionType, "Distortion Type", Categories::effects,
              0.0f, 3.0f, 0.0f, 1.0f, "", true, false);  // Soft, Hard, Wavefold, Bitcrush
    addParam (IDs::distortionDrive, "Distortion Drive", Categories::effects,
              1.0f, 100.0f, 10.0f, 0.0f, "x");
    addParam (IDs::distortionMix, "Distortion Mix", Categories::effects,
              0.0f, 1.0f, 0.5f, 0.0f, "%");
    addParam (IDs::distortionTone, "Distortion Tone", Categories::effects,
              0.0f, 1.0f, 0.5f, 0.0f, "%");

    // Chorus
    addParam (IDs::chorusEnabled, "Chorus On", Categories::effects,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::chorusRate, "Chorus Rate", Categories::effects,
              0.1f, 10.0f, 1.5f, 0.0f, "Hz");
    addParam (IDs::chorusDepth, "Chorus Depth", Categories::effects,
              0.0f, 1.0f, 0.3f, 0.0f, "%");
    addParam (IDs::chorusFeedback, "Chorus Feedback", Categories::effects,
              0.0f, 0.9f, 0.1f, 0.0f, "%");
    addParam (IDs::chorusMix, "Chorus Mix", Categories::effects,
              0.0f, 1.0f, 0.5f, 0.0f, "%");

    // ========== ARPEGGIATOR (placeholder) ==========
    addParam (IDs::arpEnabled, "Arpeggiator On", Categories::arpeggiator,
              0.0f, 1.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::arpMode, "Arp Mode", Categories::arpeggiator,
              0.0f, 5.0f, 0.0f, 1.0f, "", true, false);
    addParam (IDs::arpRate, "Arp Rate", Categories::arpeggiator,
              0.5f, 30.0f, 8.0f, 0.0f, "Hz");
    addParam (IDs::arpRange, "Arp Range", Categories::arpeggiator,
              1.0f, 4.0f, 1.0f, 1.0f, "octaves");
    addParam (IDs::arpGate, "Arp Gate", Categories::arpeggiator,
              0.1f, 1.0f, 0.5f, 0.0f, "%");

    return params;
}

const juce::Array<ParameterInfo>& ParameterSchema::getAllParameters()
{
    if (!initialized)
    {
        allParameters = createParameterList();
        initialized = true;
    }
    return allParameters;
}

const ParameterInfo* ParameterSchema::getParameterInfo (const juce::String& paramID)
{
    const auto& params = getAllParameters();
    for (const auto& p : params)
    {
        if (p.id == paramID)
            return &p;
    }
    return nullptr;
}

} // namespace zalaxy