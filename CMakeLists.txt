# Dark Ages - CMake build. This is the Linux build; Windows builds use msvc/Darkages.sln (it also works with
# CMake, which is how the SDL wiring below was first checked, but the Visual Studio project is the supported way).
#
# Two ways to get SDL:
#
#   Bundled (what the releases use): tools/get-deps.sh downloads pinned SDL2, SDL2_ttf and SDL2_mixer source
#   trees into third_party/linux/, and this file builds them as static libraries and links them into the game.
#   The result needs nothing but the C library and the system's video/audio stack at run time.
#
#       tools/get-deps.sh
#       cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release -DDA_SDL_SOURCE_DIR=third_party/linux
#       cmake --build build/linux
#
#   System: with no DA_SDL_SOURCE_DIR the SDL2, SDL2_ttf and SDL2_mixer development packages installed on the
#   machine are used (needs SDL2_mixer 2.6 or newer for Mix_MusicDuration, and pkg-config).
#
#       cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release && cmake --build build/linux
#
# The executable is written to game/, next to the assets (override with -DDA_OUTPUT_DIR=...).
# Add -DDA_VERSION_SUFFIX=-dev.47+a1b2c3d to append a development suffix to the version shown in the game.
cmake_minimum_required(VERSION 3.16)
project(DarkAges CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(DA_SDL_SOURCE_DIR "" CACHE PATH "Folder with the SDL2, SDL2_ttf and SDL2_mixer source trees (see tools/get-deps.sh); empty = use installed SDL2 packages")
set(DA_VERSION_SUFFIX "" CACHE STRING "Appended to the base version in src/Version.h, e.g. -dev.47+a1b2c3d")
set(DA_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/game" CACHE PATH "Where the executable is written")

file(GLOB DA_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(Darkages ${DA_SOURCES})
target_include_directories(Darkages PRIVATE "${CMAKE_SOURCE_DIR}/src")
set_target_properties(Darkages PROPERTIES DEBUG_POSTFIX "_d")   # Darkages_d for Debug, like the Visual Studio project

if(DA_VERSION_SUFFIX)
  target_compile_definitions(Darkages PRIVATE "DA_VERSION_SUFFIX=\"${DA_VERSION_SUFFIX}\"")
endif()

if(MSVC)
  target_compile_definitions(Darkages PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(Darkages PRIVATE -Wno-write-strings)   # the code assigns string literals to char*
endif()

# The executable goes next to the assets, whatever the generator or configuration.
set_target_properties(Darkages PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${DA_OUTPUT_DIR}")
foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  set_target_properties(Darkages PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${cfg} "${DA_OUTPUT_DIR}")
endforeach()

if(DA_SDL_SOURCE_DIR)
  # ---- bundled SDL: build the three libraries from source as static libraries -------------------
  get_filename_component(DA_SDL_SOURCE_DIR "${DA_SDL_SOURCE_DIR}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")

  # The source trees are found by name pattern so the versions live in one place, tools/get-deps.sh.
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
      message(FATAL_ERROR "Expected exactly one source tree matching '${pattern}' in ${DA_SDL_SOURCE_DIR} but found ${count}. Run tools/get-deps.sh.")
    endif()
    list(GET dirs 0 first)
    set(${out} "${first}" PARENT_SCOPE)
  endfunction()
  da_find_tree(DA_SDL2_DIR       "SDL2-[0-9]*")
  da_find_tree(DA_SDL2_TTF_DIR   "SDL2_ttf-[0-9]*")
  da_find_tree(DA_SDL2_MIXER_DIR "SDL2_mixer-[0-9]*")

  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # SDL2: static only, no test library, and don't try to install it
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TEST OFF CACHE BOOL "" FORCE)
  set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

  # SDL2_ttf: FreeType comes from the source tree's own copy; no HarfBuzz
  set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
  set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
  set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
  set(SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE)

  # SDL2_mixer: the game only plays Ogg Vorbis (decoded by the stb_vorbis inside SDL2_mixer), so every other
  # format, and with it every external codec library, is switched off
  set(SDL2MIXER_VENDORED OFF CACHE BOOL "" FORCE)
  set(SDL2MIXER_VORBIS STB CACHE STRING "" FORCE)
  foreach(opt FLAC MOD MP3 MIDI OPUS GME WAVPACK CMD SAMPLES INSTALL)
    set(SDL2MIXER_${opt} OFF CACHE BOOL "" FORCE)
  endforeach()

  add_subdirectory("${DA_SDL2_DIR}" "${CMAKE_BINARY_DIR}/sdl2" EXCLUDE_FROM_ALL)
  add_subdirectory("${DA_SDL2_TTF_DIR}" "${CMAKE_BINARY_DIR}/sdl2_ttf" EXCLUDE_FROM_ALL)
  add_subdirectory("${DA_SDL2_MIXER_DIR}" "${CMAKE_BINARY_DIR}/sdl2_mixer" EXCLUDE_FROM_ALL)

  if(TARGET SDL2::SDL2main)
    target_link_libraries(Darkages PRIVATE SDL2::SDL2main)   # Windows only; SDL provides main() there
  endif()
  target_link_libraries(Darkages PRIVATE SDL2_mixer SDL2_ttf SDL2::SDL2)

  # a portable binary: no dependency on the build machine's libstdc++ / libgcc
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT WIN32)
    target_link_options(Darkages PRIVATE -static-libstdc++ -static-libgcc)
  endif()
else()
  # ---- system SDL ---------------------------------------------------------------------------------
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(DA_SDL REQUIRED IMPORTED_TARGET sdl2 SDL2_ttf SDL2_mixer)
  target_link_libraries(Darkages PRIVATE PkgConfig::DA_SDL)
endif()
