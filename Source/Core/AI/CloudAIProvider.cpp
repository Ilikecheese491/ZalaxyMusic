#include "CloudAIProvider.h"
#include "UsageTracker.h"
#include "Licensing/SubscriptionManager.h"

namespace zalaxy
{

const std::map<juce::String, CloudAIProvider::ProviderConfig> CloudAIProvider::providerConfigs = {
    { "OpenRouter", { "OpenRouter", "https://openrouter.ai/api/v1/chat/completions", "openrouter/auto", 4096, 0.0005 } },
    { "Together AI", { "Together AI", "https://api.together.xyz/v1/chat/completions", "meta-llama/Llama-3.2-3B-Instruct-Turbo", 4096, 0.0002 } },
    { "Groq", { "Groq", "https://api.groq.com/openai/v1/chat/completions", "llama-3.1-8b-instant", 8192, 0.0 } }
};

CloudAIProvider::CloudAIProvider (UsageTracker& usage, SubscriptionManager& sub)
    : usageTracker (usage),
      subscriptionManager (sub)
{
    auto it = providerConfigs.find ("OpenRouter");
    if (it != providerConfigs.end())
    {
        currentProviderName = it->first;
        currentProviderConfig = it->second;
    }
}

CloudAIProvider::~CloudAIProvider()
{
}

void CloudAIProvider::initialize()
{
}

void CloudAIProvider::requestTextToPatch (const juce::String& prompt, std::function<void(const juce::var&)> callback)
{
    if (!subscriptionManager.canUseCloudAI())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "Cloud AI requires Pro license or active trial");
        callback (juce::var (error.get()));
        return;
    }

    if (!hasValidAPIKey())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "No API key configured. Add one in Settings.");
        callback (juce::var (error.get()));
        return;
    }

    const juce::String systemPrompt = getSystemPromptForTask ("text_to_patch");
    const juce::String body = buildRequestBody (prompt, systemPrompt);

    sendRequest (currentProviderConfig.baseURL, body, [this, callback] (const juce::var& response)
    {
        if (validateResponse (response))
        {
            int promptTokens = 0;
            int completionTokens = 0;

            if (auto* obj = response.getDynamicObject())
            {
                const juce::var usageVar = obj->getProperty ("usage");
                if (usageVar.isObject())
                {
                    if (auto* usage = usageVar.getDynamicObject())
                    {
                        const juce::var promptVar = usage->getProperty ("prompt_tokens");
                        const juce::var completionVar = usage->getProperty ("completion_tokens");

                        if (promptVar.isInt()) promptTokens = promptVar;
                        if (completionVar.isInt()) completionTokens = completionVar;
                    }
                }
            }

            usageTracker.recordUsage (promptTokens, completionTokens, currentProviderName, currentProviderConfig.defaultModel);
            callback (response);
        }
        else
        {
            juce::DynamicObject::Ptr error = new juce::DynamicObject();
            error->setProperty ("error", "Invalid AI response format");
            callback (juce::var (error.get()));
        }
    });
}

void CloudAIProvider::requestAudioAnalysis (const juce::File& wavFile, double startTime, double endTime, std::function<void(const juce::var&)> callback)
{
    if (!subscriptionManager.canUseCloudAI())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "Cloud AI requires Pro license or active trial");
        callback (juce::var (error.get()));
        return;
    }

    if (!hasValidAPIKey())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "No API key configured. Add one in Settings.");
        callback (juce::var (error.get()));
        return;
    }

    juce::String prompt = "Analyze this audio reference and generate a matching synth patch. "
                          "File: " + wavFile.getFileName() + ", Range: " + juce::String (startTime) + "-" + juce::String (endTime) + "s";

    requestTextToPatch (prompt, callback);
}

void CloudAIProvider::requestAssistantResponse (const juce::String& question, std::function<void(const juce::var&)> callback)
{
    if (!subscriptionManager.canUseCloudAI())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "Cloud AI requires Pro license or active trial");
        callback (juce::var (error.get()));
        return;
    }

    if (!hasValidAPIKey())
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "No API key configured. Add one in Settings.");
        callback (juce::var (error.get()));
        return;
    }

    const juce::String systemPrompt = getSystemPromptForTask ("assistant");
    const juce::String body = buildRequestBody (question, systemPrompt);

    sendRequest (currentProviderConfig.baseURL, body, [this, callback] (const juce::var& response)
    {
        if (validateResponse (response))
        {
            int promptTokens = 0;
            int completionTokens = 0;

            if (auto* obj = response.getDynamicObject())
            {
                const juce::var usageVar = obj->getProperty ("usage");
                if (usageVar.isObject())
                {
                    if (auto* usage = usageVar.getDynamicObject())
                    {
                        const juce::var promptVar = usage->getProperty ("prompt_tokens");
                        const juce::var completionVar = usage->getProperty ("completion_tokens");

                        if (promptVar.isInt()) promptTokens = promptVar;
                        if (completionVar.isInt()) completionTokens = completionVar;
                    }
                }
            }

            usageTracker.recordUsage (promptTokens, completionTokens, currentProviderName, currentProviderConfig.defaultModel);
            callback (response);
        }
        else
        {
            juce::DynamicObject::Ptr error = new juce::DynamicObject();
            error->setProperty ("error", "Invalid AI response format");
            callback (juce::var (error.get()));
        }
    });
}

void CloudAIProvider::setProvider (const juce::String& providerName)
{
    auto it = providerConfigs.find (providerName);
    if (it != providerConfigs.end())
    {
        currentProviderName = it->first;
        currentProviderConfig = it->second;
    }
}

juce::StringArray CloudAIProvider::getAvailableProviders() const
{
    juce::StringArray providers;
    for (const auto& [name, config] : providerConfigs)
    {
        (void) config;
        providers.add (name);
    }
    return providers;
}

bool CloudAIProvider::setAPIKey (const juce::String& provider, const juce::String& key)
{
    if (providerConfigs.find (provider) != providerConfigs.end())
    {
        apiKeys[provider] = key;
        return true;
    }
    return false;
}

juce::String CloudAIProvider::getAPIKey (const juce::String& provider) const
{
    auto it = apiKeys.find (provider);
    if (it != apiKeys.end())
        return it->second;
    return "";
}

bool CloudAIProvider::hasValidAPIKey() const
{
    auto it = apiKeys.find (currentProviderName);
    return it != apiKeys.end() && it->second.isNotEmpty();
}

void CloudAIProvider::clearAPIKey (const juce::String& provider)
{
    apiKeys.erase (provider);
}

void CloudAIProvider::setCloudMode (bool enabled)
{
    cloudModeEnabled = enabled;
}

void CloudAIProvider::recordUsage (int promptTokens, int completionTokens)
{
    usageTracker.recordUsage (promptTokens, completionTokens, currentProviderName, currentProviderConfig.defaultModel);
}

double CloudAIProvider::getEstimatedCost() const
{
    return usageTracker.getEstimatedCost();
}

void CloudAIProvider::testConnection (std::function<void(bool, const juce::String&)> callback)
{
    if (!hasValidAPIKey())
    {
        callback (false, "No API key configured");
        return;
    }

    const juce::String testPrompt = "Test connection - reply with 'OK'";
    const juce::String body = buildRequestBody (testPrompt, "You are a test endpoint. Reply only with 'OK'.");

    sendRequest (currentProviderConfig.baseURL, body, [this, callback] (const juce::var& response)
    {
        if (validateResponse (response))
            callback (true, "Connection successful");
        else
            callback (false, "Invalid response from provider");
    });
}

juce::String CloudAIProvider::buildRequestBody (const juce::String& prompt, const juce::String& systemPrompt) const
{
    juce::DynamicObject::Ptr request = new juce::DynamicObject();
    request->setProperty ("model", currentProviderConfig.defaultModel);
    request->setProperty ("max_tokens", currentProviderConfig.maxTokens);
    request->setProperty ("temperature", 0.7);

    juce::Array<juce::var> messages;

    juce::DynamicObject::Ptr sysMsg = new juce::DynamicObject();
    sysMsg->setProperty ("role", "system");
    sysMsg->setProperty ("content", systemPrompt);
    messages.add (juce::var (sysMsg.get()));

    juce::DynamicObject::Ptr userMsg = new juce::DynamicObject();
    userMsg->setProperty ("role", "user");
    userMsg->setProperty ("content", prompt);
    messages.add (juce::var (userMsg.get()));

    request->setProperty ("messages", messages);

    return juce::JSON::toString (juce::var (request.get()));
}

void CloudAIProvider::sendRequest (const juce::String& endpoint, const juce::String& body, std::function<void(const juce::var&)> callback)
{
    juce::URL url (endpoint);

    juce::URL::InputStreamOptions options (juce::URL::ParameterHandling::inAddress);
    options.withHeaders ({"Content-Type", "application/json"});

    if (hasValidAPIKey())
        options.withHeaders ({"Authorization", "Bearer " + getAPIKey (currentProviderName)});

    auto stream = url.withPOSTData (body).createInputStream (options);

    if (stream == nullptr)
    {
        juce::DynamicObject::Ptr error = new juce::DynamicObject();
        error->setProperty ("error", "Failed to create network request");
        callback (juce::var (error.get()));
        return;
    }

    auto sharedStream = std::shared_ptr<juce::InputStream> (std::move (stream));

    juce::Thread::launch ([sharedStream, callback]()
    {
        const juce::String responseText = sharedStream->readEntireStreamAsString();
        const juce::var parsed = juce::JSON::parse (responseText);

        juce::MessageManager::callAsync ([callback, parsed]()
        {
            callback (parsed);
        });
    });
}

juce::String CloudAIProvider::getSystemPromptForTask (const juce::String& task) const
{
    if (task == "text_to_patch")
    {
        return R"(You are a synth patch generator. Output ONLY valid JSON with this exact structure:
{
  "explanation": "Brief description of the patch",
  "parameters": [
    {"id": "osc1_waveform", "value": 0.0},
    {"id": "osc1_pitch", "value": 0.5},
    {"id": "filter_cutoff", "value": 0.7},
    // ... all 80+ parameters with values 0.0-1.0
  ]
}
Use these parameter IDs: osc1_waveform, osc1_pitch, osc1_detune, osc1_level, osc1_phase, osc1_wt_position,
osc2_waveform, osc2_pitch, osc2_detune, osc2_level, osc2_phase, osc2_wt_position,
noise_level, sub_osc_level,
filter_type, filter_cutoff, filter_resonance, filter_drive, filter_env_amount, filter_key_track,
filter_pan, filter_mix,
amp_attack, amp_decay, amp_sustain, amp_release, amp_vel_track,
filter_attack, filter_decay, filter_sustain, filter_release,
lfo1_rate, lfo1_waveform, lfo1_amount, lfo1_target, lfo1_fade_in, lfo1_sync,
lfo2_rate, lfo2_waveform, lfo2_amount, lfo2_target, lfo2_fade_in, lfo2_sync,
env3_attack, env3_decay, env3_sustain, env3_release, env3_target, env3_amount,
reverb_enabled, reverb_room_size, reverb_damping, reverb_wet_level, reverb_dry_level, reverb_width, reverb_pre_delay,
delay_enabled, delay_time, delay_feedback, delay_wet_level, delay_tempo_sync, delay_note_value,
distortion_enabled, distortion_type, distortion_drive, distortion_mix, distortion_tone,
chorus_enabled, chorus_rate, chorus_depth, chorus_feedback, chorus_mix,
master_volume, master_pan)";
    }
    else if (task == "assistant")
    {
        return "You are a music production assistant. Give concise, practical advice on mixing, mastering, sound design, and arrangement. Format responses as plain text, not JSON.";
    }
    else if (task == "audio_analysis")
    {
        return "You are an audio analysis expert. Given audio characteristics, output a synth patch JSON as in text_to_patch task.";
    }

    return "You are a helpful assistant.";
}

bool CloudAIProvider::validateResponse (const juce::var& response) const
{
    if (!response.isObject())
        return false;

    auto* obj = response.getDynamicObject();
    if (obj == nullptr)
        return false;

    if (obj->hasProperty ("choices"))
    {
        const juce::var choices = obj->getProperty ("choices");
        if (choices.isArray() && choices.size() > 0)
            return true;
    }

    if (obj->hasProperty ("parameters") && obj->hasProperty ("explanation"))
        return true;

    return false;
}

} // namespace zalaxy