# Build and release workflow

## GitHub Actions

The workflow `.github/workflows/build-windows.yml` runs on pushes to `main`, pull requests, tags, and manual dispatch. It configures a Visual Studio 2022 x64 build, downloads JUCE 8.0.6 through CMake FetchContent, builds the VST3 and Standalone targets, then uploads both outputs as a workflow artifact.

## Output formats

- `Chicago Beats Bloodline.vst3`: for VST3-compatible hosts, not FL Studio 11.
- `Chicago Beats Bloodline.exe`: standalone companion for FL Studio 11 audio loopback/virtual-cable use.

## Reproducible local build

```powershell
cmake --preset windows-release
cmake --build --preset windows-vst3-standalone
```

The first configuration requires Git and internet access to retrieve JUCE. Visual Studio 2022 must have the Desktop development with C++ workload installed.
