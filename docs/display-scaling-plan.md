# Display, scaling and resolution: discussion record and plan

**Status:** all of "The plan" (section 4) is implemented, on branch `display-scaling` (from `dev`, after the SDL3
port), not yet merged or pushed. Canvas/black-frame cleanup, the battle monster/hero preview UI-layer move (4.1, plus
decision 7, pulled forward from "deferred" - see below) and the scale setting/window sizing/config versioning (4.2,
4.3, 4.6) are done and **play-tested by the author**, including two real bugs the author caught in play and had
fixed the same day. Full-screen art best-fit (4.5) and the mod guidelines (4.7, [mod-guide.md](mod-guide.md)) are
done but **not yet play-tested** - reaching the death/endgame screens needs game input. See "Implementation status"
(section 10) for the detail. Recorded 2026-09-21 from a design discussion between the game's author and Claude;
implementation notes added through 2026-09-22. Everything under "How it works today" describes the code as of
2026-09-21, before any of section 10's work - the current code is ahead of it everywhere section 10 covers.

## 1. Summary of what was decided

| Topic | Decision |
| --- | --- |
| Canvas size | 16 x TileSize by 10 x TileSize for every mod (base 640x400, 32 px 512x320, 48 px 768x480, 64 px 1024x640). No rule that anything be a multiple of 640x400. |
| The black frame ("blinds") | Goes away. Leaning toward showing the extra half tiles at the edges (final feel to be playtested). Showing more than that was rejected. |
| Resolution setting | **Done 2026-09-22.** Replaced by a **scale** setting. "Resolution" is meaningless because the display mode never changes. |
| Windowed mode | **Done 2026-09-22.** Window is exactly the canvas times N (whole number). No black border. |
| Fullscreen | Stays SDL's borderless "desktop fullscreen". Each layer takes the largest whole-number scale that fits. |
| Fractional scaling | Not used anywhere, for any pixel art or full-screen image. Nearest-neighbor stays. |
| UI layer | Stays independent of the world and canvas: fixed 640x400 design space, own whole-number scale, crisp text. No fractional UI scale, no separate player setting for UI scale. |
| Battle monster and new-game hero preview | **Done 2026-09-22** (correction to the original decision - see section 10): moved into the UI layer. This turned out to fix the alignment bug the SDL3 port's viewport+scale fix exposed, so it was done as part of this work rather than deferred. |
| Title, story and death art | Keep their own best-fit whole-number sizing. Mods may supply a title screen of their own size. |
| Mod guidelines | To be written for the typical tile sizes 32, 48 and 64. |
| SDL3 | The migration comes first, as its own scoping discussion. DPI awareness is re-evaluated after it. |
| Default scale | **Done 2026-09-22, as a starting point:** fixed at 2, clamped into `[minScale, maxScale]` (so it drops to `minScale` if that's already above 2, e.g. small-tile mods on a small display). Choosing from the display size instead is still open - see section 10. |

## 2. How it works today

### 2.1 Window and fullscreen
* `CDisplay::init` creates the window. Windowed uses only `SDL_WINDOW_SHOWN`, so the window is not resizable.
  Fullscreen uses `SDL_WINDOW_FULLSCREEN_DESKTOP`, and the Options menu toggles it at run time with
  `SDL_SetWindowFullscreen(..., SDL_WINDOW_FULLSCREEN_DESKTOP)` / `0`.
* That is a **borderless window covering the display at the desktop's current resolution**. The monitor's mode never
  changes, and the width and height passed to `SDL_CreateWindow` are ignored. There is no exclusive fullscreen
  (`SDL_WINDOW_FULLSCREEN` and `SDL_SetWindowDisplayMode` are never used).

### 2.2 The resolution setting
* The list of "resolutions" comes from display 0 only and contains only modes with the desktop's current pixel format
  **and refresh rate**, so an exact size can be missing on high-refresh monitors.
* Choosing one calls `SDL_SetWindowSize`, which does nothing in fullscreen, yet the size is still saved to
  `darkages.cfg`, so the setting can disagree with the real window size.
* The built-in default is 1280x1024. If the saved size is not offered, the display picks the largest mode that fits
  inside it (and the smallest only if none does).

### 2.3 Layout: two independent layers
`CDisplay::computeLayout` computes two separate whole-number scales, each centered in the window on its own:
* **World layer:** the canvas texture (`S(640)` x `S(400)`, created in `CDarkages`' setup) is blitted at
  `worldScale = floor(min(W / canvasW, H / canvasH))` into `worldRect`.
* **UI layer:** all procedural UI (text, bevel boxes, menus) is drawn straight to the backbuffer inside `uiRect` at
  `uiScale = floor(min(W / 640, H / 400))`, in a fixed 640x400 design space. `S(x)` in the UI pass multiplies by
  `uiScale`. Text is drawn at `32 * uiScale` px.
* `beginUIPass` / `endUIPass` switch drawing into the UI layer. A few screens (the battle monster sprite, the
  new-game hero preview - see 2.7) draw world-layer content straight onto the backbuffer instead of through the
  canvas texture; `compatRectToScreenRect` converts their coordinates explicitly rather than through SDL's own
  viewport/scale (see the **SDL3 viewport+scale bug** note in 2.7). Because the two rects are letterboxed
  independently, they are not always the same size for mods whose canvas is not 640x400.
* `CDisplay::clearScreen` fills a rect instead of calling `SDL_RenderClear` (a workaround from SDL 2.0.12 that the
  SDL3 port kept as is).

### 2.4 The black frame ("blinds")
* `CDarkages::render` draws a 16x10-tile grid, offset by half a tile, onto the canvas, with the hero sprite at
  `S(300), S(180)` (the center of the canvas). It then paints four black rectangles `S(20)` wide over the edges.
* `S(20)` is **half a tile** (20 px at 40 px tiles, 16 px at 32 px tiles; 32 on-screen px for Project32 at 2x).
* What remains visible is a **15x9-tile view** (600x360 px for 40 px tiles, 480x288 for 32 px): 7 tiles left and right
  of the hero, 4 above and below. Both counts are odd, so the hero's tile is centered.
* The frame does not hide partial tiles during scrolling; they still show at the edges of the 15x9 view. Its real
  effect is to make the at-rest view a whole number of tiles. Nothing is drawn "outside" the canvas in a way that
  matters: SDL clips to the canvas texture. The rectangles are an overdraw hack that also defines the view size.

### 2.5 Full-screen art (title, death, endgame, story)
* **Title** (`CTitle::render`): a fixed 640x400 image (documented as identical across mods) drawn through the UI layer
  at `uiScale`, with the menu text over it.
* **Death, endgame and explosion art:** an image that a whole-number multiple of fits the canvas (the base game's 320x200
  art on a 640x400 canvas) is drawn onto the canvas at 2x and then scaled by `worldScale`, so its scale is
  `2 x worldScale`. Any other size or shape goes through `renderFullScreenImage`, which picks the largest whole-number
  scale that fits the window. The base-game images therefore do not always get the largest fit: at 1920x1080 they get 4x
  (1280x800) although 5x (1600x1000) fits.
* Base-game art is 640x400 (title) and 320x200 (death, endgame, explosion). Project32's is 640x400 (title, death) and
  256x256 (endgame, explosion).

### 2.6 The font
* `Font/DA1qb.ttf` is a real, scalable **TrueType** font, rendered by SDL_ttf (`TTF_RenderText_Blended`, one texture per
  glyph per size, cached in `CFont`).
* Its outlines sit on a **16-unit pixel grid** (every coordinate is a whole sixteenth of the em; every glyph is 8/16 em
  wide), so it only renders crisply when the size is a multiple of 16 px. Measured with the game's own SDL_ttf:

| Font size (px) | Soft (partially transparent) edge pixels |
| --- | --- |
| 16, 32, 48, 64, 96 | 0% |
| 12 | 83% |
| 20 | 63% |
| 24 | 37% |
| 28 | 48% |
| 40 | 24% |
| 56 | 18% |

* The game uses only sizes 16 and 32, times the UI scale, so text is always crisp. Text size therefore cannot follow
  the tile size continuously, which is one reason the UI is kept independent of the canvas.

### 2.7 Where the two layers meet
* **Battle screen** (`CBattle::render`): the monster art is drawn in the world's coordinate space while its frame and
  the neighboring panels use UI positions. The code has an explicit workaround (the "unclipped UI" pass) for the
  case where the two rects differ.
* **New-game hero preview** (`CDarkages::renderNew`): the hero sprite is drawn in the world's coordinate space at
  `S(300), S(180)`.
* Both work correctly, but not for free: see the SDL3 viewport+scale bug below.
* **SDL3 viewport+scale bug (found and fixed 2026-09-21):** these two screens used to set the position of that
  world-space content by combining `SDL_SetRenderViewport` (offset to `worldRect`) with a non-1
  `SDL_SetRenderScale` (`worldScale`) and drawing in local coordinates, the way `beginUIPass` uses the viewport
  alone. An isolated offscreen test found that SDL3 3.4.16 multiplies the *viewport's own offset* by the render
  scale when both are set together - the same bug the migration plan's proof of concept had checked and believed
  fixed from SDL 2.32, just for the viewport rectangle itself rather than the coordinates drawn through it. The
  error grows with `worldRect`'s offset and `worldScale`, which is why it went unnoticed until it pushed the hero
  preview off-screen and visibly separated the battle monster from its frame (the frame was never affected, since
  it was already computed with the explicit `compatRectToScreenRect` conversion rather than SDL's own transform).
  Both screens now use that same explicit conversion and draw with the viewport left at its default and the scale
  left neutral; `beginCompatPass`/`endCompatPass` were removed since nothing else used them. See the SDL3 migration
  plan (section 10) for the reproduction.

### 2.8 DPI
* **Update:** the SDL3 port is done and **confirmed** that SDL3 makes the process DPI-aware by default (a 1280x1024
  window measures 1280x1024 physical pixels at 150% scaling). The paragraph below describes the SDL2 behavior the
  port removed; windows are now crisp but physically smaller on scaled displays, as the last bullet of this section says.
* Under SDL2 the game was **not DPI-aware**. At 125%, 150% or 175% Windows scaling, Windows stretches the whole window with
  smoothing, which blurs pixel art whatever the game does inside. At 100% and 200% the stretch is a whole-number
  factor and looks mostly fine. Screenshots do not show this because they capture before the stretch.
* On SDL2 this can be fixed today without SDL3: an application manifest, or calling `SetProcessDPIAware` (or, on newer
  SDL2, the `SDL_HINT_WINDOWS_DPI_AWARENESS` hint) before SDL starts.
* As currently understood (to be confirmed during SDL3 scoping), SDL3 makes high-DPI a first-class concept (pixel
  density and display-scale queries), treats fullscreen as borderless desktop by default with exclusive mode opt-in, and
  has a built-in whole-number-scale presentation mode.
* Once the process is DPI-aware, sizes are physical pixels: a 2x window is crisp but physically smaller on a 150%
  display than it is today, which is one reason the default scale should consider the display.

### 2.9 Configuration
`sConf` in `Structs.h` (width, height, volume, fullscreen, vsync, mod name) is written to `darkages.cfg` as raw bytes
with no version. The release packages ship a `darkages.cfg` produced by `Darkages --write-default-config`.

## 3. Decisions and the reasoning, including rejected alternatives

1. **Scale replaces resolution.** The code already only ever does whole-number scaling of a fixed-size canvas; a
   resolution list implies a display mode change that never happens, and it depends on filtered display modes.
2. **Canvas is 16T x 10T.** The player's view is deliberately limited in tiles (parts of the world, such as dungeon
   mazes, must stay hidden until the player is near). Tile sheets are cut into tiles and sprites, never drawn whole, so
   no size rule (such as multiples of 640x400) is needed.
3. **Showing many more tiles was rejected**, because it gives away parts of the map by design. Revealing the extra
   half tile on each side (7.5 tiles instead of 7 horizontally, 4.5 instead of 4 vertically, about 18% more area) was
   considered acceptable, and removes the black frame; to be confirmed by playtesting.
4. **A designed frame or HUD in the margin** was offered as an alternative to black; not preferred if the half tiles
   can simply be shown.
5. **The mod's own canvas is the game screen** ("approach A"), instead of a fixed 640x400 screen with margins for mods
   whose tile size is not 40.
6. **The UI stays independent** (fixed 640x400 design space, own whole-number scale). A proposal to lay the UI out on a
   character grid tied to the canvas was withdrawn as unnecessary: the current UI already works for any canvas as long
   as the window is at least 640x400. No fractional UI scale; no separate player setting for it.
7. **The battle monster and hero preview stay as they are** - originally. Moving them into the UI layer as pictures
   placed at whole-number scales was discussed as a cleaner design and deferred. **Superseded 2026-09-22:** after the
   SDL3 port's viewport+scale fix, the author found the battle screen's frames and panels misaligned differently at
   different resolutions - the world layer and the UI layer are independently letterboxed and only coincidentally
   line up. Moving the monster and hero preview into the UI layer, as this decision's "cleaner design" already
   proposed, is exactly the fix, so it was done as part of this work rather than deferred. See section 10.
8. **Mods may supply their own title-screen size.** Title, death and story art keep their own best-fit rule.
9. **SDL3 first.** The author considers the migration inevitable; it is its own discussion and comes before this work.
   The layout math could go on either side of it, but `CDisplay` is where much of the port lands, so doing the port
   first avoids rewriting the display code twice.

## 4. The plan

### 4.1 Canvas and tiles - **done 2026-09-22, see section 10**
* Canvas = `16 * tileSize` by `10 * tileSize` (`CDarkages` setup and `CDisplay::setCanvasSize`), replacing `S(640)` x
  `S(400)`. The hero is drawn at the center: `((canvasW - T) / 2, (canvasH - T) / 2)`, which equals today's
  `S(300), S(180)` for 40 px and 32 px tiles.
* Delete the four black rectangles. Make the tile loop cover the whole canvas (one or two more columns and rows than
  today; the current loop draws exactly enough for a 15x9 view). SDL clips to the canvas texture, so nothing outside it
  is visible.
* Anything else expressed in `S(640)` / `S(400)` for the canvas (the fade-in rectangle, the full-screen-image overlay
  rectangle) becomes the canvas size.

### 4.2 Scale N and window size - **done 2026-09-22, see section 10**
* Player setting: **scale N** (whole number). Windowed client size = `canvasW * N` by `canvasH * N`.
* **Minimum N:** the window must be at least 640x400 for the UI, so `minN = ceil(max(640 / canvasW, 400 / canvasH))`
  (2 for 32 px mods, 1 for 40, 48 and 64 px).
* **Maximum N:** the largest that fits the display's usable area (excluding the title bar and taskbar).
* **Default N:** fixed at 2 for now (clamped into `[minN, maxN]`), as a starting point rather than the final word -
  choosing from the display size is still open, see section 8.
* The scale setting replaces the resolution list in the Options menu; `SDL_SetWindowSize` and the saved size are
  driven by N.

### 4.3 Fullscreen - **done 2026-09-22 (unchanged in concept, re-verified against the new scale setting)**
Unchanged in concept: borderless desktop fullscreen; the world takes `floor(min(W / canvasW, H / canvasH))` and the UI
takes `floor(min(W / 640, H / 400))`, each centered. The N setting is ignored in fullscreen (SDL_SetWindowSize has no
effect while fullscreen, but SDL remembers it for when the player returns to windowed).

### 4.4 UI layer
Unchanged (section 2.3). At scale N the UI scale is `floor(min(W / 640, H / 400))` of the window.

### 4.5 Full-screen art - **done 2026-09-22, see section 10**
* Every full-screen image (title, death, endgame, explosion, story) is drawn at the largest whole-number scale that
  fits the window, nearest-neighbor. This includes the base game's 320x200 images, which today can get less than the
  best fit.
* Mods may supply a title image of any size: `CTitle` must stop assuming a fixed 640x400 image and the menu text must be
  placed relative to the UI layer, not to the image.
* Mod guideline: author full-screen art at the mod's canvas size (16T x 10T), or half of it, so it scales exactly.

### 4.6 Configuration - **done 2026-09-22, see section 10**
Width and height stop being meaningful, so the `darkages.cfg` layout changes. Add a version field (or migrate) so old
files are recognized, and update `Darkages --write-default-config`, the package scripts and the player READMEs. This is
also the moment to address the "raw struct, no version" item in `docs/future-work.md` (partially - see that file for
what's still open).

### 4.7 Mod guidelines to write (tile sizes 32, 48, 64) - **done 2026-09-22**
Canvas sizes, the minimum scale, what does and does not have to change with tile size (tile and sprite sheets do; the
UI does not), recommended full-screen art sizes, and the `mod.cfg` keys involved. Written up as
[docs/mod-guide.md](mod-guide.md).

## 5. Reference numbers

### 5.1 Canvas, window and UI scale (windowed, window = canvas x N)

| Tile size | Canvas | N | Window | UI scale |
| --- | --- | --- | --- | --- |
| 32 | 512x320 | 2 (minimum) | 1024x640 | 1 |
| 32 | 512x320 | 3 | 1536x960 | 2 |
| 32 | 512x320 | 4 | 2048x1280 | 3 |
| 40 (base) | 640x400 | 1 | 640x400 | 1 |
| 40 (base) | 640x400 | 2 | 1280x800 | 2 |
| 40 (base) | 640x400 | 3 | 1920x1200 | 3 |
| 48 | 768x480 | 1 | 768x480 | 1 |
| 48 | 768x480 | 2 | 1536x960 | 2 |
| 64 | 1024x640 | 1 | 1024x640 | 1 |
| 64 | 1024x640 | 2 | 2048x1280 | 3 |

### 5.2 Fullscreen fill by whole-number scaling (world layer)

| Display | 32 px | 40 px | 48 px | 64 px |
| --- | --- | --- | --- | --- |
| 1920x1080 | 3x (80% x 89%) | 2x (67% x 74%) | 2x (80% x 89%) | 1x (53% x 59%) |
| 2560x1440 | 4x (80% x 89%) | 3x (75% x 83%) | 3x (90% x 100%) | 2x (80% x 89%) |
| 3840x2160 | 6x (80% x 89%) | 5x (83% x 93%) | 4x (80% x 89%) | 3x (80% x 89%) |

Whole-number scaling is coarse on small displays and for large tiles (64 px on 1080p); that is inherent to the
no-fractional rule.

### 5.3 The half-tile reveal (40 px tiles)

| | Now | Without the frame |
| --- | --- | --- |
| View | 600x360 (15x9 tiles) | 640x400 |
| Horizontal reach from the hero | 7 tiles | 7.5 tiles |
| Vertical reach from the hero | 4 tiles | 4.5 tiles |
| View area | 216,000 px | 256,000 px (+18.5%) |

## 6. Where the changes will land

| Area | Files and functions |
| --- | --- |
| Window, fullscreen, mode list | `CDisplay::init`, `CDisplay::computeLayout`, `CDisplay::setCanvasSize` |
| Scale/resolution menu | `COptions` (the resolution and fullscreen items and their handlers) |
| Canvas size, tile loop, frame, hero position | `CDarkages` setup (`canvas` creation), `CDarkages::render` |
| Full-screen art | `CDarkages::currentFullScreenImage` (which image, if any); `CWindow::renderFullScreenImage` (the shared draw routine - see section 10) |
| Title | `CTitle::render` |
| Config | `sConf` in `Structs.h`, `Darkages.cpp` (read/write, `--write-default-config`), `tools/package.ps1`, `tools/package-linux.sh`, both player READMEs |
| Docs | this file, `docs/releasing.md`, [docs/mod-guide.md](mod-guide.md) (done) |

## 7. Sequence

1. SDL3 scoping discussion, then the migration (its own project). **Done.**
2. Decide DPI awareness in the light of SDL3. **Done** (SDL3 is DPI-aware by default; see 2.8).
3. Canvas cleanup: 16T x 10T, remove the frame, cover the canvas with tiles; playtest the half-tile reveal. **Done and
   play-tested 2026-09-22** (plus the battle monster/hero preview move, folded in - see section 10). A real bug in
   the canvas-coverage margin was found by the author playing and fixed the same day (commit `75c2c0a`; see section
   10). The half-tile reveal itself and the battle-screen alignment were both confirmed good.
4. Scale setting, window sizing, minimum and maximum N, default N, config versioning. **Done and play-tested
   2026-09-22** - confirmed working, including a live in-game exit writing `scaleN=3` back to `darkages.cfg` after
   the author changed it in Options (the one path this session's own testing couldn't exercise; see section 10).
5. Full-screen art fitting and per-mod title size. **Done 2026-09-22**, see section 10. Verified the title screen
   renders correctly (unchanged for the existing 640x400 art, as expected); death/endgame art shares the same
   routine but hasn't been seen in play yet.
6. Mod guidelines and docs. **Done 2026-09-22**: [docs/mod-guide.md](mod-guide.md).

## 8. Open questions

* The default scale: **for now, fixed at 2** (clamped into range) - still open whether to choose it from the display
  size instead (and how that would treat high-DPI displays).
* ~~Whether the half tiles feel right in play, especially in mazes.~~ **Confirmed good** by the author 2026-09-22.
* ~~How a mod declares its title-screen size (or whether the image size alone is enough).~~ **Resolved by the
  implementation:** the image's own size is enough - `CWindow::renderFullScreenImage` reads the texture's actual
  dimensions directly, so no declaration is needed.
* The exact maximum-scale rule: **implemented** as the largest N fitting `SDL_GetDisplayUsableBounds()` on the
  primary display; multi-monitor (which display to use if the window isn't on the primary one) is still open.
* DPI awareness: settled by the SDL3 port (on by default); what remains is how the default scale accounts for it.
* ~~The config migration approach.~~ **Done and play-tested** - see section 10 (4.2/4.3/4.6 writeup).
* ~~The SDL3 scope (its own discussion).~~ **Done**, see [sdl3-migration-plan.md](sdl3-migration-plan.md) - this
  bullet predates that work starting and was never removed once it was.

## 9. How to verify the work when it is done

* Windowed client size equals canvas x N for the base game and for 32, 48 and 64 px test mods (the local `Test2` mod is
  48 px; Project32 is 32 px).
* The world is one uniform whole-number scale (no uneven pixel runs) at each N, and in fullscreen on several display
  sizes and Windows scale factors.
* Title, death, endgame and story art are at the best whole-number fit at every window size.
* No black frame; tiles cover the canvas at every scroll offset (test at offsets 0, 1 and tileSize - 1 in both axes).
* UI text is crisp (multiples of 16 px) and the UI never overflows the window (test the minimum N for 32 px mods).
* Old `darkages.cfg` files load correctly after the layout change.

## 10. Implementation status (2026-09-22)

On branch `display-scaling`, from `dev`. Debug and Release both build clean. The canvas/black-frame cleanup and the
battle monster/hero preview UI-layer move (both below) have been **play-tested by the author and confirmed working**,
including catching a real bug (see 4.1 below). The scale setting/window sizing/config versioning work (section "4.2,
4.3, 4.6", further down) has **not** been play-tested yet.

**4.1 Canvas and tiles - done:**

* `CDarkages`' constructor creates the canvas at `16 * tileSize` x `10 * tileSize` directly, using
  `display->modSettings.tileSize` (not `this->modSettings`, which `CDarkages::init()` hasn't set yet at that point).
  This is numerically identical to the old `S(640)` x `S(400)` for every tile size (`S(640) = round(640 *
  TileSize/40) = 16 * TileSize` exactly, since 640/40 = 16 with no remainder), so `worldRect`/`worldScale` are
  unaffected - only what gets *drawn* on the canvas changes.
* **The tile loop needed a wider margin than the plan's "one or two more columns and rows" estimate, not just the
  black rectangles deleted.** Working through the exact pixel math: the old loop (7 tiles left of the camera tile, 8
  right; 4 above, 5 below - 16x10 total, matching the canvas exactly) only covers the *canvas* fully at one specific
  scroll offset. At other offsets (the sub-tile scroll position, always somewhere in `[0, tileSize)`) it falls up to
  half a tile short of one edge - which the black frame's half-tile-wide rectangles happened to hide exactly. Solving
  for the margin that covers all offsets on both axes gives 8 tiles left / 10 right and 5 above / 7 below (18x12
  total): the low (left/top) side's worst case is offset 0 and needs `ceil(half the canvas, in tiles)` = 8 / 5
  tiles; the high (right/bottom) side's worst case is offset `tileSize - 1`, nearly a whole tile further, and needs
  one more than that - 10 / 7. **A first version of this (committed, then caught by the author playing the actual
  game) used 6 for the bottom margin instead of 7** - an arithmetic slip in the derivation (a Y-axis figure copied
  from the X-axis one without re-deriving it for the canvas's different height), one tile short rather than the
  "one or two more" the rest of the margins got right. It showed as a blank row opening up at the bottom of the
  canvas while scrolling down, closing the instant the movement completed (max offset resets to 0 exactly when the
  camera tile advances) - i.e. exactly the shape of an off-by-one in a scroll-margin calculation. Fixed to 7.
  The loop's position formula was also decoupled from its own bounds (it now positions each tile from
  `(i - a)`/`(j - b)` - the camera tile - and the canvas's own half-size, rather than from the loop's start
  `lowX`/`lowY`), so tile positions no longer shift if the bounds are widened again later.
* The four black "blinds" rectangles are deleted. The fade-in rectangle and the full-screen-image overlay rectangle
  (both still drawn onto the canvas render target) now use `display->canvasW`/`canvasH` instead of `S(640)`/`S(400)`.
* The hero's canvas position is now `(canvasW - tileSize) / 2, (canvasH - tileSize) / 2` computed directly, rather
  than `S(300), S(180)` - still numerically identical for every tile size, but no longer relies on `S()`'s
  mod-native scale factor, matching how the tile loop above now reads.
* **Confirmed by the author in play (2026-09-22):** the half-tile reveal (section 3 decision 3, section 8) feels
  right, including in mazes - the reason the plan called out playtesting it specifically.

**Decision 7 (battle monster + hero preview) - done, folded in ahead of schedule:**

* Both now draw entirely within `beginUIPass()`/`endUIPass()`, using `display->S()` for every position *and* size,
  exactly like the title box, stats panel and every other UI element on their screens - no more separate
  world-space/UI-space split for these two screens.
* `monsterSize` and `tileSize` (both mod settings, from `mod.cfg`) are now treated as UI-reference-space pixel
  counts scaled by `uiScale`, the same way the fixed 640x400 title image already is, rather than mod-native pixels
  scaled by `worldScale`.
* **This needed one real fix, not just a mechanical move:** the hero preview's old position, `S(300), S(180)`, was
  only correct because the *old* `S()` (mod-native scale) happened to center it within the mod's own variable-size
  world canvas. The *new* `S()` (uiScale, during a UI pass) has no notion of the mod's canvas size at all, so reusing
  those constants would have centered the hero correctly only for 40 px tiles and put it visibly off-center for every
  other tile size. The position is now computed as `(640 - tileSize) / 2, (400 - tileSize) / 2` - centered in the
  fixed 640x400 UI reference space instead, consistent with everything else in that layer. The battle monster frame's
  position (`10, 52`) needed no equivalent fix, since it was always a fixed UI-panel anchor, not a canvas-centered
  value.
* `CDisplay::compatRectToScreenRect()`, `beginUnclippedUI()` and `endUnclippedUI()` are now unused (their only
  callers were these two screens) and have been removed, along with `beginCompatPass()`/`endCompatPass()`'s
  now-stale doc comments elsewhere.

**4.2, 4.3, 4.6 (scale N, window sizing, config versioning) - done and play-tested:**

* `CDisplay::init()` now computes `canvasW`/`canvasH` itself (`16 * tileSize`, `10 * tileSize`) right after loading
  the mod, before creating the window - it needs them to size the window - and calls `setCanvasSize()` once the real
  window size is known, so `CDarkages`' constructor no longer computes the canvas size or calls `setCanvasSize()`
  itself; it just reads `display->canvasW`/`canvasH` back to create the canvas texture at the same size.
* `minScale`/`maxScale` (new `CDisplay` members) are computed once in `init()`: `minScale = ceil(max(640/canvasW,
  400/canvasH))` (2 for 32 px mods, 1 for 40/48/64 px, matching the plan); `maxScale` is the largest N whose window
  fits `SDL_GetDisplayUsableBounds()` on the primary display, never below `minScale`. `conf.scaleN` is clamped into
  this range on load, so an old or out-of-range value (e.g. a config carried over to a smaller display, or a
  different mod with a different canvas) can never produce a window bigger than the display or smaller than the UI
  needs.
* Windowed client size is `canvasW * conf.scaleN` by `canvasH * conf.scaleN`, verified directly (Project32, 32 px
  tiles, scale 2: 1024x640 window, confirmed via `GetClientRect` - the first measurement attempt read 682x426
  because the querying script itself wasn't DPI-aware, a measurement artifact rather than a game bug; a DPI-aware
  query on the same running window read the correct 1024x640). Fullscreen is unaffected in concept (4.3): SDL
  ignores the requested size while fullscreen, and `SDL_SetWindowSize` while fullscreen (from changing the scale
  setting without leaving fullscreen) has no visible effect but is remembered for when the player returns to
  windowed, same as the old resolution list already relied on.
* Options' "Screen Res:" menu item is now "Scale:", cycling `conf.scaleN` between `minScale` and `maxScale` instead
  of an index into a resolution list. It first showed the resulting window size alongside the number (e.g.
  `2 (1024x640)`); the author found that unnecessary after trying it, so it's just the number now.
* **Config versioning:** `sConf` gains `cfgVersion` (`DA_CFG_VERSION = -1`, `Structs.h`) as its first field, and `w`/
  `h` are replaced by `scaleN`. A pre-redesign `darkages.cfg`'s first field was always a positive window width, so
  `Darkages.cpp`'s read peeks that one field to tell old files from new ones apart, with no ambiguity possible (never
  negative in the old format, always negative in the new one). An old file's volume, fullscreen, vsync and mod
  selection carry over; its saved resolution is dropped (meaningless now) in favor of the default `scaleN`.
  Verified against the actual `darkages.cfg` already in `game/` (a real pre-redesign file, `w=1920, h=1080`): the
  game read it, migrated it, and opened at the correct `1024x640` (default scale 2, Project32's canvas). Verified
  the new format round-trips too: `--write-default-config` produces a file starting with the correct `cfgVersion`
  marker and `scaleN=2`, and the game reads that back correctly as well.
* **Play-tested and confirmed by the author.** The live in-game write-back path (on normal exit through the game's
  own menu) - the one path this session's own testing couldn't exercise, since reaching it needs game input - is
  confirmed working too: after the author changed the scale to 3 in Options and played normally, `game/darkages.cfg`
  was found holding `cfgVersion=-1, scaleN=3`, and the window opened at the correspondingly correct `1536x960` on
  the next start.
* `future-work.md`'s "raw struct, no version" item is updated to note `sConf` now has a version marker (though it's
  still a raw struct dump depending on compiler padding, and `da1save` - save games - has none of this yet).

**4.5, 4.7 (full-screen art best-fit, mod guidelines) - done, not yet play-tested:**

* **Unified every full-screen image (title, death, endgame, explosion, story) onto one routine**, rather than the
  title using the UI layer's own fit and death/endgame either drawing onto the canvas (when they happened to be a
  whole-number multiple of it) or going through a separate best-fit function otherwise. The shared routine
  (`CWindow::renderFullScreenImage`, new - the old `CDarkages::renderFullScreenImage` and `fitsCanvasInWholeScale`
  are gone) draws the image at its own aspect ratio, centered on the window, at the largest whole-number scale that
  fits, nearest-neighbor - matching the plan's example fix (a canvas-fit image no longer gets a worse effective
  scale than a direct window fit would give it).
* **No behavior change for the game's current art**, verified: the base game's and Project32's title images are
  both exactly 640x400, the same aspect as the UI layer's own reference space, so the old "stretch to fill the UI
  layer" and the new "best-fit directly to the window" formulas are mathematically identical for them
  (`uiScale = floor(min(W/640, H/400))` is exactly the fit formula for a 640x400 image) - confirmed by a window
  screenshot (`PrintWindow`, captured directly from the window's own content rather than the screen, so there was
  no risk of capturing anything else on screen) showing the title screen rendering identically to before. Death and
  endgame art share the same routine but weren't seen rendered in this testing pass, since reaching them needs game
  input.
* `CTitle::render()` now positions the menu text and version/copyright line in the UI layer's own fixed 640x400
  space, independent of the title image's actual size - the mechanical part of letting mods supply a title image of
  any size, which was the other half of 4.5.
* **Mod guidelines written**: [docs/mod-guide.md](mod-guide.md) - folder layout and file-resolution rules, a full
  `mod.cfg` key reference (types, defaults, parsing quirks) read directly from `CMods::loadModSettings()`, what
  does and doesn't scale with `TileSize` (cross-checked against `CGfxCollection::loadGfx()`), the minimum-scale
  table, and full-screen art sizing guidance with the exact filename-to-screen mapping (read from
  `CGfxCollection.cpp`). Also closes the "Document mod.cfg" item in `future-work.md`.

All of "The plan" (section 4) is now implemented. What's left is play-testing 4.5 (death/endgame screens) and,
eventually, the still-open items in section 8 below, merging `display-scaling` into `dev`, and pushing.
