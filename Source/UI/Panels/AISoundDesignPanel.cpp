#include "AISoundDesignPanel.h"

namespace zalaxy
{

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
        const auto explanationVar = obj->getProperty ("explanation");
        addToHistory ("AI", explanationVar.isString() ? explanationVar.toString() : "Patch generated.");
    }
}

void AISoundDesignPanel::handleAnalysisResponse (const juce::var& result)
{
    if (auto* obj = result.getDynamicObject())
    {
        displayGeneratedPatch (result);
        const auto explanationVar = obj->getProperty ("explanation");
        addToHistory ("AI", explanationVar.isString() ? explanationVar.toString() : "Analysis complete. Patch generated from reference.");
    }
}

} // namespace zalaxy
