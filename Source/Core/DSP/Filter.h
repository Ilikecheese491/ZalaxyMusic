#pragma once

#include "Common/JuceModules.h"

namespace zalaxy
{

enum class FilterType
{
    LowPass,
    HighPass,
    BandPass,
    Notch,
    LowShelf,
    HighShelf,
    Peak
};

class Filter
{
public:
    Filter();
    ~Filter();

    void prepare (double sampleRate);
    void reset();

    void setType (FilterType type);
    void setCutoff (float frequencyHz);
    void setResonance (float q); // 0.1 to 20.0
    void setGain (float gainDb); // For shelf/peak

    float processSample (float input);
    void processBlock (float* buffer, int numSamples);

    // Get frequency response for visualization
    void getFrequencyResponse (float* magnitudes, float* phases, int numPoints, float minFreq, float maxFreq) const;

private:
    FilterType currentType = FilterType::LowPass;
    double sampleRate = 44100.0;

    float cutoff = 1000.0f;
    float resonance = 1.0f;
    float gainDb = 0.0f;

    // State variables for 2nd order sections
    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    float a0 = 1, a1 = 0, a2 = 0, b1 = 0, b2 = 0;

    void updateCoefficients();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Filter)
};

} // namespace zalaxy