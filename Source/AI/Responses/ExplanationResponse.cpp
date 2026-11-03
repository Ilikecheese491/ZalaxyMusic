#include "ExplanationResponse.h"

namespace zalaxy
{

juce::String ExplanationResponse::getResponse (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
    {
        const auto value = obj->getProperty ("response");
        return value.isString() ? value.toString() : "No response generated.";
    }

    return "Invalid response format.";
}

} // namespace zalaxy
