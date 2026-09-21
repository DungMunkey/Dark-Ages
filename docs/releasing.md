# How builds and releases work

There are two kinds of published builds. Both are GitHub *pre-releases* at first, and each carries two downloads,
one per platform, plus a `.sha256` checksum file for each:

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
  previous dev build, plus the SHA-256 of the downloads.
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

One folder, `DarkAges/`, holding the program (`Darkages.exe` plus the five SDL runtime DLLs on Windows; the single
`Darkages` executable on Linux), `Font/`, `Gfx/`, `Maps/`, `Music/`, every mod in `Mods/`, `darkages.cfg`,
`README.txt`, `LICENSE` and `THIRD-PARTY.txt`. Never: source code, object or `.pdb` files, project or build files,
save games, or art-source files (`.xcf`, `.psd`, `.wav`, `.mid`, ...). Each script copies a fixed list of items
rather than "everything in `game/`", and checks its own result before writing the archive.

* **Windows:** the exe links the C++ runtime statically, so players don't need the Visual C++ Redistributable.
* **Linux:** SDL2, SDL2_ttf and SDL2_mixer 2.8.1 are compiled from source (pinned in `tools/get-deps.sh`) and linked
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
  pinned SHA-256. Windows uses SDL2 2.0.12 / SDL2_ttf 2.0.12 / SDL2_mixer 2.8.1; Linux builds SDL2 2.32.10 /
  SDL2_ttf 2.24.0 / SDL2_mixer 2.8.1 from source. SDL2_ttf 2.0.12 is only hosted on libsdl.org (not GitHub), so the
  workflow caches `third_party/`. If libsdl.org is ever unavailable and the cache is cold, mirror the zips in this
  repository and point `get-deps.ps1` at them.
* **The Linux runner image:** the Linux job uses `ubuntu-22.04` on purpose (an older glibc runs on more systems).
  If GitHub retires that image, moving to a newer one raises the minimum distribution version.
* **Action versions:** the workflows use `actions/checkout`, `cache`, `upload-artifact` and `download-artifact` at
  major version 4. If GitHub shows a deprecation warning on a run, bump them.
