#include "WaveformDisplay.h"

namespace zalaxy
{

WaveformDisplay::WaveformDisplay (ZalaxyTheme& t)
    : theme (t)
{
    startTimerHz (30); // 30 FPS for smooth visualization
    waveformBuffer.resize (bufferSize);
    waveformBuffer.fill (0.0f);
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (4);
    g.fillAll (theme.getColor (ZalaxyTheme::ColorId::Background));

    drawGrid (g, bounds);
    drawWaveform (g, bounds);
    drawLabels (g, bounds);

    // Border
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRect (bounds, 1.0f);
}

void WaveformDisplay::resized()
{
    // Buffer size based on width
    bufferSize = juce::jmax (256, getWidth());
    waveformBuffer.resize (bufferSize);
}

void WaveformDisplay::timerCallback()
{
    // In a real implementation, this would copy from AudioEngine's circular buffer
    // For now, generate a placeholder waveform
    if (audioEngine && oscMode)
    {
        // This would read from AudioEngine's output buffer
    }
    repaint();
}

void WaveformDisplay::drawGrid (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::GridColor));

    // Vertical lines (time divisions)
    for (int i = 1; i < 8; ++i)
    {
        float x = bounds.getX() + bounds.getWidth() * i / 8.0f;
        g.drawVerticalLine (static_cast<int> (x), bounds.getY(), bounds.getBottom());
    }

    // Horizontal lines (amplitude divisions)
    for (int i = 1; i < 4; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * i / 4.0f;
        g.drawHorizontalLine (static_cast<int> (y), bounds.getX(), bounds.getRight());
    }

    // Center line
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::GridColor).brighter (0.2f));
    g.drawHorizontalLine (static_cast<int> (bounds.getCentreY()), bounds.getX(), bounds.getRight());
}

void WaveformDisplay::drawWaveform (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    if (waveformBuffer.size() < 2) return;

    g.setColour (theme.getColor (ZalaxyTheme::ColorId::WaveformColor));

    juce::Path path;
    bool started = false;

    for (int i = 0; i < waveformBuffer.size(); ++i)
    {
        float x = bounds.getX() + bounds.getWidth() * static_cast<float> (i) / (waveformBuffer.size() - 1);
        float y = bounds.getCentreY() - waveformBuffer[i] * bounds.getHeight() * 0.4f;

        if (!started)
        {
            path.startNewSubPath (x, y);
            started = true;
        }
        else
        {
            path.lineTo (x, y);
        }
    }

    g.strokePath (path, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved));
}

void WaveformDisplay::drawLabels (juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    g.setFont (theme.getFont ("caption"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    // Time labels
    g.drawText ("0 ms", bounds.getX() + 4, bounds.getBottom() - 18, 40, 16, juce::Justification::left);
    g.drawText (juce::String (getWidth() / 30) + " ms", bounds.getRight() - 44, bounds.getBottom() - 18, 40, 16, juce::Justification::right);

    // Amplitude labels
    g.drawText ("+1.0", bounds.getX() + 4, bounds.getY() + 2, 30, 14, juce::Justification::left);
    g.drawText ("-1.0", bounds.getX() + 4, bounds.getBottom() - 16, 30, 14, juce::Justification::left);
}

} // namespace zalaxy