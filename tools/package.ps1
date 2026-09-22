<#
.SYNOPSIS
  Builds the game and packs it into a distributable zip: dist/DarkAges-<version>-win64.zip

.DESCRIPTION
  Used both locally and by the GitHub Actions workflows, so a zip you make on your machine is
  put together exactly like the ones CI publishes.

  The zip holds a single folder, DarkAges/, containing only what is needed to play:

    Darkages.exe (SDL is linked into it: no DLLs)
    Font/  Gfx/  Maps/  Music/  Mods/     (every mod in game/Mods)
    darkages.cfg                           (default settings, with -DefaultMod selected)
    README.txt  LICENSE  THIRD-PARTY.txt

  It never contains source code, object files, symbol (.pdb) files, Visual Studio files or save
  games. Only the fixed list above is copied - not "everything in game/" - so files that building
  or playing from game/ leaves behind cannot leak in; in particular the darkages.cfg in the zip is
  generated fresh (by Darkages.exe --write-default-config), never copied from game/. Art-source
  files (.xcf, .psd, .wav, .mid ...) are left out even when they sit inside a mod's folder.

  Note: mods that exist only on your machine (game/Mods/Test, Test2) are inside game/Mods, so a
  zip made locally includes them. CI builds from a clean checkout and does not have them.

.PARAMETER VersionSuffix
  Appended to the base version from src/Version.h, e.g. "-dev.47+a1b2c3d". Leave empty for a
  release build.

.PARAMETER DefaultMod
  The mod selected in the zip's darkages.cfg, so it is the first thing players see. It must exist
  in game/Mods. Default: Project32

.PARAMETER OutputDir
  Where the zip goes. Default: <repo>/dist

.PARAMETER SkipBuild
  Package whatever is already built in game/ (still requires third_party/ for the notices).

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\package.ps1

.EXAMPLE
  # A development version: run it from a PowerShell prompt. ("powershell -File ..." mangles a value that
  # starts with "-", so don't pass a suffix that way.)
  .\tools\package.ps1 -VersionSuffix "-dev.47+a1b2c3d"
#>
[CmdletBinding()]
param(
  [string]$VersionSuffix = '',
  [string]$DefaultMod = 'Project32',
  [string]$OutputDir = '',
  [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
if (-not $OutputDir) { $OutputDir = Join-Path $repo 'dist' }

# ---- version ---------------------------------------------------------------------------------
$versionH = Get-Content (Join-Path $repo 'src\Version.h') -Raw
function Get-VersionDefine([string]$name) {
  if ($versionH -match ('(?m)^\s*#define\s+' + $name + '\s+"([^"]*)"')) { return $Matches[1] }
  throw "Could not find $name in src/Version.h"
}
$base      = Get-VersionDefine 'DA_VERSION_BASE'
$copyright = (Get-VersionDefine 'DA_COPYRIGHT_YEARS') + ', ' + (Get-VersionDefine 'DA_COPYRIGHT_OWNER')
$version   = $base + $VersionSuffix
if ($version.Length -gt 30) { throw "Version '$version' is longer than 30 characters and won't fit on the credits line." }

# The file name carries the version and dev number only: the build metadata after '+' (the commit id) stays out of it,
# though it is still in the version shown in the game.
$zipName = 'DarkAges-' + ($version -replace '\+.*$', '') + '-win64.zip'
Write-Host "Packaging Dark Ages $version  ->  $zipName"

# ---- build -----------------------------------------------------------------------------------
function Find-MSBuild {
  $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
  if (Test-Path $vswhere) {
    $found = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
                        -find 'MSBuild\**\Bin\amd64\MSBuild.exe' | Select-Object -First 1
    if ($found) { return $found }
  }
  $cmd = Get-Command msbuild.exe -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }
  throw 'MSBuild with the C++ (v143) tools was not found. Install Visual Studio 2022 with "Desktop development with C++".'
}

if (-not $SkipBuild) {
  & (Join-Path $PSScriptRoot 'get-deps.ps1') -ReleaseOnly   # packaging only needs the Release libraries
  $msbuild = Find-MSBuild
  $msbuildArgs = @((Join-Path $repo 'msvc\Darkages.sln'), '/p:Configuration=Release', '/p:Platform=x64', '/m', '/nr:false', '/nologo', '/v:m')   # /nr:false: no idle worker processes left holding the folder
  if ($VersionSuffix) { $msbuildArgs += "/p:DAVersionSuffix=$VersionSuffix" }
  Write-Host "Building with $msbuild"
  & $msbuild @msbuildArgs
  if ($LASTEXITCODE -ne 0) { throw "MSBuild failed (exit code $LASTEXITCODE)." }
}

# ---- stage exactly what ships ----------------------------------------------------------------
$game       = Join-Path $repo 'game'
$thirdParty = Join-Path $repo 'third_party'
$stageRoot  = Join-Path $repo 'build\package'
$stage      = Join-Path $stageRoot 'DarkAges'
if (Test-Path $stageRoot) { Remove-Item $stageRoot -Recurse -Force }
New-Item -ItemType Directory -Force -Path $stage | Out-Null

$runtimeFiles = 'Darkages.exe' #a single executable: SDL, SDL_ttf and SDL_mixer are statically linked into it
foreach ($f in $runtimeFiles) {
  $src = Join-Path $game $f
  if (-not (Test-Path $src)) { throw "$f is missing from game/ - the build did not produce it." }
  Copy-Item $src $stage
}

# Art / audio source files and OS junk are never shipped, even inside a mod's folder.
$denyExtensions = '.xcf', '.psd', '.kra', '.aseprite', '.blend', '.sfd', '.wav', '.mid', '.midi', '.pdf'
$denyNames      = 'Thumbs.db', 'desktop.ini', '.DS_Store'
$assetFolders   = 'Font', 'Gfx', 'Maps', 'Music', 'Mods'
foreach ($dir in $assetFolders) {
  $srcDir = Join-Path $game $dir
  if (-not (Test-Path $srcDir)) { throw "game/$dir is missing." }
  $srcRoot = (Resolve-Path $srcDir).Path.TrimEnd('\')
  Get-ChildItem $srcDir -Recurse -File | Where-Object {
    ($denyExtensions -notcontains $_.Extension.ToLowerInvariant()) -and ($denyNames -notcontains $_.Name)
  } | ForEach-Object {
    $target = Join-Path (Join-Path $stage $dir) $_.FullName.Substring($srcRoot.Length + 1)
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
    Copy-Item $_.FullName $target
  }
}

# darkages.cfg: every setting at its default, except the mod, so the game opens with it on first run. The game writes
# the file itself (it owns the struct's layout and the default values); it opens no window and exits straight away.
if (-not (Test-Path (Join-Path $stage "Mods\$DefaultMod"))) { throw "The default mod '$DefaultMod' is not in game/Mods." }
$cfgPath = Join-Path $stage 'darkages.cfg'
$cfgRun = Start-Process -FilePath (Join-Path $stage 'Darkages.exe') -Wait -PassThru `
            -ArgumentList @('--write-default-config', ('"' + $cfgPath + '"'), $DefaultMod)
if ($cfgRun.ExitCode -ne 0 -or -not (Test-Path $cfgPath)) { throw "Darkages.exe --write-default-config failed (exit code $($cfgRun.ExitCode))." }
$cfgBytes = [IO.File]::ReadAllBytes($cfgPath)
if ($cfgBytes.Length -lt 46 -or [Text.Encoding]::ASCII.GetString($cfgBytes, 14, 32).TrimEnd([char]0) -ne $DefaultMod) {
  throw "darkages.cfg was written but does not select '$DefaultMod' (the layout of sConf in src/Structs.h may have changed)."
}

# Text files: written with Windows line endings so they open cleanly in Notepad.
function Write-TextFile([string]$path, [string]$text) {
  [IO.File]::WriteAllText($path, ($text -replace "`r?`n", "`r`n"), (New-Object Text.UTF8Encoding($false)))
}

Copy-Item (Join-Path $repo 'LICENSE') (Join-Path $stage 'LICENSE')

$devNote = ''
if ($VersionSuffix -match 'dev') {
  $devNote = @"

  *** DEVELOPMENT BUILD ***
  This is an early, work-in-progress build made automatically from the latest
  development code. Expect bugs, and features may be incomplete or change
  without notice. Please keep a backup of your save games.

"@
}
$readme = (Get-Content (Join-Path $repo 'tools\game-readme.txt') -Raw).
  Replace('{{VERSION}}', $version).Replace('{{COPYRIGHT}}', $copyright).Replace('{{DEV_NOTE}}', $devNote)
Write-TextFile (Join-Path $stage 'README.txt') $readme

# Third-party notices are assembled from the license files that came with the libraries we ship.
$sdlSrc = Join-Path $thirdParty 'sdl3-src'   # the source trees tools\get-deps.ps1 built the libraries from
$libs = @(
  @{ Name = 'SDL3 3.4.16';       Site = 'https://www.libsdl.org/';                       Files = @('SDL3-3.4.16\LICENSE.txt'); Note = '' },
  @{ Name = 'SDL3_ttf 3.2.2';    Site = 'https://github.com/libsdl-org/SDL_ttf';          Files = @('SDL3_ttf-3.2.2\LICENSE.txt');
     Note = 'SDL3_ttf includes FreeType; its license follows.' },
  @{ Name = 'FreeType';          Site = 'https://www.freetype.org/';                      Files = @('SDL3_ttf-3.2.2\external\freetype\LICENSE.TXT', 'SDL3_ttf-3.2.2\external\freetype\docs\FTL.TXT');
     Note = 'Portions of this software are copyright (c) The FreeType Project (www.freetype.org). All rights reserved.' },
  @{ Name = 'SDL3_mixer 3.2.4';  Site = 'https://github.com/libsdl-org/SDL_mixer';        Files = @('SDL3_mixer-3.2.4\LICENSE.txt');
     Note = 'SDL3_mixer includes the stb_vorbis decoder for Ogg Vorbis, released by its author under public-domain or permissive terms.' }
)
$notice = New-Object Text.StringBuilder
[void]$notice.AppendLine('THIRD-PARTY SOFTWARE')
[void]$notice.AppendLine('====================')
[void]$notice.AppendLine('Dark Ages is built with the following third-party libraries, compiled into the program.')
foreach ($lib in $libs) {
  [void]$notice.AppendLine('')
  [void]$notice.AppendLine('------------------------------------------------------------------------------')
  [void]$notice.AppendLine($lib.Name + '  -  ' + $lib.Site)
  if ($lib.Note) { [void]$notice.AppendLine($lib.Note) }
  [void]$notice.AppendLine('------------------------------------------------------------------------------')
  foreach ($rel in $lib.Files) {
    $p = Join-Path $sdlSrc $rel
    if (-not (Test-Path $p)) { throw "License file $rel not found in third_party/sdl3-src/ - run tools\get-deps.ps1." }
    [void]$notice.AppendLine((Get-Content $p -Raw).TrimEnd())
  }
}
Write-TextFile (Join-Path $stage 'THIRD-PARTY.txt') $notice.ToString()

# ---- self-check: refuse to produce a zip that is missing something or contains something it must not
$required = @('Darkages.exe', 'darkages.cfg', 'README.txt', 'LICENSE', 'THIRD-PARTY.txt', 'Font\DA1qb.ttf', 'Maps\Aaryak.map') +
            ($runtimeFiles | Where-Object { $_ -ne 'Darkages.exe' }) +
            @('DA1TilesL', 'DA1HeroL', 'DA1ExtraL', 'DA1MonstL', 'death', 'daend1', 'daend2', 'explode', 'datitle' | ForEach-Object { "Gfx\$_.bmp" }) +
            @('battle', 'dungeon', 'title', 'town' | ForEach-Object { "Music\$_.ogg" })
foreach ($r in $required) {
  if (-not (Test-Path (Join-Path $stage $r))) { throw "Package check failed: $r is missing from the package." }
}
$forbidden = Get-ChildItem $stage -Recurse -Force | Where-Object {
  $_.Name -match '\.(dll|pdb|obj|ilk|iobj|ipdb|exp|lib|recipe|tlog|log|cpp|h|vcxproj|sln|xcf|psd|wav|mid|pdf)$' -or
  ($_.Name -ieq 'darkages.cfg' -and $_.DirectoryName -ne $stage) -or   # only the generated one at the top is allowed
  $_.Name -ieq 'Saves' -or $_.Name -ieq 'Darkages_d.exe'
}
if ($forbidden) { throw ("Package check failed: unexpected files in the package: " + (($forbidden | ForEach-Object { $_.FullName.Substring($stage.Length + 1) }) -join ', ')) }

# ---- zip (forward-slash entry names, sorted, so the archive opens correctly everywhere) ------
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
$zipPath = Join-Path (Resolve-Path $OutputDir).Path $zipName
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::Open($zipPath, [IO.Compression.ZipArchiveMode]::Create)
try {
  $stageFull = (Resolve-Path $stage).Path
  $parent    = Split-Path -Parent $stageFull
  Get-ChildItem $stageFull -Recurse -File | Sort-Object FullName | ForEach-Object {
    $entry = $_.FullName.Substring($parent.Length + 1).Replace('\', '/')     # "DarkAges/Gfx/..."
    [void][IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive, $_.FullName, $entry, [IO.Compression.CompressionLevel]::Optimal)
  }
} finally { $archive.Dispose() }

$fileCount = (Get-ChildItem $stageFull -Recurse -File).Count
Write-Host ''
Write-Host ("Created {0}  ({1:N1} MB, {2} files)" -f $zipPath, ((Get-Item $zipPath).Length / 1MB), $fileCount)
# GitHub computes and shows its own SHA-256 digest for each release asset, generated from the bytes it
# actually stored - checking a download against that is at least as good as checking it against a
# checksum file we publish ourselves from the same build, and does not have a downloaded-file-verifies-
# itself problem. So no separate .sha256 file: see the discussion recorded in docs/releasing.md.
Write-Host ("SHA-256  {0}" -f (Get-FileHash $zipPath -Algorithm SHA256).Hash.ToLowerInvariant())

# Hand the results to a GitHub Actions workflow, if that is what is running us.
if ($env:GITHUB_OUTPUT) {
  Add-Content -Path $env:GITHUB_OUTPUT -Value "version=$version"
  Add-Content -Path $env:GITHUB_OUTPUT -Value "zip_name=$zipName"
  Add-Content -Path $env:GITHUB_OUTPUT -Value "zip_path=$zipPath"
}
