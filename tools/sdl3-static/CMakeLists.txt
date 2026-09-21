# Builds the SDL3 static libraries that msvc/Darkages.vcxproj links. Run by tools/get-deps.ps1, which passes
# -DDA_SDL_SOURCE_DIR (the unpacked SDL source trees) and a static C runtime (/MT for Release, /MTd for Debug).
# It produces nothing of its own; the point is the libraries the three SDL projects build.
cmake_minimum_required(VERSION 3.16)
project(DarkAgesSDL3 C CXX)

set(DA_SDL_SOURCE_DIR "" CACHE PATH "Folder holding the SDL3, SDL3_ttf and SDL3_mixer source trees")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/SDL3Bundled.cmake")

# One target that builds all three (and FreeType, which SDL3_ttf depends on); building this single target is more
# reliable with the Visual Studio generator than naming the three libraries separately.
add_custom_target(da_sdl3_libs)
add_dependencies(da_sdl3_libs SDL3-static SDL3_ttf-static SDL3_mixer-static)
