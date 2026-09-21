# Dark Ages: The Continents

A classic tile-based role-playing game for Windows, written in C++ with SDL2.

## Download

Every build is published on the [Releases page](https://github.com/DungMunkey/Dark-Ages/releases):

| What | How to recognize it | Who it is for |
| --- | --- | --- |
| **Release** | Titled "Dark Ages 2.0.0-alpha.3" (a `v...` tag) and *not* marked "Pre-release" | Everyone |
| **Dev build** | Titled "Dev build 47 (...)" (a `dev-...` tag), marked "Pre-release" | Early adopters. Built automatically from the `dev` branch on every push: expect bugs, unfinished features, and back up your saves. |

A release that is still marked "Pre-release" has not been approved yet. (GitHub's "latest release" link
ignores pre-releases, so use the Releases page rather than that link.)

## Playing

Download the zip, **extract the whole folder** somewhere you can write to (not "Program Files"), and run
`Darkages.exe`. It needs 64-bit Windows 10 or later and nothing else. Save games and settings are kept in that
folder, next to the exe. The zip contains a README with the controls.

## Building from source (Windows)

You need Visual Studio 2022 (the free Community edition is fine) with the **Desktop development with C++**
workload.

1. Fetch the SDL libraries (one time; they are downloaded, checked against pinned SHA-256 values, and unpacked into
   `third_party/`):

   ```
   powershell -ExecutionPolicy Bypass -File tools\get-deps.ps1
   ```

2. Open `msvc\Darkages.sln` and press F5, or build from a command line:

   ```
   msbuild msvc\Darkages.sln /p:Configuration=Release /p:Platform=x64
   ```

   The exe is built straight into `game/`, next to the assets: `Darkages.exe` for Release, `Darkages_d.exe` for
   Debug (which also opens a console window). Run it from anywhere; it finds its own folder.

3. To make the same zip the automated builds publish:

   ```
   powershell -ExecutionPolicy Bypass -File tools\package.ps1
   ```

   The result is `dist\DarkAges-<version>-win64.zip` with a `.sha256` file next to it.

## Repository layout

| Folder | Contents |
| --- | --- |
| `src/` | All C++ source |
| `game/` | Everything the game loads at run time (`Font`, `Gfx`, `Maps`, `Music`, `Mods`). Building puts the exe and DLLs here too; your saves and `darkages.cfg` appear here when you play. |
| `msvc/` | The Visual Studio 2022 solution and project |
| `sources/` | Art, audio and font sources the game itself never loads (the FontForge project, GIMP files for mods, MIDI files, ...) |
| `tools/` | `get-deps.ps1` (fetch SDL), `package.ps1` (build the zip), and the README that goes in the zip |
| `docs/` | [How releases work](docs/releasing.md) and [ideas for future work](docs/future-work.md) |
| `.github/workflows/` | The automated builds |
| `Makefile` | For a future Linux build. It does not compile yet. |

## Mods

Each folder in `game/Mods` is a mod that can replace the game's graphics, maps and music, and is chosen from the
Options menu (it takes effect on the next start). A mod holds its own `Gfx`, `Maps` and `Music` files plus a
`mod.cfg`; anything a mod doesn't provide falls back to the base game. `game/Mods/Project32` is a full example.
Every mod in `game/Mods` is included in the zips.

## Branches

* `master` is stable; releases are built from it.
* `dev` is where development happens; every push to it publishes a dev build.

## License

The game's code is under the [Apache License 2.0](LICENSE). It uses SDL2, SDL2_ttf and SDL2_mixer; their licenses
are included in every zip as `THIRD-PARTY.txt`.
