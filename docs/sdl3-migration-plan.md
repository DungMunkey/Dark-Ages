# SDL3 migration: scoping record and plan

**Status:** ported on the `SDL3` branch (2026-09-21); it builds on Windows (Debug and Release), packages, and starts,
and a clean-clone rehearsal of the CI build passed. During the author's first play-through, an SDL3 viewport+scale
bug was found and fixed (the new-character and battle screens) - see section 10. The rest of a human play-through,
the controller, and the Linux build are still untested: see section 10. Recorded 2026-09-21 from a design discussion
between the game's author and Claude.
The migration comes **before** the display and scaling redesign (see [display-scaling-plan.md](display-scaling-plan.md)).
Facts marked "verified" were checked against the SDL3 headers or source, or by running a proof of concept; items marked
"to confirm" are understood from the API but must be checked during the port.

## 1. Decisions

| # | Question | Decision |
| --- | --- | --- |
| 1 | Approach | One straight port on its own branch, no compatibility layer. SDL2 stays on `master` until the branch is merged. |
| 2 | Linking | Build SDL3, SDL3_ttf and SDL3_mixer **from source and link them statically on both platforms**, unless the libraries turn out to be best used as DLLs (they do not; see section 3). |
| 3 | Scope | A like-for-like port first: behavior unchanged. The DPI and scale redesign comes afterward. |
| 4 | Audio parity | Looping music, play-once for the credits, the song duration for the credits timing, a 0-10 volume, and an "is it playing" check. Nothing else is needed. |
| 5 | Gamepad | Supported and tested. The author has an Xbox One Elite Controller connected to the development machine and will test beyond what Claude can do. |
| 6 | Windows build system | **Keep Visual Studio**: `msvc/Darkages.vcxproj` stays and links prebuilt static libraries, so F5 keeps working. Linux keeps using `CMakeLists.txt`. |
| 7 | Rectangles | **Convert mechanically** to `SDL_FRect` in the drawing calls. No wrapper layer in `CDisplay`. |
| 8 | Branch | The branch is called **`SDL3`**. SDL2 is **not** kept alive in parallel: there is no SDL2 maintenance branch and no code that supports both. `master` and `dev` keep their SDL2 code only until the port is merged, after which SDL2 is gone. *(Assumed: the branch starts from `dev` and merges back into `dev`; to be confirmed when it is created.)* |

## 2. Versions to pin

| Library | Version | Source archive SHA-256 |
| --- | --- | --- |
| SDL3 | 3.4.16 (2026-09-02) | `7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68` |
| SDL3_ttf | 3.2.2 (2025-03-31) | `63547d58d0185c833213885b635a2c0548201cc8f301e6587c0be1a67e1e045d` |
| SDL3_mixer | 3.2.4 (2026-06-03) | `182a07c745375e113dc740d43964ff21b0be29f29f59876c4dbc4db3d32f6901` |
| FreeType (SDL's fork) | `libsdl-org/freetype`, branch `VER-2-13-2-SDL` | commit `9973564cfa63763a3e4ac67c09147899539b1e07` |

The copy of SDL3 on the development machine is 3.4.14, two patch releases behind. SDL3 3.4.x releases arrive roughly
monthly, so the pin should be bumped deliberately, not automatically.

## 3. Proof of concept (verified 2026-09-21)

A small program was built with SDL3, SDL3_ttf and SDL3_mixer compiled from source and linked statically (CMake, Visual
Studio 2022, Windows). It ran against the game's real font and music files and the connected controller. It played no
sound and read no button presses.

* **It builds and links:** one 3.3 MB exe that imports no SDL DLLs (only Windows system libraries and the C++
  runtime, which `/MT` removes).
* **Font:** text rendering through SDL3_ttf is identical to SDL2's. The percentage of soft (partially transparent) edge
  pixels was the same at every size measured: 0% at 16, 32, 48 and 64 px, and 83%, 63%, 37%, 48% and 24% at 12, 20, 24,
  28 and 40 px. The crispness rules in the display plan carry over unchanged.
* **Audio:** `MIX_GetAudioDuration` gives the same lengths the game gets from `Mix_MusicDuration`: base `title.ogg`
  79.54 s, Project32's `title.ogg` 83.48 s and `world.ogg` 109.64 s (`battle.ogg` 67.63 s). The credits timing
  therefore carries over.
* **Gamepad:** the Xbox One Elite Controller is detected, with the same mapping. SDL 2.0.12 called it "XInput
  Controller"; SDL3 calls it "Xbox One Controller".
* **Configuration that works:** SDL3 static only, no test library, no install; SDL3_ttf with its vendored FreeType and
  with HarfBuzz and PlutoSVG off; SDL3_mixer with Ogg Vorbis through its built-in stb_vorbis and everything else off
  (WAVE stays), so no external codec library is needed.
* **The one catch: SDL3_ttf and SDL3_mixer no longer bundle third-party sources in their release archives.** SDL2's
  did. They ship an `external/download.sh` that runs `git clone` on named branches, which is not reproducible enough
  for a pinned build. The plan is for `get-deps` to fetch the FreeType fork at the pinned commit above (SDL3_ttf needs
  only FreeType here) and place it in `external/freetype`, instead of running that script. The exact archive URL and its
  SHA-256 are to be settled when `get-deps` is written.

Static linking is therefore recommended: nothing about these libraries needs to be a DLL, the packages get simpler (a
single executable on Windows), and versions are identical on both platforms.

## 4. What changes, by area

The game uses about 130 distinct SDL, SDL_ttf and SDL_mixer names. Five files include the SDL headers (`CDisplay.h`,
`CFont.h`, `CGraphic.h`, `CInput.h`, `CMusic.h`); the drawing calls are spread over the title, menus, dialogs, shops,
stats, spell, battle, load/save and options screens. Almost every SDL2 name the game uses is gone in SDL3 (kept only as
`SDL_oldnames.h` aliases). All of the following were checked against the SDL3 headers unless marked "to confirm".

### 4.1 Rendering (about 130 call sites)
* `SDL_RenderCopy` (29 uses) becomes `SDL_RenderTexture`, and `SDL_RenderCopyEx` becomes `SDL_RenderTextureRotated`.
* `SDL_RenderFillRect` (25) and `SDL_RenderDrawRect` (3, now `SDL_RenderRect`) take `SDL_FRect` (float rectangles).
  `SDL_Rect` (39 uses) stays for viewports and clip rectangles. Keep drawing coordinates whole numbers: fractional
  positions would blur pixel art.
* `SDL_RenderSetViewport` (9) becomes `SDL_SetRenderViewport`; `SDL_RenderGetViewport` becomes `SDL_GetRenderViewport`;
  `SDL_RenderSetScale` (3) becomes `SDL_SetRenderScale`; `SDL_RenderSetLogicalSize` (2) becomes
  `SDL_SetRenderLogicalPresentation`. **Correction (see section 10):** this was originally recorded as verified
  fixed from the SDL 2.32 viewport-times-scale bug; an isolated test after the port found SDL3 3.4.16 still
  multiplies the viewport's own offset by the render scale when both are set together (just not the rectangle a
  read-back of `SDL_GetRenderViewport` would show), which did affect two `CDisplay` call sites. Both were rewritten
  to avoid the combination rather than rely on it.
* `SDL_QueryTexture` (3) becomes `SDL_GetTextureSize`; `SDL_FreeSurface` (5) becomes `SDL_DestroySurface`;
  `SDL_MapRGB` now takes a pixel-format-details pointer and a palette (or use `SDL_MapSurfaceRGB`).
* **Return values flip:** most SDL3 functions return `bool` (true = success) where SDL2 returned 0 for success. Every
  `< 0` and `!= 0` check on an SDL call must be reviewed (for example the `SDL_Init` check in `CDisplay::init`).
* **Nearest-neighbor filtering:** the `SDL_HINT_RENDER_SCALE_QUALITY` hint is gone, and **verified:** SDL3's default
  texture scale mode is `SDL_SCALEMODE_LINEAR`, which would blur the pixel art. Call
  `SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_NEAREST)` right after creating the renderer so that every
  new texture, including the canvas and the glyph textures, is nearest-neighbor; then check that nothing sets a
  different mode.
* **Renderer:** `SDL_CreateRenderer(window, NULL)`; vsync through `SDL_SetRenderVSync` (the property is
  `SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER`). **Verified:** on Windows SDL3 tries Direct3D 11 first, then
  Direct3D 12, with Direct3D 9 last. The Direct3D 9 and OpenGL blank-window problem seen with SDL 2.32 on the
  development machine used Direct3D 9, which SDL3 no longer defaults to.

### 4.2 Window, display and fullscreen (`CDisplay::init`, `COptions`)
* `SDL_CreateWindow(title, w, h, flags)`: no position arguments (position with `SDL_SetWindowPosition` or the create
  properties; the `SDL_WINDOWPOS_*` macros still exist); `SDL_WINDOW_SHOWN` is gone (windows are shown unless
  `SDL_WINDOW_HIDDEN`).
* `SDL_SetWindowFullscreen(window, bool)`; `SDL_WINDOW_FULLSCREEN_DESKTOP` is gone. Fullscreen is borderless desktop by
  default, with exclusive mode opt-in through `SDL_SetWindowFullscreenMode`. This matches what the game does today.
* The display-mode API is rewritten: `SDL_GetNumDisplayModes` is gone; use `SDL_GetFullscreenDisplayModes` and
  `SDL_GetDesktopDisplayMode`, with `SDL_DisplayID` instead of indexes. The like-for-like port keeps the resolution
  list working in the simplest way, since the scaling redesign removes it. `SDL_GetDisplayUsableBounds` exists and will
  be needed for the maximum-scale rule.
* DPI: `SDL_WINDOW_HIGH_PIXEL_DENSITY`, `SDL_GetWindowPixelDensity`, `SDL_GetWindowDisplayScale` and
  `SDL_GetWindowSizeInPixels` exist. SDL3 has no Windows DPI-awareness hint (consistent with it being built in). The
  behavior of the game window on scaled displays after the port is to be confirmed and is decided in the display plan.

### 4.3 Text (`CFont`)
`TTF_OpenFont(path, float size)`; `TTF_RenderText_Blended(font, text, length, color)` gains a length argument (0 for a
NUL-terminated string, to confirm); `TTF_Init` returns a bool. The proof of concept shows identical rendering.

### 4.4 Audio (`CMusic`, about 100 lines)
SDL3_mixer has no `Mix_` API. It uses a mixer, audio objects and tracks. The mapping (all names verified in the header):

| Today (SDL2_mixer) | SDL3_mixer |
| --- | --- |
| `Mix_Init`, `Mix_OpenAudio` | `MIX_Init`, `MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)` |
| `Mix_LoadMUS` | `MIX_LoadAudio(mixer, path, false)` (false = stream, not predecode) |
| `Mix_PlayMusic(m, -1)` (loop) | `MIX_SetTrackAudio(track, audio)` then `MIX_PlayTrack(track, props)` with `MIX_PROP_PLAY_LOOPS_NUMBER` = -1 |
| `Mix_PlayMusic(m, 1)` (once, the credits) | the same with loops 0 |
| `Mix_PlayingMusic` | `MIX_TrackPlaying(track)` |
| `Mix_VolumeMusic` (0-128) | `MIX_SetTrackGain(track, 0.0-1.0)` or `MIX_SetMasterGain`; the game's 0-10 setting maps to it |
| `Mix_MusicDuration` (seconds) | `MIX_GetAudioDuration` (sample frames) then `MIX_AudioFramesToMS` (unknown is -1) |
| `Mix_FreeMusic`, `Mix_CloseAudio` | `MIX_DestroyAudio`, `MIX_DestroyTrack`, `MIX_DestroyMixer`, `MIX_Quit` |

The existing behavior must be preserved: `playSong` restarts a song looping unless it is already playing looping, and
`playSongOnce` restarts from the beginning and plays once (the credits, timed to the scroll). SDL3_mixer is the newest
and least battle-tested of the three libraries; the credits timing and song switching deserve a careful test.

### 4.5 Input (`CInput`, the event loops in `CDarkages`)
* `SDL_KEYDOWN`/`SDL_KEYUP` become `SDL_EVENT_KEY_DOWN`/`SDL_EVENT_KEY_UP`; `e.key.keysym.sym` becomes `e.key.key`;
  `SDL_TEXTINPUT` becomes `SDL_EVENT_TEXT_INPUT`; `SDL_StartTextInput`/`SDL_StopTextInput` take the window. Mouse
  events carry float coordinates.
* Game controllers become gamepads: `SDL_INIT_GAMECONTROLLER` becomes `SDL_INIT_GAMEPAD`; `SDL_GameController*` becomes
  `SDL_Gamepad*`; the controller events and `SDL_CONTROLLER_BUTTON_*` become `SDL_EVENT_GAMEPAD_BUTTON_*` and
  `SDL_GAMEPAD_BUTTON_*` (A/B/X/Y become the positional `SOUTH`/`EAST`/`WEST`/`NORTH`; A is `SOUTH` on an Xbox
  controller). **Gamepads are no longer opened automatically:** open them from `SDL_GetGamepads` at start-up and on
  `SDL_EVENT_GAMEPAD_ADDED`, and close them on removal, so plugging in a controller mid-game works.

### 4.6 Program start and files
* `main` needs `#include <SDL3/SDL_main.h>` in the file that defines it (there is no `SDL2main` library).
* `SDL_GetBasePath` returns a `const char*` that must **not** be freed. The Linux branch of `changeToExeFolder` in
  `Darkages.cpp` currently calls `SDL_free` on it and must change. (The Windows branch already uses
  `GetModuleFileNameW`, added because SDL 2.0.12's `SDL_GetBasePath` failed for long paths.)

## 5. Build, CI and packaging

* **Windows:** `get-deps.ps1` builds the three libraries as static libraries with CMake, once, for Release (`/MT`) and
  Debug (`/MTd`), into `third_party/`, and `msvc/Darkages.vcxproj` links them plus the Windows system libraries SDL
  needs (`winmm`, `imm32`, `version`, `setupapi`, `ole32`, `oleaut32`, `gdi32`, `user32`, `advapi32`, `shell32`). The
  Visual Studio workflow (F5) is kept. The exe becomes a single file; the DLL copy step and the five DLLs disappear from
  the package. Building SDL adds a few minutes to a cold CI run, avoided by the existing cache keyed on the `get-deps`
  script.
* **Linux:** `CMakeLists.txt` already builds the libraries from source; its option names change for SDL3
  (`SDL_TEST_LIBRARY`, `SDLTTF_*`, `SDLMIXER_*`), and `get-deps.sh` gains the pinned FreeType fork.
* **Both:** `get-deps` scripts, the version pins and hashes above, `THIRD-PARTY.txt` generation (SDL3 and SDL3_ttf under
  the zlib license; FreeType under the FreeType License, the fork noted; stb_vorbis inside SDL3_mixer), the package
  scripts (no DLL list on Windows), the workflow cache keys, `README.md` and `docs/releasing.md`.
* SDL3 supports all desktop Windows versions back to XP (per its own documentation), so no platform floor changes.

## 6. Risks and gotchas

* The boolean return values (section 4.1) are the likeliest source of subtle bugs: an unchanged `if (SDL_Init(...) < 0)`
  silently means the opposite.
* Float rectangles: keep all draw coordinates whole numbers so scaling stays crisp.
* Nearest-neighbor filtering must be set as the renderer's default (section 4.1), or the pixel art blurs.
* Gamepad hot-plug and opening (section 4.5).
* SDL3_mixer's new model and maturity (section 4.4).
* Rendering differences between backends: the port should be checked on Direct3D 11 (the default) and on Linux's
  OpenGL/Vulkan backends; Linux is still untested on real hardware.
* No automated tests exist, so the port depends on a manual checklist (section 7).

## 7. How to verify the port

Claude can build both platforms, run the game, capture the title screen and the other screens reachable without input,
and compare them with the SDL2 build at the same window size. The author tests the rest, including the controller.

Manual checklist: title screen and menu; Options (window/fullscreen, vsync, volume, mod change and restart); new game;
walking and scrolling smoothness at several window sizes; dialogue, shops, stats, spells, save and load; battle;
death screen; the endgame scenes and credits (music timing and scroll); the title music restarting after the credits;
gamepad navigation everywhere the keyboard works, plus plugging a controller in and out mid-game; text entry (character
name); Project32 and a 48 px test mod; the shipped `darkages.cfg` and the package scripts; the Linux build on WSL2 and,
if possible, a real machine.

## 8. Sequence

1. Create the `SDL3` branch; update `get-deps` and the build files so all three libraries build statically on Windows
   and Linux (compile-first, no code changes yet).
2. Core port: `CDisplay`, `CGraphic`, `CFont`, `CMusic`, `CInput`, `main`, then the drawing calls screen by screen.
   Milestone: the game compiles and the title screen renders identically.
3. Behavior parity: run the checklist; fix differences.
4. Gamepad support and hot-plug.
5. CI, packaging and documentation; tag it as a dev build for early adopters.
6. Merge, then start the display and scaling redesign.

## 9. Open questions

Answered (see section 1): the Windows build system (keep Visual Studio), float rectangles (convert mechanically), and
the branch (`SDL3`, no parallel SDL2). Resolved during the port: the FreeType source is fetched by commit id (section
10). Still open:

* **Default renderer and vsync** on the development machine (Direct3D 11 is expected; the game does not print which
  renderer SDL chose).
* **The branch's base** (built from `dev`, to merge back into `dev`; not explicitly confirmed).

## 10. Port status and findings

**Done and checked by building or running** (Windows, 2026-09-21):

* All game sources ported: rendering to `SDL_FRect` mechanically, `bool` return checks, nearest-neighbor default
  texture scale mode, window and fullscreen (borderless desktop) and display-mode list, `SDL_GetBasePath` (not freed),
  `SDL_main.h`, uppercase letter key codes (`SDLK_Q`), text input on the window, SDL3_mixer rewrite of `CMusic`
  (`playSong` loops, `playSongOnce`, `getSongDuration`, 0-10 volume), and gamepad open/close/hot-plug through
  `DA_PollEvent` (in `CInput.cpp`), which every event loop uses. The build has only the three warnings it had before.
* Debug and Release both build; `get-deps.ps1` builds the three static libraries per configuration; the exe is 3.9 MB
  and imports only Windows system DLLs. `package.ps1` makes a zip with no DLLs, and `Darkages.exe` from a clean
  clone of the branch (built by the same steps CI runs) starts, shows the title screen with the right version string,
  and the Project32 title screen and font look identical to SDL2.
* Linux: all 18 sources compile for x86_64 Linux against the SDL3 headers (with a cross-compiler), the root CMake
  configures in bundled mode, and `get-deps.sh` runs. **Not** linked or run.

**Findings worth remembering:**

* **SDL3 3.4.16 multiplies the render viewport's offset by the render scale when both are set together, found and
  fixed 2026-09-21.** Reported by the author as the new-character hero sprite drawn off-screen and the battle
  monster sprite drawn away from its own frame. `CDisplay::beginCompatPass()`/`endCompatPass()` combined a
  non-default `SDL_SetRenderViewport` (offset to `worldRect`) with a non-1 `SDL_SetRenderScale` (`worldScale`) so
  that draw calls could use local, un-scaled coordinates - exactly the pattern the SDL 2.32 viewport bug affected,
  which this plan's proof of concept had checked and recorded as fixed in SDL3 (see the correction on this in
  section 4.1). It is fixed for the *viewport rectangle a caller reads back*, but an isolated offscreen test (draw
  a rect at local (10,10) with viewport `(50,50,200,200)` and scale 2; expected screen position (70,70)) actually
  landed at (120,120) - matching `(viewport.offset + local) * scale` rather than `viewport.offset + local * scale`.
  This affected only the two call sites that used that combination (nothing else in the game does: the world map
  renders to an off-screen canvas texture and blits it with an explicit rect, and the UI pass uses the viewport
  alone with scale left at 1, which is why nothing else was affected). Fix: both call sites now compute the
  destination rect explicitly with `CDisplay::compatRectToScreenRect()` (the same conversion the battle screen's
  frame border already used, which is why the frame itself was never misplaced) and draw with the viewport at its
  default and the scale left neutral; `beginCompatPass()`/`endCompatPass()` were removed as unused. See
  `docs/display-scaling-plan.md` section 2.7 for the player-facing description.
* **The process is DPI-aware by default in SDL3** (confirmed by measurement): a 1280x1024 window is 1280x1024 physical
  pixels at 150% Windows scaling, where SDL2 let Windows stretch it to 1920x1536 with smoothing. Windows are therefore
  physically smaller on scaled displays than they were, which the display redesign will address.
* Default resolution behavior is unchanged: 1280x1024, or the largest listed mode that fits inside the saved size.
* SDL3_ttf and SDL3_mixer do not bundle FreeType, so `get-deps` fetches the fork at the pinned commit
  (`git init`, `git fetch --depth 1 <sha>`), which is what makes it reproducible. SDL3_mixer needs no external codec
  library (Ogg Vorbis through built-in stb_vorbis).
* The library and target names are `SDL3-static`, `SDL3_ttf-static`, `SDL3_mixer-static` and `freetype`; a single
  aggregate CMake target (`da_sdl3_libs`) builds them together.
* The Windows static build needs the static C++ runtime matching the game (`MultiThreaded` / `MultiThreadedDebug`),
  and its path budget is tight (MSBuild's 260-character limit), hence the 90-character repository-path guard in
  `get-deps.ps1`.
* Bulk-editing source files with `sed -i` under Git Bash strips carriage returns, and several source files are
  committed with CRLF line endings; check `git ls-files --eol` (it should not show `i/crlf w/lf`) after any bulk edit.

**Still needs a human (the author):**

* Play through: title and menu, Options (fullscreen, vsync, scale, volume), new game and text entry, walking,
  dialogue, shops, stats, spells, save/load, battle, death, endgame scenes and credits (music timing and the title
  music restarting afterward).
* The Xbox One Elite Controller everywhere the keyboard works, including unplugging and re-plugging mid-game.
* A 48 px test mod.
* Linux: link and run under WSL2, or through the first GitHub Actions run (which also proves the Windows CI job).
