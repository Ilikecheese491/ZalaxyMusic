#include "ReferenceAnalyzer.h"

namespace zalaxy
{

ReferenceAnalyzer::ReferenceAnalyzer()
    : processor (std::make_unique<AudioReferenceProcessor>())
    , spectralAnalyzer (std::make_unique<SpectralAnalyzer> (2048))
{}

ReferenceAnalyzer::~ReferenceAnalyzer() {}

juce::var ReferenceAnalyzer::analyzeReference (const juce::File& wavFile, double startTime, double endTime)
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();

    if (!processor->loadFile (wavFile))
    {
        obj->setProperty ("error", "Failed to load audio file");
        return obj;
    }

    processor->processRange (startTime, endTime);

    // Process audio for spectral features
    const auto& buffer = processor->getBuffer();
    int numSamples = buffer.getNumSamples();
    int channel = 0; // Use first channel for analysis

    spectralAnalyzer->processBlock (buffer.getReadPointer (channel), numSamples, static_cast<float> (processor->getSampleRate()));

    auto features = spectralAnalyzer->getFeatures();
    auto params = mapFeaturesToParameters (features);

    obj->setProperty ("explanation", generateExplanation (features));

    juce::Array<juce::var> paramsArray;
    for (auto& param : params)
    {
        juce::DynamicObject::Ptr p = new juce::DynamicObject();
        p->setProperty ("id", param.first);
        p->setProperty ("value", param.second);
        paramsArray.add (p);
    }
    obj->setProperty ("parameters", paramsArray);

    return obj;
}

juce::Array<std::pair<juce::String, float>> ReferenceAnalyzer::mapFeaturesToParameters (const SpectralFeatures& features)
{
    juce::Array<std::pair<juce::String, float>> result;

    // Map spectral features to synth parameters (simplified heuristic)
    result.add ({"osc1_waveform", features.brightness > 0.5f ? 1.0f : 0.0f}); // Saw for bright, sine for dull
    result.add ({"osc1_pitch", 0.5f});
    result.add ({"osc1_level", 0.8f});

    result.add ({"osc2_waveform", features.noisiness > 0.5f ? 2.0f : 1.0f}); // Noise for noisy
    result.add ({"osc2_pitch", features.fundamental / 440.0f * 0.5f + 0.5f}); // Relative to A4
    result.add ({"osc2_level", 0.5f});

    result.add ({"filter_type", features.centroid > 2000 ? 1.0f : 0.0f}); // HP for bright
    result.add ({"filter_cutoff", juce::jmap (features.centroid, 100.0f, 8000.0f, 0.2f, 0.9f)});
    result.add ({"filter_resonance", features.flatness * 0.5f});
    result.add ({"filter_env_amount", 0.4f});

    result.add ({"amp_attack", features.flux > 0.5f ? 0.01f : 0.1f});
    result.add ({"amp_decay", 0.4f});
    result.add ({"amp_sustain", features.brightness > 0.5f ? 0.3f : 0.7f});
    result.add ({"amp_release", 0.3f});

    result.add ({"filter_attack", 0.05f});
    result.add ({"filter_decay", 0.3f});
    result.add ({"filter_sustain", 0.5f});
    result.add ({"filter_release", 0.4f});

    return result;
}

juce::String ReferenceAnalyzer::generateExplanation (const SpectralFeatures& features)
{
    juce::String desc = "Analyzed reference: ";
    desc << "centroid=" << juce::String (features.centroid, 0) << "Hz";
    desc << ", flatness=" << juce::String (features.flatness, 2);
    desc << ", flux=" << juce::String (features.flux, 2);
    desc << ", fundamental=" << juce::String (features.fundamental, 0) << "Hz";
    desc << ". Mapped to synth parameters.";

    return desc;
}

} // namespace zalaxy