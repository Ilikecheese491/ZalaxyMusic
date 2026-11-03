#include "ExplanationResponse.h"

namespace zalaxy
{

juce::String ExplanationResponse::getResponse (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
    {
        const auto value = obj->getProperty ("response");
        return value.isVoid() ? juce::String ("No response generated.") : value.toString();
    }
    return "Invalid response format.";
}

} // namespace zalaxy
