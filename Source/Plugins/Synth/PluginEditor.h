#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "UI/Panels/OscillatorPanel.h"
#include "UI/Panels/FilterPanel.h"
#include "UI/Panels/EnvelopePanel.h"
#include "UI/Panels/ModulationPanel.h"
#include "UI/Panels/EffectsPanel.h"
#include "UI/Panels/AISoundDesignPanel.h"
#include "UI/Panels/AssistantPanel.h"
#include "UI/Panels/SettingsPanel.h"
#include "UI/Panels/AboutPanel.h"

namespace zalaxy
{

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    // AudioProcessorEditor overrides
    void paint (juce::Graphics&) override;
    void resized() override;

    // AI response handlers (called from message thread)
    void handleAIResponse (const juce::var& result);
    void handleAnalysisResponse (const juce::var& result);

    // Access to processor
    PluginProcessor& getProcessor() { return processor; }

private:
    PluginProcessor& processor;

    // Theme
    ZalaxyTheme theme;

    // Main layout components
    std::unique_ptr<juce::Component> topBar;
    std::unique_ptr<juce::Component> leftPanel;
    std::unique_ptr<juce::Component> centerWorkspace;
    std::unique_ptr<juce::Component> rightPanel;
    std::unique_ptr<juce::Component> bottomPanel;

    // Parameter panels
    std::unique_ptr<OscillatorPanel> oscillatorPanel;
    std::unique_ptr<FilterPanel> filterPanel;
    std::unique_ptr<EnvelopePanel> envelopePanel;
    std::unique_ptr<ModulationPanel> modulationPanel;
    std::unique_ptr<EffectsPanel> effectsPanel;

    // AI panels
    std::unique_ptr<AISoundDesignPanel> aiSoundDesignPanel;
    std::unique_ptr<AssistantPanel> assistantPanel;

    // Settings & About
    std::unique_ptr<SettingsPanel> settingsPanel;
    std::unique_ptr<AboutPanel> aboutPanel;

    // Visualization
    std::unique_ptr<juce::Component> mainVisualization;

    // Right panel tabs
    std::unique_ptr<juce::TabbedComponent> rightPanelTabs;

    // Virtual keyboard
    std::unique_ptr<juce::MidiKeyboardComponent> midiKeyboard;
    juce::MidiKeyboardState midiKeyboardState;

    // Layout constants
    static constexpr int topBarHeight = 48;
    static constexpr int leftPanelWidth = 280;
    static constexpr int rightPanelWidth = 380;
    static constexpr int bottomPanelHeight = 120;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

} // namespace zalaxy