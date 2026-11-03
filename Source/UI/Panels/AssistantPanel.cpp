#include "AssistantPanel.h"

namespace zalaxy
{

void AssistantPanel::sendMessage()
{
    juce::String message = inputField->getText().trim();
    if (message.isEmpty() || isProcessing) return;

    isProcessing = true;
    sendButton->setEnabled (false);
    inputField->setEnabled (false);

    addMessage ("You", message);
    inputField->clear();

    juce::String context = contextSelector->getItemText (contextSelector->getSelectedItemIndex());
    juce::String fullPrompt = "[Context: " + context + "] " + message;

    aiProvider.requestAssistantResponse (fullPrompt, [this] (const juce::var& result)
    {
        isProcessing = false;
        sendButton->setEnabled (inputField->getText().trim().isNotEmpty());
        inputField->setEnabled (true);

        if (auto* obj = result.getDynamicObject())
        {
            const auto responseVar = obj->getProperty ("response");
            juce::String response = responseVar.isString() ? responseVar.toString()
                                                           : "I couldn't generate a response.";
            addMessage ("Assistant", response);
        }
    });
}

} // namespace zalaxy
