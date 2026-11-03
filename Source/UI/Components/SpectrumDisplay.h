#pragma once

#include <JuceHeader.h>
#include "UI/Theme/ZalaxyTheme.h"

namespace zalaxy
{

class SpectrumDisplay : public juce::Component,
                        public juce::Timer
{
public:
    SpectrumDisplay (ZalaxyTheme& theme);
    ~SpectrumDisplay() override;

    void setAudioEngine (class AudioEngine* engine) { audioEngine = engine; }

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    ZalaxyTheme& theme;
    class AudioEngine* audioEngine = nullptr;

    juce::Array<float> spectrumData;
    int fftSize = 512;

    void drawGrid (juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawSpectrum (juce::Graphics& g, const juce::Rectangle<float>& bounds);
    void drawLabels (juce::Graphics& g, const juce::Rectangle<float>& bounds);
    float freqToX (float freq, const juce::Rectangle<float>& bounds) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDisplay)
};

} // namespace zalaxy