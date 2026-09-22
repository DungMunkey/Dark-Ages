# Future work

A running list of updates we will need or might want. Add to it freely; delete items as they are done.

## Platform and libraries

* **Display, scaling and resolution redesign - in progress.** A "scale" setting instead of "resolution", a
  16 x TileSize by 10 x TileSize canvas without the black frame, windowed mode exactly the size of the game, and
  best-fit art. Canvas/black-frame cleanup and the scale setting/window sizing/config versioning are done, on branch
  `display-scaling`; full-screen art best-fit and mod guidelines are not started. See
  [display-scaling-plan.md](display-scaling-plan.md) (section 10) for status.

* **SDL3 port: finish testing and merge.** The port (SDL3, SDL3_ttf and SDL3_mixer, statically linked on both
  platforms, like-for-like) is done on the `SDL3` branch; see [sdl3-migration-plan.md](sdl3-migration-plan.md) for
  what changed and what still needs a human play-through. It comes before the display redesign.
* **Test the Linux build on real systems.** It has been cross-compiled and packaged, but running it needs a Linux
  machine or WSL2: check X11 and Wayland, PulseAudio/PipeWire/ALSA, and the default renderer. An AppImage or
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
  compiler's padding and (except `sConf`, now - see below) are not versioned. A versioned, explicit format would
  survive changes and compilers (and matters for a Linux port).
  * `sConf` (`darkages.cfg`) got a version marker as part of the display-scaling redesign (`DA_CFG_VERSION` in
    `Structs.h`), so a config from before that redesign is recognized and migrated (`Darkages.cpp`'s read logic) -
    but it is still a raw struct dump underneath, still depends on the compiler's padding being identical to the
    build that wrote it, and only handles this one past layout change, not compiler/platform portability in general.
  * `da1save` (save games) has none of this yet.
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
* **A release-notes helper** that groups commits by type, and a script that bumps `DA_VERSION_BASE`.
