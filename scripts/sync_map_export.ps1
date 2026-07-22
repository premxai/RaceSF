# Copies the offline map export into the Unreal Content tree for packaged/playable use.
param(
    [switch]$Junction
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$Source = Join-Path $ProjectRoot "data\exports\sf_mvp"
$Dest = Join-Path $ProjectRoot "unreal\SFRouteRacer\Content\Maps\sf_mvp"

if (-not (Test-Path (Join-Path $Source "manifest.json"))) {
    throw "Missing export at $Source. Run: python -m sf_racer_geo.cli build-all"
}

New-Item -ItemType Directory -Force -Path (Split-Path $Dest) | Out-Null
if (Test-Path $Dest) {
    Remove-Item -Recurse -Force $Dest
}

if ($Junction) {
    cmd /c mklink /J "$Dest" "$Source" | Out-Host
} else {
    Copy-Item -Recurse -Force $Source $Dest
}

Write-Host "Map export available at: $Dest"
Write-Host "USFMapDataSubsystem will also still try ../../data/exports/sf_mvp from the project dir."
