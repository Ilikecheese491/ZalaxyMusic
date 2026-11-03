#include "TextToPatchRequest.h"

namespace zalaxy
{

juce::var TextToPatchRequest::createRequest (const juce::String& prompt)
{
    auto obj = new juce::DynamicObject();
    obj->setProperty ("prompt", prompt);
    obj->setProperty ("max_parameters", 80);
    obj->setProperty ("temperature", 0.7);
    return juce::var (obj);
}

bool TextToPatchRequest::validateResponse (const juce::var& response)
{
    if (!response.isObject()) return false;
    auto* obj = response.getDynamicObject();
    if (!obj->hasProperty ("parameters")) return false;

    auto params = obj->getProperty ("parameters");
    if (!params.isArray()) return false;

    for (auto& param : *params.getArray())
    {
        if (!param.isObject()) return false;
        auto* p = param.getDynamicObject();
        if (!p->hasProperty ("id") || !p->hasProperty ("value")) return false;
        if (!p->getProperty ("id").isString()) return false;
        auto value = p->getProperty ("value");
        if (! (value.isDouble() || value.isInt() || value.isInt64()))
            return false;
    }
    return true;
}

juce::String TextToPatchRequest::getSchema()
{
    return R"({
        "type": "object",
        "properties": {
            "prompt": {"type": "string"},
            "max_parameters": {"type": "integer"},
            "temperature": {"type": "number"}
        },
        "required": ["prompt"]
    })";
}

} // namespace zalaxy