#include "AudioReferenceProcessor.h"

namespace zalaxy
{

AudioReferenceProcessor::AudioReferenceProcessor()
{
    formatManager.registerBasicFormats();
}

AudioReferenceProcessor::~AudioReferenceProcessor() {}

bool AudioReferenceProcessor::loadFile (const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (!reader)
        return false;

    sampleRate = static_cast<int> (reader->sampleRate);
    numChannels = reader->numChannels;
    duration = reader->lengthInSamples / reader->sampleRate;

    buffer.setSize (numChannels, static_cast<int> (reader->lengthInSamples));
    reader->read (&buffer, 0, static_cast<int> (reader->lengthInSamples), 0, true, true);
    loaded = true;

    return true;
}

void AudioReferenceProcessor::processRange (double startTime, double endTime)
{
    if (!loaded) return;

    int startSample = static_cast<int> (startTime * sampleRate);
    int endSample = static_cast<int> (endTime * sampleRate);
    startSample = juce::jlimit (0, buffer.getNumSamples() - 1, startSample);
    endSample = juce::jlimit (startSample + 1, buffer.getNumSamples(), endSample);

    // In a full implementation, this would extract the range for analysis
    // For now, we just mark the range
}

} // namespace zalaxy