#include "AssistantPanel.h"

namespace zalaxy
{

AssistantPanel::AssistantPanel (class AIProvider& ai, ParameterManager& params)
    : aiProvider (ai)
    , parameterManager (params)
{
    // Chat display
    chatDisplay = std::make_unique<juce::TextEditor>();
    chatDisplay->setMultiLine (true);
    chatDisplay->setReadOnly (true);
    chatDisplay->setScrollbarsShown (true);
    chatDisplay->setFont (theme.getFont ("bodySmall"));
    chatDisplay->setColour (juce::TextEditor::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Background));
    chatDisplay->setColour (juce::TextEditor::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
    chatDisplay->setColour (juce::TextEditor::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));

    // Context selector
    contextSelector = std::make_unique<juce::ComboBox>();
    contextSelector->addItem ("General Production", 1);
    contextSelector->addItem ("Mixing", 2);
    contextSelector->addItem ("Mastering", 3);
    contextSelector->addItem ("Sound Design", 4);
    contextSelector->addItem ("Arrangement", 5);
    contextSelector->setSelectedId (1);
    contextSelector->setColour (juce::ComboBox::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    contextSelector->setColour (juce::ComboBox::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    contextSelector->setColour (juce::ComboBox::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderDefault));

    // Input field
    inputField = std::make_unique<juce::TextEditor>();
    inputField->setMultiLine (false);
    inputField->setReturnKeyStartsNewLine (false);
    inputField->setPlaceholderText ("Ask about mixing, mastering, sound design...");
    inputField->setFont (theme.getFont ("body"));
    inputField->setColour (juce::TextEditor::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    inputField->setColour (juce::TextEditor::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    inputField->setColour (juce::TextEditor::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderDefault));
    inputField->setColour (juce::TextEditor::focusedOutlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderFocus));
    inputField->addListener (this);

    // Buttons
    sendButton = std::make_unique<juce::TextButton> ("Send");
    sendButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    sendButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
    sendButton->addListener (this);

    clearButton = std::make_unique<juce::TextButton> ("Clear");
    clearButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    clearButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    clearButton->addListener (this);

    addAndMakeVisible (chatDisplay.get());
    addAndMakeVisible (contextSelector.get());
    addAndMakeVisible (inputField.get());
    addAndMakeVisible (sendButton.get());
    addAndMakeVisible (clearButton.get());

    addMessage ("Assistant", "Hi! I'm your production assistant. Ask me about mixing, mastering, sound design, or anything else. Select a context above for more specific advice.");
}

AssistantPanel::~AssistantPanel()
{
    sendButton->removeListener (this);
    clearButton->removeListener (this);
    inputField->removeListener (this);
}

void AssistantPanel::initialize() {}

void AssistantPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    g.fillRect (juce::Rectangle<float> (bounds.getX() + 8.0f,
                                    bounds.getY() + 2.0f,
                                    bounds.getWidth() - 16.0f,
                                    2.0f));
}

void AssistantPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (2));
    int titleHeight = 24;
    bounds.removeFromTop (titleHeight);

    int bottomHeight = 60;
    int spacing = theme.getSpace (1);

    // Bottom: context selector + input + buttons
    auto bottomArea = bounds.removeFromBottom (bottomHeight);
    clearButton->setBounds (bottomArea.removeFromRight (80).withTrimmedTop (8).withTrimmedBottom (8));
    sendButton->setBounds (bottomArea.removeFromRight (80).withTrimmedTop (8).withTrimmedBottom (8));
    contextSelector->setBounds (bottomArea.removeFromLeft (180).withTrimmedTop (8).withTrimmedBottom (8));
    inputField->setBounds (bottomArea.withTrimmedTop (8).withTrimmedBottom (8));

    bounds.removeFromBottom (spacing);

    // Chat display takes remaining space
    chatDisplay->setBounds (bounds);
}

void AssistantPanel::textEditorTextChanged (juce::TextEditor& editor)
{
    if (&editor == inputField.get())
    {
        sendButton->setEnabled (inputField->getText().trim().isNotEmpty() && !isProcessing);
    }
}

void AssistantPanel::textEditorReturnKeyPressed (juce::TextEditor& editor)
{
    if (&editor == inputField.get() && !isProcessing)
    {
        sendMessage();
    }
}

void AssistantPanel::buttonClicked (juce::Button* button)
{
    if (button == sendButton.get())
    {
        sendMessage();
    }
    else if (button == clearButton.get())
    {
        chatDisplay->clear();
        addMessage ("Assistant", "Chat cleared. How can I help?");
    }
}

void AssistantPanel::sendMessage()
{
    juce::String message = inputField->getText().trim();
    if (message.isEmpty() || isProcessing) return;

    isProcessing = true;
    sendButton->setEnabled (false);
    inputField->setEnabled (false);

    addMessage ("You", message);
    inputField->clear();

    // Build context-aware prompt
    juce::String context = contextSelector->getItemText (contextSelector->getSelectedItemIndex());
    juce::String fullPrompt = "[Context: " + context + "] " + message;

    aiProvider.requestAssistantResponse (fullPrompt, [this] (const juce::var& result)
    {
        isProcessing = false;
        sendButton->setEnabled (inputField->getText().trim().isNotEmpty());
        inputField->setEnabled (true);

        if (auto* obj = result.getDynamicObject())
        {
            juce::String response = obj->getProperty ("response", "I couldn't generate a response.");
            addMessage ("Assistant", response);
        }
    });
}

void AssistantPanel::addMessage (const juce::String& role, const juce::String& message)
{
    juce::String timestamp = juce::Time::getCurrentTime().toString (true, true, false, false);
    juce::Colour roleColor = (role == "You") ? theme.getColor (ZalaxyTheme::ColorId::AccentPrimary)
                                             : theme.getColor (ZalaxyTheme::ColorId::AccentSecondary);

    chatDisplay->moveCaretToEnd();
    chatDisplay->setColour (juce::TextEditor::highlightColourId, roleColor.withAlpha (0.2f));
    chatDisplay->insertTextAtCaret ("[" + timestamp + "] " + role + ": " + message + "\n\n");
    chatDisplay->moveCaretToEnd();
}

void AssistantPanel::requestAssistantResponse (const juce::String& question)
{
    // Handled in sendMessage with context
}

} // namespace zalaxy