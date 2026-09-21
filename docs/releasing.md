# How builds and releases work

There are two kinds of published builds. Both are GitHub *pre-releases* at first, both contain the same kind of
zip (`DarkAges-<version>-win64.zip` plus a `.sha256` checksum file), and both are made by GitHub Actions running
[`tools/package.ps1`](../tools/package.ps1), the same script you can run locally. The zip's file name has the
version and dev number only, e.g. `DarkAges-2.0.0-alpha.4-dev.47-win64.zip`; the commit id (the part after `+`)
is left out of it, though it stays in the version shown in the game.

## Dev builds (automatic)

* **Trigger:** every push to the `dev` branch, except a push that only changes files under `docs/` or Markdown
  files. (A manual run of the workflow from another branch does nothing.)
* **Version:** the base version from `src/Version.h` plus a suffix, e.g. `2.0.0-alpha.4-dev.47+a1b2c3d`
  (`47` is the workflow's run number, `a1b2c3d` the commit). It is shown on the title screen and in the credits,
  and at the top of the zip's README, which also carries a "development build" warning.
* **Where:** a pre-release with tag `dev-47`, titled "Dev build 47 (...)". Its notes list the commits since the
  previous dev build, plus the zip's SHA-256.
* **Retention:** the newest 10 dev builds are kept. Older ones, and their tags, are deleted automatically.

Pull requests to `dev` or `master` get a build-only check (nothing is published; the zip is kept for 3 days as a
workflow artifact).

## Making a release (by hand)

1. **Get `master` ready.** Merge `dev` into `master`. `DA_VERSION_BASE` in `src/Version.h` is the version that will
   be released, e.g. `2.0.0-alpha.4`.
2. **Run the workflow.** On GitHub: *Actions* → *Release* → *Run workflow* (on `master`). It:
   * stops immediately if the tag `v<version>` already exists (it never overwrites a release),
   * builds `master`, and
   * publishes a **pre-release** titled "Dark Ages 2.0.0-alpha.4" with tag `v2.0.0-alpha.4`.
3. **Check it.** Download the zip from the release page and try it.
4. **Approve it by hand.** Edit the release, untick **Set as a pre-release**, and tick **Set as the latest
   release**. Nothing in the automation does this step.
5. **Start the next version.** On `dev`, bump `DA_VERSION_BASE` (e.g. to `2.0.0-alpha.5`) so development builds
   don't label themselves as the release that just shipped.

The *Release* button only appears once `release.yml` exists on the default branch (`master`), so the first release
needs the merge in step 1 anyway.

## What is in the zip

One folder, `DarkAges/`: `Darkages.exe`, the five SDL runtime DLLs, `Font/`, `Gfx/`, `Maps/`, `Music/`, every
mod in `Mods/`, `darkages.cfg`, `README.txt`, `LICENSE` and `THIRD-PARTY.txt`. Never: source code, object or `.pdb`
files, Visual Studio files, save games, or art-source files (`.xcf`, `.psd`, `.wav`, `.mid`, ...). The script
copies a fixed list of items rather than "everything in `game/`", and checks its own result before writing the zip.
The exe links the C++ runtime statically, so players don't need the Visual C++ Redistributable.

**`darkages.cfg`** holds every setting at its default except the mod, which is set to `Project32` so that is what
players see first. It is never copied from your `game/` folder: the script has the freshly built exe write it
(`Darkages.exe --write-default-config <file> <mod>`, which opens no window), so the file always matches the game's
current settings layout. `-DefaultMod` on the script picks a different mod. Because it is in the zip, the zip's
README tells players to extract updates into a new folder and copy their `Saves` and `darkages.cfg` across;
extracting over an old install would reset their settings.

A zip you build on your own machine includes any local-only mods in `game/Mods` (such as `Test` and `Test2`,
which are git-ignored); the automated builds only see what is committed.

## Good to know

* **Version length:** the whole version string must be 30 characters or fewer (the credits line has 38 columns
  and starts with "Version "). `package.ps1` and a `static_assert` in the source both refuse anything longer, so
  run numbers stop fitting at 5 digits or with a much longer base version.
* **SDL downloads:** `tools/get-deps.ps1` checks every download against a pinned SHA-256. SDL2_ttf 2.0.12 is only
  hosted on libsdl.org (not GitHub), so the workflow caches `third_party/`. If libsdl.org is ever unavailable and
  the cache is cold, mirror the three zips in this repository and point `get-deps.ps1` at them.
* **Action versions:** the workflows use `actions/checkout`, `cache`, `upload-artifact` and `download-artifact` at
  major version 4. If GitHub shows a deprecation warning on a run, bump them.
