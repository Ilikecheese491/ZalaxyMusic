#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "Core/Parameters/ParameterManager.h"

namespace zalaxy
{

class AssistantPanel : public juce::Component,
                       public juce::TextEditor::Listener,
                       public juce::Button::Listener
{
public:
    AssistantPanel (class AIProvider& aiProvider, ParameterManager& params);
    ~AssistantPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

    void textEditorTextChanged (juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed (juce::TextEditor& editor) override;
    void buttonClicked (juce::Button* button) override;

private:
    class AIProvider& aiProvider;
    ParameterManager& parameterManager;
    ZalaxyTheme theme;

    std::unique_ptr<juce::TextEditor> chatDisplay;
    std::unique_ptr<juce::TextEditor> inputField;
    std::unique_ptr<juce::TextButton> sendButton;
    std::unique_ptr<juce::TextButton> clearButton;
    std::unique_ptr<juce::ComboBox> contextSelector;

    bool isProcessing = false;

    void sendMessage();
    void addMessage (const juce::String& role, const juce::String& message);
    void requestAssistantResponse (const juce::String& question);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AssistantPanel)
};

} // namespace zalaxy