# FL Studio 11 compatibility and installation boundary

- FL Studio 11 supports legacy VST2 plugins, not VST3. The VST3 artifact is for newer hosts only.
- The standalone target is the practical route for live visual feedback alongside FL Studio 11. Route FL Studio's master output to a Windows loopback source or virtual audio cable, then choose that input in the companion's audio device settings.
- The FL11 skin folder is a legacy bitmap skin. Its own `config.txt` includes Image-Line's warning: `SKINNING IS NOT SUPPORTED ANYMORE`. Treat it as an experimental legacy skin and keep backups.
- A VST wrapper window cannot paint itself into the native Playlist canvas. Playlist embedding requires an Image-Line-supported host extension or another verified host integration mechanism.
- A native VST2 build is deliberately not provided until a lawful VST2 SDK path and the target FL11 architecture (32-bit or 64-bit) are confirmed.
