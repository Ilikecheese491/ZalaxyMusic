cmake_minimum_required(VERSION 3.22)
project(ZalaxyMusic VERSION 0.1.0 LANGUAGES CXX)

# C++20 required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Policy settings for modern CMake
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)

# Build configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# VST3 SDK path (can be overridden)
set(VST3_SDK_PATH "" CACHE PATH "Path to VST3 SDK")

# JUCE configuration
set(JUCE_DIR ${CMAKE_SOURCE_DIR}/JUCE CACHE PATH "Path to JUCE")
set(JUCE_BUILD_EXTRAS ON)
set(JUCE_ENABLE_VST3 ON)
set(JUCE_ENABLE_AU OFF)
set(JUCE_ENABLE_LADSPA OFF)
set(JUCE_ENABLE_STANDALONE_APP OFF)
set(JUCE_ENABLE_IOS OFF)
set(JUCE_ENABLE_ANDROID OFF)

# Plugin configuration
set(ZALAXY_PLUGIN_NAME "ZalaxyMusic")
set(ZALAXY_PLUGIN_MANUFACTURER "ZalaxyMusic")
set(ZALAXY_PLUGIN_VERSION ${PROJECT_VERSION})
set(ZALAXY_PLUGIN_CODE "Zalx")
set(ZALAXY_PLUGIN_CHANNEL_CONFIGS "{0, 2}")

# Include JUCE
add_subdirectory(${JUCE_DIR} EXCLUDE_FROM_ALL)

# Main plugin target
juce_add_plugin(${PROJECT_NAME}
    PRODUCT_NAME "${ZALAXY_PLUGIN_NAME}"
    COMPANY_NAME "${ZALAXY_PLUGIN_MANUFACTURER}"
    VERSION ${ZALAXY_PLUGIN_VERSION}
    PLUGIN_CODE "${ZALAXY_PLUGIN_CODE}"
    PLUGIN_CHANNEL_CONFIGS ${ZALAXY_PLUGIN_CHANNEL_CONFIGS}
    FORMATS VST3
    BUNDLE_SUFFIX ""
    VST3_DEFAULT_BUNDLE_VALUE "VST3"
    SOURCES
        Source/Plugins/Synth/PluginProcessor.cpp
        Source/Plugins/Synth/PluginEditor.cpp
    INCLUDE_DIRS
        Source/Core
        Source/AI
        Source/Analysis
        Source/UI
    RESOURCES
        Resources/Presets
        Resources/Images
        Resources/Fonts
)

# Core library
add_library(zalaxy_core STATIC
    Source/Core/Audio/AudioEngine.cpp
    Source/Core/DSP/Oscillator.cpp
    Source/Core/DSP/WavetableOscillator.cpp
    Source/Core/DSP/Filter.cpp
    Source/Core/DSP/Envelope.cpp
    Source/Core/DSP/LFO.cpp
    Source/Core/DSP/Effects.cpp
    Source/Core/Parameters/ParameterManager.cpp
    Source/Core/Parameters/ParameterSchema.cpp
    Source/Core/Presets/PresetManager.cpp
    Source/Core/Presets/PresetFormat.cpp
    Source/Core/Licensing/SubscriptionManager.cpp
    Source/Core/AI/CloudAIProvider.cpp
    Source/Core/AI/UsageTracker.cpp
)

target_include_directories(zalaxy_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
)

target_link_libraries(zalaxy_core PUBLIC
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

target_link_libraries(${PROJECT_NAME} PRIVATE zalaxy_core)

# AI module
add_library(zalaxy_ai STATIC
    Source/AI/Providers/AIProvider.cpp
    Source/AI/Providers/LocalAIProvider.cpp
    Source/AI/Requests/TextToPatchRequest.cpp
    Source/AI/Requests/AudioAnalysisRequest.cpp
    Source/AI/Responses/PatchResponse.cpp
    Source/AI/Responses/ExplanationResponse.cpp
    Source/AI/Validation/ParameterValidator.cpp
    Source/AI/Validation/SchemaValidator.cpp
)

target_include_directories(zalaxy_ai PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/AI
)

target_link_libraries(zalaxy_ai PRIVATE
    zalaxy_core
    juce::juce_core
    juce::juce_data_structures
)

target_link_libraries(${PROJECT_NAME} PRIVATE zalaxy_ai)

# Analysis module
add_library(zalaxy_analysis STATIC
    Source/Analysis/FFT/FFTAnalyzer.cpp
    Source/Analysis/Spectral/SpectralAnalyzer.cpp
    Source/Analysis/AudioReference/AudioReferenceProcessor.cpp
    Source/Analysis/AudioReference/ReferenceAnalyzer.cpp
)

target_include_directories(zalaxy_analysis PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Analysis
)

target_link_libraries(zalaxy_analysis PRIVATE
    zalaxy_core
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_dsp
)

target_link_libraries(${PROJECT_NAME} PRIVATE zalaxy_analysis)

# UI module
add_library(zalaxy_ui STATIC
    Source/UI/Components/ParameterSlider.cpp
    Source/UI/Components/ParameterKnob.cpp
    Source/UI/Components/WaveformDisplay.cpp
    Source/UI/Components/SpectrumDisplay.cpp
    Source/UI/Panels/OscillatorPanel.cpp
    Source/UI/Panels/FilterPanel.cpp
    Source/UI/Panels/EnvelopePanel.cpp
    Source/UI/Panels/ModulationPanel.cpp
    Source/UI/Panels/EffectsPanel.cpp
    Source/UI/Panels/AISoundDesignPanel.cpp
    Source/UI/Panels/AssistantPanel.cpp
    Source/UI/Panels/SettingsPanel.cpp
    Source/UI/Theme/ZalaxyTheme.cpp
    Source/UI/Components/VisualizationEngine.cpp
)

target_include_directories(zalaxy_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
)

target_link_libraries(zalaxy_ui PRIVATE
    zalaxy_core
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

target_link_libraries(${PROJECT_NAME} PRIVATE zalaxy_ui)

# Tests
enable_testing()
add_subdirectory(Tests)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)