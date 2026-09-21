# Display, scaling and resolution: discussion record and plan

**Status:** discussion only. Nothing described in "The plan" has been built. It is deliberately parked until after the
SDL3 migration has been scoped and done (see "Sequence"). Recorded 2026-09-21 from a design discussion between the
game's author and Claude. Everything under "How it works today" was checked against the code on that date; function
names are given instead of line numbers so the references survive edits.

## 1. Summary of what was decided

| Topic | Decision |
| --- | --- |
| Canvas size | 16 x TileSize by 10 x TileSize for every mod (base 640x400, 32 px 512x320, 48 px 768x480, 64 px 1024x640). No rule that anything be a multiple of 640x400. |
| The black frame ("blinds") | Goes away. Leaning toward showing the extra half tiles at the edges (final feel to be playtested). Showing more than that was rejected. |
| Resolution setting | Replaced by a **scale** setting. "Resolution" is meaningless because the display mode never changes. |
| Windowed mode | Window is exactly the canvas times N (whole number). No black border. |
| Fullscreen | Stays SDL's borderless "desktop fullscreen". Each layer takes the largest whole-number scale that fits. |
| Fractional scaling | Not used anywhere, for any pixel art or full-screen image. Nearest-neighbor stays. |
| UI layer | Stays independent of the world and canvas: fixed 640x400 design space, own whole-number scale, crisp text. No fractional UI scale, no separate player setting for UI scale. |
| Battle monster and new-game hero preview | Stay as they are. May be revisited later. |
| Title, story and death art | Keep their own best-fit whole-number sizing. Mods may supply a title screen of their own size. |
| Mod guidelines | To be written for the typical tile sizes 32, 48 and 64. |
| SDL3 | The migration comes first, as its own scoping discussion. DPI awareness is re-evaluated after it. |
| Default scale | Undecided; to be based on display size. |

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
* `beginUIPass` / `endUIPass` and `beginCompatPass` / `endCompatPass` switch between them. Because the two rects are
  letterboxed independently, they are not always the same size for mods whose canvas is not 640x400.
* `CDisplay::clearScreen` fills a rect instead of calling `SDL_RenderClear` (a workaround from SDL 2.0.12 that the
  SDL3 port kept as is). SDL3 does not multiply viewport rectangles by the render scale.

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
* **Battle screen** (`CBattle::render`): the monster art is drawn in the world's coordinate space (compat pass) while
  its frame and the neighboring panels use UI positions. The code has an explicit workaround (the "unclipped UI" pass
  and `compatRectToScreenRect`) for the case where the two rects differ.
* **New-game hero preview** (`CDarkages::renderNew`): the hero sprite is drawn in the compat pass at `S(300), S(180)`.
* Both work acceptably today. See the decision in section 3.

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
7. **The battle monster and hero preview stay as they are.** Moving them into the UI layer as pictures placed at
   whole-number scales was discussed as a cleaner design and deferred; the author may revisit.
8. **Mods may supply their own title-screen size.** Title, death and story art keep their own best-fit rule.
9. **SDL3 first.** The author considers the migration inevitable; it is its own discussion and comes before this work.
   The layout math could go on either side of it, but `CDisplay` is where much of the port lands, so doing the port
   first avoids rewriting the display code twice.

## 4. The plan

### 4.1 Canvas and tiles
* Canvas = `16 * tileSize` by `10 * tileSize` (`CDarkages` setup and `CDisplay::setCanvasSize`), replacing `S(640)` x
  `S(400)`. The hero is drawn at the center: `((canvasW - T) / 2, (canvasH - T) / 2)`, which equals today's
  `S(300), S(180)` for 40 px and 32 px tiles.
* Delete the four black rectangles. Make the tile loop cover the whole canvas (one or two more columns and rows than
  today; the current loop draws exactly enough for a 15x9 view). SDL clips to the canvas texture, so nothing outside it
  is visible.
* Anything else expressed in `S(640)` / `S(400)` for the canvas (the fade-in rectangle, the full-screen-image overlay
  rectangle) becomes the canvas size.

### 4.2 Scale N and window size
* Player setting: **scale N** (whole number). Windowed client size = `canvasW * N` by `canvasH * N`.
* **Minimum N:** the window must be at least 640x400 for the UI, so `minN = ceil(max(640 / canvasW, 400 / canvasH))`
  (2 for 32 px mods, 1 for 40, 48 and 64 px).
* **Maximum N:** the largest that fits the display's usable area (excluding the title bar and taskbar).
* **Default N:** undecided. The author wants 2 as the general default but will consider choosing from the display size
  (options: the largest N whose window fits within about 90% of the usable area, never below the minimum).
* The scale setting replaces the resolution list in the Options menu; `SDL_SetWindowSize` and the saved size are
  driven by N.

### 4.3 Fullscreen
Unchanged in concept: borderless desktop fullscreen; the world takes `floor(min(W / canvasW, H / canvasH))` and the UI
takes `floor(min(W / 640, H / 400))`, each centered. The N setting is ignored in fullscreen.

### 4.4 UI layer
Unchanged (section 2.3). At scale N the UI scale is `floor(min(W / 640, H / 400))` of the window.

### 4.5 Full-screen art
* Every full-screen image (title, death, endgame, explosion, story) is drawn at the largest whole-number scale that
  fits the window, nearest-neighbor. This includes the base game's 320x200 images, which today can get less than the
  best fit.
* Mods may supply a title image of any size: `CTitle` must stop assuming a fixed 640x400 image and the menu text must be
  placed relative to the UI layer, not to the image.
* Mod guideline: author full-screen art at the mod's canvas size (16T x 10T), or half of it, so it scales exactly.

### 4.6 Configuration
Width and height stop being meaningful, so the `darkages.cfg` layout changes. Add a version field (or migrate) so old
files are recognized, and update `Darkages --write-default-config`, the package scripts and the player READMEs. This is
also the moment to address the "raw struct, no version" item in `docs/future-work.md`.

### 4.7 Mod guidelines to write (tile sizes 32, 48, 64)
Canvas sizes, the minimum scale, what does and does not have to change with tile size (tile and sprite sheets do; the
UI does not), recommended full-screen art sizes, and the `mod.cfg` keys involved.

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
| Full-screen art | `CDarkages::currentFullScreenImage`, `fitsCanvasInWholeScale`, `renderFullScreenImage` |
| Title | `CTitle::render` |
| Config | `sConf` in `Structs.h`, `Darkages.cpp` (read/write, `--write-default-config`), `tools/package.ps1`, `tools/package-linux.sh`, both player READMEs |
| Docs | this file, `docs/releasing.md`, the new mod guidelines |

## 7. Sequence

1. SDL3 scoping discussion, then the migration (its own project).
2. Decide DPI awareness in the light of SDL3.
3. Canvas cleanup: 16T x 10T, remove the frame, cover the canvas with tiles; playtest the half-tile reveal.
4. Scale setting, window sizing, minimum and maximum N, default N, config versioning.
5. Full-screen art fitting and per-mod title size.
6. Mod guidelines and docs.

## 8. Open questions

* The default scale: fixed 2, or chosen from the display size (and how it treats high-DPI displays).
* Whether the half tiles feel right in play, especially in mazes.
* How a mod declares its title-screen size (or whether the image size alone is enough).
* The exact maximum-scale rule (usable area, multi-monitor: which display).
* DPI awareness: settled by the SDL3 port (on by default); what remains is how the default scale accounts for it.
* The config migration approach.
* The SDL3 scope (its own discussion).

## 9. How to verify the work when it is done

* Windowed client size equals canvas x N for the base game and for 32, 48 and 64 px test mods (the local `Test2` mod is
  48 px; Project32 is 32 px).
* The world is one uniform whole-number scale (no uneven pixel runs) at each N, and in fullscreen on several display
  sizes and Windows scale factors.
* Title, death, endgame and story art are at the best whole-number fit at every window size.
* No black frame; tiles cover the canvas at every scroll offset (test at offsets 0, 1 and tileSize - 1 in both axes).
* UI text is crisp (multiples of 16 px) and the UI never overflows the window (test the minimum N for 32 px mods).
* Old `darkages.cfg` files load correctly after the layout change.
