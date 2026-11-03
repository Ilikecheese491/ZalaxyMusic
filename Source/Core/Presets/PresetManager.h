#pragma once

#include <JuceHeader.h>
#include "ParameterManager.h"
#include "Core/Audio/AudioEngine.h"
#include "PresetFormat.h"

namespace zalaxy
{

class PresetManager
{
public:
    PresetManager (ParameterManager& params, AudioEngine& engine);
    ~PresetManager();

    void initializeFactoryPresets();

    // Preset management
    int getNumPresets() const { return presets.size(); }
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    juce::String getPresetName (int index) const;
    void loadPreset (int index);
    void savePreset (int index, const juce::String& name);
    void savePresetAs (const juce::File& file, const juce::String& name);
    void deletePreset (int index);
    void renamePreset (int index, const juce::String& newName);

    // Factory presets
    void loadFactoryPreset (const juce::String& name);
    juce::StringArray getFactoryPresetNames() const;
    juce::StringArray getUserPresetNames() const;

    // Import/Export
    bool importPreset (const juce::File& file);
    bool exportPreset (int index, const juce::File& file);

    // State save/restore for DAW
    void saveCurrentState (juce::MemoryOutputStream& stream);
    void loadCurrentState (juce::MemoryInputStream& stream);

    // Current preset state
    juce::ValueTree getCurrentState() const;
    void setCurrentState (const juce::ValueTree& state);

    // Directories
    static juce::File getFactoryPresetDirectory();
    static juce::File getUserPresetDirectory();

private:
    struct PresetEntry
    {
        juce::String name;
        juce::File file;
        bool isFactory = false;
        PresetFormat::PresetMetadata metadata;
    };

    ParameterManager& parameterManager;
    AudioEngine& audioEngine;

    juce::Array<PresetEntry> presets;
    int currentPresetIndex = -1;
    juce::ValueTree currentState;

    void scanPresetDirectories();
    void createFactoryPresets();
    void addPresetEntry (const PresetEntry& entry);
    int findPresetByName (const juce::String& name) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};

} // namespace zalaxy