#include "VisualizationEngine.h"

namespace zalaxy
{

VisualizationEngine::VisualizationEngine (AudioEngine& engine)
    : audioEngine (engine)
{
    setOpaque (true);
    startTimerHz (30); // 30 FPS update rate

    waveformBuffer.resize (waveformSize);
    spectrumBuffer.resize (spectrumSize);
    spectrogramHistory.resize (spectrumSize * spectrogramHeight);

    // Initialize spectrogram image
    spectrogramImage = juce::Image (juce::Image::ARGB, spectrumSize, spectrogramHeight, true);
}

VisualizationEngine::~VisualizationEngine()
{
    stopTimer();
}

void VisualizationEngine::initialize()
{
    // Called after construction, can do additional setup here
}

void VisualizationEngine::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::Background));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));

    // Border
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);

    // Accent line at top
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    g.fillRect (bounds.getX() + 8, bounds.getY() + 2, bounds.getWidth() - 16, 2);

    // Draw grid
    drawGrid (g, bounds);

    // Draw visualization based on mode
    switch (currentMode)
    {
        case Waveform:
            drawWaveform (g, bounds);
            break;
        case Spectrum:
            drawSpectrum (g, bounds);
            break;
        case Spectrogram:
            drawSpectrogram (g, bounds);
            break;
        case Oscilloscope:
            drawOscilloscope (g, bounds);
            break;
    }

    // Draw labels
    drawLabels (g, bounds);
}

void VisualizationEngine::resized()
{
    // Component resized - paths will be rebuilt on next paint
}

void VisualizationEngine::timerCallback()
{
    updateWaveform();
    updateSpectrum();
    updateSpectrogram();
    repaint();
}

void VisualizationEngine::setVisualizationMode (int mode)
{
    if (mode >= 0 && mode <= 3)
    {
        currentMode = mode;
        repaint();
    }
}

void VisualizationEngine::updateWaveform()
{
    // Get waveform data from audio engine (lock-free ring buffer)
    audioEngine.copyWaveformData (waveformBuffer.data(), waveformSize);

    // Build path
    waveformPath.clear();
    if (waveformSize > 0)
    {
        auto bounds = getLocalBounds().toFloat().reduced (20);
        float centerY = bounds.getCentreY();
        float scaleY = bounds.getHeight() * 0.4f;

        waveformPath.startNewSubPath (bounds.getX(), centerY);

        for (int i = 0; i < waveformSize; ++i)
        {
            float x = bounds.getX() + (float)i / (waveformSize - 1) * bounds.getWidth();
            float y = centerY - waveformBuffer[i] * scaleY;
            waveformPath.lineTo (x, y);
        }
    }
}

void VisualizationEngine::updateSpectrum()
{
    // Get spectrum data from audio engine
    audioEngine.copySpectrumData (spectrumBuffer.data(), spectrumSize);

    // Build path
    spectrumPath.clear();
    if (spectrumSize > 0)
    {
        auto bounds = getLocalBounds().toFloat().reduced (20);
        float bottomY = bounds.getBottom();
        float scaleY = bounds.getHeight() * 0.8f;

        spectrumPath.startNewSubPath (bounds.getX(), bottomY);

        for (int i = 0; i < spectrumSize; ++i)
        {
            float x = bounds.getX() + (float)i / (spectrumSize - 1) * bounds.getWidth();
            float level = juce::jlimit (0.0f, 1.0f, spectrumBuffer[i]);
            float y = bottomY - level * scaleY;
            spectrumPath.lineTo (x, y);
        }

        spectrumPath.lineTo (bounds.getRight(), bottomY);
        spectrumPath.closeSubPath();
    }
}

void VisualizationEngine::updateSpectrogram()
{
    // Shift history up by one row
    for (int y = spectrogramHeight - 1; y > 0; --y)
    {
        for (int x = 0; x < spectrumSize; ++x)
        {
            spectrogramHistory[y * spectrumSize + x] = spectrogramHistory[(y - 1) * spectrumSize + x];
        }
    }

    // Add new spectrum data at bottom
    for (int x = 0; x < spectrumSize; ++x)
    {
        spectrogramHistory[x] = spectrumBuffer[x];
    }

    // Update image
    juce::Image::BitmapData bitmap (spectrogramImage, juce::Image::BitmapData::writeOnly);
    if (bitmap.data != nullptr)
    {
        for (int y = 0; y < spectrogramHeight; ++y)
        {
            for (int x = 0; x < spectrumSize; ++x)
            {
                float level = juce::jlimit (0.0f, 1.0f, spectrogramHistory[y * spectrumSize + x]);

                // Color map: blue -> cyan -> green -> yellow -> red
                juce::Colour colour;
                if (level < 0.25f)
                    colour = juce::Colour::fromFloatRGBA (0.0f, level * 4.0f, 1.0f, level);
                else if (level < 0.5f)
                    colour = juce::Colour::fromFloatRGBA (0.0f, 1.0f, 1.0f - (level - 0.25f) * 4.0f, level);
                else if (level < 0.75f)
                    colour = juce::Colour::fromFloatRGBA ((level - 0.5f) * 4.0f, 1.0f, 0.0f, level);
                else
                    colour = juce::Colour::fromFloatRGBA (1.0f, 1.0f - (level - 0.75f) * 4.0f, 0.0f, level);

                uint32_t* pixel = (uint32_t*)bitmap.getPixelPointer (x, y);
                *pixel = colour.getPixelARGB();
            }
        }
    }
}

void VisualizationEngine::drawWaveform (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto drawBounds = bounds.reduced (20);

    // Center line
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle).withAlpha (0.5f));
    g.drawHorizontalLine (drawBounds.getCentreY(), drawBounds.getX(), drawBounds.getRight());

    // Waveform
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    g.strokePath (waveformPath, juce::PathStrokeType (1.5f));
}

void VisualizationEngine::drawSpectrum (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto drawBounds = bounds.reduced (20);

    // Filled spectrum
    juce::ColourGradient gradient (theme.getColor (ZalaxyTheme::ColorId::AccentPrimary).withAlpha (0.3f),
                                    drawBounds.getX(), drawBounds.getBottom(),
                                    theme.getColor (ZalaxyTheme::ColorId::AccentSecondary).withAlpha (0.1f),
                                    drawBounds.getX(), drawBounds.getY(),
                                    false);
    g.setGradientFill (gradient);
    g.fillPath (spectrumPath);

    // Outline
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    g.strokePath (spectrumPath, juce::PathStrokeType (1.0f));
}

void VisualizationEngine::drawSpectrogram (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto drawBounds = bounds.reduced (20);

    if (spectrogramImage.isValid())
    {
        g.drawImage (spectrogramImage, drawBounds.toFloat(),
                     juce::RectanglePlacement::stretchToFit);
    }
}

void VisualizationEngine::drawOscilloscope (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto drawBounds = bounds.reduced (20);
    float centerX = drawBounds.getCentreX();
    float centerY = drawBounds.getCentreY();
    float radius = juce::jmin (drawBounds.getWidth(), drawBounds.getHeight()) * 0.4f;

    // Circular grid
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle).withAlpha (0.3f));
    for (int i = 1; i <= 4; ++i)
    {
        float r = radius * i / 4.0f;
        g.drawEllipse (centerX - r, centerY - r, r * 2, r * 2, 0.5f);
    }

    // Crosshairs
    g.drawLine (centerX - radius, centerY, centerX + radius, centerY, 0.5f);
    g.drawLine (centerX, centerY - radius, centerX, centerY + radius, 0.5f);

    // Lissajous-style display using first two oscillators' phase
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));

    juce::Path lissajousPath;
    bool started = false;

    for (int i = 0; i < 1024; ++i)
    {
        float t = (float)i / 1024.0f * juce::MathConstants<float>::twoPi * 4.0f;
        float x = centerX + radius * std::sin (t) * 0.8f;
        float y = centerY + radius * std::sin (t * 1.5f + 0.5f) * 0.8f;

        if (!started)
        {
            lissajousPath.startNewSubPath (x, y);
            started = true;
        }
        else
        {
            lissajousPath.lineTo (x, y);
        }
    }

    g.strokePath (lissajousPath, juce::PathStrokeType (2.0f));
}

void VisualizationEngine::drawGrid (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto drawBounds = bounds.reduced (20);

    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle).withAlpha (0.2f));

    // Vertical lines (time/frequency divisions)
    for (int i = 1; i < 8; ++i)
    {
        float x = drawBounds.getX() + drawBounds.getWidth() * i / 8.0f;
        g.drawVerticalLine (x, drawBounds.getY(), drawBounds.getBottom());
    }

    // Horizontal lines (amplitude divisions)
    for (int i = 1; i < 4; ++i)
    {
        float y = drawBounds.getY() + drawBounds.getHeight() * i / 4.0f;
        g.drawHorizontalLine (y, drawBounds.getX(), drawBounds.getRight());
    }
}

void VisualizationEngine::drawLabels (juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setFont (theme.getFont ("caption"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    // Mode label
    const char* modeNames[] = {"WAVEFORM", "SPECTRUM", "SPECTROGRAM", "OSCILLOSCOPE"};
    juce::String modeLabel = "[" + juce::String (modeNames[currentMode]) + "]";
    g.drawText (modeLabel, bounds.reduced (10).removeFromTop (20),
                juce::Justification::centredLeft);

    // Scale labels
    if (currentMode == Waveform)
    {
        g.drawText ("1.0", bounds.getX() + 10, bounds.getY() + 25, 30, 16, juce::Justification::centredLeft);
        g.drawText ("-1.0", bounds.getX() + 10, bounds.getBottom() - 40, 30, 16, juce::Justification::centredLeft);
    }
    else if (currentMode == Spectrum || currentMode == Spectrogram)
    {
        g.drawText ("0 dB", bounds.getX() + 10, bounds.getY() + 25, 30, 16, juce::Justification::centredLeft);
        g.drawText ("-60 dB", bounds.getX() + 10, bounds.getBottom() - 40, 40, 16, juce::Justification::centredLeft);
    }
}

} // namespace zalaxy