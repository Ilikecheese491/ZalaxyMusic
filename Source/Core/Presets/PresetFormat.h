#pragma once

#include "JuceModules.h"

namespace zalaxy
{

struct PresetMetadata
{
    juce::String name;
    juce::String author;
    juce::String category;
    juce::String description;
    juce::String version;
    juce::String dateCreated;
    juce::String dateModified;
    juce::String pluginVersion;
    int numParameters = 0;
    bool isFactory = false;
};

class PresetFormat
{
public:
    static constexpr const char* FILE_EXTENSION = ".zalaxy";
    static constexpr const char* FILE_IDENTIFIER = "ZLX1";
    static constexpr int CURRENT_VERSION = 1;

    static bool writePreset (const juce::File& file,
                             const juce::ValueTree& state,
                             const PresetMetadata& metadata);

    static bool readPreset (const juce::File& file,
                            juce::ValueTree& state,
                            PresetMetadata& metadata);

    static bool writePresetJSON (const juce::File& file,
                                 const juce::ValueTree& state,
                                 const PresetMetadata& metadata);

    static bool readPresetJSON (const juce::File& file,
                                juce::ValueTree& state,
                                PresetMetadata& metadata);

    static PresetMetadata createDefaultMetadata (const juce::String& name = "Untitled",
                                                  bool factory = false);

    static bool validatePresetFile (const juce::File& file);

    static bool getPresetInfo (const juce::File& file, PresetMetadata& metadata);

private:
    static juce::ValueTree createMetadataTree (const PresetMetadata& metadata);
    static PresetMetadata parseMetadataTree (const juce::ValueTree& tree);
    static bool writeBinaryHeader (juce::OutputStream& stream);
    static bool readBinaryHeader (juce::InputStream& stream);
};

} // namespace zalaxy