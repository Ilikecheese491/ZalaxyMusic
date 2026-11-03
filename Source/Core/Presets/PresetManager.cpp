#include "PresetManager.h"
#include <ctime>

namespace zalaxy
{

PresetManager::PresetManager (ParameterManager& params, AudioEngine& engine)
    : parameterManager (params)
    , audioEngine (engine)
{
    scanPresetDirectories();
    createFactoryPresets();
}

PresetManager::~PresetManager()
{
}

void PresetManager::initializeFactoryPresets()
{
    createFactoryPresets();
}

juce::String PresetManager::getPresetName (int index) const
{
    if (index >= 0 && index < presets.size())
        return presets[index].name;
    return {};
}

void PresetManager::loadPreset (int index)
{
    if (index < 0 || index >= presets.size())
        return;

    const auto& entry = presets[index];
    juce::ValueTree state;
    PresetMetadata metadata;

    if (PresetFormat::readPreset (entry.file, state, metadata))
    {
        // Apply parameter state
        parameterManager.getAPVTS().replaceState (state);
        currentPresetIndex = index;
        currentState = state;
    }
}

void PresetManager::savePreset (int index, const juce::String& name)
{
    if (index < 0 || index >= presets.size())
        return;

    auto& entry = presets[index];
    entry.name = name;

    // Update metadata
    auto now = std::time (nullptr);
    auto* tm = std::localtime (&now);
    char buffer[64];
    std::strftime (buffer, sizeof (buffer), "%Y-%m-%d %H:%M:%S", tm);
    entry.metadata.dateModified = buffer;
    entry.metadata.name = name;

    // Save
    juce::ValueTree state = parameterManager.getAPVTS().copyState();
    entry.metadata.numParameters = state.getNumChildren();
    PresetFormat::writePreset (entry.file, state, entry.metadata);

    currentState = state;
    currentPresetIndex = index;
}

void PresetManager::savePresetAs (const juce::File& file, const juce::String& name)
{
    auto metadata = PresetFormat::createDefaultMetadata (name, false);
    juce::ValueTree state = parameterManager.getAPVTS().copyState();
    metadata.numParameters = state.getNumChildren();

    if (PresetFormat::writePreset (file, state, metadata))
    {
        PresetEntry entry;
        entry.name = name;
        entry.file = file;
        entry.metadata = metadata;
        entry.isFactory = false;

        addPresetEntry (entry);
        currentPresetIndex = presets.size() - 1;
        currentState = state;
    }
}

void PresetManager::deletePreset (int index)
{
    if (index < 0 || index >= presets.size())
        return;

    if (!presets[index].isFactory)
    {
        presets[index].file.deleteFile();
    }

    presets.remove (index);

    if (currentPresetIndex >= presets.size())
        currentPresetIndex = presets.size() - 1;
}

void PresetManager::renamePreset (int index, const juce::String& newName)
{
    if (index < 0 || index >= presets.size())
        return;

    auto& entry = presets[index];
    if (entry.isFactory)
        return; // Can't rename factory presets

    juce::File newFile = entry.file.getSiblingFile (newName + PresetFormat::FILE_EXTENSION);
    if (entry.file.moveFileTo (newFile))
    {
        entry.name = newName;
        entry.file = newFile;
        entry.metadata.name = newName;

        auto now = std::time (nullptr);
        auto* tm = std::localtime (&now);
        char buffer[64];
        std::strftime (buffer, sizeof (buffer), "%Y-%m-%d %H:%M:%S", tm);
        entry.metadata.dateModified = buffer;

        savePreset (index, newName);
    }
}

void PresetManager::loadFactoryPreset (const juce::String& name)
{
    int index = findPresetByName (name);
    if (index >= 0)
        loadPreset (index);
}

juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray names;
    for (const auto& entry : presets)
    {
        if (entry.isFactory)
            names.add (entry.name);
    }
    return names;
}

juce::StringArray PresetManager::getUserPresetNames() const
{
    juce::StringArray names;
    for (const auto& entry : presets)
    {
        if (!entry.isFactory)
            names.add (entry.name);
    }
    return names;
}

bool PresetManager::importPreset (const juce::File& file)
{
    if (!PresetFormat::validatePresetFile (file))
        return false;

    juce::ValueTree state;
    PresetMetadata metadata;

    if (PresetFormat::readPreset (file, state, metadata))
    {
        juce::File destDir = getUserPresetDirectory();
        juce::File destFile = destDir.getChildFile (file.getFileName());

        // Handle duplicate names
        int counter = 1;
        juce::String baseName = metadata.name;
        while (destFile.exists())
        {
            metadata.name = baseName + " (" + juce::String (counter++) + ")";
            destFile = destDir.getChildFile (metadata.name + PresetFormat::FILE_EXTENSION);
        }

        if (file.copyFileTo (destFile))
        {
            PresetEntry entry;
            entry.name = metadata.name;
            entry.file = destFile;
            entry.metadata = metadata;
            entry.isFactory = false;

            addPresetEntry (entry);
            return true;
        }
    }
    return false;
}

bool PresetManager::exportPreset (int index, const juce::File& file)
{
    if (index < 0 || index >= presets.size())
        return false;

    const auto& entry = presets[index];
    juce::ValueTree state = parameterManager.getAPVTS().copyState();
    auto metadata = entry.metadata;
    metadata.numParameters = state.getNumChildren();

    auto now = std::time (nullptr);
    auto* tm = std::localtime (&now);
    char buffer[64];
    std::strftime (buffer, sizeof (buffer), "%Y-%m-%d %H:%M:%S", tm);
    metadata.dateModified = buffer;

    return PresetFormat::writePreset (file, state, metadata);
}

void PresetManager::saveCurrentState (juce::MemoryOutputStream& stream)
{
    parameterManager.saveState (stream);
}

void PresetManager::loadCurrentState (juce::MemoryInputStream& stream)
{
    parameterManager.loadState (stream);
}

juce::ValueTree PresetManager::getCurrentState() const
{
    return parameterManager.getAPVTS().copyState();
}

void PresetManager::setCurrentState (const juce::ValueTree& state)
{
    parameterManager.getAPVTS().replaceState (state);
    currentState = state;
}

juce::File PresetManager::getFactoryPresetDirectory()
{
    // Factory presets embedded in plugin binary - use temp location for now
    return juce::File::getSpecialLocation (juce::File::tempDirectory)
           .getChildFile ("ZalaxyMusic/Presets/Factory");
}

juce::File PresetManager::getUserPresetDirectory()
{
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
           .getChildFile ("ZalaxyMusic/Presets/User");
}

void PresetManager::scanPresetDirectories()
{
    presets.clear();

    // Scan factory
    juce::File factoryDir = getFactoryPresetDirectory();
    if (factoryDir.exists())
    {
        juce::Array<juce::File> files;
        factoryDir.findChildFiles (files, juce::File::findFiles, false, "*" + juce::String (PresetFormat::FILE_EXTENSION));

        for (const auto& file : files)
        {
            PresetEntry entry;
            entry.file = file;
            entry.isFactory = true;
            entry.name = file.getFileNameWithoutExtension();

            PresetMetadata metadata;
            if (PresetFormat::getPresetInfo (file, metadata))
            {
                entry.metadata = metadata;
            }
            else
            {
                entry.metadata = PresetFormat::createDefaultMetadata (entry.name, true);
            }

            addPresetEntry (entry);
        }
    }

    // Scan user
    juce::File userDir = getUserPresetDirectory();
    if (!userDir.exists())
        userDir.createDirectory();

    juce::Array<juce::File> userFiles;
    userDir.findChildFiles (userFiles, juce::File::findFiles, false, "*" + juce::String (PresetFormat::FILE_EXTENSION));

    for (const auto& file : userFiles)
    {
        PresetEntry entry;
        entry.file = file;
        entry.isFactory = false;
        entry.name = file.getFileNameWithoutExtension();

        PresetMetadata metadata;
        if (PresetFormat::getPresetInfo (file, metadata))
        {
            entry.metadata = metadata;
        }
        else
        {
            entry.metadata = PresetFormat::createDefaultMetadata (entry.name, false);
        }

        addPresetEntry (entry);
    }
}

void PresetManager::createFactoryPresets()
{
    juce::File factoryDir = getFactoryPresetDirectory();
    if (!factoryDir.exists())
        factoryDir.createDirectory();

    // Define factory presets
    struct FactoryPreset
    {
        const char* name;
        const char* category;
        const char* description;
        std::function<void()> configure;
    };

    std::vector<FactoryPreset> factoryPresets = {
        {
            "Init Patch",
            "Basic",
            "Clean starting point",
            [this]()
            {
                parameterManager.resetToDefaults();
            }
        },
        {
            "Analog Bass",
            "Bass",
            "Warm analog-style bass",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 2.0f); // Saw
                parameterManager.setParameterValue ("osc1Level", 1.0f);
                parameterManager.setParameterValue ("osc2Waveform", 3.0f); // Square
                parameterManager.setParameterValue ("osc2Level", 0.5f);
                parameterManager.setParameterValue ("osc2Pitch", -12.0f);
                parameterManager.setParameterValue ("filter1Type", 0.0f); // LP
                parameterManager.setParameterValue ("filter1Cutoff", 800.0f);
                parameterManager.setParameterValue ("filter1Resonance", 2.0f);
                parameterManager.setParameterValue ("filter1Keytrack", 0.5f);
                parameterManager.setParameterValue ("ampAttack", 0.005f);
                parameterManager.setParameterValue ("ampDecay", 0.15f);
                parameterManager.setParameterValue ("ampSustain", 0.6f);
                parameterManager.setParameterValue ("ampRelease", 0.2f);
            }
        },
        {
            "Bright Lead",
            "Lead",
            "Cutting lead with filter envelope",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 2.0f); // Saw
                parameterManager.setParameterValue ("osc1Level", 0.8f);
                parameterManager.setParameterValue ("osc2Waveform", 2.0f);
                parameterManager.setParameterValue ("osc2Level", 0.6f);
                parameterManager.setParameterValue ("osc2Pitch", 12.0f);
                parameterManager.setParameterValue ("osc2FineTune", 3.0f);
                parameterManager.setParameterValue ("filter1Type", 0.0f);
                parameterManager.setParameterValue ("filter1Cutoff", 2000.0f);
                parameterManager.setParameterValue ("filter1Resonance", 3.0f);
                parameterManager.setParameterValue ("filter1Keytrack", 0.8f);
                parameterManager.setParameterValue ("filterEnvAmount", 0.7f);
                parameterManager.setParameterValue ("filterAttack", 0.001f);
                parameterManager.setParameterValue ("filterDecay", 0.08f);
                parameterManager.setParameterValue ("filterSustain", 0.3f);
                parameterManager.setParameterValue ("filterRelease", 0.1f);
                parameterManager.setParameterValue ("ampAttack", 0.001f);
                parameterManager.setParameterValue ("ampDecay", 0.1f);
                parameterManager.setParameterValue ("ampSustain", 0.8f);
                parameterManager.setParameterValue ("ampRelease", 0.15f);
            }
        },
        {
            "Evolving Pad",
            "Pad",
            "Slow evolving atmospheric pad",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 0.0f); // Sine
                parameterManager.setParameterValue ("osc1Level", 0.7f);
                parameterManager.setParameterValue ("osc2Waveform", 2.0f); // Saw
                parameterManager.setParameterValue ("osc2Level", 0.4f);
                parameterManager.setParameterValue ("osc2Pitch", -12.0f);
                parameterManager.setParameterValue ("osc3Waveform", 5.0f); // Wavetable
                parameterManager.setParameterValue ("osc3Level", 0.3f);
                parameterManager.setParameterValue ("filter1Type", 0.0f);
                parameterManager.setParameterValue ("filter1Cutoff", 3000.0f);
                parameterManager.setParameterValue ("filter1Resonance", 1.5f);
                parameterManager.setParameterValue ("lfo1Waveform", 0.0f);
                parameterManager.setParameterValue ("lfo1Rate", 0.15f);
                parameterManager.setParameterValue ("lfo1Depth", 0.3f);
                parameterManager.setParameterValue ("lfo1Delay", 1.0f);
                parameterManager.setParameterValue ("lfo1FadeIn", 2.0f);
                parameterManager.setParameterValue ("modSlot1Source", 1.0f); // LFO1
                parameterManager.setParameterValue ("modSlot1Dest", 10.0f); // Filter1 Cutoff
                parameterManager.setParameterValue ("modSlot1Amount", 0.4f);
                parameterManager.setParameterValue ("ampAttack", 2.0f);
                parameterManager.setParameterValue ("ampDecay", 1.0f);
                parameterManager.setParameterValue ("ampSustain", 0.8f);
                parameterManager.setParameterValue ("ampRelease", 3.0f);
                parameterManager.setParameterValue ("reverbEnabled", 1.0f);
                parameterManager.setParameterValue ("reverbRoomSize", 0.8f);
                parameterManager.setParameterValue ("reverbWetLevel", 0.5f);
            }
        },
        {
            "Plucky Synth",
            "Pluck",
            "Short percussive pluck",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 3.0f); // Square
                parameterManager.setParameterValue ("osc1Level", 1.0f);
                parameterManager.setParameterValue ("osc1PulseWidth", 0.3f);
                parameterManager.setParameterValue ("filter1Type", 0.0f);
                parameterManager.setParameterValue ("filter1Cutoff", 4000.0f);
                parameterManager.setParameterValue ("filter1Resonance", 2.5f);
                parameterManager.setParameterValue ("filterEnvAmount", 0.8f);
                parameterManager.setParameterValue ("filterAttack", 0.001f);
                parameterManager.setParameterValue ("filterDecay", 0.05f);
                parameterManager.setParameterValue ("filterSustain", 0.1f);
                parameterManager.setParameterValue ("filterRelease", 0.05f);
                parameterManager.setParameterValue ("ampAttack", 0.001f);
                parameterManager.setParameterValue ("ampDecay", 0.08f);
                parameterManager.setParameterValue ("ampSustain", 0.0f);
                parameterManager.setParameterValue ("ampRelease", 0.1f);
            }
        },
        {
            "FM Bell",
            "FM",
            "Metallic FM-style bell",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 0.0f); // Sine carrier
                parameterManager.setParameterValue ("osc1Level", 1.0f);
                parameterManager.setParameterValue ("osc2Waveform", 0.0f); // Sine modulator
                parameterManager.setParameterValue ("osc2Level", 0.0f); // Level via FM
                parameterManager.setParameterValue ("osc2Pitch", 12.0f);
                parameterManager.setParameterValue ("osc1PhaseMod", 0.6f);
                parameterManager.setParameterValue ("filter1Type", 0.0f);
                parameterManager.setParameterValue ("filter1Cutoff", 6000.0f);
                parameterManager.setParameterValue ("ampAttack", 0.001f);
                parameterManager.setParameterValue ("ampDecay", 0.5f);
                parameterManager.setParameterValue ("ampSustain", 0.0f);
                parameterManager.setParameterValue ("ampRelease", 1.5f);
                parameterManager.setParameterValue ("reverbEnabled", 1.0f);
                parameterManager.setParameterValue ("reverbRoomSize", 0.7f);
                parameterManager.setParameterValue ("reverbWetLevel", 0.4f);
            }
        },
        {
            "Wobble Bass",
            "Bass",
            "Dubstep-style wobble bass",
            [this]()
            {
                parameterManager.resetToDefaults();
                parameterManager.setParameterValue ("osc1Waveform", 2.0f); // Saw
                parameterManager.setParameterValue ("osc1Level", 1.0f);
                parameterManager.setParameterValue ("osc2Waveform", 2.0f);
                parameterManager.setParameterValue ("osc2Level", 0.7f);
                parameterManager.setParameterValue ("osc2Pitch", -12.0f);
                parameterManager.setParameterValue ("filter1Type", 0.0f);
                parameterManager.setParameterValue ("filter1Cutoff", 200.0f);
                parameterManager.setParameterValue ("filter1Resonance", 8.0f);
                parameterManager.setParameterValue ("filter1Drive", 0.5f);
                parameterManager.setParameterValue ("lfo1Waveform", 0.0f);
                parameterManager.setParameterValue ("lfo1Rate", 8.0f);
                parameterManager.setParameterValue ("lfo1Depth", 1.0f);
                parameterManager.setParameterValue ("modSlot1Source", 1.0f); // LFO1
                parameterManager.setParameterValue ("modSlot1Dest", 10.0f); // Filter1 Cutoff
                parameterManager.setParameterValue ("modSlot1Amount", 1.0f);
                parameterManager.setParameterValue ("ampAttack", 0.01f);
                parameterManager.setParameterValue ("ampDecay", 0.2f);
                parameterManager.setParameterValue ("ampSustain", 0.5f);
                parameterManager.setParameterValue ("ampRelease", 0.1f);
                parameterManager.setParameterValue ("distortionEnabled", 1.0f);
                parameterManager.setParameterValue ("distortionType", 0.0f);
                parameterManager.setParameterValue ("distortionDrive", 15.0f);
                parameterManager.setParameterValue ("distortionMix", 0.4f);
            }
        }
    };

    for (const auto& fp : factoryPresets)
    {
        fp.configure();

        auto metadata = PresetFormat::createDefaultMetadata (fp.name, true);
        metadata.category = fp.category;
        metadata.description = fp.description;

        juce::ValueTree state = parameterManager.getAPVTS().copyState();
        metadata.numParameters = state.getNumChildren();

        juce::File presetFile = factoryDir.getChildFile (fp.name + PresetFormat::FILE_EXTENSION);
        if (PresetFormat::writePreset (presetFile, state, metadata))
        {
            PresetEntry entry;
            entry.name = fp.name;
            entry.file = presetFile;
            entry.metadata = metadata;
            entry.isFactory = true;

            // Check if already exists
            if (findPresetByName (fp.name) < 0)
            {
                addPresetEntry (entry);
            }
        }
    }
}

void PresetManager::addPresetEntry (const PresetEntry& entry)
{
    // Insert sorted by name (factory first)
    int insertIndex = 0;
    for (int i = 0; i < presets.size(); ++i)
    {
        if (presets[i].isFactory && !entry.isFactory)
        {
            insertIndex = i + 1;
        }
        else if (!presets[i].isFactory && entry.isFactory)
        {
            insertIndex = i;
            break;
        }
        else if (entry.name.compareNatural (presets[i].name) < 0)
        {
            insertIndex = i;
            break;
        }
        else
        {
            insertIndex = i + 1;
        }
    }

    presets.insert (insertIndex, entry);
}

int PresetManager::findPresetByName (const juce::String& name) const
{
    for (int i = 0; i < presets.size(); ++i)
    {
        if (presets[i].name == name)
            return i;
    }
    return -1;
}

} // namespace zalaxy