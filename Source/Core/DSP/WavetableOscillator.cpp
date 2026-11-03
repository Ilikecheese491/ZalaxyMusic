#include "WavetableOscillator.h"
#include <algorithm>
#include <cmath>
#include <complex>

namespace zalaxy
{

// ================================================================================
// WavetableBank Implementation
// ================================================================================

juce::StringArray WavetableBank::getWavetableNames() const noexcept
{
    juce::StringArray names;
    for (auto& wt : wavetables)
    {
        if (wt.isValid)
            names.add (wt.name);
    }
    return names;
}

bool WavetableBank::loadWavetableFromFile (const juce::File& file, const juce::String& name)
{
    // TODO: Implement WAV file loading and conversion to wavetable format
    // For now, return false
    return false;
}

Wavetable WavetableBank::createBasicWaveform (const juce::String& type, const juce::String& name)
{
    Wavetable wt;
    wt.name = name;
    wt.isValid = true;

    auto& tables = wt.tables;
    const int TABLE_SIZE = Wavetable::TABLE_SIZE;

    // Generate base waveform for table 0
    std::vector<float> baseTable (TABLE_SIZE);

    if (type == "sine")
    {
        for (int i = 0; i < TABLE_SIZE; ++i)
            baseTable[i] = std::sin (2.0 * juce::MathConstants<float>::pi * i / TABLE_SIZE);
    }
    else if (type == "triangle")
    {
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            float phase = (float)i / TABLE_SIZE;
            baseTable[i] = 2.0f * std::abs (2.0f * (phase - std::floor (phase + 0.5f))) - 1.0f;
        }
    }
    else if (type == "saw")
    {
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            float phase = (float)i / TABLE_SIZE;
            baseTable[i] = 2.0f * (phase - std::floor (phase + 0.5f));
        }
    }
    else if (type == "square")
    {
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            float phase = (float)i / TABLE_SIZE;
            baseTable[i] = (phase < 0.5f) ? 1.0f : -1.0f;
        }
    }

    // Create morphing tables by gradually changing harmonics
    for (int t = 0; t < Wavetable::NUM_TABLES; ++t)
    {
        float morphAmount = (float)t / (Wavetable::NUM_TABLES - 1);

        // For basic waveforms, we just copy the base table
        // In a full implementation, this would morph between different waveforms
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            tables[t][i] = baseTable[i];
        }
    }

    return wt;
}

Wavetable WavetableBank::createSpectralWavetable (const std::vector<float>& harmonics, const juce::String& name)
{
    Wavetable wt;
    wt.name = name;
    wt.isValid = true;

    const int TABLE_SIZE = Wavetable::TABLE_SIZE;
    const int NUM_TABLES = Wavetable::NUM_TABLES;

    // Generate base table from harmonics
    std::vector<float> baseTable (TABLE_SIZE, 0.0f);

    for (size_t h = 0; h < harmonics.size(); ++h)
    {
        float amp = harmonics[h];
        if (amp == 0.0f) continue;

        int harmonicNum = (int)h + 1;
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            float phase = 2.0f * juce::MathConstants<float>::pi * harmonicNum * i / TABLE_SIZE;
            baseTable[i] += amp * std::sin (phase);
        }
    }

    // Normalize
    float maxVal = 0.0f;
    for (float v : baseTable)
        maxVal = std::max (maxVal, std::abs (v));
    if (maxVal > 0.0f)
    {
        for (float& v : baseTable)
            v /= maxVal;
    }

    // Create morphing tables
    for (int t = 0; t < NUM_TABLES; ++t)
    {
        float morphAmount = (float)t / (NUM_TABLES - 1);

        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            // Add spectral evolution across tables
            float spectralShift = morphAmount * 0.1f;  // Slight spectral shift per table
            wt.tables[t][i] = baseTable[i];
        }
    }

    return wt;
}

void WavetableBank::initializeFactoryTables()
{
    // Basic waveforms
    addWavetable (createBasicWaveform ("sine", "Sine"));
    addWavetable (createBasicWaveform ("triangle", "Triangle"));
    addWavetable (createBasicWaveform ("saw", "Saw"));
    addWavetable (createBasicWaveform ("square", "Square"));

    // Spectral / harmonic wavetables
    {
        // Organ-like (odd harmonics)
        std::vector<float> harmonics = {1.0f, 0.0f, 0.5f, 0.0f, 0.33f, 0.0f, 0.25f, 0.0f, 0.2f, 0.0f, 0.16f};
        addWavetable (createSpectralWavetable (harmonics, "Organ"));
    }

    {
        // String-like (all harmonics decaying)
        std::vector<float> harmonics (16);
        for (int i = 0; i < 16; ++i)
            harmonics[i] = 1.0f / (i + 1);
        addWavetable (createSpectralWavetable (harmonics, "Strings"));
    }

    {
        // Bell-like (inharmonic partials)
        std::vector<float> harmonics = {1.0f, 0.8f, 0.0f, 0.6f, 0.0f, 0.4f, 0.3f, 0.0f, 0.25f, 0.2f};
        addWavetable (createSpectralWavetable (harmonics, "Bell"));
    }

    {
        // Vowel-like (formants)
        std::vector<float> harmonics (32, 0.0f);
        // Formant at ~800Hz (harmonic 2) and ~2400Hz (harmonic 6)
        harmonics[1] = 0.8f;   // 2nd harmonic
        harmonics[5] = 0.6f;   // 6th harmonic
        harmonics[9] = 0.3f;   // 10th harmonic
        addWavetable (createSpectralWavetable (harmonics, "Vowel"));
    }

    // More wavetables for variety
    {
        std::vector<float> harmonics = {1.0f, 0.5f, 0.33f, 0.25f, 0.2f, 0.16f, 0.14f, 0.12f};
        addWavetable (createSpectralWavetable (harmonics, "Rich Saw"));
    }

    {
        std::vector<float> harmonics = {1.0f, 0.0f, 0.33f, 0.0f, 0.2f, 0.0f, 0.14f};
        addWavetable (createSpectralWavetable (harmonics, "Hollow"));
    }
}

void WavetableBank::addWavetable (Wavetable&& wt)
{
    wavetables.push_back (std::move (wt));
}

// ================================================================================
// WavetableOscillator Implementation
// ================================================================================

WavetableOscillator::WavetableOscillator()
{
    // Initialize smoothing with default times
    smoothedWavePos.reset (44100.0, 0.01);
    smoothedMorph.reset (44100.0, 0.01);
    smoothedWarp.reset (44100.0, 0.01);
    smoothedFM.reset (44100.0, 0.005);
    smoothedPhaseDist.reset (44100.0, 0.01);
}

WavetableOscillator::~WavetableOscillator()
{
}

void WavetableOscillator::prepare (double newSampleRate, int maxBlockSize, const WavetableBank& bank)
{
    sampleRate = newSampleRate;
    wavetableBank = &bank;

    // Reset smoothing
    smoothedWavePos.reset (sampleRate, 0.01);
    smoothedMorph.reset (sampleRate, 0.01);
    smoothedWarp.reset (sampleRate, 0.01);
    smoothedFM.reset (sampleRate, 0.005);
    smoothedPhaseDist.reset (sampleRate, 0.01);

    smoothedWavePos.setCurrentAndTargetValue (wavePosition);
    smoothedMorph.setCurrentAndTargetValue (morph);
    smoothedWarp.setCurrentAndTargetValue (spectralWarp);
    smoothedFM.setCurrentAndTargetValue (fmAmount);
    smoothedPhaseDist.setCurrentAndTargetValue (phaseDistortion);

    // Recalculate phase increment
    phaseIncrement = currentFreq / sampleRate;
}

void WavetableOscillator::process (juce::AudioBuffer<float>& outputBuffer,
                                   const juce::AudioBuffer<float>* fmInput,
                                   int numSamples)
{
    if (numSamples <= 0)
        numSamples = outputBuffer.getNumSamples();

    int numChannels = outputBuffer.getNumChannels();

    // Get smoothed parameter values
    float wtPos = smoothedWavePos.getNextValue();
    float morphVal = smoothedMorph.getNextValue();
    float warpVal = smoothedWarp.getNextValue();
    float fmAmt = smoothedFM.getNextValue();
    float pdAmt = smoothedPhaseDist.getNextValue();

    const Wavetable* wt = getCurrentWavetable();
    if (!wt || !wt->isValid)
    {
        outputBuffer.clear();
        return;
    }

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* out = outputBuffer.getWritePointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float fm = 0.0f;
            if (fmInput && fmInput->getNumChannels() > ch)
                fm = fmInput->getReadPointer (ch)[i] * fmAmt;

            float sample = 0.0f;

            if (unisonVoices <= 1)
            {
                // Single voice
                float ph = (float)phase;
                ph = applyPhaseDistortion (ph, pdAmt);
                sample = renderWavetable (wt, ph, wtPos, morphVal, warpVal);
                phase += phaseIncrement;
                if (phase >= 1.0) phase -= 1.0;
            }
            else
            {
                // Unison - would need VoiceState for proper implementation
                // Simplified for now
                VoiceState vs;
                vs.phase = phase;
                vs.phaseIncrement = phaseIncrement;
                for (int u = 0; u < unisonVoices; ++u)
                {
                    vs.unisonPhases[u] = (float)(phase + u * unisonSpread * 0.01);
                    vs.unisonIncrements[u] = (float)(phaseIncrement * (1.0 + (u - unisonVoices * 0.5f) * unisonDetune * 0.01));
                }
                sample = renderUnison (wt, vs, wtPos, morphVal, warpVal, fm);
                phase += phaseIncrement;
                if (phase >= 1.0) phase -= 1.0;
            }

            out[i] = sample;
        }
    }
}

float WavetableOscillator::processSample (float fmInput)
{
    const Wavetable* wt = getCurrentWavetable();
    if (!wt || !wt->isValid)
        return 0.0f;

    // Get smoothed parameter values
    float wtPos = smoothedWavePos.getNextValue();
    float morphVal = smoothedMorph.getNextValue();
    float warpVal = smoothedWarp.getNextValue();
    float fmAmt = smoothedFM.getNextValue();
    float pdAmt = smoothedPhaseDist.getNextValue();

    float fm = fmInput * fmAmt;
    float ph = (float)phase;
    ph = applyPhaseDistortion (ph, pdAmt);

    float sample = 0.0f;

    if (unisonVoices <= 1)
    {
        sample = renderWavetable (wt, ph, wtPos, morphVal, warpVal);
    }
    else
    {
        VoiceState vs;
        vs.phase = phase;
        vs.phaseIncrement = phaseIncrement;
        for (int u = 0; u < unisonVoices; ++u)
        {
            vs.unisonPhases[u] = (float)(phase + u * unisonSpread * 0.01);
            vs.unisonIncrements[u] = (float)(phaseIncrement * (1.0 + (u - unisonVoices * 0.5f) * unisonDetune * 0.01));
        }
        sample = renderUnison (wt, vs, wtPos, morphVal, warpVal, fm);
    }

    phase += phaseIncrement;
    if (phase >= 1.0) phase -= 1.0;

    return sample;
}

void WavetableOscillator::setWavePosition (float position) noexcept
{
    wavePosition = juce::jlimit (0.0f, 1.0f, position);
    smoothedWavePos.setTargetValue (wavePosition);
}

void WavetableOscillator::setMorph (float morphVal) noexcept
{
    morph = juce::jlimit (0.0f, 1.0f, morphVal);
    smoothedMorph.setTargetValue (morph);
}

void WavetableOscillator::setSpectralWarp (float warp) noexcept
{
    spectralWarp = juce::jlimit (-1.0f, 1.0f, warp);
    smoothedWarp.setTargetValue (spectralWarp);
}

void WavetableOscillator::setFMAmount (float amount) noexcept
{
    fmAmount = juce::jlimit (0.0f, 1.0f, amount);
    smoothedFM.setTargetValue (fmAmount);
}

void WavetableOscillator::setFMSource (int source) noexcept
{
    fmSource = juce::jlimit (0, 3, source);
}

void WavetableOscillator::setPhaseDistortion (float amount) noexcept
{
    phaseDistortion = juce::jlimit (0.0f, 1.0f, amount);
    smoothedPhaseDist.setTargetValue (phaseDistortion);
}

void WavetableOscillator::setUnison (int voices, float detune, float spread) noexcept
{
    unisonVoices = juce::jlimit (1, 8, voices);
    unisonDetune = juce::jlimit (0.0f, 1.0f, detune);
    unisonSpread = juce::jlimit (0.0f, 1.0f, spread);
}

void WavetableOscillator::setWavetable (int index) noexcept
{
    if (wavetableBank && index >= 0 && index < wavetableBank->getNumWavetables())
    {
        currentWavetableIndex = index;
    }
}

void WavetableOscillator::setFrequency (float hz) noexcept
{
    currentFreq = juce::jlimit (0.1f, 20000.0f, hz);
    phaseIncrement = currentFreq / sampleRate;
}

void WavetableOscillator::setNote (int midiNote, float centsOffset) noexcept
{
    float freq = 440.0f * std::pow (2.0f, (midiNote - 69 + centsOffset / 100.0f) / 12.0f);
    setFrequency (freq);
}

const Wavetable* WavetableOscillator::getCurrentWavetable() const noexcept
{
    if (wavetableBank)
        return wavetableBank->getWavetable (currentWavetableIndex);
    return nullptr;
}

void WavetableOscillator::updateVoiceState (VoiceState& voice, float freq) noexcept
{
    voice.phaseIncrement = freq / sampleRate;
}

float WavetableOscillator::renderWavetable (const Wavetable* wt, float phase, float wtPos, float morph, float warp) const noexcept
{
    if (!wt || !wt->isValid)
        return 0.0f;

    // Convert 0-1 position to table index
    float tableIndex = wtPos * (Wavetable::NUM_TABLES - 1);

    // Apply spectral warp by shifting the effective table index
    // Positive warp = shift to brighter tables, negative = darker
    float warpShift = warp * 0.5f * (Wavetable::NUM_TABLES - 1);
    tableIndex += warpShift;

    // Clamp
    tableIndex = juce::jlimit (0.0f, (float)(Wavetable::NUM_TABLES - 1), tableIndex);

    // Get sample with morphing
    return wt->getSampleInterpolated (tableIndex, phase);
}

float WavetableOscillator::applyPhaseDistortion (float phase, float amount) const noexcept
{
    if (amount <= 0.0f)
        return phase;

    // Classic phase distortion: phase = phase + amount * sin(2π * phase)
    // This creates harmonic richness similar to Casio CZ / Yamaha DX
    float distorted = phase + amount * std::sin (2.0f * juce::MathConstants<float>::pi * phase);

    // Wrap
    distorted = distorted - std::floor (distorted);

    return distorted;
}

float WavetableOscillator::renderUnison (const Wavetable* wt, VoiceState& voice,
                                         float wtPos, float morph, float warp, float fmInput) const noexcept
{
    float sum = 0.0f;
    float gain = 1.0f / unisonVoices;

    for (int u = 0; u < unisonVoices; ++u)
    {
        float ph = voice.unisonPhases[u];
        ph = applyPhaseDistortion (ph, phaseDistortion);
        sum += renderWavetable (wt, ph, wtPos, morph, warp);

        // Advance unison phase
        voice.unisonPhases[u] += voice.unisonIncrements[u];
        if (voice.unisonPhases[u] >= 1.0f) voice.unisonPhases[u] -= 1.0f;
    }

    return sum * gain;
}

// ================================================================================
// WavetableUtils Implementation
// ================================================================================

std::vector<float> WavetableUtils::generateHarmonicSeries (const std::vector<float>& amplitudes,
                                                            const std::vector<float>& phases,
                                                            int tableSize)
{
    std::vector<float> table (tableSize, 0.0f);

    size_t numHarmonics = std::min (amplitudes.size(), phases.size());

    for (size_t h = 0; h < numHarmonics; ++h)
    {
        float amp = amplitudes[h];
        float phase = phases[h];
        if (amp == 0.0f) continue;

        int harmonicNum = (int)h + 1;
        for (int i = 0; i < tableSize; ++i)
        {
            float angle = 2.0f * juce::MathConstants<float>::pi * harmonicNum * i / tableSize + phase;
            table[i] += amp * std::sin (angle);
        }
    }

    normalizeWavetable (table);
    return table;
}

void WavetableUtils::applySpectralWarp (std::vector<float>& table, float warpAmount)
{
    // Simple spectral warp via phase modulation in frequency domain
    // This is a simplified version - real implementation would use FFT
    int size = (int)table.size();
    std::vector<float> output (size);

    // Warp by non-linear phase mapping
    for (int i = 0; i < size; ++i)
    {
        float phase = (float)i / size;
        // Apply warp function
        float warpedPhase = phase + warpAmount * std::sin (2.0f * juce::MathConstants<float>::pi * phase);
        warpedPhase = warpedPhase - std::floor (warpedPhase);

        int idx = (int)(warpedPhase * (size - 1));
        output[i] = table[idx];
    }

    table.swap (output);
    normalizeWavetable (table);
}

void WavetableUtils::morphWavetables (const std::vector<float>& a, const std::vector<float>& b,
                                       float amount, std::vector<float>& output)
{
    int size = (int)a.size();
    output.resize (size);

    for (int i = 0; i < size; ++i)
    {
        output[i] = a[i] + amount * (b[i] - a[i]);
    }

    normalizeWavetable (output);
}

void WavetableUtils::normalizeWavetable (std::vector<float>& table)
{
    float maxVal = 0.0f;
    for (float v : table)
        maxVal = std::max (maxVal, std::abs (v));

    if (maxVal > 0.0f && maxVal != 1.0f)
    {
        float scale = 1.0f / maxVal;
        for (float& v : table)
            v *= scale;
    }

    // Remove DC offset
    float dc = 0.0f;
    for (float v : table)
        dc += v;
    dc /= table.size();

    if (std::abs (dc) > 0.001f)
    {
        for (float& v : table)
            v -= dc;
    }
}

std::vector<float> WavetableUtils::generateSine (int tableSize)
{
    std::vector<float> table (tableSize);
    for (int i = 0; i < tableSize; ++i)
        table[i] = std::sin (2.0f * juce::MathConstants<float>::pi * i / tableSize);
    return table;
}

std::vector<float> WavetableUtils::generateTriangle (int tableSize)
{
    std::vector<float> table (tableSize);
    for (int i = 0; i < tableSize; ++i)
    {
        float phase = (float)i / tableSize;
        table[i] = 2.0f * std::abs (2.0f * (phase - std::floor (phase + 0.5f))) - 1.0f;
    }
    return table;
}

std::vector<float> WavetableUtils::generateSaw (int tableSize)
{
    std::vector<float> table (tableSize);
    for (int i = 0; i < tableSize; ++i)
    {
        float phase = (float)i / tableSize;
        table[i] = 2.0f * (phase - std::floor (phase + 0.5f));
    }
    return table;
}

std::vector<float> WavetableUtils::generateSquare (int tableSize, float pulseWidth)
{
    std::vector<float> table (tableSize);
    for (int i = 0; i < tableSize; ++i)
    {
        float phase = (float)i / tableSize;
        table[i] = (phase < pulseWidth) ? 1.0f : -1.0f;
    }
    return table;
}

std::vector<float> WavetableUtils::generateNoise (int tableSize)
{
    std::vector<float> table (tableSize);
    juce::Random rnd;
    for (int i = 0; i < tableSize; ++i)
        table[i] = rnd.nextFloat() * 2.0f - 1.0f;
    normalizeWavetable (table);
    return table;
}

void WavetableUtils::applyWindow (std::vector<float>& data, const juce::String& windowType)
{
    int size = (int)data.size();

    if (windowType == "hann" || windowType == "hanning")
    {
        for (int i = 0; i < size; ++i)
        {
            float window = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi * i / (size - 1)));
            data[i] *= window;
        }
    }
    else if (windowType == "hamming")
    {
        for (int i = 0; i < size; ++i)
        {
            float window = 0.54f - 0.46f * std::cos (2.0f * juce::MathConstants<float>::pi * i / (size - 1));
            data[i] *= window;
        }
    }
    else if (windowType == "blackman")
    {
        for (int i = 0; i < size; ++i)
        {
            float window = 0.42f - 0.5f * std::cos (2.0f * juce::MathConstants<float>::pi * i / (size - 1))
                           + 0.08f * std::cos (4.0f * juce::MathConstants<float>::pi * i / (size - 1));
            data[i] *= window;
        }
    }
}

void WavetableUtils::spectralMorph (const std::vector<float>& a, const std::vector<float>& b,
                                     float amount, std::vector<float>& output)
{
    // FFT-based spectral morphing - simplified version
    // Real implementation would use FFT, morph magnitude/phase separately, then iFFT
    int size = (int)a.size();
    output.resize (size);

    // For now, just do time-domain morph with normalization
    morphWavetables (a, b, amount, output);
}

} // namespace zalaxy