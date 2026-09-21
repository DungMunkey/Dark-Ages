#!/usr/bin/env bash
# Downloads the SDL source releases that the Linux build compiles into the game (statically), into
# third_party/linux/ (git-ignored):
#
#   third_party/linux/SDL2-2.32.10/
#   third_party/linux/SDL2_ttf-2.24.0/
#   third_party/linux/SDL2_mixer-2.8.1/
#
# CMakeLists.txt finds them by name when given -DDA_SDL_SOURCE_DIR=third_party/linux. Every download is
# checked against a pinned SHA-256, and the script is safe to re-run: sources that are already unpacked and
# verified are left alone. tools/package-linux.sh and the GitHub Actions workflows use this same script.
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
  "SDL2 2.32.10|SDL2-2.32.10.tar.gz|https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-2.32.10.tar.gz|5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165|SDL2-2.32.10"
  "SDL2_ttf 2.24.0|SDL2_ttf-2.24.0.tar.gz|https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-2.24.0.tar.gz|0b2bf1e7b6568adbdbc9bb924643f79d9dedafe061fa1ed687d1d9ac4e453bfd|SDL2_ttf-2.24.0"
  "SDL2_mixer 2.8.1|SDL2_mixer-2.8.1.tar.gz|https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-2.8.1.tar.gz|cb760211b056bfe44f4a1e180cc7cb201137e4d1572f2002cc1be728efd22660|SDL2_mixer-2.8.1"
)

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

echo "Dependencies are ready in third_party/linux/."
