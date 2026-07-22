# Finds Unreal Engine 5.6+, generates project files, and builds the editor target.
param(
    [string]$EngineRoot = "",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$UProject = Join-Path $ProjectRoot "unreal\SFRouteRacer\SFRouteRacer.uproject"

if (-not (Test-Path $UProject)) {
    throw "Missing project file: $UProject"
}

function Find-UE56 {
    param([string]$Hint)
    if ($Hint -and (Test-Path (Join-Path $Hint "Engine\Build\BatchFiles\Build.bat"))) {
        return (Resolve-Path $Hint).Path
    }

    $candidates = @(
        "C:\Program Files\Epic Games\UE_5.6",
        "C:\Program Files\Epic Games\UE_5.7",
        "D:\Program Files\Epic Games\UE_5.6",
        "D:\UE_5.6"
    )
    foreach ($path in $candidates) {
        $drive = Split-Path -Qualifier $path -ErrorAction SilentlyContinue
        if ($drive -and -not (Test-Path "$drive\")) {
            continue
        }
        if (Test-Path (Join-Path $path "Engine\Build\BatchFiles\Build.bat")) {
            return $path
        }
    }

    $launcher = "$env:ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat"
    if (Test-Path $launcher) {
        $json = Get-Content $launcher -Raw | ConvertFrom-Json
        foreach ($item in $json.InstallationList) {
            $loc = $item.InstallLocation
            if ($loc -match "UE_5\.[6-9]" -and (Test-Path (Join-Path $loc "Engine\Build\BatchFiles\Build.bat"))) {
                return $loc
            }
        }
    }
    return $null
}

$Engine = Find-UE56 -Hint $EngineRoot
if (-not $Engine) {
    Write-Host "Unreal Engine 5.6+ was not found."
    Write-Host "Install UE 5.6 from the Epic Games Launcher, then rerun:"
    Write-Host "  .\scripts\setup_unreal.ps1"
    Write-Host "Or pass -EngineRoot 'C:\Path\To\UE_5.6'"
    exit 2
}

Write-Host "Using engine: $Engine"
$BuildBat = Join-Path $Engine "Engine\Build\BatchFiles\Build.bat"
$UBT = Join-Path $Engine "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

Write-Host "Generating Visual Studio project files..."
& $UBT -projectfiles -project="$UProject" -game -rocket -progress
if ($LASTEXITCODE -ne 0) {
    throw "Project file generation failed with exit code $LASTEXITCODE"
}

if ($SkipBuild) {
    Write-Host "SkipBuild set; project files generated only."
    exit 0
}

Write-Host "Building SFRouteRacerEditor (Development Win64)..."
& $BuildBat SFRouteRacerEditor Win64 Development -Project="$UProject" -WaitMutex
if ($LASTEXITCODE -ne 0) {
    throw "Unreal build failed with exit code $LASTEXITCODE"
}

Write-Host "Build finished. Open the .uproject or run:"
Write-Host "  & '$Engine\Engine\Binaries\Win64\UnrealEditor.exe' '$UProject'"
