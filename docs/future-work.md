# Future work

A running list of updates we will need or might want. Add to it freely; delete items as they are done.

## Platform and libraries

* **Display, scaling and resolution redesign.** A "scale" setting instead of "resolution", a 16 x TileSize by
  10 x TileSize canvas without the black frame, windowed mode exactly the size of the game, and best-fit art. It is
  fully worked out in [display-scaling-plan.md](display-scaling-plan.md) and waits on the SDL3 migration.

* **Migrate to SDL3** (with SDL3_ttf and SDL3_mixer). It is a real port, scoped and planned in
  [sdl3-migration-plan.md](sdl3-migration-plan.md): statically linked on both platforms, a like-for-like port first,
  and it comes before the display redesign.
* **The SDL libraries are old.** SDL2 is 2.0.12 (2020), SDL2_ttf is 2.0.12 (2015), and the FreeType and zlib DLLs
  that come with it date from 2015. SDL2 2.0.12 also has a bug that already bit us: `SDL_GetBasePath()` returns
  the wrong folder when the exe's path is long. `src/Darkages.cpp` works around it on Windows with
  `GetModuleFileNameW`; a newer SDL would let us drop that.
* **Windows could use the newer SDL2 too.** The Linux build already uses SDL2 2.32.10, SDL2_ttf 2.24.0 and
  SDL2_mixer 2.8.1. The game was run on those exact versions (built statically on Windows) and its title screen
  renders correctly on SDL's Direct3D 11 and software renderers. Two things to know before switching Windows over:
  SDL 2.32 multiplies the rectangle given to `SDL_RenderSetViewport` by the current render scale (2.0.12 did not),
  which the game is unaffected by, since every viewport call (all in `CDisplay.cpp`) is made while the scale is 1; and on the
  machine used for that test, even a minimal SDL 2.32 program showed nothing on the Direct3D 9 and OpenGL backends
  (2.32 picks Direct3D 9 first on Windows), so re-check on other hardware, and consider the hint
  `SDL_HINT_RENDER_DRIVER=direct3d11` if needed.
* **Test the Linux build on real systems.** It has been cross-compiled and packaged, but running it needs a Linux
  machine or WSL2: check X11 and Wayland, PulseAudio/PipeWire/ALSA, and the default OpenGL renderer. An AppImage or
  Flatpak could follow, as could an ARM64 build.
* **macOS build**, if there is interest.
* **Windows on ARM**: the project is x64 only.

## Distribution

* **Code-sign `Darkages.exe`.** Downloads are unsigned, so Windows SmartScreen warns on first run (the zip's README
  explains how to continue). Signing needs a certificate.
* **Windows version resource and icon.** The exe reports version 0.0.0.0 and has no icon. A resource file could
  carry the real version (the build already knows it) and an icon.
* **An installer or a portable-mode switch.** The game keeps its settings and saves next to the exe, so it must be
  in a writable folder. Keeping them under `SDL_GetPrefPath()` when installed would remove that limit.

## Code

* **Save and config files are raw C structs** written with `fwrite` (`sConf`, `da1save`). They depend on the
  compiler's padding and are not versioned. A versioned, explicit format would survive changes and compilers
  (and matters for a Linux port).
* **Compiler warnings.** Three long-standing ones: `size_t` to `int` conversions in `COptions.cpp` (lines 15 and
  80) and `time_t` to `unsigned int` in the `srand` call in `Darkages.cpp`.
* **`CDarkages.cpp` is about 170 KB** and holds most of the game logic and its hard-coded data (dialogue, items,
  shops, stairs). Splitting it, and moving data into files, would make mods more powerful and the code easier to
  navigate.
* **Line endings are mixed** (some files CRLF, some LF) and there is no `.gitattributes` or `.editorconfig`.
  Adding them means renormalizing files, which should be done in one dedicated commit.
* **Document `mod.cfg`.** Its keys (`TileSize`, `MonsterSize`, `HeroWalkFrames`, `HeroIdleAnimations`, `SolidTiles`,
  `HighResBorders`, `BevelDetail`) are only described in code comments. The existing test mods' hero sheets also
  still need rearranging to the current walk-frame layout.

## Automation

* **A CI smoke test.** Launch the freshly built exe with SDL's dummy video and audio drivers and check that it
  starts and stays up, and/or check that every map and asset loads.
* **Build the Debug configuration in CI** too, and run a code-analysis pass.
* **Keep the Actions current.** Consider Dependabot for the action versions, or pinning them to commit hashes.
* **Mirror the SDL zips** in this repository if libsdl.org turns out to be unreliable (see `docs/releasing.md`).
* **A release-notes helper** that groups commits by type, and a script that bumps `DA_VERSION_BASE`.
