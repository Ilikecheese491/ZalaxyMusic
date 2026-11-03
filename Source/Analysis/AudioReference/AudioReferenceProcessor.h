#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

class AudioReferenceProcessor
{
public:
    AudioReferenceProcessor();
    ~AudioReferenceProcessor();

    bool loadFile (const juce::File& file);
    void processRange (double startTime, double endTime);
    const juce::AudioBuffer<float>& getBuffer() const { return buffer; }
    double getDuration() const { return duration; }
    int getSampleRate() const { return sampleRate; }
    int getNumChannels() const { return numChannels; }
    bool isLoaded() const { return loaded; }

private:
    juce::AudioBuffer<float> buffer;
    double duration = 0.0;
    int sampleRate = 0;
    int numChannels = 0;
    bool loaded = false;

    juce::AudioFormatManager formatManager;
};

} // namespace zalaxy