#include "PatchResponse.h"

namespace zalaxy
{

juce::String PatchResponse::getExplanation (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
        return obj->getProperty ("explanation", "No explanation provided.");
    return "Invalid response format.";
}

juce::Array<std::pair<juce::String, float>> PatchResponse::getParameters (const juce::var& response)
{
    juce::Array<std::pair<juce::String, float>> result;
    if (auto* obj = response.getDynamicObject())
    {
        if (auto* paramsArray = obj->getProperty ("parameters").getArray())
        {
            for (auto& param : *paramsArray)
            {
                if (auto* p = param.getDynamicObject())
                {
                    juce::String id = p->getProperty ("id").toString();
                    float value = p->getProperty ("value");
                    result.add ({id, value});
                }
            }
        }
    }
    return result;
}

bool PatchResponse::hasVariations (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
        return obj->hasProperty ("variations");
    return false;
}

} // namespace zalaxy