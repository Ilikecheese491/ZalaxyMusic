#include "Envelope.h"

namespace zalaxy
{

Envelope::Envelope() = default;
Envelope::~Envelope() = default;

void Envelope::prepare (double sr)
{
    sampleRate = sr;
}

void Envelope::reset()
{
    stage = EnvelopeStage::Idle;
    currentLevel = 0.0f;
}

void Envelope::setAttack (float seconds)
{
    attackTime = juce::jmax (0.001f, seconds);
}

void Envelope::setDecay (float seconds)
{
    decayTime = juce::jmax (0.001f, seconds);
}

void Envelope::setSustain (float level)
{
    sustainLevel = juce::jlimit (0.0f, 1.0f, level);
}

void Envelope::setRelease (float seconds)
{
    releaseTime = juce::jmax (0.001f, seconds);
}

void Envelope::noteOn (float velocity)
{
    lastVelocity = velocity;
    enterStage (EnvelopeStage::Attack);
}

void Envelope::noteOff()
{
    if (stage != EnvelopeStage::Idle && stage != EnvelopeStage::Release)
    {
        enterStage (EnvelopeStage::Release);
    }
}

void Envelope::enterStage (EnvelopeStage newStage)
{
    stage = newStage;

    switch (stage)
    {
        case EnvelopeStage::Attack:
            targetLevel = 1.0f * lastVelocity;
            stageSamplesRemaining = timeToSamples (attackTime);
            stageSampleIncrement = (targetLevel - currentLevel) / stageSamplesRemaining;
            break;

        case EnvelopeStage::Decay:
            targetLevel = sustainLevel;
            stageSamplesRemaining = timeToSamples (decayTime);
            stageSampleIncrement = (targetLevel - currentLevel) / stageSamplesRemaining;
            break;

        case EnvelopeStage::Sustain:
            targetLevel = sustainLevel;
            stageSamplesRemaining = -1; // Infinite
            stageSampleIncrement = 0.0f;
            break;

        case EnvelopeStage::Release:
            targetLevel = 0.0f;
            stageSamplesRemaining = timeToSamples (releaseTime);
            stageSampleIncrement = (targetLevel - currentLevel) / stageSamplesRemaining;
            break;

        case EnvelopeStage::Idle:
            currentLevel = 0.0f;
            stageSamplesRemaining = 0;
            stageSampleIncrement = 0.0f;
            break;
    }
}

float Envelope::getNextSample()
{
    if (stage == EnvelopeStage::Idle)
        return 0.0f;

    if (stage == EnvelopeStage::Sustain)
        return currentLevel;

    currentLevel += static_cast<float> (stageSampleIncrement);
    stageSamplesRemaining -= 1.0;

    // Check if stage is complete
    if (stageSamplesRemaining <= 0.0)
    {
        currentLevel = targetLevel;

        switch (stage)
        {
            case EnvelopeStage::Attack:
                enterStage (EnvelopeStage::Decay);
                break;
            case EnvelopeStage::Decay:
                enterStage (EnvelopeStage::Sustain);
                break;
            case EnvelopeStage::Release:
                enterStage (EnvelopeStage::Idle);
                break;
            default:
                break;
        }
    }

    return currentLevel;
}

void Envelope::processBlock (float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = getNextSample();
    }
}

void Envelope::getShapePoints (juce::Array<juce::Point<float>>& points, float width, float height) const
{
    points.clear();
    if (width <= 0 || height <= 0) return;

    // Calculate time proportions
    float totalTime = attackTime + decayTime + releaseTime;
    if (totalTime <= 0) return;

    float x = 0;
    float y = height; // Bottom = 0 level

    // Attack
    float attackWidth = (attackTime / totalTime) * width;
    points.add ({x, y});
    x += attackWidth;
    y = height * (1.0f - 1.0f); // Top = 1.0 level
    points.add ({x, y});

    // Decay
    float decayWidth = (decayTime / totalTime) * width;
    x += decayWidth;
    y = height * (1.0f - sustainLevel);
    points.add ({x, y});

    // Sustain (represented as a flat line - we'll just add a point)
    // Release
    float releaseWidth = (releaseTime / totalTime) * width;
    x += releaseWidth;
    y = height; // Back to 0
    points.add ({x, y});
}

double Envelope::timeToSamples (float seconds) const
{
    return seconds * sampleRate;
}

} // namespace zalaxy