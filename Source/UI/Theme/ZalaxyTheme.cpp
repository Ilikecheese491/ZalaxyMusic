#include "ZalaxyTheme.h"

namespace zalaxy
{

ZalaxyTheme::ZalaxyTheme()
{
    buildColorMap();
}

ZalaxyTheme::~ZalaxyTheme()
{
}

void ZalaxyTheme::initialize()
{
    // Called once at startup to register colors with JUCE LookAndFeel
}

void ZalaxyTheme::registerColors (juce::LookAndFeel& laf) const
{
    for (int i = 0; i <= (int) ColorId::ToggleButtonTick; ++i)
    {
        if (colorMap[i] != juce::Colour())
        {
            laf.setColour (i, colorMap[i]);
        }
    }
}

juce::Colour ZalaxyTheme::getColor (ColorId id) const
{
    return colorMap[(int) id];
}

juce::Colour ZalaxyTheme::getParameterColor (const juce::String& paramID) const
{
    switch (getParameterCategory (paramID))
    {
        case ParamCategory::Oscillator:  return ColorScheme::oscColor;
        case ParamCategory::Filter:      return ColorScheme::filterColor;
        case ParamCategory::Envelope:    return ColorScheme::envColor;
        case ParamCategory::LFO:         return ColorScheme::lfoColor;
        case ParamCategory::Modulation:  return ColorScheme::modColor;
        case ParamCategory::Effect:      return ColorScheme::fxColor;
        case ParamCategory::Master:      return ColorScheme::masterColor;
        default:                         return ColorScheme::textPrimary;
    }
}

juce::Font ZalaxyTheme::getFont (const juce::String& style) const
{
    if (style == "heading1") return ColorScheme::getFontHeading1();
    if (style == "heading2") return ColorScheme::getFontHeading2();
    if (style == "heading3") return ColorScheme::getFontHeading3();
    if (style == "body")     return ColorScheme::getFontBody();
    if (style == "bodySmall")return ColorScheme::getFontBodySmall();
    if (style == "caption")  return ColorScheme::getFontCaption();
    if (style == "mono")     return ColorScheme::getFontMono();
    if (style == "value")    return ColorScheme::getFontValue();
    return ColorScheme::getFontBody();
}

int ZalaxyTheme::getSpace (int level) const
{
    switch (level)
    {
        case 1: return ColorScheme::space1;
        case 2: return ColorScheme::space2;
        case 3: return ColorScheme::space3;
        case 4: return ColorScheme::space4;
        case 5: return ColorScheme::space5;
        case 6: return ColorScheme::space6;
        case 7: return ColorScheme::space7;
        default: return ColorScheme::space4;
    }
}

float ZalaxyTheme::getRadius (const juce::String& size) const
{
    if (size == "small")   return ColorScheme::radiusSmall;
    if (size == "medium")  return ColorScheme::radiusMedium;
    if (size == "large")   return ColorScheme::radiusLarge;
    if (size == "xlarge")  return ColorScheme::radiusXLarge;
    return ColorScheme::radiusMedium;
}

juce::DropShadow ZalaxyTheme::getShadow (int elevation) const
{
    switch (elevation)
    {
        case 1: return ColorScheme::getShadowElevation1();
        case 2: return ColorScheme::getShadowElevation2();
        case 3: return ColorScheme::getShadowElevation3();
        default: return juce::DropShadow();
    }
}

int ZalaxyTheme::getAnimDuration (const juce::String& speed) const
{
    if (speed == "fast")   return ColorScheme::animFast;
    if (speed == "normal") return ColorScheme::animNormal;
    if (speed == "slow")   return ColorScheme::animSlow;
    return ColorScheme::animNormal;
}

juce::Rectangle<int> ZalaxyTheme::expandHitArea (const juce::Rectangle<int>& bounds) const
{
    int minSize = ColorScheme::minHitSize;
    int w = juce::jmax (bounds.getWidth(), minSize);
    int h = juce::jmax (bounds.getHeight(), minSize);
    return bounds.withSize (w, h);
}

ZalaxyTheme::ParamCategory ZalaxyTheme::getParameterCategory (const juce::String& paramID)
{
    if (paramID.startsWith ("osc") || paramID.startsWith ("subOsc") || paramID.startsWith ("noise"))
        return ParamCategory::Oscillator;
    if (paramID.startsWith ("filter"))
        return ParamCategory::Filter;
    if (paramID.startsWith ("amp") || paramID.startsWith ("filter") && paramID.contains ("Env") || paramID.startsWith ("mod") && paramID.contains ("Attack") || paramID.startsWith ("mod") && paramID.contains ("Decay") || paramID.startsWith ("mod") && paramID.contains ("Sustain") || paramID.startsWith ("mod") && paramID.contains ("Release"))
        return ParamCategory::Envelope;
    if (paramID.startsWith ("lfo"))
        return ParamCategory::LFO;
    if (paramID.startsWith ("modSlot"))
        return ParamCategory::Modulation;
    if (paramID.startsWith ("reverb") || paramID.startsWith ("delay") || paramID.startsWith ("distortion") || paramID.startsWith ("chorus"))
        return ParamCategory::Effect;
    if (paramID.startsWith ("master") || paramID == "polyphony")
        return ParamCategory::Master;
    return ParamCategory::Unknown;
}

juce::String ZalaxyTheme::formatParameterValue (const juce::String& paramID, float value, int maxChars)
{
    juce::String unit = getParameterUnit (paramID);
    juce::String formatted;

    if (unit == "Hz")
    {
        if (value >= 1000.0f)
            formatted = juce::String (value / 1000.0f, 2) + " kHz";
        else
            formatted = juce::String ((int) value) + " Hz";
    }
    else if (unit == "ms")
    {
        formatted = juce::String (value, 1) + " ms";
    }
    else if (unit == "s")
    {
        if (value >= 1.0f)
            formatted = juce::String (value, 2) + " s";
        else
            formatted = juce::String (value * 1000.0f, 0) + " ms";
    }
    else if (unit == "%")
    {
        formatted = juce::String ((int) (value * 100.0f)) + "%";
    }
    else if (unit == "dB")
    {
        formatted = juce::String (value, 1) + " dB";
    }
    else if (unit == "semitones")
    {
        formatted = juce::String (value, 1) + " st";
    }
    else if (unit == "cents")
    {
        formatted = juce::String ((int) value) + " ct";
    }
    else if (unit == "x")
    {
        formatted = juce::String (value, 1) + "x";
    }
    else if (unit == "voices")
    {
        formatted = juce::String ((int) value);
    }
    else
    {
        formatted = juce::String (value, 2);
    }

    if (formatted.length() > maxChars)
        formatted = formatted.substring (0, maxChars);

    return formatted;
}

juce::String ZalaxyTheme::getParameterUnit (const juce::String& paramID)
{
    if (paramID.contains ("Cutoff") || paramID.contains ("Rate") && paramID.contains ("lfo") || paramID.contains ("Frequency"))
        return "Hz";
    if (paramID.contains ("Time") || paramID.contains ("Delay") || paramID.contains ("Attack") || paramID.contains ("Decay") || paramID.contains ("Release") || paramID.contains ("PreDelay"))
        return "ms";
    if (paramID.contains ("FadeIn") || (paramID.contains ("Attack") || paramID.contains ("Decay") || paramID.contains ("Release")) && !paramID.contains ("Filter"))
        return "s";
    if (paramID.contains ("Level") || paramID.contains ("Mix") || paramID.contains ("Depth") || paramID.contains ("Amount") || paramID.contains ("Feedback") || paramID.contains ("Sustain") || paramID.contains ("Velocity") || paramID.contains ("Keytrack") || paramID.contains ("Drive") || paramID.contains ("Width") || paramID.contains ("Damping") || paramID.contains ("RoomSize") || paramID.contains ("DryLevel") || paramID.contains ("WetLevel") || paramID.contains ("Gate") || paramID.contains ("Blend"))
        return "%";
    if (paramID.contains ("Gain") || paramID.contains ("Tone"))
        return "dB";
    if (paramID.contains ("Pitch") && !paramID.contains ("Mod"))
        return "semitones";
    if (paramID.contains ("FineTune"))
        return "cents";
    if (paramID.contains ("PhaseMod"))
        return "x";
    if (paramID == "polyphony")
        return "voices";
    if (paramID.contains ("Pan"))
        return "%";

    return "";
}

void ZalaxyTheme::buildColorMap()
{
    // Backgrounds
    colorMap[(int) ColorId::Background]         = ColorScheme::bgDeepest;
    colorMap[(int) ColorId::PanelBackground]    = ColorScheme::bgDeep;
    colorMap[(int) ColorId::ElevatedBackground] = ColorScheme::bgElevated;
    colorMap[(int) ColorId::HoverBackground]    = ColorScheme::bgHover;
    colorMap[(int) ColorId::ActiveBackground]   = ColorScheme::bgActive;

    // Surfaces
    colorMap[(int) ColorId::Surface]         = ColorScheme::surface;
    colorMap[(int) ColorId::SurfaceHover]    = ColorScheme::surfaceHover;
    colorMap[(int) ColorId::SurfaceActive]   = ColorScheme::surfaceActive;

    // Borders
    colorMap[(int) ColorId::BorderSubtle]    = ColorScheme::borderSubtle;
    colorMap[(int) ColorId::BorderDefault]   = ColorScheme::borderDefault;
    colorMap[(int) ColorId::BorderStrong]    = ColorScheme::borderStrong;
    colorMap[(int) ColorId::BorderFocus]     = ColorScheme::borderFocus;

    // Text
    colorMap[(int) ColorId::TextPrimary]     = ColorScheme::textPrimary;
    colorMap[(int) ColorId::TextSecondary]   = ColorScheme::textSecondary;
    colorMap[(int) ColorId::TextMuted]       = ColorScheme::textMuted;
    colorMap[(int) ColorId::TextDisabled]    = ColorScheme::textDisabled;
    colorMap[(int) ColorId::TextInverse]     = ColorScheme::textInverse;

    // Accents
    colorMap[(int) ColorId::AccentPrimary]      = ColorScheme::accentPrimary;
    colorMap[(int) ColorId::AccentPrimaryDim]   = ColorScheme::accentPrimaryDim;
    colorMap[(int) ColorId::AccentSecondary]    = ColorScheme::accentSecondary;
    colorMap[(int) ColorId::AccentWarm]         = ColorScheme::accentWarm;
    colorMap[(int) ColorId::AccentDanger]       = ColorScheme::accentDanger;
    colorMap[(int) ColorId::AccentSuccess]      = ColorScheme::accentSuccess;

    // Parameter categories
    colorMap[(int) ColorId::OscillatorColor]   = ColorScheme::oscColor;
    colorMap[(int) ColorId::FilterColor]       = ColorScheme::filterColor;
    colorMap[(int) ColorId::EnvelopeColor]     = ColorScheme::envColor;
    colorMap[(int) ColorId::LFOColor]          = ColorScheme::lfoColor;
    colorMap[(int) ColorId::ModulationColor]   = ColorScheme::modColor;
    colorMap[(int) ColorId::EffectsColor]      = ColorScheme::fxColor;
    colorMap[(int) ColorId::MasterColor]       = ColorScheme::masterColor;

    // Visualization
    colorMap[(int) ColorId::WaveformColor]     = ColorScheme::waveformColor;
    colorMap[(int) ColorId::SpectrumColor]     = ColorScheme::spectrumColor;
    colorMap[(int) ColorId::GridColor]         = ColorScheme::gridColor;
    colorMap[(int) ColorId::CursorColor]       = ColorScheme::cursorColor;

    // JUCE standard overrides
    colorMap[(int) ColorId::TextEditorBackground]    = ColorScheme::surface;
    colorMap[(int) ColorId::TextEditorOutline]       = ColorScheme::borderDefault;
    colorMap[(int) ColorId::TextEditorFocusOutline]  = ColorScheme::borderFocus;
    colorMap[(int) ColorId::TextEditorText]          = ColorScheme::textPrimary;
    colorMap[(int) ColorId::TextEditorHighlight]     = ColorScheme::accentPrimary.withAlpha (0.3f);
    colorMap[(int) ColorId::ComboBoxBackground]      = ColorScheme::surface;
    colorMap[(int) ColorId::ComboBoxOutline]         = ColorScheme::borderDefault;
    colorMap[(int) ColorId::ComboBoxArrow]           = ColorScheme::textSecondary;
    colorMap[(int) ColorId::PopupMenuBackground]     = ColorScheme::bgElevated;
    colorMap[(int) ColorId::PopupMenuText]           = ColorScheme::textPrimary;
    colorMap[(int) ColorId::PopupMenuHighlightedBackground] = ColorScheme::bgHover;
    colorMap[(int) ColorId::PopupMenuHighlightedText]       = ColorScheme::textPrimary;
    colorMap[(int) ColorId::SliderTrackBackground]   = ColorScheme::borderSubtle;
    colorMap[(int) ColorId::SliderThumb]             = ColorScheme::accentPrimary;
    colorMap[(int) ColorId::ButtonBackground]        = ColorScheme::surface;
    colorMap[(int) ColorId::ButtonText]              = ColorScheme::textPrimary;
    colorMap[(int) ColorId::ToggleButtonBackground]  = ColorScheme::surface;
    colorMap[(int) ColorId::ToggleButtonTick]        = ColorScheme::accentPrimary;
}

} // namespace zalaxy