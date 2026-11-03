#include "PresetFormat.h"
#include <ctime>

namespace zalaxy
{

bool PresetFormat::writeBinaryHeader (juce::OutputStream& stream)
{
    stream.writeString (FILE_IDENTIFIER);
    stream.writeInt (CURRENT_VERSION);
    return true; // OutputStream doesn't have getStatus() in JUCE 9.0.4
}

bool PresetFormat::readBinaryHeader (juce::InputStream& stream)
{
    juce::String identifier = stream.readString();
    int version = stream.readInt();

    if (identifier != FILE_IDENTIFIER)
        return false;

    if (version > CURRENT_VERSION)
        return false; // Future version

    return true;
}

juce::ValueTree PresetFormat::createMetadataTree (const PresetMetadata& metadata)
{
    juce::ValueTree meta ("METADATA");
    meta.setProperty ("name", metadata.name, nullptr);
    meta.setProperty ("author", metadata.author, nullptr);
    meta.setProperty ("category", metadata.category, nullptr);
    meta.setProperty ("description", metadata.description, nullptr);
    meta.setProperty ("version", metadata.version, nullptr);
    meta.setProperty ("dateCreated", metadata.dateCreated, nullptr);
    meta.setProperty ("dateModified", metadata.dateModified, nullptr);
    meta.setProperty ("pluginVersion", metadata.pluginVersion, nullptr);
    meta.setProperty ("numParameters", metadata.numParameters, nullptr);
    meta.setProperty ("isFactory", metadata.isFactory, nullptr);
    return meta;
}

PresetMetadata PresetFormat::parseMetadataTree (const juce::ValueTree& tree)
{
    PresetMetadata metadata;
    metadata.name = tree.getProperty ("name", "Untitled");
    metadata.author = tree.getProperty ("author", "Unknown");
    metadata.category = tree.getProperty ("category", "Uncategorized");
    metadata.description = tree.getProperty ("description", "");
    metadata.version = tree.getProperty ("version", "1.0");
    metadata.dateCreated = tree.getProperty ("dateCreated", "");
    metadata.dateModified = tree.getProperty ("dateModified", "");
    metadata.pluginVersion = tree.getProperty ("pluginVersion", "0.1.0");
    metadata.numParameters = tree.getProperty ("numParameters", 0);
    metadata.isFactory = tree.getProperty ("isFactory", false);
    return metadata;
}

PresetMetadata PresetFormat::createDefaultMetadata (const juce::String& name, bool factory)
{
    PresetMetadata metadata;
    metadata.name = name;
    metadata.author = "ZalaxyMusic";
    metadata.category = "User";
    metadata.description = "";
    metadata.version = "1.0";
    metadata.pluginVersion = "0.1.0";
    metadata.isFactory = factory;

    // Current date/time
    auto now = std::time (nullptr);
    auto* tm = std::localtime (&now);
    char buffer[64];
    std::strftime (buffer, sizeof (buffer), "%Y-%m-%d %H:%M:%S", tm);
    metadata.dateCreated = buffer;
    metadata.dateModified = buffer;

    return metadata;
}

bool PresetFormat::writePreset (const juce::File& file,
                                const juce::ValueTree& state,
                                const PresetMetadata& metadata)
{
    // Create a copy of state with metadata
    juce::ValueTree fullState ("ZALEXY_PRESET");
    fullState.addChild (createMetadataTree (metadata), 0, nullptr);
    fullState.addChild (state.createCopy(), 1, nullptr);

    // Write binary format
    juce::FileOutputStream fileStream (file);
    if (fileStream.failedToOpen())
        return false;

    if (!writeBinaryHeader (fileStream))
        return false;

    // Write the ValueTree as XML then compress to binary
    std::unique_ptr<juce::XmlElement> xml (fullState.createXml());
    if (!xml)
        return false;

    juce::MemoryBlock xmlData;
    juce::MemoryOutputStream memStream (xmlData, false);
    xml->writeToStream (memStream, juce::String(), false); // JUCE 9.0.4 signature

    // Compress using GZIPCompressorOutputStream
    juce::MemoryBlock compressed;
    {
        juce::MemoryOutputStream compressedStream (compressed, false);
        juce::GZIPCompressorOutputStream gzipStream (compressedStream, true);
        gzipStream.write (xmlData.getData(), xmlData.getSize());
        // gzipStream destructor flushes
    }

    // Write compressed size and data
    fileStream.writeInt ((int) compressed.getSize());
    fileStream.write (compressed.getData(), compressed.getSize());

    return true;
}

bool PresetFormat::readPreset (const juce::File& file,
                               juce::ValueTree& state,
                               PresetMetadata& metadata)
{
    juce::FileInputStream fileStream (file);
    if (fileStream.failedToOpen())
        return false;

    if (!readBinaryHeader (fileStream))
        return false;

    // Read compressed size
    int compressedSize = fileStream.readInt();
    if (compressedSize <= 0 || compressedSize > 1024 * 1024) // Max 1MB
        return false;

    // Read compressed data
    juce::MemoryBlock compressed (compressedSize);
    fileStream.read (compressed.getData(), compressedSize);

    // Decompress
    juce::MemoryBlock xmlData;
    {
        juce::MemoryInputStream compressedStream (compressed, false);
        juce::GZIPDecompressorInputStream decompressor (compressedStream);
        decompressor.readIntoMemoryBlock (xmlData);
    }

    // Parse XML from MemoryBlock via MemoryInputStream
    juce::MemoryInputStream xmlStream (xmlData, false);
    juce::String xmlString = xmlStream.readEntireStreamAsString();
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (xmlString));
    if (!xml)
        return false;

    juce::ValueTree fullState = juce::ValueTree::fromXml (*xml);
    if (!fullState.isValid() || fullState.getNumChildren() < 2)
        return false;

    // Extract metadata
    metadata = parseMetadataTree (fullState.getChild (0));

    // Extract parameter state
    state = fullState.getChild (1);

    return true;
}

bool PresetFormat::writePresetJSON (const juce::File& file,
                                    const juce::ValueTree& state,
                                    const PresetMetadata& metadata)
{
    juce::ValueTree fullState ("ZALEXY_PRESET");
    fullState.addChild (createMetadataTree (metadata), 0, nullptr);
    fullState.addChild (state.createCopy(), 1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (fullState.createXml());
    if (!xml)
        return false;

    juce::FileOutputStream stream (file);
    if (stream.failedToOpen())
        return false;

    xml->writeToStream (stream, juce::String(), false); // JUCE 9.0.4 signature
    return true;
}

bool PresetFormat::readPresetJSON (const juce::File& file,
                                   juce::ValueTree& state,
                                   PresetMetadata& metadata)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (file));
    if (!xml)
        return false;

    juce::ValueTree fullState = juce::ValueTree::fromXml (*xml);
    if (!fullState.isValid() || fullState.getNumChildren() < 2)
        return false;

    metadata = parseMetadataTree (fullState.getChild (0));
    state = fullState.getChild (1);

    return true;
}

bool PresetFormat::validatePresetFile (const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    juce::String ext = file.getFileExtension().toLowerCase();
    if (ext != "zalaxy")
        return false;

    // Try to read header
    juce::FileInputStream stream (file);
    if (stream.failedToOpen())
        return false;

    return readBinaryHeader (stream);
}

bool PresetFormat::getPresetInfo (const juce::File& file, PresetMetadata& metadata)
{
    juce::FileInputStream stream (file);
    if (stream.failedToOpen())
        return false;

    if (!readBinaryHeader (stream))
        return false;

    // Skip compressed size
    stream.readInt();

    // We'd need to decompress just to get metadata - for now return false
    // In production, we'd store metadata separately or have a quick-read mode
    return false;
}

} // namespace zalaxy