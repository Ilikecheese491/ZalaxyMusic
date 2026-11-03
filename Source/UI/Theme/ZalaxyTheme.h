#pragma once

#include <JuceHeader.h>
#include "ColorScheme.h"

namespace zalaxy
{

class ZalaxyTheme
{
public:
    // Color IDs for JUCE LookAndFeel
    enum class ColorId
    {
        // Backgrounds
        Background = 0x1000000,
        PanelBackground,
        ElevatedBackground,
        HoverBackground,
        ActiveBackground,

        // Surfaces
        Surface,
        SurfaceHover,
        SurfaceActive,

        // Borders
        BorderSubtle,
        BorderDefault,
        BorderStrong,
        BorderFocus,

        // Text
        TextPrimary,
        TextSecondary,
        TextMuted,
        TextDisabled,
        TextInverse,

        // Accents
        AccentPrimary,
        AccentPrimaryDim,
        AccentSecondary,
        AccentWarm,
        AccentDanger,
        AccentSuccess,

        // Parameter categories
        OscillatorColor,
        FilterColor,
        EnvelopeColor,
        LFOColor,
        ModulationColor,
        EffectsColor,
        MasterColor,

        // Visualization
        WaveformColor,
        SpectrumColor,
        GridColor,
        CursorColor,

        // Standard JUCE overrides
        TextEditorBackground,
        TextEditorOutline,
        TextEditorFocusOutline,
        TextEditorText,
        TextEditorHighlight,
        ComboBoxBackground,
        ComboBoxOutline,
        ComboBoxArrow,
        PopupMenuBackground,
        PopupMenuText,
        PopupMenuHighlightedBackground,
        PopupMenuHighlightedText,
        SliderTrackBackground,
        SliderThumb,
        ButtonBackground,
        ButtonText,
        ToggleButtonBackground,
        ToggleButtonTick,
    };

    ZalaxyTheme();
    ~ZalaxyTheme();

    void initialize();
    void registerColors (juce::LookAndFeel& laf) const;

    // Color access
    juce::Colour getColor (ColorId id) const;
    juce::Colour getParameterColor (const juce::String& paramID) const;

    // Font access
    juce::Font getFont (const juce::String& style) const;

    // Spacing
    int getSpace (int level) const;
    float getRadius (const juce::String& size) const;

    // Shadows
    juce::DropShadow getShadow (int elevation) const;

    // Animation
    int getAnimDuration (const juce::String& speed) const;

    // Hit area
    juce::Rectangle<int> expandHitArea (const juce::Rectangle<int>& bounds) const;

    // Parameter category detection
    enum class ParamCategory
    {
        Master,
        Oscillator,
        Filter,
        Envelope,
        LFO,
        Modulation,
        Effect,
        Unknown
    };

    static ParamCategory getParameterCategory (const juce::String& paramID);

    // Value formatting
    static juce::String formatParameterValue (const juce::String& paramID, float value, int maxChars = 6);

private:
    mutable juce::Colour colorMap [(int) ColorId::ToggleButtonTick + 1];

    void buildColorMap();
    static juce::String getParameterUnit (const juce::String& paramID);
};

} // namespace zalaxy