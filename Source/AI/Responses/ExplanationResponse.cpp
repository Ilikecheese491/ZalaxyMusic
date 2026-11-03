#include "ExplanationResponse.h"

namespace zalaxy
{

juce::String ExplanationResponse::getResponse (const juce::var& response)
{
    if (auto* obj = response.getDynamicObject())
        return obj->getProperty ("response", "No response generated.");
    return "Invalid response format.";
}

} // namespace zalaxy