#!/usr/bin/env bash
# Downloads the SDL source releases that the Linux build compiles into the game (statically), into
# third_party/linux/ (git-ignored):
#
#   third_party/linux/SDL3-3.4.16/
#   third_party/linux/SDL3_ttf-3.2.2/     (with FreeType in external/freetype)
#   third_party/linux/SDL3_mixer-3.2.4/
#
# CMakeLists.txt finds them by name when given -DDA_SDL_SOURCE_DIR=third_party/linux. Every download is
# checked against a pinned SHA-256, and FreeType (which the SDL3_ttf archive does not bundle) is fetched at one
# exact git commit. The script is safe to re-run: sources that are already unpacked and verified are left alone.
# tools/package-linux.sh and the GitHub Actions workflows use this same script. The Windows counterpart is
# tools/get-deps.ps1, which pins the same versions.
#
# Usage: tools/get-deps.sh [--force]
#
# To move to newer versions, change the entries below (url, sha256, folder) and run it again.
set -euo pipefail

force=0
[ "${1:-}" = "--force" ] && force=1

repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
dest="$repo/third_party/linux"
downloads="$repo/third_party/_downloads"

# name|file|url|sha256|folder
deps=(
  "SDL3 3.4.16|SDL3-3.4.16.tar.gz|https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz|7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68|SDL3-3.4.16"
  "SDL3_ttf 3.2.2|SDL3_ttf-3.2.2.tar.gz|https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz|63547d58d0185c833213885b635a2c0548201cc8f301e6587c0be1a67e1e045d|SDL3_ttf-3.2.2"
  "SDL3_mixer 3.2.4|SDL3_mixer-3.2.4.tar.gz|https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.4/SDL3_mixer-3.2.4.tar.gz|182a07c745375e113dc740d43964ff21b0be29f29f59876c4dbc4db3d32f6901|SDL3_mixer-3.2.4"
)
# SDL3_ttf's FreeType: SDL's fork (branch VER-2-13-2-SDL), pinned to one commit.
freetype_url="https://github.com/libsdl-org/freetype.git"
freetype_commit="9973564cfa63763a3e4ac67c09147899539b1e07"
ttf_folder="SDL3_ttf-3.2.2"

sha256_of() {
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "$1" | cut -d' ' -f1
  else
    shasum -a 256 "$1" | cut -d' ' -f1   # macOS
  fi
}

fetch() {   # url, output file
  if command -v curl >/dev/null 2>&1; then
    curl --fail --location --silent --show-error --retry 3 --retry-delay 2 --output "$2" "$1"
  elif command -v wget >/dev/null 2>&1; then
    wget --quiet --tries=3 --output-document="$2" "$1"
  else
    echo "Neither curl nor wget is installed." >&2
    return 1
  fi
}

mkdir -p "$dest" "$downloads"

for entry in "${deps[@]}"; do
  IFS='|' read -r name file url sha folder <<<"$entry"
  target="$dest/$folder"
  marker="$target/.da-deps-sha256"   # records which tarball the unpacked folder came from
  archive="$downloads/$file"

  if [ "$force" -eq 0 ] && [ -f "$marker" ] && [ "$(cat "$marker")" = "$sha" ]; then
    printf '%-18s up to date\n' "$name"
    continue
  fi

  # use the cached download if it is intact, otherwise fetch it
  if [ ! -f "$archive" ] || [ "$(sha256_of "$archive")" != "$sha" ]; then
    printf '%-18s downloading %s\n' "$name" "$url"
    fetch "$url" "$archive"
    actual="$(sha256_of "$archive")"
    if [ "$actual" != "$sha" ]; then
      rm -f "$archive"
      echo "Checksum mismatch for $file: expected $sha, got $actual." >&2
      exit 1
    fi
  fi

  rm -rf "$target"
  tar -xzf "$archive" -C "$dest"
  [ -d "$target" ] || { echo "$file did not contain the expected folder '$folder'." >&2; exit 1; }
  echo "$sha" > "$marker"
  printf '%-18s ready in %s\n' "$name" "$target"
done

# FreeType: fetched by exact commit (git is content-addressed, so it cannot silently change)
ft_dir="$dest/$ttf_folder/external/freetype"
ft_head() { if [ -d "$ft_dir/.git" ]; then git -C "$ft_dir" rev-parse HEAD 2>/dev/null || true; fi; }
if [ "$(ft_head)" != "$freetype_commit" ]; then
  echo "FreeType           fetching commit $freetype_commit"
  rm -rf "$ft_dir"
  mkdir -p "$ft_dir"
  git -C "$ft_dir" init -q
  git -C "$ft_dir" remote add origin "$freetype_url"
  git -C "$ft_dir" fetch -q --depth 1 origin "$freetype_commit"
  git -C "$ft_dir" checkout -q FETCH_HEAD
  if [ "$(ft_head)" != "$freetype_commit" ]; then
    echo "FreeType checkout is not at the pinned commit $freetype_commit." >&2
    exit 1
  fi
fi
echo "FreeType           ready at commit $freetype_commit"

echo "Dependencies are ready in third_party/linux/."
