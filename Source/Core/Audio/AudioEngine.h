#pragma once

#include <JuceHeader.h>
#include "Core/Parameters/ParameterManager.h"
#include "Core/DSP/Oscillator.h"
#include "Core/DSP/Filter.h"
#include "Core/DSP/Envelope.h"
#include "Core/DSP/LFO.h"
#include "Core/DSP/Effects.h"

namespace zalaxy
{

class AudioEngine : public juce::MidiKeyboardStateListener
{
public:
    AudioEngine (ParameterManager& params);
    ~AudioEngine();

    void prepare (double sampleRate, int samplesPerBlock);
    void release();
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    // MidiKeyboardStateListener
    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    // Parameter change callbacks
    void onParameterChanged (const juce::String& paramID, float newValue);

    // Voice management
    int getActiveVoiceCount() const { return activeVoices; }

private:
    struct Voice
    {
        bool active = false;
        int note = -1;
        float velocity = 0.0f;
        double phase[4] = {0};

        Envelope ampEnvelope;
        Envelope filterEnvelope;
        Envelope modEnvelope;

        LFO lfo1;
        LFO lfo2;

        Filter filter1;
        Filter filter2;

        Oscillator osc[4];
    };

    ParameterManager& parameters;

    static constexpr int MAX_VOICES = 16;
    Voice voices[MAX_VOICES];
    int activeVoices = 0;
    int nextVoiceIndex = 0;

    Effects effects;
    double sampleRate = 44100.0;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> masterVolume;

    Voice* allocateVoice (int note, float velocity);
    void releaseVoice (Voice* voice);
    Voice* findVoice (int note);

    void processVoice (Voice& voice, juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void updateVoiceParameters (Voice& voice);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioEngine)
};

} // namespace zalaxy