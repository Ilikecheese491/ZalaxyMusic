#include "PatchResponse.h"

namespace zalaxy
{

juce::String PatchResponse::getExplanation (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
    {
        const auto explanation = obj->getProperty (juce::String ("explanation"));
        return explanation.isString() ? explanation.toString() : "No explanation provided.";
    }

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
                    const auto idVar = p->getProperty (juce::String ("id"));
                    const auto valueVar = p->getProperty (juce::String ("value"));

                    if (idVar.isString())
                    {
                        juce::String id = idVar.toString();

                        float numericValue = 0.0f;

                        if (valueVar.isDouble())
                            numericValue = static_cast<float> (static_cast<double> (valueVar));
                        else if (valueVar.isInt())
                            numericValue = static_cast<float> (static_cast<int> (valueVar));
                        else if (valueVar.isInt64())
                            numericValue = static_cast<float> (static_cast<double> (static_cast<int64_t> (valueVar)));
                        else
                            continue;

                        result.add ({ id, numericValue });
                    }
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
