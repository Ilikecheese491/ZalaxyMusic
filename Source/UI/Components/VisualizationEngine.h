#pragma once

#include "JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"
#include "Core/Audio/AudioEngine.h"

namespace zalaxy
{

class VisualizationEngine : public juce::Component,
                            public juce::Timer
{
public:
    VisualizationEngine (AudioEngine& engine);
    ~VisualizationEngine() override;

    void initialize();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setVisualizationMode (int mode);
    int getVisualizationMode() const { return currentMode; }

private:
    AudioEngine& audioEngine;
    ZalaxyTheme theme;

    enum Mode
    {
        Waveform = 0,
        Spectrum = 1,
        Spectrogram = 2,
        Oscilloscope = 3
    };

    int currentMode = Waveform;
    juce::Array<float> waveformBuffer;
    juce::Array<float> spectrumBuffer;
    juce::Array<float> spectrogramHistory;

    static constexpr int waveformSize = 1024;
    static constexpr int spectrumSize = 512;
    static constexpr int spectrogramHeight = 128;

    juce::Path waveformPath;
    juce::Path spectrumPath;
    juce::Image spectrogramImage;

    void updateWaveform();
    void updateSpectrum();
    void updateSpectrogram();
    void drawWaveform (juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSpectrum (juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSpectrogram (juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawOscilloscope (juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawGrid (juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawLabels (juce::Graphics& g, juce::Rectangle<float> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualizationEngine)
};

} // namespace zalaxy