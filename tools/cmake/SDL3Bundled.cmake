# Builds SDL3, SDL3_ttf and SDL3_mixer from source as STATIC libraries, from the source trees that tools/get-deps.ps1
# (Windows) or tools/get-deps.sh (Linux) unpack into DA_SDL_SOURCE_DIR. It is included by the top-level CMakeLists.txt
# (the Linux build) and by tools/sdl3-static/CMakeLists.txt (which get-deps.ps1 uses to build the libraries the Visual
# Studio project links), so both platforms build the libraries with exactly the same options.
#
# After including it, the targets SDL3::SDL3 (static), SDL3_ttf::SDL3_ttf and SDL3_mixer::SDL3_mixer exist.
#
# What is switched off, and why:
#  * SDL3_ttf uses its own copy of FreeType and nothing else (no HarfBuzz, no PlutoSVG): the game draws plain text.
#  * SDL3_mixer decodes Ogg Vorbis with its built-in stb_vorbis and WAVE, and nothing else: that is all the game plays,
#    and it means no external codec library is needed at all.

if(NOT DA_SDL_SOURCE_DIR)
  message(FATAL_ERROR "DA_SDL_SOURCE_DIR must point at the folder holding the SDL3, SDL3_ttf and SDL3_mixer source trees.")
endif()
get_filename_component(DA_SDL_SOURCE_DIR "${DA_SDL_SOURCE_DIR}" ABSOLUTE)

# The trees are found by name pattern so the versions live in one place, the get-deps script.
function(da_find_tree out pattern)
  file(GLOB found LIST_DIRECTORIES true "${DA_SDL_SOURCE_DIR}/${pattern}")
  set(dirs "")
  foreach(candidate IN LISTS found)
    if(IS_DIRECTORY "${candidate}" AND EXISTS "${candidate}/CMakeLists.txt")
      list(APPEND dirs "${candidate}")
    endif()
  endforeach()
  list(LENGTH dirs count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "Expected exactly one source tree matching '${pattern}' in ${DA_SDL_SOURCE_DIR} but found ${count}. Run tools/get-deps.")
  endif()
  list(GET dirs 0 first)
  set(${out} "${first}" PARENT_SCOPE)
endfunction()
da_find_tree(DA_SDL3_DIR       "SDL3-[0-9]*")
da_find_tree(DA_SDL3_TTF_DIR   "SDL3_ttf-[0-9]*")
da_find_tree(DA_SDL3_MIXER_DIR "SDL3_mixer-[0-9]*")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SDL3: static only, no test library, and don't try to install it
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

# SDL3_ttf: FreeType only. Its release archive does not bundle FreeType; get-deps puts the pinned copy in external/freetype.
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)

# SDL3_mixer: Ogg Vorbis through the built-in stb_vorbis, plus WAVE; no external codec libraries
set(SDLMIXER_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VORBIS_STB ON CACHE BOOL "" FORCE)
foreach(opt VORBIS_VORBISFILE VORBIS_TREMOR FLAC MOD MP3 MIDI OPUS GME WAVPACK AIFF VOC AU TESTS EXAMPLES INSTALL)
  set(SDLMIXER_${opt} OFF CACHE BOOL "" FORCE)
endforeach()

add_subdirectory("${DA_SDL3_DIR}"       "${CMAKE_BINARY_DIR}/sdl3"       EXCLUDE_FROM_ALL)
add_subdirectory("${DA_SDL3_TTF_DIR}"   "${CMAKE_BINARY_DIR}/sdl3_ttf"   EXCLUDE_FROM_ALL)
add_subdirectory("${DA_SDL3_MIXER_DIR}" "${CMAKE_BINARY_DIR}/sdl3_mixer" EXCLUDE_FROM_ALL)
