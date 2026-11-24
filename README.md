# Chicago Beats Bloodline — FL Studio 11 Visualizer & Skin Project

A coordinated source project for the Chicago Beats Bloodline interface and audio-reactive visual system.

## Current deliverables in this repository

- `FL11_SKIN_BASE/CHICAGO_BEATS_BLOODLINE_FL11/` — the 105-file skin folder supplied in the user's FL Studio 11 archive, retained as the working baseline.
- `src/plugin/` — JUCE audio-effect plugin source. It passes audio through unchanged, analyzes low/mid/high spectral energy and low-frequency transients, accepts incoming MIDI note events, and paints the Bloodline visualizer interface.
- `assets/` — user's exact Batmobile reference, speaker-room reference, and Bloodline studio concept.
- `src/visualizer/` — browser companion prototype (where present) for rapid UI iteration.
- `.github/workflows/build-windows.yml` — Windows x64 CI build for VST3 and Standalone formats.
- `docs/` — architecture, build and FL Studio 11 compatibility notes.

## Important compatibility fact: FL Studio 11

FL Studio 11 predates Image-Line's VST3 support. The VST3 target produced here is for modern VST3 hosts; **it will not load in FL Studio 11**. Do not try to install the VST3 into FL Studio 11.

For FL Studio 11, use the Standalone companion and route FL Studio's output to it through a Windows loopback input or virtual audio cable. A native FL11 VST2 build is a separate legacy target and requires a properly licensed VST2 SDK; that SDK is not included or redistributed here. A normal FL11 skin cannot embed an animated canvas in the Playlist or toolbar.

## Build on GitHub

1. Create a private repository and upload the contents of this folder.
2. Push to the `main` branch.
3. Open **Actions** and run **Build Windows VST3 and Standalone** (or wait for the push workflow).
4. Download the `ChicagoBeatsBloodline-Windows-x64` artifact from the completed workflow run.

The first build downloads JUCE 8.0.6 from its official GitHub repository. This project uses JUCE under its GPLv3 terms; review `docs/LICENSING.md` before distributing binaries or source.

## Local build

Requires Windows 10/11, Visual Studio 2022 C++ workload, CMake 3.22+, and an internet connection for the JUCE source dependency.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --target ChicagoBeatsBloodline_VST3 ChicagoBeatsBloodline_Standalone
```

## Honest implementation status

The C++ plugin has a real audio pass-through path, FFT band analysis, transient envelope, MIDI note-event capture, a custom animated editor, a car image display with motion modes, a piano-note display, and a speaker-room background with live spectrum overlays. The visualizer motion is real UI animation; the exact car's individual wheel/rim geometry is not yet separately rigged, and smoke/donut physics are stylized UI effects rather than a 3D simulation. The speaker reference is used as the scene background, with live spectrum overlays; exact cone excursion deformation still requires layered speaker artwork.

The FL11 skin folder is preserved, not yet fully recolored or installed. Native Playlist embedding and FL11 VST2 support are not claimed as completed.
