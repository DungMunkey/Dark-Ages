<#
.SYNOPSIS
  Downloads and builds the SDL libraries the game links: SDL3, SDL3_ttf and SDL3_mixer, as static libraries.

.DESCRIPTION
  1. Downloads the pinned SDL3, SDL3_ttf and SDL3_mixer source releases, verifying each against a SHA-256, and unpacks
     them under third_party/sdl3-src/.
  2. Fetches the FreeType fork that SDL3_ttf uses, at one exact commit, into SDL3_ttf's external/freetype (the SDL3_ttf
     release archive does not bundle it). Git verifies the commit, so no moving branch can change what gets built.
  3. Builds all three libraries with CMake (tools/sdl3-static) for Release and Debug, with a static C runtime (/MT and
     /MTd), and installs the results into third_party/sdl3/ (include/ and lib/<Configuration>/).

  msvc/Darkages.vcxproj links those static libraries, so the game is one executable with nothing else to ship. Run this
  once after cloning, and again whenever the pins below change. It is safe to re-run: if everything is already built for
  the same pins and build files, it does nothing. The GitHub Actions workflows use this same script.

  Needs: Git, CMake, and Visual Studio 2022 with "Desktop development with C++" (all present on GitHub's Windows runners).

.PARAMETER Force
  Download, unpack and rebuild everything, even if it looks up to date.

.PARAMETER ReleaseOnly
  Build only the Release libraries (enough to package the game; the workflows and tools\package.ps1 use this).
  Without it the Debug libraries are built too, which Visual Studio's Debug configuration needs.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\get-deps.ps1
#>
[CmdletBinding()]
param(
  [switch]$Force,
  [switch]$ReleaseOnly
)

$ErrorActionPreference = 'Stop'
$ProgressPreference    = 'SilentlyContinue'   # Invoke-WebRequest is very slow with the progress bar on Windows PowerShell 5.1
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$repoRoot   = Split-Path -Parent $PSScriptRoot
$thirdParty = Join-Path $repoRoot 'third_party'
$downloads  = Join-Path $thirdParty '_downloads'
$srcRoot    = Join-Path $thirdParty 'sdl3-src'
$buildDir   = Join-Path $thirdParty 'sdl3-build'
$outDir     = Join-Path $thirdParty 'sdl3'

# To move to newer versions, change the entries here (url, sha256, folder) and re-run. SDL3 3.4.x patch releases come
# out roughly monthly, so bump deliberately.
$deps = @(
  @{
    Name   = 'SDL3 3.4.16'
    File   = 'SDL3-3.4.16.tar.gz'
    Url    = 'https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz'
    Sha256 = '7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68'
    Folder = 'SDL3-3.4.16'
  },
  @{
    Name   = 'SDL3_ttf 3.2.2'
    File   = 'SDL3_ttf-3.2.2.tar.gz'
    Url    = 'https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz'
    Sha256 = '63547d58d0185c833213885b635a2c0548201cc8f301e6587c0be1a67e1e045d'
    Folder = 'SDL3_ttf-3.2.2'
  },
  @{
    Name   = 'SDL3_mixer 3.2.4'
    File   = 'SDL3_mixer-3.2.4.tar.gz'
    Url    = 'https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.4/SDL3_mixer-3.2.4.tar.gz'
    Sha256 = '182a07c745375e113dc740d43964ff21b0be29f29f59876c4dbc4db3d32f6901'
    Folder = 'SDL3_mixer-3.2.4'
  }
)
# SDL3_ttf's FreeType: SDL's fork (branch VER-2-13-2-SDL), pinned to one commit.
$freetypeUrl    = 'https://github.com/libsdl-org/freetype.git'
$freetypeCommit = '9973564cfa63763a3e4ac67c09147899539b1e07'
$ttfFolder      = 'SDL3_ttf-3.2.2'

function Get-Sha256([string]$path) {
  return (Get-FileHash -Path $path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-StringSha256([string]$text) {
  $sha = [Security.Cryptography.SHA256]::Create()
  return ([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($text))) -replace '-', '').ToLowerInvariant()
}

# What the built libraries depend on: every pin above plus the build files. If any of it changes, rebuild.
$wrapperCmake = Join-Path $PSScriptRoot 'sdl3-static\CMakeLists.txt'
$bundledCmake = Join-Path $PSScriptRoot 'cmake\SDL3Bundled.cmake'
$stampText = (($deps | ForEach-Object { $_.Sha256 }) -join ',') + "|$freetypeCommit|" +
             (Get-Content $wrapperCmake -Raw) + '|' + (Get-Content $bundledCmake -Raw) + '|static-crt-v1'
$stamp     = Get-StringSha256 $stampText
$stampFile = Join-Path $outDir '.da-deps-stamp'

# Release is what the packages use; Debug is for developing in Visual Studio. -ReleaseOnly skips the Debug libraries.
$configs = if ($ReleaseOnly) { @('Release') } else { @('Release', 'Debug') }
$expectedLibs = 'SDL3-static.lib', 'SDL3_ttf-static.lib', 'SDL3_mixer-static.lib', 'freetype.lib'
function Test-Built {
  if (-not (Test-Path $stampFile) -or ((Get-Content $stampFile -Raw).Trim() -ne $stamp)) { return $false }
  foreach ($cfg in $configs) {
    foreach ($lib in $expectedLibs) { if (-not (Test-Path (Join-Path $outDir "lib\$cfg\$lib"))) { return $false } }
  }
  return (Test-Path (Join-Path $outDir 'include\SDL3\SDL.h'))
}

if (-not $Force -and (Test-Built)) {
  Write-Host 'SDL3 libraries are up to date in third_party/sdl3/.'
  return
}

# Building SDL creates files with long names several folders deep, and MSBuild (which CMake drives here) cannot open a
# path longer than 260 characters. From a deep folder that fails with confusing "could not find a part of the path"
# errors halfway through, so say so up front.
if ($repoRoot.Length -gt 90) {
  throw ("This folder's path is $($repoRoot.Length) characters long ($repoRoot). Building the SDL libraries needs paths " +
         "under 260 characters, so please clone or move the repository somewhere with a shorter path (for example C:\dev\Dark-Ages).")
}

# ---- 1. sources ---------------------------------------------------------------------------------------------
New-Item -ItemType Directory -Force -Path $downloads, $srcRoot | Out-Null

foreach ($d in $deps) {
  $archive = Join-Path $downloads $d.File
  $target  = Join-Path $srcRoot $d.Folder

  # use the cached download if it is intact, otherwise fetch it (a few retries for flaky networks)
  if (-not (Test-Path $archive) -or (Get-Sha256 $archive) -ne $d.Sha256) {
    $ok = $false
    for ($attempt = 1; $attempt -le 3 -and -not $ok; $attempt++) {
      Write-Host ("{0,-18} downloading (attempt {1}) {2}" -f $d.Name, $attempt, $d.Url)
      try {
        Invoke-WebRequest -Uri $d.Url -OutFile $archive -UseBasicParsing
        $ok = ((Get-Sha256 $archive) -eq $d.Sha256)
        if (-not $ok) { Write-Warning "Checksum mismatch for $($d.File)." }
      } catch {
        Write-Warning "Download failed: $($_.Exception.Message)"
      }
      if (-not $ok -and $attempt -lt 3) { Start-Sleep -Seconds (2 * $attempt) }
    }
    if (-not $ok) {
      if (Test-Path $archive) { Remove-Item $archive -Force }
      throw "Could not obtain a verified copy of $($d.File). Expected SHA-256 $($d.Sha256)."
    }
  }

  # Unpack once per verified archive. Windows' tar cannot create the macOS-framework symlinks inside these archives
  # (it says so and returns an error code), which is harmless here, so the result is checked by looking for the files
  # that matter instead of trusting the exit code.
  $marker = Join-Path $target '.da-deps-sha256'
  if ($Force -or -not (Test-Path $marker) -or ((Get-Content $marker -Raw).Trim() -ne $d.Sha256)) {
    if (Test-Path $target) { Remove-Item $target -Recurse -Force }
    $saved = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    & tar -xzf $archive -C $srcRoot 2>$null
    $ErrorActionPreference = $saved
    if (-not (Test-Path (Join-Path $target 'CMakeLists.txt'))) { throw "$($d.File) did not unpack a usable '$($d.Folder)' folder." }
    Set-Content -Path $marker -Value $d.Sha256 -Encoding ASCII
  }
  Write-Host ("{0,-18} ready in {1}" -f $d.Name, $target)
}

# FreeType: fetched by exact commit (content-addressed, so it cannot silently change)
$ftDir = Join-Path $srcRoot "$ttfFolder\external\freetype"
function Get-GitHead([string]$dir) {
  if (-not (Test-Path (Join-Path $dir '.git'))) { return '' }
  $saved = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
  $head = (& git -C $dir rev-parse HEAD 2>$null)
  $ErrorActionPreference = $saved
  if ($head) { return "$head".Trim() } else { return '' }
}
if ((Get-GitHead $ftDir) -ne $freetypeCommit) {
  Write-Host "FreeType           fetching commit $freetypeCommit"
  if (Test-Path $ftDir) { Remove-Item $ftDir -Recurse -Force }
  New-Item -ItemType Directory -Force -Path $ftDir | Out-Null
  & git -C $ftDir init -q
  & git -C $ftDir remote add origin $freetypeUrl
  & git -C $ftDir fetch -q --depth 1 origin $freetypeCommit
  if ($LASTEXITCODE -ne 0) { throw "Could not fetch FreeType commit $freetypeCommit from $freetypeUrl." }
  & git -C $ftDir checkout -q FETCH_HEAD
  if ((Get-GitHead $ftDir) -ne $freetypeCommit) { throw "FreeType checkout is not at the pinned commit $freetypeCommit." }
}
Write-Host "FreeType           ready at commit $freetypeCommit"

# ---- 2. build -----------------------------------------------------------------------------------------------
$cmake = (Get-Command cmake -ErrorAction SilentlyContinue).Source
if (-not $cmake) {
  $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
  if (Test-Path $vswhere) {
    $cmake = & $vswhere -latest -products * -find 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' | Select-Object -First 1
  }
}
if (-not $cmake) { throw 'CMake was not found. Install CMake, or Visual Studio 2022 with "Desktop development with C++".' }

# The build folder is kept between runs (CMake rebuilds only what changed); -Force starts from scratch.
if ($Force -and (Test-Path $buildDir)) { Remove-Item $buildDir -Recurse -Force }
Write-Host "Configuring the SDL3 build with $cmake"
& $cmake -S (Join-Path $PSScriptRoot 'sdl3-static') -B $buildDir -G 'Visual Studio 17 2022' -A x64 `
         "-DDA_SDL_SOURCE_DIR=$srcRoot" '-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>'
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed (exit code $LASTEXITCODE)." }

if (Test-Path $outDir) { Remove-Item $outDir -Recurse -Force }
foreach ($cfg in $configs) {
  Write-Host "Building SDL3, SDL3_ttf and SDL3_mixer ($cfg) - a few minutes..."
  & $cmake --build $buildDir --config $cfg --target da_sdl3_libs --parallel
  if ($LASTEXITCODE -ne 0) { throw "CMake build ($cfg) failed (exit code $LASTEXITCODE)." }

  # collect the finished libraries under fixed names (a Debug build may add a "d" postfix)
  $libDir = Join-Path $outDir "lib\$cfg"
  New-Item -ItemType Directory -Force -Path $libDir | Out-Null
  $found = Get-ChildItem $buildDir -Recurse -File -Filter *.lib | Where-Object {
    $_.Directory.Name -eq $cfg -and $_.Name -match '^(SDL3-static|SDL3_ttf-static|SDL3_mixer-static|freetype)d?\.lib$'
  }
  foreach ($f in $found) {
    $fixed = ($f.Name -replace 'd\.lib$', '.lib')
    if ($f.Name -match '^freetype') { $fixed = 'freetype.lib' }
    Copy-Item $f.FullName (Join-Path $libDir $fixed) -Force
  }
  foreach ($lib in $expectedLibs) {
    if (-not (Test-Path (Join-Path $libDir $lib))) { throw "The $cfg build did not produce $lib." }
  }
}

# ---- 3. headers ---------------------------------------------------------------------------------------------
$incDir = Join-Path $outDir 'include'
New-Item -ItemType Directory -Force -Path $incDir | Out-Null
Copy-Item (Join-Path $srcRoot 'SDL3-3.4.16\include\SDL3')             (Join-Path $incDir 'SDL3')       -Recurse -Force
Copy-Item (Join-Path $srcRoot "$ttfFolder\include\SDL3_ttf")          (Join-Path $incDir 'SDL3_ttf')   -Recurse -Force
Copy-Item (Join-Path $srcRoot 'SDL3_mixer-3.2.4\include\SDL3_mixer')  (Join-Path $incDir 'SDL3_mixer') -Recurse -Force

Set-Content -Path $stampFile -Value $stamp -Encoding ASCII
Write-Host 'SDL3 libraries are ready in third_party/sdl3/.'
