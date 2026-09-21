# Dark Ages - CMake build. This is the Linux build; Windows builds use msvc/Darkages.sln (it also works with CMake,
# which is how the SDL wiring was first checked, but the Visual Studio project is the supported way).
#
# Two ways to get SDL:
#
#   Bundled (what the releases use): tools/get-deps.sh downloads pinned SDL3, SDL3_ttf and SDL3_mixer source trees into
#   third_party/linux/, and this file builds them as static libraries and links them into the game (the options are in
#   tools/cmake/SDL3Bundled.cmake, shared with the Windows dependency build). The result needs nothing but the C library
#   and the system's video/audio stack at run time.
#
#       tools/get-deps.sh
#       cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release -DDA_SDL_SOURCE_DIR=third_party/linux
#       cmake --build build/linux
#
#   System: with no DA_SDL_SOURCE_DIR the SDL3, SDL3_ttf and SDL3_mixer development packages installed on the machine
#   are used (found with pkg-config).
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

set(DA_SDL_SOURCE_DIR "" CACHE PATH "Folder with the SDL3, SDL3_ttf and SDL3_mixer source trees (see tools/get-deps.sh); empty = use installed SDL3 packages")
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
  include("${CMAKE_SOURCE_DIR}/tools/cmake/SDL3Bundled.cmake")

  if(TARGET SDL3::SDL3)
    target_link_libraries(Darkages PRIVATE SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf SDL3::SDL3)
  else()
    target_link_libraries(Darkages PRIVATE SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf SDL3::SDL3-static)
  endif()

  # a portable binary: no dependency on the build machine's libstdc++ / libgcc
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT WIN32)
    target_link_options(Darkages PRIVATE -static-libstdc++ -static-libgcc)
  endif()
else()
  # ---- system SDL ---------------------------------------------------------------------------------
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(DA_SDL REQUIRED IMPORTED_TARGET sdl3 sdl3-ttf sdl3-mixer)
  target_link_libraries(Darkages PRIVATE PkgConfig::DA_SDL)
endif()
