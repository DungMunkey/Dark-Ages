#!/usr/bin/env bash
# Builds the game for Linux (x86_64) and packs it into dist/DarkAges-<version>-linux64.tar.gz
#
# The Linux counterpart of tools/package.ps1, used both locally and by the GitHub Actions workflows. The archive holds
# a single folder, DarkAges/, with only what is needed to play:
#
#   Darkages (the executable, with SDL statically linked in)
#   Font/  Gfx/  Maps/  Music/  Mods/     (every mod in game/Mods)
#   darkages.cfg                           (default settings, with the default mod selected)
#   README.txt  LICENSE  THIRD-PARTY.txt
#
# It never contains source code, build files, saves, or art-source files (.xcf .psd .wav .mid ...). Only that fixed
# list is copied - not "everything in game/" - and darkages.cfg is generated fresh by the built game
# (Darkages --write-default-config), never copied from game/.
#
# Usage: tools/package-linux.sh [--suffix -dev.47+a1b2c3d] [--default-mod Project32] [--output dir] [--skip-build]
#
#   --suffix        appended to the base version in src/Version.h (leave out for a release)
#   --default-mod   the mod selected in darkages.cfg (default Project32; must exist in game/Mods)
#   --output        where the archive goes (default: <repo>/dist)
#   --skip-build    package whatever is already built in game/ (still needs third_party/linux for the notices)
#
# Needs: cmake, a C++17 compiler, and the development packages SDL itself needs (X11, ALSA, ... - see
# .github/workflows/build.yml for the list used on Ubuntu 22.04).
set -euo pipefail

suffix=""
default_mod="Project32"
skip_build=0
repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
output="$repo/dist"

while [ $# -gt 0 ]; do
  case "$1" in
    --suffix)       suffix="${2?--suffix needs a value}"; shift 2 ;;   # an empty value is fine: it means a release
    --default-mod)  default_mod="${2:?--default-mod needs a value}"; shift 2 ;;
    --output)       output="${2:?--output needs a folder}"; shift 2 ;;
    --skip-build)   skip_build=1; shift ;;
    -h|--help)      sed -n '2,22p' "${BASH_SOURCE[0]}"; exit 0 ;;
    *) echo "Unknown option: $1" >&2; exit 2 ;;
  esac
done

# ---- version -------------------------------------------------------------------------------------
version_h="$repo/src/Version.h"
define() {   # the quoted value of a #define in Version.h (tr strips the CR of a Windows checkout)
  tr -d '\r' < "$version_h" | sed -n "s/^[[:space:]]*#define[[:space:]]\{1,\}$1[[:space:]]\{1,\}\"\(.*\)\".*/\1/p" | head -n1
}
base="$(define DA_VERSION_BASE)"
copyright="$(define DA_COPYRIGHT_YEARS), $(define DA_COPYRIGHT_OWNER)"
[ -n "$base" ] || { echo "Could not read DA_VERSION_BASE from src/Version.h" >&2; exit 1; }
version="${base}${suffix}"
if [ "${#version}" -gt 30 ]; then
  echo "Version '$version' is longer than 30 characters and won't fit on the credits line." >&2
  exit 1
fi
archive_name="DarkAges-${version%%+*}-linux64.tar.gz"   # the commit id after '+' stays out of the file name
echo "Packaging Dark Ages $version  ->  $archive_name"

# ---- build ---------------------------------------------------------------------------------------
if [ "$skip_build" -eq 0 ]; then
  "$repo/tools/get-deps.sh"
  generator=()
  command -v ninja >/dev/null 2>&1 && generator=(-G Ninja)
  cmake -S "$repo" -B "$repo/build/linux" "${generator[@]}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DDA_SDL_SOURCE_DIR="$repo/third_party/linux" \
        -DDA_VERSION_SUFFIX="$suffix"
  cmake --build "$repo/build/linux" --parallel
fi
[ -x "$repo/game/Darkages" ] || { echo "game/Darkages is missing - the build did not produce it." >&2; exit 1; }

# ---- stage exactly what ships ----------------------------------------------------------------------
stage_root="$repo/build/package-linux"
stage="$stage_root/DarkAges"
rm -rf "$stage_root"
mkdir -p "$stage"

cp "$repo/game/Darkages" "$stage/Darkages"
if command -v strip >/dev/null 2>&1; then strip "$stage/Darkages"; fi

[ -d "$repo/game/Mods/$default_mod" ] || { echo "The default mod '$default_mod' is not in game/Mods." >&2; exit 1; }

# Art / audio source files and OS junk are never shipped, even inside a mod's folder.
( cd "$repo/game" && find Font Gfx Maps Music Mods -type f \
    ! \( -iname '*.xcf' -o -iname '*.psd' -o -iname '*.kra' -o -iname '*.aseprite' -o -iname '*.blend' \
         -o -iname '*.sfd' -o -iname '*.wav' -o -iname '*.mid' -o -iname '*.midi' -o -iname '*.pdf' \
         -o -name 'Thumbs.db' -o -name 'desktop.ini' -o -name '.DS_Store' \) -print0 ) \
  | tar -C "$repo/game" --null -T - -cf - | tar -C "$stage" -xf -

# darkages.cfg: every setting at its default, except the mod. The game writes it itself (it owns the layout and the
# defaults); it opens no window and exits straight away.
"$stage/Darkages" --write-default-config "$stage/darkages.cfg" "$default_mod"
cfg_mod="$(dd if="$stage/darkages.cfg" bs=1 skip=14 count=32 2>/dev/null | tr -d '\0')"
if [ "$cfg_mod" != "$default_mod" ]; then
  echo "darkages.cfg was written but selects '$cfg_mod', not '$default_mod' (has the layout of sConf in src/Structs.h changed?)." >&2
  exit 1
fi

cp "$repo/LICENSE" "$stage/LICENSE"

# README: the template with the version filled in (development builds also get a warning at the top)
dev_note=""
case "$suffix" in
  *dev*) dev_note=$'\n  *** DEVELOPMENT BUILD ***\n  This is an early, work-in-progress build made automatically from the latest\n  development code. Expect bugs, and features may be incomplete or change\n  without notice. Please keep a backup of your save games.\n' ;;
esac
readme="$(cat "$repo/tools/game-readme-linux.txt")"
readme="${readme//'{{VERSION}}'/$version}"
readme="${readme//'{{COPYRIGHT}}'/$copyright}"
readme="${readme//'{{DEV_NOTE}}'/$dev_note}"
printf '%s\n' "$readme" > "$stage/README.txt"

# THIRD-PARTY.txt: assembled from the license files in the source trees that were compiled in
deps="$repo/third_party/linux"
tree() { local match; match="$(find "$deps" -maxdepth 1 -type d -name "$1" | head -n1)"; [ -n "$match" ] || { echo "No source tree matching '$1' in third_party/linux - run tools/get-deps.sh." >&2; exit 1; }; printf '%s' "$match"; }
sdl3="$(tree 'SDL3-[0-9]*')"; ttf="$(tree 'SDL3_ttf-[0-9]*')"; mixer="$(tree 'SDL3_mixer-[0-9]*')"
notice() {   # heading, site, note, files...
  local heading="$1" site="$2" note="$3"; shift 3
  printf '\n------------------------------------------------------------------------------\n%s  -  %s\n' "$heading" "$site"
  [ -z "$note" ] || printf '%s\n' "$note"
  printf -- '------------------------------------------------------------------------------\n'
  local f
  for f in "$@"; do
    [ -f "$f" ] || { echo "License file $f not found." >&2; exit 1; }
    tr -d '\r' < "$f" | sed -e :a -e '/^\n*$/{$d;N;ba' -e '}'
    printf '\n'
  done
}
{
  printf 'THIRD-PARTY SOFTWARE\n====================\nDark Ages ships with the following third-party libraries, compiled into the game.\n'
  notice "$(basename "$sdl3")" "https://www.libsdl.org/" "" "$sdl3/LICENSE.txt"
  notice "$(basename "$ttf")" "https://github.com/libsdl-org/SDL_ttf" "SDL3_ttf includes FreeType; its license follows." "$ttf/LICENSE.txt"
  notice "FreeType" "https://www.freetype.org/" "Portions of this software are copyright (c) The FreeType Project (www.freetype.org). All rights reserved." "$ttf/external/freetype/LICENSE.TXT" "$ttf/external/freetype/docs/FTL.TXT"
  notice "$(basename "$mixer")" "https://github.com/libsdl-org/SDL_mixer" "SDL3_mixer includes the stb_vorbis decoder for Ogg Vorbis, released by its author under public-domain or permissive terms." "$mixer/LICENSE.txt"
} > "$stage/THIRD-PARTY.txt"

# ---- self-check: refuse to produce an archive that is missing something or contains something it must not ---
required=(Darkages darkages.cfg README.txt LICENSE THIRD-PARTY.txt Font/DA1qb.ttf Maps/Aaryak.map
          Gfx/DA1TilesL.bmp Gfx/DA1HeroL.bmp Gfx/DA1ExtraL.bmp Gfx/DA1MonstL.bmp Gfx/death.bmp Gfx/daend1.bmp
          Gfx/daend2.bmp Gfx/explode.bmp Gfx/datitle.bmp Music/battle.ogg Music/dungeon.ogg Music/title.ogg Music/town.ogg)
for f in "${required[@]}"; do
  [ -e "$stage/$f" ] || { echo "Package check failed: $f is missing from the package." >&2; exit 1; }
done
forbidden="$(cd "$stage" && find . \( -iname '*.o' -o -iname '*.a' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.cmake' \
    -o -iname '*.xcf' -o -iname '*.psd' -o -iname '*.wav' -o -iname '*.mid' -o -iname '*.pdf' -o -name Saves -o -name 'Darkages_d' \
    -o \( -iname darkages.cfg ! -path ./darkages.cfg \) \) -print)"
if [ -n "$forbidden" ]; then
  echo "Package check failed: unexpected files in the package:" >&2
  echo "$forbidden" >&2
  exit 1
fi

# ---- archive: sorted, root-owned, sensible permissions --------------------------------------------------
mkdir -p "$output"
output="$(cd "$output" && pwd)"
chmod -R u+rwX,go+rX,go-w "$stage"
chmod 755 "$stage/Darkages"
rm -f "$output/$archive_name"
tar -C "$stage_root" --sort=name --owner=0 --group=0 --numeric-owner -czf "$output/$archive_name" DarkAges

count="$(find "$stage" -type f | wc -l)"
size="$(du -m "$output/$archive_name" | cut -f1)"
echo
echo "Created $output/$archive_name  (${size} MB, ${count} files)"
# GitHub computes and shows its own SHA-256 digest for each release asset, generated from the bytes it
# actually stored - checking a download against that is at least as good as checking it against a
# checksum file we publish ourselves from the same build, and does not have a downloaded-file-verifies-
# itself problem. So no separate .sha256 file: see the discussion recorded in docs/releasing.md.
if command -v sha256sum >/dev/null 2>&1; then
  echo "SHA-256  $(cd "$output" && sha256sum "$archive_name" | cut -d' ' -f1)"
else
  echo "SHA-256  $(cd "$output" && shasum -a 256 "$archive_name" | cut -d' ' -f1)"
fi

# hand the results to a GitHub Actions workflow, if that is what is running us
if [ -n "${GITHUB_OUTPUT:-}" ]; then
  {
    echo "version=$version"
    echo "archive_name=$archive_name"
    echo "archive_path=$output/$archive_name"
  } >> "$GITHUB_OUTPUT"
fi
