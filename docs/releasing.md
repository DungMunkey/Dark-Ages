# How builds and releases work

There are two kinds of published builds. Both are GitHub *pre-releases* at first, and each carries two downloads,
one per platform. There's no separate `.sha256` file for either: GitHub already computes and shows its own SHA-256
digest for each release asset, generated from the bytes it stored, so a downloaded file can be checked against that
directly. A checksum file we published ourselves alongside the download would come from the very same build and the
very same release - no more trustworthy than the download it's checking, and it would need its own way to be
verified in turn.

| Platform | Download | Made by (the same script you can run locally) |
| --- | --- | --- |
| Windows x64 | `DarkAges-<version>-win64.zip` | [`tools/package.ps1`](../tools/package.ps1) |
| Linux x86_64 | `DarkAges-<version>-linux64.tar.gz` | [`tools/package-linux.sh`](../tools/package-linux.sh) |

The file names have the version and dev number only, e.g. `DarkAges-2.0.0-alpha.4-dev.47-win64.zip`; the commit id
(the part after `+`) is left out of them, though it stays in the version shown in the game.

Both platforms are built in parallel by one shared workflow (`build.yml`). **A release or dev build is only
published if both succeed**: if either platform fails, nothing is published.

## Dev builds (automatic)

* **Trigger:** every push to the `dev` branch, except a push that only changes files under `docs/` or Markdown
  files. (A manual run of the workflow from another branch does nothing.)
* **Version:** the base version from `src/Version.h` plus a suffix, e.g. `2.0.0-alpha.4-dev.47+a1b2c3d`
  (`47` is the workflow's run number, `a1b2c3d` the commit). It is shown on the title screen and in the credits,
  and at the top of each download's README, which also carries a "development build" warning.
* **Where:** a pre-release with tag `dev-47`, titled "Dev build 47 (...)". Its notes list the commits since the
  previous dev build.
* **Retention:** the newest 10 dev builds are kept. Older ones, and their tags, are deleted automatically.

Pull requests to `dev` or `master` get a build-only check for both platforms (nothing is published; the archives
are kept for 3 days as workflow artifacts).

## Making a release (by hand)

1. **Get `master` ready.** Merge `dev` into `master`. `DA_VERSION_BASE` in `src/Version.h` is the version that will
   be released, e.g. `2.0.0-alpha.4`.
2. **Run the workflow.** On GitHub: *Actions* → *Release* → *Run workflow* (on `master`). It:
   * stops immediately if the tag `v<version>` already exists (it never overwrites a release),
   * builds `master`, and
   * publishes a **pre-release** titled "Dark Ages 2.0.0-alpha.4" with tag `v2.0.0-alpha.4`.
3. **Check it.** Download the zip and the tar.gz from the release page and try them.
4. **Approve it by hand.** Edit the release, untick **Set as a pre-release**, and tick **Set as the latest
   release**. Nothing in the automation does this step.
5. **Start the next version.** On `dev`, bump `DA_VERSION_BASE` (e.g. to `2.0.0-alpha.5`) so development builds
   don't label themselves as the release that just shipped.

The *Release* button only appears once `release.yml` exists on the default branch (`master`), so the first release
needs the merge in step 1 anyway.

## What is in the downloads

One folder, `DarkAges/`, holding the program (a single executable on each platform: SDL is linked into it, so there
are no DLLs), `Font/`, `Gfx/`, `Maps/`, `Music/`, every mod in `Mods/`, `darkages.cfg`,
`README.txt`, `LICENSE` and `THIRD-PARTY.txt`. Never: source code, object or `.pdb` files, project or build files,
save games, or art-source files (`.xcf`, `.psd`, `.wav`, `.mid`, ...). Each script copies a fixed list of items
rather than "everything in `game/`", and checks its own result before writing the archive.

* **Windows:** SDL3, SDL3_ttf and SDL3_mixer are built from source as static libraries (`tools/get-deps.ps1`) and
  the exe links the C++ runtime statically, so players need neither DLLs nor the Visual C++ Redistributable. The
  exe imports only Windows system DLLs, and `package.ps1` refuses a package containing any `.dll`.
* **Linux:** SDL3, SDL3_ttf and SDL3_mixer are compiled from source (pinned in `tools/get-deps.sh`) and linked
  in statically, along with the C++ runtime, so the download needs nothing installed. It is built on Ubuntu 22.04
  (glibc 2.35), so it runs on most current distributions; SDL loads the X11/Wayland/audio libraries of the player's
  system at run time. The archive is root-owned with sensible permissions, and the executable keeps its execute bit.

**`darkages.cfg`** holds every setting at its default except the mod, which is set to `Project32` so that is what
players see first. It is never copied from your `game/` folder: the script has the freshly built exe write it
(`Darkages.exe --write-default-config <file> <mod>`, which opens no window), so the file always matches the game's
current settings layout. `-DefaultMod` on `package.ps1` (`--default-mod` on `package-linux.sh`) picks a different
mod. Because it is in the download, the README tells players to extract updates into a new folder and copy their
`Saves` and `darkages.cfg` across; extracting over an old install would reset their settings.

A package you build on your own machine includes any local-only mods in `game/Mods` (such as `Test` and `Test2`,
which are git-ignored); the automated builds only see what is committed.

## Good to know

* **Version length:** the whole version string must be 30 characters or fewer (the credits line has 38 columns
  and starts with "Version "). `package.ps1` and a `static_assert` in the source both refuse anything longer, so
  run numbers stop fitting at 5 digits or with a much longer base version.
* **SDL downloads:** `tools/get-deps.ps1` (Windows) and `tools/get-deps.sh` (Linux) check every download against a
  pinned SHA-256; both platforms use SDL3 3.4.16, SDL3_ttf 3.2.2 and SDL3_mixer 3.2.4, all built from source. The
  SDL3_ttf release tarball no longer bundles FreeType, so the scripts fetch the SDL project's FreeType fork
  (`libsdl-org/freetype`, branch `VER-2-13-2-SDL`) at one pinned commit id, which is as fixed as a hash. The tarballs
  come from GitHub releases; if GitHub is ever unavailable and the cache is cold, the build cannot run. To move to
  newer SDL versions, change the pins at the top of both scripts (and the notices in `package.ps1` if a license
  file moved).
* **Windows build details:** the SDL libraries are built through a small CMake wrapper (`tools/sdl3-static/`, sharing
  settings with the Linux build in `tools/cmake/SDL3Bundled.cmake`) with the static C++ runtime to match the game
  (`/MT`, `/MTd` for Debug). The built libraries live in `third_party/sdl3/lib/<Configuration>`, and the Visual
  Studio project only links them. Windows tools cannot open paths over 260 characters and SDL's build nests deeply,
  so `get-deps.ps1` refuses a repository path longer than 90 characters; the Actions runner's path is short enough.
* **The Linux runner image:** the Linux job uses `ubuntu-22.04` on purpose (an older glibc runs on more systems).
  If GitHub retires that image, moving to a newer one raises the minimum distribution version.
* **Action versions:** the workflows use `actions/checkout`, `cache`, `upload-artifact` and `download-artifact` at
  major version 4. If GitHub shows a deprecation warning on a run, bump them.
