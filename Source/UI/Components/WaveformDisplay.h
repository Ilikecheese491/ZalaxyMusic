#pragma once

#include "Common/JuceModules.h"
#include "UI/Theme/ZalaxyTheme.h"

namespace zalaxy
{

class WaveformDisplay : public juce::Component,
                        public juce::Timer
{
public:
    WaveformDisplay (ZalaxyTheme& theme);
    ~WaveformDisplay() override;

    void setAudioEngine (class AudioEngine* engine) { audioEngine = engine; }
    void setOscilloscopeMode (bool enabled) { oscMode = enabled; repaint(); }

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    ZalaxyTheme& theme;
    class AudioEngine* audioEngine = nullptr;
    bool oscMode = true;

    // Waveform buffer for drawing
    juce::Array<float> waveformBuffer;
    int bufferSize = 512;
    int writePos = 0;

    void drawGrid (juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawWaveform (juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawLabels (juce::Graphics& g, const juce::Rectangle<float>& bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};

} // namespace zalaxy