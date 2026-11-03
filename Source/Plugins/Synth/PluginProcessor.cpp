#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace zalaxy
{

PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , parameterManager (*this)
    , audioEngine (parameterManager)
    , presetManager (parameterManager, audioEngine)
    , usageTracker()
    , cloudAIProvider (usageTracker, subscriptionManager)
    , audioReferenceProcessor (parameterManager, audioEngine)
    , subscriptionManager()
{
    // Initialize parameter manager (creates all parameters)
    parameterManager.initializeParameters();

    // Initialize preset manager with factory presets
    presetManager.initializeFactoryPresets();

    // Initialize subscription manager (loads license, starts trial if needed)
    subscriptionManager.initialize();

    // Initialize cloud AI provider
    cloudAIProvider.initialize();
}

PluginProcessor::~PluginProcessor()
{
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    audioEngine.prepare (sampleRate, samplesPerBlock);
    parameterManager.prepare (sampleRate);
    audioReferenceProcessor.prepare (sampleRate);
}

void PluginProcessor::releaseResources()
{
    audioEngine.release();
    audioReferenceProcessor.release();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Clear output buffer
    buffer.clear();

    // Process MIDI and generate audio
    audioEngine.processBlock (buffer, midiMessages);

    // Process audio reference analysis if active (non-blocking)
    audioReferenceProcessor.processIfNeeded();
}

void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save full plugin state including parameters and preset info
    juce::MemoryOutputStream stream (destData, false);
    presetManager.saveCurrentState (stream);
    parameterManager.saveState (stream);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore full plugin state
    juce::MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);
    parameterManager.loadState (stream);
    presetManager.loadCurrentState (stream);
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

void PluginProcessor::onTextToPatchRequest (const juce::String& prompt)
{
    // Called from UI thread - delegate to AI provider
    cloudAIProvider.requestTextToPatch (prompt, [this] (const juce::var& result)
    {
        // Handle AI response on message thread
        if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        {
            editor->handleAIResponse (result);
        }
    });
}

void PluginProcessor::onAudioAnalysisRequest (const juce::File& wavFile, double startTime, double endTime)
{
    // Called from UI thread - delegate to audio reference processor
    audioReferenceProcessor.analyzeReference (wavFile, startTime, endTime, [this] (const juce::var& result)
    {
        // Handle analysis response on message thread
        if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        {
            editor->handleAnalysisResponse (result);
        }
    });
}

} // namespace zalaxy

// JUCE plugin entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new zalaxy::PluginProcessor();
}