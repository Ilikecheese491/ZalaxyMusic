#include "AudioAnalysisRequest.h"
#include "TextToPatchRequest.h"

namespace zalaxy
{

juce::var AudioAnalysisRequest::createRequest (const juce::File& wavFile, double startTime, double endTime)
{
    auto obj = new juce::DynamicObject();
    obj->setProperty ("file_path", wavFile.getFullPathName());
    obj->setProperty ("start_time", startTime);
    obj->setProperty ("end_time", endTime);
    obj->setProperty ("sample_rate", 44100);
    obj->setProperty ("channels", 2);

    return juce::var (obj);
}

bool AudioAnalysisRequest::validateResponse (const juce::var& response)
{
    return TextToPatchRequest::validateResponse (response);
}

juce::String AudioAnalysisRequest::getSchema()
{
    return R"({
        "type": "object",
        "properties": {
            "file_path": {"type": "string"},
            "start_time": {"type": "number"},
            "end_time": {"type": "number"},
            "sample_rate": {"type": "integer"},
            "channels": {"type": "integer"}
        },
        "required": ["file_path"]
    })";
}

} // namespace zalaxy