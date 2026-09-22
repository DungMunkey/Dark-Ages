# Modding guide

A mod replaces the base game's graphics, maps and music. This is the reference for `mod.cfg` and for sizing a mod's
art now that the display-scaling redesign (`display-scaling-plan.md`) is done: the canvas size follows the mod's own
tile size, there's no fixed 640x400 assumption for full-screen art, and the game imposes a minimum window size.

## 1. Folder layout and file resolution

A mod is a folder under `Mods/`, e.g. `Mods/Project32/`, containing any of `Gfx/`, `Maps/`, `Music/` and a `mod.cfg`.
Players pick one from the Options menu (or "None" for the base game); it takes effect on the next start. `Mods/` and
the mod's own subfolders are optional - a mod only needs to provide what it's changing.

Every asset the game loads by a fixed path (see section 5) is resolved through the same rule: if the selected mod's
folder has that file, it's used; otherwise the base game's copy under `game/` is used instead. This means a mod can
replace just a few files (say, only the tile sheet and the maps) and let everything else - including full-screen art
- fall back to the base game's.

The one exception is `Gfx/DA1HeroIdle.bmp` (idle animations): it is **only** ever loaded from inside the mod's own
folder, never falling back to the base game (which has no idle animations at all), and only if the mod declared
`HeroIdleAnimations` in `mod.cfg` in the first place.

## 2. `mod.cfg` reference

A plain-text file, one `Key=Value` per line. Keys are case-insensitive; leading/trailing whitespace around the key
and value is trimmed; a line starting with `#` (after leading whitespace) or a blank line is ignored; a UTF-8 BOM at
the start of the file is tolerated. Unknown keys are silently ignored (so a typo doesn't crash the game, but also
won't be reported - check spelling carefully). Any key you omit keeps its default.

| Key | Type | Default | Notes |
| --- | --- | --- | --- |
| `TileSize` | whole number (px) | 40 | The mod's native tile/sprite pixel size. Drives the canvas size (section 3) and the minimum window scale (section 4). Ignored if 0 or negative. |
| `MonsterSize` | whole number (px) | 100 | Native pixel size of `DA1MonstL.bmp`'s monster portraits. Independent of `TileSize`. Ignored if 0 or negative. |
| `HeroWalkFrames` | whole number | 2 | Walk-cycle frames per direction in `DA1HeroL.bmp`. Clamped to 32 (the sheet is 16 columns wide, 4 columns per direction block, so 32 is as many 8-row blocks as the layout supports). Ignored if 0 or negative. |
| `HeroIdleAnimations` | comma-separated whole numbers | none (feature off) | Frame count per idle animation, e.g. `12,16,8` for three animations. Enables loading `Gfx/DA1HeroIdle.bmp` (see the fallback exception in section 1); one row per animation, frames left-to-right within the row, `TileSize x TileSize` per frame. |
| `SolidTiles` | comma-separated whole numbers | none | Extra map-tile values to treat as impassable, on top of the game's built-in table (see `CDarkages::checkTile()`). Same numbering as the raw values stored in `.map` files. |
| `HighResBorders` | `true`/`yes`/any non-zero number | `false` | Dialog and menu border strokes are 1 px per band instead of 2 - a thinner bevel overall. |
| `BevelDetail` | whole number | 1 | Multiplies the border's band count for a finer gradient. `1` is the base game's look; higher values add more, thinner bands within the same border thickness. Ignored if 0 or negative. |

Example (Project32's actual `mod.cfg`):

```
TileSize=32
MonsterSize=100
HeroWalkFrames=4
HeroIdleAnimations=16,16
SolidTiles=130,139,140,143,144,145,146,147,148,149,150,151,152,153,154,155
```

A mod that only needs a different tile size can be as short as:

```
TileSize=48
```

## 3. Canvas and tile art

The world canvas is `16 x TileSize` by `10 x TileSize` pixels (base 640x400, 32 px 512x320, 48 px 768x480, 64 px
1024x640) - this is what's actually on screen at once, before the player's chosen window scale magnifies it. There's
no requirement that it be a multiple of 640x400 or of anything else: tile and sprite sheets are always cut into
individual tiles, never drawn whole, so any `TileSize` works.

**Scales with `TileSize`:** `Gfx/DA1TilesL.bmp` (the tile sheet, cut into `TileSize x TileSize` tiles), `Gfx/DA1HeroL.bmp`
(the hero walk sheet) and `Gfx/DA1HeroIdle.bmp` if used.

**Scales with `MonsterSize` instead** (independent of `TileSize`): `Gfx/DA1MonstL.bmp`.

**Does not scale with `TileSize` at all:** `Gfx/DA1ExtraL.bmp` (the small UI icon sheet - fixed 16x16 px tiles,
regardless of the mod), and everything procedurally drawn (text, dialog borders, menus) - all of that lives in the UI
layer's own fixed 640x400 design space and its own whole-number scale, entirely independent of the canvas. This is
deliberate (display-scaling-plan.md decision 6): it's what keeps the UI crisp and identically sized across mods with
different tile sizes.

## 4. Minimum and typical window scale

The player's window is `canvas x N` for a whole number `N` they choose in Options (`Scale:`). The game won't go
below the N that keeps the window at least 640x400 (`minN = ceil(max(640 / canvasW, 400 / canvasH))`), since that's
the UI layer's own reference size:

| Tile size | Canvas | Minimum N | Typical window at N=2 |
| --- | --- | --- | --- |
| 32 px | 512x320 | 2 | 1024x640 |
| 40 px (base) | 640x400 | 1 | 1280x800 |
| 48 px | 768x480 | 1 | 1536x960 |
| 64 px | 1024x640 | 1 | 2048x1280 |

Smaller tiles need a higher minimum N to stay above 640x400 in canvas pixels; larger tiles reach it at N=1 already.
There's no maximum tile size, but very large tiles reach a small maximum N on modest displays (see
`display-scaling-plan.md` section 5.2) - keep that in mind if a mod's canvas is meant to fill the screen.

## 5. Full-screen art

These images are no longer assumed to be any particular size (the redesign's one behavior change worth designing
for): each is drawn centered on the window at its own aspect ratio, at the largest whole-number scale that fits,
nearest-neighbor, with black bars on whichever axis is left over - the same routine for all of them
(`CWindow::renderFullScreenImage`).

| File | Shown | Base game size | Project32's size |
| --- | --- | --- | --- |
| `Gfx/datitle.bmp` | Title screen, behind the menu | 640x400 | 640x400 |
| `Gfx/death.bmp` | Hero's HP reaches 0 | 320x200 | 640x400 |
| `Gfx/daend1.bmp` | First endgame stage | 320x200 | 256x256 |
| `Gfx/explode.bmp` | Second endgame stage | 320x200 | 256x256 |
| `Gfx/daend2.bmp` | Third (final) endgame stage | 320x200 | 256x256 |

**Sizing guideline:** any size and aspect ratio works, but a size that's an exact whole-number multiple (or the exact
size) of the mod's own canvas (`16 x TileSize` by `10 x TileSize`, section 3) scales without ever landing on a
fractional window size relative to the canvas - not that it needs to match the canvas's *aspect ratio* particularly,
just that whole-number art scales more predictably across a wider range of window sizes. A square or portrait image
(like Project32's 256x256 endgame art) is equally fine; it simply gets its own best fit and letterboxes on whichever
axis is shorter.

**The title screen only:** the menu text ("New Game", "Load Game", "Options", "Exit", the version/copyright line) is
positioned in the UI layer's fixed 640x400 reference space, not relative to the title image - so it stays in the
same place on screen regardless of what size title image a mod supplies. Keep the image's important artwork clear of
roughly the vertical middle third and the bottom-left corner if you want it to read well under the menu and the
version text.
