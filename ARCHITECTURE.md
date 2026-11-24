# Architecture

## Plugin formats

- **VST3**: intended for hosts with VST3 support. FL Studio 11 does not support this format.
- **Standalone**: intended as the FL Studio 11 companion. Select an audio input that receives the DAW output (Windows loopback or virtual audio cable).
- **VST2 / FL11 native plugin**: intentionally not built in this repository because the legacy VST2 SDK is not included and must be obtained under its applicable license. FL Studio 11 is a VST2-era host.

## Signal flow

Audio input -> pass-through output + mono analysis tap -> 2048-point FFT -> low (25–120 Hz), mid (120 Hz–4 kHz), high (4–15 kHz) envelopes -> editor animation.

A low-band positive-change detector provides a kick-like transient envelope. It is a heuristic, not semantic drum recognition. MIDI note-on events received by the plugin update the piano display; actual Playlist clip note access requires a separate FL11 MIDI/event bridge.

## UI areas

1. Batmobile runway: user's car reference displayed as a moving sprite; modes are back/forth, burnout-style spin, donut-style rotation and hold. 28-inch rim artwork must be refined against the source image before calling the rim geometry final.
2. Piano: falling note visualization and keyboard; note label updates from incoming MIDI note-on events.
3. Speaker array: speaker-room reference as the scene plate; low/mid/high bars update from live audio. Layered cone deformation, port particles and physically distinct speaker animation are next-stage artwork/renderer tasks.

## Audio safety

The plugin does not alter audio samples. It analyzes the input and copies the signal through. Always test at low monitoring volume first. No automatic gain, limiting, or audio effects are applied.
