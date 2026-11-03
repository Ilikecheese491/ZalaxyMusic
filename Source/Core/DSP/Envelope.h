#pragma once

#include "JuceModules.h"

namespace zalaxy
{

enum class EnvelopeStage
{
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
};

class Envelope
{
public:
    Envelope();
    ~Envelope();

    void prepare (double sampleRate);
    void reset();

    void setAttack (float seconds);
    void setDecay (float seconds);
    void setSustain (float level); // 0.0 to 1.0
    void setRelease (float seconds);

    void noteOn (float velocity = 1.0f);
    void noteOff();
    void setRetrigger (bool shouldRetrigger) { retrigger = shouldRetrigger; }

    float getNextSample();
    void processBlock (float* buffer, int numSamples);

    EnvelopeStage getCurrentStage() const { return stage; }
    bool isActive() const { return stage != EnvelopeStage::Idle; }

    // For visualization
    void getShapePoints (juce::Array<juce::Point<float>>& points, float width, float height) const;

private:
    double sampleRate = 44100.0;
    EnvelopeStage stage = EnvelopeStage::Idle;

    float attackTime = 0.01f;
    float decayTime = 0.1f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.2f;

    float currentLevel = 0.0f;
    float targetLevel = 0.0f;
    double stageSamplesRemaining = 0;
    double stageSampleIncrement = 0;

    bool retrigger = false;
    float lastVelocity = 1.0f;

    void enterStage (EnvelopeStage newStage);
    double timeToSamples (float seconds) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Envelope)
};

} // namespace zalaxy