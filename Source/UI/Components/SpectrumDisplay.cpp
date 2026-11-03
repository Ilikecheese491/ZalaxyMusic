#include "SpectrumDisplay.h"

namespace zalaxy
{

SpectrumDisplay::SpectrumDisplay (ZalaxyTheme& t)
    : theme (t)
{
    startTimerHz (30);
    spectrumData.resize (fftSize / 2);
    spectrumData.fill (0.0f);
}

SpectrumDisplay::~SpectrumDisplay()
{
    stopTimer();
}

void SpectrumDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (4);
    g.fillAll (theme.getColor (ZalaxyTheme::ColorId::Background));

    drawGrid (g, bounds);
    drawSpectrum (g, bounds);
    drawLabels (g, bounds);

    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRect (bounds, 1.0f);
}

void SpectrumDisplay::resized()
{
    // Nothing special needed
}

void SpectrumDisplay::timerCallback()
{
    // In real implementation, read from AudioEngine's FFT analyzer
    repaint();
}

void SpectrumDisplay::drawGrid (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::GridColor));

    // Frequency lines (logarithmic)
    float freqs[] = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    for (float f : freqs)
    {
        float x = freqToX (f, bounds);
        g.drawVerticalLine (static_cast<int> (x), bounds.getY(), bounds.getBottom());
    }

    // Level lines
    for (int i = 1; i < 6; ++i)
    {
        float y = bounds.getBottom() - bounds.getHeight() * i / 6.0f;
        g.drawHorizontalLine (static_cast<int> (y), bounds.getX(), bounds.getRight());
    }
}

void SpectrumDisplay::drawSpectrum (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    if (spectrumData.size() < 2) return;

    g.setColour (theme.getColor (ZalaxyTheme::ColorId::SpectrumColor));

    juce::Path path;
    path.startNewSubPath (bounds.getX(), bounds.getBottom());

    for (int i = 0; i < spectrumData.size(); ++i)
    {
        float freq = 20.0f * std::pow (1000.0f, static_cast<float> (i) / (spectrumData.size() - 1));
        float x = freqToX (freq, bounds);
        float level = juce::jlimit (-120.0f, 0.0f, 20.0f * std::log10 (juce::jmax (0.00001f, spectrumData[i])));
        float y = bounds.getBottom() + (level / 120.0f) * bounds.getHeight();

        path.lineTo (x, y);
    }

    path.lineTo (bounds.getRight(), bounds.getBottom());
    path.closeSubPath();

    // Fill with gradient
    juce::ColourGradient gradient (theme.getColor (ZalaxyTheme::ColorId::SpectrumColor).withAlpha (0.3f),
                                    bounds.getCentreX(), bounds.getBottom(),
                                    theme.getColor (ZalaxyTheme::ColorId::SpectrumColor).withAlpha (0.0f),
                                    bounds.getCentreX(), bounds.getY(),
                                    false);
    g.setGradientFill (gradient);
    g.fillPath (path);

    // Outline
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::SpectrumColor));
    g.strokePath (path, juce::PathStrokeType (1.5f));
}

void SpectrumDisplay::drawLabels (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setFont (theme.getFont ("caption"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    float freqs[] = {20, 100, 1000, 10000, 20000};
    for (float f : freqs)
    {
        float x = freqToX (f, bounds);
        juce::String label = f >= 1000 ? juce::String (f / 1000, 1) + "k" : juce::String ((int) f);
        g.drawText (label, static_cast<int> (x) - 20, bounds.getBottom() + 2, 40, 14, juce::Justification::centred);
    }

    // Level labels
    g.drawText ("0 dB", bounds.getX() + 4, bounds.getY() + 2, 30, 14, juce::Justification::left);
    g.drawText ("-60 dB", bounds.getX() + 4, bounds.getCentreY() - 7, 40, 14, juce::Justification::left);
    g.drawText ("-120 dB", bounds.getX() + 4, bounds.getBottom() - 16, 40, 14, juce::Justification::left);
}

float SpectrumDisplay::freqToX (float freq, const juce::Rectangle<float>& bounds) const
{
    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    float logMin = std::log10 (minFreq);
    float logMax = std::log10 (maxFreq);
    float logFreq = std::log10 (freq);
    float norm = (logFreq - logMin) / (logMax - logMin);
    return bounds.getX() + norm * bounds.getWidth();
}

} // namespace zalaxy