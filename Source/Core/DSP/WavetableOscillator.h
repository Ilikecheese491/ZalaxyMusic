#pragma once

#include "JuceModules.h"
#include <vector>
#include <array>
#include <memory>

namespace zalaxy
{

// ================================================================================
// Wavetable Data Structure
// ================================================================================

struct Wavetable
{
    static constexpr int TABLE_SIZE = 2048;
    static constexpr int NUM_TABLES = 256;  // For morphing across wavetable positions

    std::array<float, TABLE_SIZE> tables[NUM_TABLES];
    juce::String name;
    bool isValid = false;

    float getSample (int tableIndex, float phase) const noexcept
    {
        if (!isValid || tableIndex < 0 || tableIndex >= NUM_TABLES)
            return 0.0f;

        const auto& table = tables[juce::jlimit (0, NUM_TABLES - 1, tableIndex)];
        float index = phase * (TABLE_SIZE - 1);
        int idx0 = (int)index;
        int idx1 = (idx0 + 1) % TABLE_SIZE;
        float frac = index - idx0;

        return table[idx0] + frac * (table[idx1] - table[idx0]);
    }

    float getSampleInterpolated (float tablePos, float phase) const noexcept
    {
        if (!isValid)
            return 0.0f;

        float clampedPos = juce::jlimit (0.0f, (float)(NUM_TABLES - 1), tablePos);
        int idx0 = (int)clampedPos;
        int idx1 = juce::jmin (idx0 + 1, NUM_TABLES - 1);
        float frac = clampedPos - idx0;

        float sample0 = getSample (idx0, phase);
        float sample1 = getSample (idx1, phase);

        return sample0 + frac * (sample1 - sample0);
    }
};

// ================================================================================
// Wavetable Bank - Manages multiple wavetables
// ================================================================================

class WavetableBank
{
public:
    WavetableBank()
    {
        initializeFactoryTables();
    }

    const Wavetable* getWavetable (int index) const noexcept
    {
        if (index >= 0 && index < (int)wavetables.size())
            return &wavetables[index];
        return nullptr;
    }

    const Wavetable* getWavetableByName (const juce::String& name) const noexcept
    {
        for (auto& wt : wavetables)
        {
            if (wt.name == name && wt.isValid)
                return &wt;
        }
        return nullptr;
    }

    int getNumWavetables() const noexcept { return (int)wavetables.size(); }
    const juce::StringArray getWavetableNames() const noexcept;

    // Load custom wavetable from file (WAV, or serialized format)
    bool loadWavetableFromFile (const juce::File& file, const juce::String& name);

    // Create basic wavetables programmatically
    static Wavetable createBasicWaveform (const juce::String& type, const juce::String& name);
    static Wavetable createSpectralWavetable (const std::vector<float>& harmonics, const juce::String& name);

private:
    std::vector<Wavetable> wavetables;

    void initializeFactoryTables();
    void addWavetable (Wavetable&& wt);
};

// ================================================================================
// Wavetable Oscillator - Vital/Serum Style
// ================================================================================

class WavetableOscillator
{
public:
    // Parameter IDs for APVTS integration
    struct ParameterIDs
    {
        static constexpr const char* wavePosition = "osc_wt_pos";
        static constexpr const char* morph = "osc_wt_morph";
        static constexpr const char* spectralWarp = "osc_wt_spectral_warp";
        static constexpr const char* fmAmount = "osc_wt_fm_amount";
        static constexpr const char* fmSource = "osc_wt_fm_source";
        static constexpr const char* phaseDistortion = "osc_wt_phase_dist";
        static constexpr const char* unisonVoices = "osc_wt_unison_voices";
        static constexpr const char* unisonDetune = "osc_wt_unison_detune";
        static constexpr const char* unisonSpread = "osc_wt_unison_spread";
        static constexpr const char* wtSelect = "osc_wt_select";
    };

    WavetableOscillator();
    ~WavetableOscillator();

    // Initialize with sample rate and wavetable bank
    void prepare (double sampleRate, int maxBlockSize, const WavetableBank& bank);

    // Process block - renders audio into output buffer
    void process (juce::AudioBuffer<float>& outputBuffer,
                  const juce::AudioBuffer<float>* fmInput = nullptr,
                  int numSamples = -1);

    // Process single sample (for mono voice)
    float processSample (float fmInput = 0.0f);

    // Parameter setters (called from audio thread - must be lock-free)
    void setWavePosition (float position) noexcept;      // 0.0 - 1.0
    void setMorph (float morph) noexcept;                // 0.0 - 1.0 (inter-table morphing)
    void setSpectralWarp (float warp) noexcept;          // -1.0 to 1.0 (spectral shift)
    void setFMAmount (float amount) noexcept;            // 0.0 - 1.0
    void setFMSource (int source) noexcept;              // 0=off, 1=osc2, 2=noise, 3=filter
    void setPhaseDistortion (float amount) noexcept;     // 0.0 - 1.0
    void setUnison (int voices, float detune, float spread) noexcept;
    void setWavetable (int index) noexcept;

    // Frequency control
    void setFrequency (float hz) noexcept;
    void setNote (int midiNote, float centsOffset = 0.0f) noexcept;

    // Phase control
    void resetPhase() noexcept { phase = 0.0; }
    void setPhase (float newPhase) noexcept { phase = newPhase; }

    // Getters
    float getFrequency() const noexcept { return currentFreq; }
    int getCurrentWavetableIndex() const noexcept { return currentWavetableIndex; }
    const Wavetable* getCurrentWavetable() const noexcept;

    // Polyphonic voice support
    struct VoiceState
    {
        double phase = 0.0;
        double phaseIncrement = 0.0;
        float lastOutput = 0.0f;
        std::array<float, 8> unisonPhases{};  // Max 8 unison voices
        std::array<float, 8> unisonIncrements{};
    };

    void updateVoiceState (VoiceState& voice, float freq) noexcept;

private:
    // Render wavetable with spectral warp
    float renderWavetable (const Wavetable* wt, float phase, float wtPos, float morph, float warp) const noexcept;

    // Apply phase distortion
    float applyPhaseDistortion (float phase, float amount) const noexcept;

    // Unison rendering
    float renderUnison (const Wavetable* wt, VoiceState& voice, float wtPos, float morph, float warp, float fmInput) const noexcept;

    // Wavetable bank reference
    const WavetableBank* wavetableBank = nullptr;

    // Current state
    int currentWavetableIndex = 0;
    double currentFreq = 440.0;
    double phaseIncrement = 0.0;
    double phase = 0.0;
    double sampleRate = 44100.0;

    // Parameters (smoothed for audio thread)
    float wavePosition = 0.0f;
    float morph = 0.0f;
    float spectralWarp = 0.0f;
    float fmAmount = 0.0f;
    int fmSource = 0;
    float phaseDistortion = 0.0f;

    // Unison
    int unisonVoices = 1;
    float unisonDetune = 0.0f;
    float unisonSpread = 0.0f;

    // Smoothing
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedWavePos;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedMorph;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedWarp;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedFM;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedPhaseDist;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableOscillator)
};

// ================================================================================
// Wavetable Editor/Utilities
// ================================================================================

namespace WavetableUtils
{
    // Generate a single-cycle waveform from harmonic series
    std::vector<float> generateHarmonicSeries (const std::vector<float>& amplitudes,
                                                const std::vector<float>& phases,
                                                int tableSize = Wavetable::TABLE_SIZE);

    // Apply spectral warp (frequency domain shifting)
    void applySpectralWarp (std::vector<float>& table, float warpAmount);

    // Morph between two wavetables
    void morphWavetables (const std::vector<float>& a, const std::vector<float>& b,
                          float amount, std::vector<float>& output);

    // Normalize wavetable to prevent DC offset and ensure consistent amplitude
    void normalizeWavetable (std::vector<float>& table);

    // Generate standard wavetables
    std::vector<float> generateSine (int tableSize = Wavetable::TABLE_SIZE);
    std::vector<float> generateTriangle (int tableSize = Wavetable::TABLE_SIZE);
    std::vector<float> generateSaw (int tableSize = Wavetable::TABLE_SIZE);
    std::vector<float> generateSquare (int tableSize = Wavetable::TABLE_SIZE, float pulseWidth = 0.5f);
    std::vector<float> generateNoise (int tableSize = Wavetable::TABLE_SIZE);

    // Window functions for spectral processing
    void applyWindow (std::vector<float>& data, const juce::String& windowType = "hann");

    // FFT-based spectral morphing (for advanced warp)
    void spectralMorph (const std::vector<float>& a, const std::vector<float>& b,
                        float amount, std::vector<float>& output);
}

} // namespace zalaxy