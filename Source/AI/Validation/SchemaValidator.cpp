#include "SchemaValidator.h"
#include "Requests/TextToPatchRequest.h"
#include "Requests/AudioAnalysisRequest.h"

namespace zalaxy
{

bool SchemaValidator::validateJSON (const juce::String& json, const juce::String& schema)
{
    juce::var parsed = juce::JSON::parse (json);
    return parsed.isObject();
}

juce::String SchemaValidator::getTextToPatchSchema()
{
    return TextToPatchRequest::getSchema();
}

juce::String SchemaValidator::getAudioAnalysisSchema()
{
    return AudioAnalysisRequest::getSchema();
}

juce::String SchemaValidator::getAssistantResponseSchema()
{
    return R"({
        "type": "object",
        "properties": {
            "response": {"type": "string"}
        },
        "required": ["response"]
    })";
}

} // namespace zalaxy
