#include "PluginEditor.h"
#include "PluginProcessor.h"

namespace zalaxy
{

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
{
    // Set plugin size
    setSize (1200, 800);
    setResizable (true, true);
    setResizeLimits (900, 600, 1920, 1200);

    // Initialize theme
    theme.initialize();

    // Create main layout components
    topBar = std::make_unique<juce::Component>();
    topBar->setName ("TopBar");
    addAndMakeVisible (topBar.get());

    leftPanel = std::make_unique<juce::Component>();
    leftPanel->setName ("LeftPanel");
    addAndMakeVisible (leftPanel.get());

    centerWorkspace = std::make_unique<juce::Component>();
    centerWorkspace->setName ("CenterWorkspace");
    addAndMakeVisible (centerWorkspace.get());

    rightPanel = std::make_unique<juce::Component>();
    rightPanel->setName ("RightPanel");
    addAndMakeVisible (rightPanel.get());

    bottomPanel = std::make_unique<juce::Component>();
    bottomPanel->setName ("BottomPanel");
    addAndMakeVisible (bottomPanel.get());

    // Create parameter panels
    oscillatorPanel = std::make_unique<OscillatorPanel> (processor.getParameterManager(), processor.getAudioEngine());
    leftPanel->addAndMakeVisible (oscillatorPanel.get());

    filterPanel = std::make_unique<FilterPanel> (processor.getParameterManager(), processor.getAudioEngine());
    leftPanel->addAndMakeVisible (filterPanel.get());

    envelopePanel = std::make_unique<EnvelopePanel> (processor.getParameterManager(), processor.getAudioEngine());
    leftPanel->addAndMakeVisible (envelopePanel.get());

    modulationPanel = std::make_unique<ModulationPanel> (processor.getParameterManager(), processor.getAudioEngine());
    leftPanel->addAndMakeVisible (modulationPanel.get());

    effectsPanel = std::make_unique<EffectsPanel> (processor.getParameterManager(), processor.getAudioEngine());
    leftPanel->addAndMakeVisible (effectsPanel.get());

    // Create AI panels
    aiSoundDesignPanel = std::make_unique<AISoundDesignPanel> (processor.getAIProvider(), processor.getParameterManager());

    assistantPanel = std::make_unique<AssistantPanel> (processor.getAIProvider(), processor.getParameterManager());

    // Create Settings and About panels
    settingsPanel = std::make_unique<SettingsPanel> (processor.getCloudAIProvider(), processor.getSubscriptionManager());

    aboutPanel = std::make_unique<AboutPanel>();

    // Create tabbed component for right panel
    rightPanelTabs = std::make_unique<juce::TabbedComponent> (juce::TabbedButtonBar::TabsAtTop);
    rightPanelTabs->addTab ("AI Design", theme.getColor (ZalaxyTheme::ColorId::AccentPrimary), aiSoundDesignPanel.get(), false);
    rightPanelTabs->addTab ("Assistant", theme.getColor (ZalaxyTheme::ColorId::AccentSecondary), assistantPanel.get(), false);
    rightPanelTabs->addTab ("Settings", theme.getColor (ZalaxyTheme::ColorId::AccentWarm), settingsPanel.get(), false);
    rightPanelTabs->addTab ("About", theme.getColor (ZalaxyTheme::ColorId::TextMuted), aboutPanel.get(), false);
    rightPanel->addAndMakeVisible (rightPanelTabs.get());

    // Create main visualization
    mainVisualization = std::make_unique<juce::Component>();
    mainVisualization->setName ("MainVisualization");
    centerWorkspace->addAndMakeVisible (mainVisualization.get());

    // Create virtual MIDI keyboard
    midiKeyboard = std::make_unique<juce::MidiKeyboardComponent> (midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);
    midiKeyboard->setAvailableRange (21, 108); // A0 to C8
    bottomPanel->addAndMakeVisible (midiKeyboard.get());

    // Connect MIDI keyboard to processor
    midiKeyboardState.addListener (&processor.getAudioEngine());

    // Initialize child components
    oscillatorPanel->initialize();
    filterPanel->initialize();
    envelopePanel->initialize();
    modulationPanel->initialize();
    effectsPanel->initialize();
    aiSoundDesignPanel->initialize();
    assistantPanel->initialize();
    settingsPanel->initialize();
    aboutPanel->initialize();
}

PluginEditor::~PluginEditor()
{
    midiKeyboardState.removeListener (&processor.getAudioEngine());
    midiKeyboard.reset();
    mainVisualization.reset();
    rightPanelTabs.reset();
    aboutPanel.reset();
    settingsPanel.reset();
    assistantPanel.reset();
    aiSoundDesignPanel.reset();
    effectsPanel.reset();
    modulationPanel.reset();
    envelopePanel.reset();
    filterPanel.reset();
    oscillatorPanel.reset();
    bottomPanel.reset();
    rightPanel.reset();
    centerWorkspace.reset();
    leftPanel.reset();
    topBar.reset();
}

void PluginEditor::paint (juce::Graphics& g)
{
    // Fill background with cosmic theme color
    g.fillAll (theme.getColor (ZalaxyTheme::ColorId::Background));

    // Draw a subtle grid using the theme's existing grid color.
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::GridColor).withAlpha (0.16f));
    constexpr int gridSpacing = 32;
    for (int x = 0; x < getWidth(); x += gridSpacing)
        g.drawVerticalLine (x, 0.0f, static_cast<float> (getHeight()));
    for (int y = 0; y < getHeight(); y += gridSpacing)
        g.drawHorizontalLine (y, 0.0f, static_cast<float> (getWidth()));
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top bar
    topBar->setBounds (bounds.removeFromTop (topBarHeight));

    // Bottom panel (keyboard)
    bottomPanel->setBounds (bounds.removeFromBottom (bottomPanelHeight));
    midiKeyboard->setBounds (bottomPanel->getLocalBounds().reduced (10));

    // Left panel (synth parameters)
    leftPanel->setBounds (bounds.removeFromLeft (leftPanelWidth));

    // Right panel (AI)
    rightPanel->setBounds (bounds.removeFromRight (rightPanelWidth));

    // Center workspace (visualization)
    centerWorkspace->setBounds (bounds);
    mainVisualization->setBounds (centerWorkspace->getLocalBounds().reduced (10));

    // Layout left panel vertically
    auto leftBounds = leftPanel->getLocalBounds().reduced (5);
    const int panelHeight = leftBounds.getHeight() / 5;

    oscillatorPanel->setBounds (leftBounds.removeFromTop (panelHeight));
    filterPanel->setBounds (leftBounds.removeFromTop (panelHeight));
    envelopePanel->setBounds (leftBounds.removeFromTop (panelHeight));
    modulationPanel->setBounds (leftBounds.removeFromTop (panelHeight));
    effectsPanel->setBounds (leftBounds);

    // Layout right panel with tabs
    auto rightBounds = rightPanel->getLocalBounds().reduced (5);
    rightPanelTabs->setBounds (rightBounds);
}

void PluginEditor::handleAIResponse (const juce::var& result)
{
    // Forward to AI sound design panel
    if (aiSoundDesignPanel)
    {
        aiSoundDesignPanel->handleAIResponse (result);
    }
}

void PluginEditor::handleAnalysisResponse (const juce::var& result)
{
    // Forward to AI sound design panel for reference analysis results
    if (aiSoundDesignPanel)
    {
        aiSoundDesignPanel->handleAnalysisResponse (result);
    }
}

} // namespace zalaxy