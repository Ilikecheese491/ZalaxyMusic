#pragma once

#include <JuceHeader.h>

namespace zalaxy
{

struct ColorScheme
{
    // BASE PALETTE (Cosmic Dark - Professional)
    static constexpr juce::Colour bgDeepest       = juce::Colour (0xFF0A0E17);
    static constexpr juce::Colour bgDeep          = juce::Colour (0xFF111827);
    static constexpr juce::Colour bgElevated      = juce::Colour (0xFF1A2234);
    static constexpr juce::Colour bgHover         = juce::Colour (0xFF222D44);
    static constexpr juce::Colour bgActive        = juce::Colour (0xFF2D3A55);

    static constexpr juce::Colour surface         = juce::Colour (0xFF1E2A3E);
    static constexpr juce::Colour surfaceHover    = juce::Colour (0xFF283652);
    static constexpr juce::Colour surfaceActive   = juce::Colour (0xFF344566);

    static constexpr juce::Colour borderSubtle    = juce::Colour (0xFF2A3A55);
    static constexpr juce::Colour borderDefault   = juce::Colour (0xFF3A4D6E);
    static constexpr juce::Colour borderStrong    = juce::Colour (0xFF4A6080);
    static constexpr juce::Colour borderFocus     = juce::Colour (0xFF5B8DEF);

    static constexpr juce::Colour textPrimary     = juce::Colour (0xFFE8ECF4);
    static constexpr juce::Colour textSecondary   = juce::Colour (0xFFB8C1D4);
    static constexpr juce::Colour textMuted       = juce::Colour (0xFF8A95AA);
    static constexpr juce::Colour textDisabled    = juce::Colour (0xFF5A6578);
    static constexpr juce::Colour textInverse     = juce::Colour (0xFF0A0E17);

    static constexpr juce::Colour accentPrimary   = juce::Colour (0xFF5B8DEF);
    static constexpr juce::Colour accentPrimaryDim = juce::Colour (0xFF4A74C8);
    static constexpr juce::Colour accentSecondary = juce::Colour (0xFF8B6FF6);
    static constexpr juce::Colour accentWarm      = juce::Colour (0xFFF5A623);
    static constexpr juce::Colour accentDanger    = juce::Colour (0xFFE85D5D);
    static constexpr juce::Colour accentSuccess   = juce::Colour (0xFF4CD97A);

    static constexpr juce::Colour oscColor        = juce::Colour (0xFF5B8DEF);
    static constexpr juce::Colour filterColor     = juce::Colour (0xFFE85D5D);
    static constexpr juce::Colour envColor        = juce::Colour (0xFF4CD97A);
    static constexpr juce::Colour lfoColor        = juce::Colour (0xFF8B6FF6);
    static constexpr juce::Colour modColor        = juce::Colour (0xFFF5A623);
    static constexpr juce::Colour fxColor         = juce::Colour (0xFF22D3EE);
    static constexpr juce::Colour masterColor     = juce::Colour (0xFFFFFFFF);

    static constexpr juce::Colour waveformColor   = juce::Colour (0xFF5B8DEF);
    static constexpr juce::Colour spectrumColor   = juce::Colour (0xFF4CD97A);
    static constexpr juce::Colour gridColor       = juce::Colour (0xFF1A2535);
    static constexpr juce::Colour cursorColor     = juce::Colour (0xFF8B6FF6);

    static constexpr juce::Colour shadowUmbra     = juce::Colour (0x40000000);
    static constexpr juce::Colour shadowPenumbra  = juce::Colour (0x20000000);
    static constexpr juce::Colour shadowAmbient   = juce::Colour (0x10000000);

    // SPACING SYSTEM (4px base)
    static constexpr int space1 = 4;
    static constexpr int space2 = 8;
    static constexpr int space3 = 12;
    static constexpr int space4 = 16;
    static constexpr int space5 = 24;
    static constexpr int space6 = 32;
    static constexpr int space7 = 48;

    // BORDER RADIUS (concentric)
    static constexpr float radiusSmall   = 4.0f;
    static constexpr float radiusMedium  = 8.0f;
    static constexpr float radiusLarge   = 12.0f;
    static constexpr float radiusXLarge  = 16.0f;

    static float getConcentricRadius (float parentRadius, float padding)
    {
        return juce::jmax (0.0f, parentRadius - padding);
    }

    // TYPOGRAPHY
    static juce::Font getFontHeading1() { return juce::Font (juce::Font::getDefaultSansSerifFontName(), 20.0f, juce::Font::bold); }
    static juce::Font getFontHeading2() { return juce::Font (juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::bold); }
    static juce::Font getFontHeading3() { return juce::Font (juce::Font::getDefaultSansSerifFontName(), 13.0f, juce::Font::bold); }
    static juce::Font getFontBody()     { return juce::Font (juce::Font::getDefaultSansSerifFontName(), 12.0f, juce::Font::plain); }
    static juce::Font getFontBodySmall(){ return juce::Font (juce::Font::getDefaultSansSerifFontName(), 11.0f, juce::Font::plain); }
    static juce::Font getFontCaption()  { return juce::Font (juce::Font::getDefaultSansSerifFontName(), 10.0f, juce::Font::plain); }
    static juce::Font getFontMono()     { return juce::Font (juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain); }
    static juce::Font getFontValue()    { return juce::Font (juce::Font::getDefaultMonospacedFontName(), 13.0f, juce::Font::bold); }

    // SHADOWS
    static juce::DropShadow getShadowElevation1() { return juce::DropShadow (shadowAmbient, 4, {0, 2}); }
    static juce::DropShadow getShadowElevation2() { return juce::DropShadow (shadowPenumbra, 8, {0, 4}); }
    static juce::DropShadow getShadowElevation3() { return juce::DropShadow (shadowUmbra, 16, {0, 8}); }

    // ANIMATION
    static constexpr int animFast    = 80;
    static constexpr int animNormal  = 150;
    static constexpr int animSlow    = 250;

    // HIT AREAS
    static constexpr int minHitSize = 40;
    static constexpr int preferredHitSize = 44;

    // KNOB SIZES
    static constexpr int knobSmall  = 44;
    static constexpr int knobMedium = 56;
    static constexpr int knobLarge  = 72;

    // PANEL LAYOUT
    static constexpr int panelPadding = 16;
    static constexpr int panelSpacing = 12;
    static constexpr int labelHeight = 18;
    static constexpr int valueHeight = 20;
    static constexpr int controlHeight = 56;
};

} // namespace zalaxy