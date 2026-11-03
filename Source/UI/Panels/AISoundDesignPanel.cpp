#include "AISoundDesignPanel.h"

namespace zalaxy
{

AISoundDesignPanel::AISoundDesignPanel (class AIProvider& ai, ParameterManager& params)
    : aiProvider (ai)
    , parameterManager (params)
{
    createPromptSection();
    createPreviewSection();
    createHistorySection();

    addAndMakeVisible (promptInput.get());
    addAndMakeVisible (generateButton.get());
    addAndMakeVisible (analyzeButton.get());
    addAndMakeVisible (variationsButton.get());
    addAndMakeVisible (previewArea.get());
    addAndMakeVisible (previewTitle.get());
    addAndMakeVisible (statusLabel.get());
    addAndMakeVisible (chatHistory.get());
    addAndMakeVisible (clearHistoryButton.get());

    generateButton->addListener (this);
    analyzeButton->addListener (this);
    variationsButton->addListener (this);
    clearHistoryButton->addListener (this);
    promptInput->addListener (this);
}

AISoundDesignPanel::~AISoundDesignPanel()
{
    generateButton->removeListener (this);
    analyzeButton->removeListener (this);
    variationsButton->removeListener (this);
    clearHistoryButton->removeListener (this);
    promptInput->removeListener (this);
}

void AISoundDesignPanel::createPromptSection()
{
    promptInput = std::make_unique<juce::TextEditor>();
    promptInput->setMultiLine (true);
    promptInput->setReturnKeyStartsNewLine (true);
    promptInput->setPlaceholderText ("Describe your sound... e.g. \"bright bouncy pluck with short decay, soft attack, stereo movement\"");
    promptInput->setFont (theme.getFont ("body"));
    promptInput->setColour (juce::TextEditor::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    promptInput->setColour (juce::TextEditor::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    promptInput->setColour (juce::TextEditor::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderDefault));
    promptInput->setColour (juce::TextEditor::focusedOutlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderFocus));
    promptInput->setColour (juce::TextEditor::placeholderColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    generateButton = std::make_unique<juce::TextButton> ("Generate Patch");
    generateButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    generateButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
    generateButton->setColour (juce::TextButton::buttonOnColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimaryDim));

    analyzeButton = std::make_unique<juce::TextButton> ("Analyze WAV...");
    analyzeButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    analyzeButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));

    variationsButton = std::make_unique<juce::TextButton> ("New Variation");
    variationsButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    variationsButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    variationsButton->setColour (juce::TextButton::buttonOnColourId, theme.getColor (ZalaxyTheme::ColorId::SurfaceHover));
    variationsButton->setEnabled (false);
}

void AISoundDesignPanel::createPreviewSection()
{
    previewArea = std::make_unique<juce::Component>();
    previewArea->setOpaque (true);

    previewTitle = std::make_unique<juce::Label> ("previewTitle", "Generated Patch Preview");
    previewTitle->setFont (theme.getFont ("heading3"));
    previewTitle->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    previewTitle->setJustificationType (juce::Justification::centredLeft);

    statusLabel = std::make_unique<juce::Label> ("statusLabel", "Ready");
    statusLabel->setFont (theme.getFont ("caption"));
    statusLabel->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    statusLabel->setJustificationType (juce::Justification::centredRight);
}

void AISoundDesignPanel::createHistorySection()
{
    chatHistory = std::make_unique<juce::TextEditor>();
    chatHistory->setMultiLine (true);
    chatHistory->setReadOnly (true);
    chatHistory->setScrollbarsShown (true);
    chatHistory->setFont (theme.getFont ("bodySmall"));
    chatHistory->setColour (juce::TextEditor::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Background));
    chatHistory->setColour (juce::TextEditor::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
    chatHistory->setColour (juce::TextEditor::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));

    clearHistoryButton = std::make_unique<juce::TextButton> ("Clear");
    clearHistoryButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    clearHistoryButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
}

void AISoundDesignPanel::initialize() {}

void AISoundDesignPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    g.fillRect (bounds.getX() + 8, bounds.getY() + 2, bounds.getWidth() - 16, 2);
}

void AISoundDesignPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    auto titleArea = bounds.removeFromTop (titleHeight);

    // Layout: prompt at top, preview middle, history bottom
    int promptHeight = 140;
    int previewHeight = 160;
    int buttonHeight = 36;
    int spacing = theme.getSpace (1);

    // Prompt section
    auto promptSection = bounds.removeFromTop (promptHeight);
    promptInput->setBounds (promptSection.removeFromTop (promptSection.getHeight() - buttonHeight - spacing));
    auto btnRow = promptSection;
    int btnWidth = (btnRow.getWidth() - spacing * 2) / 3;
    generateButton->setBounds (btnRow.removeFromLeft (btnWidth + spacing));
    analyzeButton->setBounds (btnRow.removeFromLeft (btnWidth + spacing));
    variationsButton->setBounds (btnRow);

    bounds.removeFromTop (spacing);

    // Preview section
    auto previewSection = bounds.removeFromTop (previewHeight);
    previewTitle->setBounds (previewSection.removeFromTop (24));
    statusLabel->setBounds (previewSection.removeFromBottom (20));
    previewArea->setBounds (previewSection);

    bounds.removeFromTop (spacing);

    // History section
    clearHistoryButton->setBounds (bounds.removeFromBottom (28).removeFromRight (80));
    chatHistory->setBounds (bounds);
}

void AISoundDesignPanel::textEditorTextChanged (juce::TextEditor& editor)
{
    if (&editor == promptInput.get())
    {
        generateButton->setEnabled (promptInput->getText().trim().isNotEmpty() && !isGenerating);
    }
}

void AISoundDesignPanel::textEditorReturnKeyPressed (juce::TextEditor& editor)
{
    if (&editor == promptInput.get() && !isGenerating)
    {
        requestTextToPatch (promptInput->getText().trim());
    }
}

void AISoundDesignPanel::buttonClicked (juce::Button* button)
{
    if (button == generateButton.get())
    {
        requestTextToPatch (promptInput->getText().trim());
    }
    else if (button == analyzeButton.get())
    {
        requestAudioAnalysis();
    }
    else if (button == variationsButton.get())
    {
        requestTextToPatch (promptInput->getText().trim() + " (variation " + juce::String (currentVariation + 1) + ")");
    }
    else if (button == clearHistoryButton.get())
    {
        chatHistory->clear();
    }
}

void AISoundDesignPanel::requestTextToPatch (const juce::String& prompt)
{
    if (prompt.isEmpty() || isGenerating) return;

    isGenerating = true;
    generateButton->setEnabled (false);
    variationsButton->setEnabled (false);
    statusLabel->setText ("Generating...", juce::dontSendNotification);
    statusLabel->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentWarm));

    addToHistory ("You", prompt);
    aiProvider.requestTextToPatch (prompt, [this] (const juce::var& result)
    {
        // Called on message thread via PluginProcessor
        handleAIResponse (result);
    });
}

void AISoundDesignPanel::requestAudioAnalysis()
{
    // Open file dialog for WAV
    juce::FileChooser chooser ("Select WAV file to analyze",
                                juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
                                "*.wav");
    if (chooser.browseForFileToOpen())
    {
        juce::File file = chooser.getResult();
        // For now, analyze full file - in production, user would select range
        aiProvider.requestAudioAnalysis (file, 0, file.getSize() / 44100.0, [this] (const juce::var& result)
        {
            handleAnalysisResponse (result);
        });
    }
}

void AISoundDesignPanel::handleAIResponse (const juce::var& result)
{
    isGenerating = false;
    generateButton->setEnabled (promptInput->getText().trim().isNotEmpty());
    variationsButton->setEnabled (true);
    statusLabel->setText ("Done", juce::dontSendNotification);
    statusLabel->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSuccess));

    if (auto* obj = result.getDynamicObject())
    {
        displayGeneratedPatch (result);
        addToHistory ("AI", obj->getProperty ("explanation", "Patch generated."));
    }
}

void AISoundDesignPanel::handleAnalysisResponse (const juce::var& result)
{
    if (auto* obj = result.getDynamicObject())
    {
        displayGeneratedPatch (result);
        addToHistory ("AI", obj->getProperty ("explanation", "Analysis complete. Patch generated from reference."));
    }
}

void AISoundDesignPanel::displayGeneratedPatch (const juce::var& result)
{
    generatedParams.clear();
    currentVariation = 0;

    if (auto* obj = result.getDynamicObject())
    {
        if (auto* paramsArray = obj->getProperty ("parameters").getArray())
        {
            for (auto& param : *paramsArray)
            {
                if (auto* p = param.getDynamicObject())
                {
                    juce::String id = p->getProperty ("id");
                    float value = p->getProperty ("value");
                    generatedParams.add ({id, value});
                }
            }
        }
    }
    updatePreviewDisplay();
}

void AISoundDesignPanel::applyGeneratedPatch (int variationIndex)
{
    for (auto& param : generatedParams)
    {
        parameterManager.setParameterValue (param.first, param.second);
    }
    addToHistory ("System", "Applied variation " + juce::String (variationIndex + 1));
}

void AISoundDesignPanel::updatePreviewDisplay()
{
    previewArea->repaint();
}

void AISoundDesignPanel::addToHistory (const juce::String& role, const juce::String& message)
{
    juce::String timestamp = juce::Time::getCurrentTime().toString (true, true, false, false);
    chatHistory->moveCaretToEnd();
    chatHistory->insertTextAtCaret ("[" + timestamp + "] " + role + ": " + message + "\n\n");
    chatHistory->moveCaretToEnd();
}

} // namespace zalaxy