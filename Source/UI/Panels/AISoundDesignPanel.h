#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Components/ParameterKnob.h"
#include "Core/Parameters/ParameterManager.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class AISoundDesignPanel : public juce::Component,
                           public juce::TextEditor::Listener,
                           public juce::Button::Listener
{
public:
    AISoundDesignPanel (class AIProvider& aiProvider, ParameterManager& params);
    ~AISoundDesignPanel() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;

    void handleAIResponse (const juce::var& result);
    void handleAnalysisResponse (const juce::var& result);

    void textEditorTextChanged (juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed (juce::TextEditor& editor) override;
    void buttonClicked (juce::Button* button) override;

private:
    class AIProvider& aiProvider;
    ParameterManager& parameterManager;
    ZalaxyTheme theme;

    std::unique_ptr<juce::TextEditor> promptInput;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::TextButton> generateButton;
    std::unique_ptr<juce::TextButton> analyzeButton;
    std::unique_ptr<juce::TextButton> variationsButton;

    std::unique_ptr<juce::Component> previewArea;
    std::unique_ptr<juce::Label> previewTitle;
    juce::Array<std::pair<juce::String, float>> generatedParams;
    int currentVariation = 0;

    std::unique_ptr<juce::Label> statusLabel;
    bool isGenerating = false;

    std::unique_ptr<juce::TextEditor> chatHistory;
    std::unique_ptr<juce::TextButton> clearHistoryButton;

    void createPromptSection();
    void createPreviewSection();
    void createHistorySection();
    void requestTextToPatch (const juce::String& prompt);
    void requestAudioAnalysis();
    void displayGeneratedPatch (const juce::var& result);
    void applyGeneratedPatch (int variationIndex = 0);
    void updatePreviewDisplay();
    void addToHistory (const juce::String& role, const juce::String& message);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AISoundDesignPanel)
};

} // namespace zalaxy