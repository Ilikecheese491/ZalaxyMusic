#include "LocalAIProvider.h"

namespace zalaxy
{

LocalAIProvider::LocalAIProvider() {}

LocalAIProvider::~LocalAIProvider() {}

void LocalAIProvider::requestTextToPatch (const juce::String& prompt, std::function<void(const juce::var&)> callback)
{
    juce::Timer::callAfterDelay (500, [callback, this, prompt]()
    {
        callback (createStubPatchResponse (prompt));
    });
}

void LocalAIProvider::requestAudioAnalysis (const juce::File& wavFile, double startTime, double endTime, std::function<void(const juce::var&)> callback)
{
    juce::Timer::callAfterDelay (800, [callback, this]()
    {
        callback (createStubAnalysisResponse());
    });
}

void LocalAIProvider::requestAssistantResponse (const juce::String& question, std::function<void(const juce::var&)> callback)
{
    juce::Timer::callAfterDelay (300, [callback, this, question]()
    {
        callback (createStubAssistantResponse (question));
    });
}

juce::var LocalAIProvider::createStubPatchResponse (const juce::String& prompt)
{
    auto obj = new juce::DynamicObject();
    obj->setProperty ("explanation", "Generated patch based on: " + prompt);

    juce::Array<juce::var> params;

    struct Param { const char* id; float value; };
    Param defaults[] = {
        {"osc1_waveform", 0.0f}, {"osc1_pitch", 0.5f}, {"osc1_detune", 0.5f}, {"osc1_level", 0.8f},
        {"osc2_waveform", 1.0f}, {"osc2_pitch", 0.5f}, {"osc2_detune", 0.5f}, {"osc2_level", 0.6f},
        {"filter_type", 0.0f}, {"filter_cutoff", 0.7f}, {"filter_resonance", 0.3f}, {"filter_env_amount", 0.5f},
        {"amp_attack", 0.1f}, {"amp_decay", 0.4f}, {"amp_sustain", 0.7f}, {"amp_release", 0.3f},
        {"filter_attack", 0.05f}, {"filter_decay", 0.3f}, {"filter_sustain", 0.5f}, {"filter_release", 0.4f},
        {"lfo1_rate", 0.3f}, {"lfo1_amount", 0.2f}, {"reverb_wet", 0.2f}, {"delay_wet", 0.15f}
    };

    for (auto& p : defaults)
    {
        auto param = new juce::DynamicObject();
        param->setProperty ("id", p.id);
        param->setProperty ("value", static_cast<double> (p.value));
        params.add (juce::var (param));
    }

    obj->setProperty ("parameters", params);
    return juce::var (obj);
}

juce::var LocalAIProvider::createStubAnalysisResponse()
{
    auto obj = new juce::DynamicObject();
    obj->setProperty ("explanation", "Analyzed reference audio. Generated matching patch.");
    return juce::var (obj); // or return createStubPatchResponse ("reference analysis");
}

juce::var LocalAIProvider::createStubAssistantResponse (const juce::String& question)
{
    auto obj = new juce::DynamicObject();

    juce::String response;
    if (question.containsIgnoreCase ("mix"))
        response = "For mixing: start with gain staging, use EQ to carve space, compress for control, reference commercial tracks.";
    else if (question.containsIgnoreCase ("master"))
        response = "For mastering: subtle EQ, gentle compression (1.5:1), limit to -1dB TP, check mono compatibility.";
    else if (question.containsIgnoreCase ("sound design") || question.containsIgnoreCase ("patch"))
        response = "For sound design: layer oscillators, modulate filter with envelope, add movement with LFO, use effects sparingly.";
    else if (question.containsIgnoreCase ("arrang"))
        response = "For arrangement: establish groove first, add elements every 8-16 bars, use tension/release, leave space.";
    else
        response = "I'm a stub AI. Ask about mixing, mastering, sound design, or arrangement for more specific advice.";

    obj->setProperty ("response", response);
    return juce::var (obj);
}

} // namespace zalaxy