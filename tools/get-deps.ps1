<#
.SYNOPSIS
  Downloads the SDL libraries the game builds against into third_party/.

.DESCRIPTION
  Fetches the pinned SDL2, SDL2_ttf and SDL2_mixer "devel VC" packages, verifies each
  download against a SHA-256 checksum, and unpacks them under third_party/ (git-ignored):

    third_party/SDL2-2.0.12/
    third_party/SDL2_ttf-2.0.12/
    third_party/SDL2_mixer-2.8.1/

  msvc/Darkages.vcxproj looks for the libraries there. Run this once after cloning, and again
  whenever the pinned versions below change. It is safe to re-run: packages that are already
  unpacked and verified are left alone.

  The same script is used by the GitHub Actions workflows, so a local build and a CI build get
  exactly the same library files.

.PARAMETER Force
  Download and unpack everything again, even if it looks up to date.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\get-deps.ps1
#>
[CmdletBinding()]
param(
  [switch]$Force
)

$ErrorActionPreference = 'Stop'
$ProgressPreference    = 'SilentlyContinue'   # Invoke-WebRequest is very slow with the progress bar on Windows PowerShell 5.1
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$repoRoot   = Split-Path -Parent $PSScriptRoot
$thirdParty = Join-Path $repoRoot 'third_party'
$downloads  = Join-Path $thirdParty '_downloads'

# To move to a newer version: change the entry here (url, sha256, folder) and re-run.
# SDL2_ttf 2.0.12 was only ever published on libsdl.org, not on GitHub releases.
$deps = @(
  @{
    Name   = 'SDL2 2.0.12'
    File   = 'SDL2-devel-2.0.12-VC.zip'
    Url    = 'https://github.com/libsdl-org/SDL/releases/download/release-2.0.12/SDL2-devel-2.0.12-VC.zip'
    Sha256 = '00c55a597cebdb9a4eb2723f2ad2387a4d7fd605e222c69b46099b15d5d8b32d'
    Folder = 'SDL2-2.0.12'
  },
  @{
    Name   = 'SDL2_ttf 2.0.12'
    File   = 'SDL2_ttf-devel-2.0.12-VC.zip'
    Url    = 'https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.12-VC.zip'
    Sha256 = '0c74634a8e74f0a909194962ce9512e638e66d7fc71677a9273fd6a34e627628'
    Folder = 'SDL2_ttf-2.0.12'
  },
  @{
    Name   = 'SDL2_mixer 2.8.1'
    File   = 'SDL2_mixer-devel-2.8.1-VC.zip'
    Url    = 'https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-VC.zip'
    Sha256 = '12dc2bb724afaf19bcc23fdd7e6fcdcf40274edf13b8c6ec3723089a72537832'
    Folder = 'SDL2_mixer-2.8.1'
  }
)

function Get-Sha256([string]$path) {
  return (Get-FileHash -Path $path -Algorithm SHA256).Hash.ToLowerInvariant()
}

New-Item -ItemType Directory -Force -Path $downloads | Out-Null

foreach ($d in $deps) {
  $zip    = Join-Path $downloads $d.File
  $target = Join-Path $thirdParty $d.Folder
  $marker = Join-Path $target '.da-deps-sha256'   # records which package the unpacked folder came from

  if (-not $Force -and (Test-Path $marker) -and ((Get-Content $marker -Raw).Trim() -eq $d.Sha256)) {
    Write-Host ("{0,-18} up to date" -f $d.Name)
    continue
  }

  # Use the cached download if it is intact, otherwise fetch it (a few retries for flaky networks).
  if (-not (Test-Path $zip) -or (Get-Sha256 $zip) -ne $d.Sha256) {
    $ok = $false
    for ($attempt = 1; $attempt -le 3 -and -not $ok; $attempt++) {
      Write-Host ("{0,-18} downloading (attempt {1}) {2}" -f $d.Name, $attempt, $d.Url)
      try {
        Invoke-WebRequest -Uri $d.Url -OutFile $zip -UseBasicParsing
        $ok = ((Get-Sha256 $zip) -eq $d.Sha256)
        if (-not $ok) { Write-Warning "Checksum mismatch for $($d.File)." }
      } catch {
        Write-Warning "Download failed: $($_.Exception.Message)"
      }
      if (-not $ok -and $attempt -lt 3) { Start-Sleep -Seconds (2 * $attempt) }
    }
    if (-not $ok) {
      if (Test-Path $zip) { Remove-Item $zip -Force }
      throw "Could not obtain a verified copy of $($d.File). Expected SHA-256 $($d.Sha256)."
    }
  }

  if (Test-Path $target) { Remove-Item $target -Recurse -Force }
  Expand-Archive -Path $zip -DestinationPath $thirdParty -Force
  if (-not (Test-Path $target)) { throw "$($d.File) did not contain the expected folder '$($d.Folder)'." }
  Set-Content -Path $marker -Value $d.Sha256 -Encoding ASCII
  Write-Host ("{0,-18} ready in {1}" -f $d.Name, $target)
}

Write-Host 'Dependencies are ready in third_party/.'
