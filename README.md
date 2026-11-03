# ZalaxyMusic

**AI-assisted virtual synthesizer and music production assistant**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()
[![Platform](https://img.shields.io/badge/platform-Windows%2011-lightgrey)]()
[![Format](https://img.shields.io/badge/plugin-VST3-orange)]()

---

## Overview

ZalaxyMusic is a free-first, AI-powered virtual synthesizer designed to make professional sound design accessible, intuitive, and creative. It translates your sound ideas into playable synthesizer patches without requiring deep knowledge of synthesis theory.

### Key Features

- 🎹 **Full Synthesizer Engine** — Polyphonic, multi-oscillator, wavetable-capable
- 🤖 **AI Text-to-Sound** — Describe a sound in natural language, get an editable patch
- 🎵 **Audio Reference Analysis** — Import WAV, select a region, recreate the timbre
- 💬 **Production Assistant** — Context-aware mixing, mastering, and sound-design guidance
- 🎨 **Cosmic Interface** — Dark, futuristic UI with real-time visualizations
- 🆓 **Free Core** — Essential synthesis works offline, no account required

---

## Quick Start

### Prerequisites

- **Windows 11** (x64)
- **Visual Studio 2022** with C++ workload
- **CMake 3.22+**
- **JUCE 9.0.4** (cloned as submodule or local path)
- **FL Studio** (for VST3 testing)

### Building

```bash
# Clone with JUCE submodule
git clone --recurse-submodules https://github.com/your-org/ZalaxyMusic.git
cd ZalaxyMusic

# Configure (Release)
cmake --preset windows-msvc-release

# Build
cmake --build build/windows-msvc-release --config Release

# The VST3 will be at:
# build/windows-msvc-release/bin/Release/ZalaxyMusic.vst3
```

### Installing in FL Studio

1. Copy `ZalaxyMusic.vst3` to your VST3 folder:
   - `C:\Program Files\Common Files\VST3\`
   - Or your custom VST3 scan path
2. Rescan plugins in FL Studio
3. Load ZalaxyMusic as an instrument

---

## Project Structure

```
ZalaxyMusic/
├── CMakeLists.txt           # Root CMake configuration
├── CMakePresets.json        # Build presets
├── JUCE/                    # JUCE framework (git submodule)
├── Source/
│   ├── Core/                # Audio engine, DSP, parameters, presets
│   ├── AI/                  # AI providers, requests, validation
│   ├── Analysis/            # FFT, spectral, audio reference
│   ├── UI/                  # Components, panels, theme, visualizations
│   └── Plugins/Synth/       # VST3 entry point
├── Tests/                   # Unit & integration tests
├── Resources/               # Presets, images, fonts
└── Docs/                    # Documentation
```

---

## Development Phases

| Phase | Focus | Status |
|-------|-------|--------|
| **1** | Working Synthesizer (JUCE, VST3, MIDI, oscillators, envelopes, filters, effects, presets) | 🟡 In Progress |
| **2** | AI Sound Design (local AI, text-to-patch, schema validation, explanations) | ⏳ Planned |
| **3** | WAV Reference Reconstruction (import, analysis, parameter fitting) | ⏳ Planned |
| **4** | Production Assistant (conversational UI, DAW-specific guidance) | ⏳ Planned |
| **5** | Expansion (wavetables, FM, macOS/AU, cloud services) | ⏳ Planned |

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      ZalaxyMusic Plugin                     │
├──────────────┬──────────────┬──────────────┬────────────────┤
│  Audio Core  │    AI Core   │  Analysis    │      UI        │
│  (DSP, MIDI, │  (providers, │  (FFT, spec, │  (cosmic theme,│
│   params,    │   requests,  │   reference) │   panels, viz) │
│   presets)   │   validation)│              │                │
└──────┬───────┴──────┬───────┴──────┬───────┴───────┬────────┘
       │              │              │               │
       └──────────────┴──────────────┴───────────────┘
                          │
              ┌───────────▼───────────┐
              │    JUCE / VST3        │
              │  (host abstraction)   │
              └───────────────────────┘
```

- **Audio thread**: Lock-free, no allocations, no network
- **AI/Analysis**: Background threads only
- **UI**: Message thread, async updates

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Contributing

See [CONTRIBUTING.md](Docs/CONTRIBUTING.md) (coming soon).

---

## Roadmap

- [ ] Phase 1: Core synthesizer working in FL Studio
- [ ] Phase 2: Local AI text-to-patch (Ollama-compatible)
- [ ] Phase 3: WAV reference → patch reconstruction
- [ ] Phase 4: Production assistant chat
- [ ] macOS / Audio Unit support
- [ ] Ableton Live / REAPER compatibility
- [ ] Advanced synthesis (wavetable, FM, granular)

---

**ZalaxyMusic — Explore the sound beyond the ordinary.** 🌌