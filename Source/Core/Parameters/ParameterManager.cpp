#include "ParameterManager.h"
#include <algorithm>

namespace zalaxy
{

ParameterManager::ParameterManager (juce::AudioProcessor& p)
    : processor (p)
    , apvts (p, nullptr, "PARAMETERS", createParameterLayout())
{
}

ParameterManager::~ParameterManager()
{
}

void ParameterManager::initializeParameters()
{
    // Parameters are already created by APVTS constructor
    // This method can be used for post-initialization setup
}

void ParameterManager::prepare (double sampleRate)
{
    // Prepare parameter smoothing if needed
    // APVTS handles smoothing internally via RangedAudioParameter
}

juce::RangedAudioParameter* ParameterManager::getParameter (const juce::String& paramID)
{
    return apvts.getParameter (paramID);
}

const juce::RangedAudioParameter* ParameterManager::getParameter (const juce::String& paramID) const
{
    return apvts.getParameter (paramID);
}

float ParameterManager::getParameterValue (const juce::String& paramID) const
{
    if (auto* param = apvts.getParameter (paramID))
        return param->getValue();
    return 0.0f;
}

void ParameterManager::setParameterValue (const juce::String& paramID, float newValue)
{
    if (auto* param = apvts.getParameter (paramID))
    {
        param->setValueNotifyingHost (newValue);
    }
}

void ParameterManager::addParameterListener (const juce::String& paramID, juce::AudioProcessorValueTreeState::Listener* listener)
{
    apvts.addParameterListener (paramID, listener);
}

void ParameterManager::removeParameterListener (const juce::String& paramID, juce::AudioProcessorValueTreeState::Listener* listener)
{
    apvts.removeParameterListener (paramID, listener);
}

void ParameterManager::saveState (juce::MemoryOutputStream& stream)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml)
        xml->writeToStream (stream, juce::String(), false); // JUCE 9.0.4 signature
}

void ParameterManager::loadState (juce::MemoryInputStream& stream)
{
    juce::String xmlString = stream.readEntireStreamAsString();
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (xmlString));
    if (xml)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

void ParameterManager::resetToDefaults()
{
    for (const auto& paramInfo : ParameterSchema::getAllParameters())
    {
        if (auto* param = apvts.getParameter (paramInfo.id))
        {
            param->setValueNotifyingHost (paramInfo.defaultValue);
        }
    }
}

juce::StringArray ParameterManager::getParameterIDsForCategory (const juce::String& category) const
{
    juce::StringArray ids;
    for (const auto& paramInfo : ParameterSchema::getAllParameters())
    {
        if (paramInfo.category == category)
            ids.add (paramInfo.id);
    }
    return ids;
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ParameterLayout> ParameterManager::createParameterLayout()
{
    auto layout = std::make_unique<juce::AudioProcessorValueTreeState::ParameterLayout>();

    for (const auto& paramInfo : ParameterSchema::getAllParameters())
    {
        if (paramInfo.isMetaParameter)
            continue; // Skip meta parameters (UI-only)

        juce::RangedAudioParameter* param = nullptr;

        if (paramInfo.interval > 0.0f)
        {
            // Discrete parameter (choice/enum)
            juce::StringArray choices;
            int numSteps = static_cast<int> ((paramInfo.maxValue - paramInfo.minValue) / paramInfo.interval) + 1;
            for (int i = 0; i < numSteps; ++i)
            {
                float val = paramInfo.minValue + i * paramInfo.interval;
                choices.add (juce::String (val, 2));
            }
            param = new juce::AudioParameterChoice (paramInfo.id, paramInfo.name, choices,
                                                     static_cast<int> ((paramInfo.defaultValue - paramInfo.minValue) / paramInfo.interval));
        }
        else
        {
            // Continuous parameter
            auto range = juce::NormalisableRange<float> (paramInfo.minValue, paramInfo.maxValue, 0.001f, 1.0f);
            param = new juce::AudioParameterFloat (paramInfo.id, paramInfo.name, range, paramInfo.defaultValue);
        }

        if (param)
        {
            layout->add (std::unique_ptr<juce::RangedAudioParameter> (param));
        }
    }

    return layout;
}

juce::String ParameterManager::modSourceToString (ModSource source)
{
    switch (source)
    {
        case ModSource::None: return "None";
        case ModSource::LFO1: return "LFO 1";
        case ModSource::LFO2: return "LFO 2";
        case ModSource::AmpEnvelope: return "Amp Env";
        case ModSource::FilterEnvelope: return "Filter Env";
        case ModSource::ModEnvelope: return "Mod Env";
        case ModSource::Velocity: return "Velocity";
        case ModSource::KeyTrack: return "Key Track";
        case ModSource::ModWheel: return "Mod Wheel";
        case ModSource::PitchBend: return "Pitch Bend";
        case ModSource::Aftertouch: return "Aftertouch";
        case ModSource::Expression: return "Expression";
        case ModSource::Breath: return "Breath";
        case ModSource::Random: return "Random";
        case ModSource::StepSequencer: return "Step Seq";
        case ModSource::Macro1: return "Macro 1";
    }
    return "Unknown";
}

juce::String ParameterManager::modDestToString (ModDestination dest)
{
    switch (dest)
    {
        case ModDestination::None: return "None";
        case ModDestination::Osc1Pitch: return "Osc 1 Pitch";
        case ModDestination::Osc2Pitch: return "Osc 2 Pitch";
        case ModDestination::Osc3Pitch: return "Osc 3 Pitch";
        case ModDestination::Osc4Pitch: return "Osc 4 Pitch";
        case ModDestination::AllOscPitch: return "All Osc Pitch";
        case ModDestination::Osc1Level: return "Osc 1 Level";
        case ModDestination::Osc2Level: return "Osc 2 Level";
        case ModDestination::Osc3Level: return "Osc 3 Level";
        case ModDestination::Osc4Level: return "Osc 4 Level";
        case ModDestination::Filter1Cutoff: return "Filter 1 Cutoff";
        case ModDestination::Filter2Cutoff: return "Filter 2 Cutoff";
        case ModDestination::Filter1Resonance: return "Filter 1 Reso";
        case ModDestination::Filter2Resonance: return "Filter 2 Reso";
        case ModDestination::AmpLevel: return "Amp Level";
        case ModDestination::Pan: return "Pan";
        case ModDestination::LFO1Rate: return "LFO 1 Rate";
        case ModDestination::LFO2Rate: return "LFO 2 Rate";
        case ModDestination::LFO1Depth: return "LFO 1 Depth";
        case ModDestination::LFO2Depth: return "LFO 2 Depth";
        case ModDestination::Filter1Drive: return "Filter 1 Drive";
        case ModDestination::Filter2Drive: return "Filter 2 Drive";
        case ModDestination::ReverbMix: return "Reverb Mix";
        case ModDestination::DelayMix: return "Delay Mix";
        case ModDestination::DistortionDrive: return "Dist Drive";
        case ModDestination::MasterVolume: return "Master Vol";
    }
    return "Unknown";
}

ParameterManager::ModSource ParameterManager::stringToModSource (const juce::String& str)
{
    if (str == "LFO 1") return ModSource::LFO1;
    if (str == "LFO 2") return ModSource::LFO2;
    if (str == "Amp Env") return ModSource::AmpEnvelope;
    if (str == "Filter Env") return ModSource::FilterEnvelope;
    if (str == "Mod Env") return ModSource::ModEnvelope;
    if (str == "Velocity") return ModSource::Velocity;
    if (str == "Key Track") return ModSource::KeyTrack;
    if (str == "Mod Wheel") return ModSource::ModWheel;
    if (str == "Pitch Bend") return ModSource::PitchBend;
    if (str == "Aftertouch") return ModSource::Aftertouch;
    if (str == "Expression") return ModSource::Expression;
    if (str == "Breath") return ModSource::Breath;
    if (str == "Random") return ModSource::Random;
    if (str == "Step Seq") return ModSource::StepSequencer;
    if (str == "Macro 1") return ModSource::Macro1;
    return ModSource::None;
}

ParameterManager::ModDestination ParameterManager::stringToModDest (const juce::String& str)
{
    if (str == "Osc 1 Pitch") return ModDestination::Osc1Pitch;
    if (str == "Osc 2 Pitch") return ModDestination::Osc2Pitch;
    if (str == "Osc 3 Pitch") return ModDestination::Osc3Pitch;
    if (str == "Osc 4 Pitch") return ModDestination::Osc4Pitch;
    if (str == "All Osc Pitch") return ModDestination::AllOscPitch;
    if (str == "Osc 1 Level") return ModDestination::Osc1Level;
    if (str == "Osc 2 Level") return ModDestination::Osc2Level;
    if (str == "Osc 3 Level") return ModDestination::Osc3Level;
    if (str == "Osc 4 Level") return ModDestination::Osc4Level;
    if (str == "Filter 1 Cutoff") return ModDestination::Filter1Cutoff;
    if (str == "Filter 2 Cutoff") return ModDestination::Filter2Cutoff;
    if (str == "Filter 1 Reso") return ModDestination::Filter1Resonance;
    if (str == "Filter 2 Reso") return ModDestination::Filter2Resonance;
    if (str == "Amp Level") return ModDestination::AmpLevel;
    if (str == "Pan") return ModDestination::Pan;
    if (str == "LFO 1 Rate") return ModDestination::LFO1Rate;
    if (str == "LFO 2 Rate") return ModDestination::LFO2Rate;
    if (str == "LFO 1 Depth") return ModDestination::LFO1Depth;
    if (str == "LFO 2 Depth") return ModDestination::LFO2Depth;
    if (str == "Filter 1 Drive") return ModDestination::Filter1Drive;
    if (str == "Filter 2 Drive") return ModDestination::Filter2Drive;
    if (str == "Reverb Mix") return ModDestination::ReverbMix;
    if (str == "Delay Mix") return ModDestination::DelayMix;
    if (str == "Dist Drive") return ModDestination::DistortionDrive;
    if (str == "Master Vol") return ModDestination::MasterVolume;
    return ModDestination::None;
}

} // namespace zalaxy