# Creates SFWaterfrontMVP via editor Python, then optionally opens the editor.
param(
    [string]$EngineRoot = "C:\Program Files\Epic Games\UE_5.8",
    [switch]$OpenEditor
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$UProject = Join-Path $ProjectRoot "unreal\SFRouteRacer\SFRouteRacer.uproject"
$PythonScript = Join-Path $ProjectRoot "unreal\SFRouteRacer\Content\Python\bootstrap_playable.py"
$EditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$Editor = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"

if (-not (Test-Path $UProject)) { throw "Missing $UProject" }
if (-not (Test-Path $PythonScript)) { throw "Missing $PythonScript" }
if (-not (Test-Path $EditorCmd)) { throw "Missing UnrealEditor-Cmd at $EditorCmd" }

Write-Host "Bootstrapping playable map via UnrealEditor-Cmd..."
& $EditorCmd $UProject -ExecutePythonScript="$PythonScript" -unattended -nop4 -nosplash -log
if ($LASTEXITCODE -ne 0) {
    throw "Map bootstrap failed with exit code $LASTEXITCODE"
}

$mapAsset = Join-Path $ProjectRoot "unreal\SFRouteRacer\Content\Maps\SFWaterfrontMVP.umap"
if (-not (Test-Path $mapAsset)) {
    Write-Host "Warning: $mapAsset not found after bootstrap (check editor log)."
} else {
    Write-Host "Map ready: $mapAsset"
}

if ($OpenEditor) {
    Write-Host "Opening Unreal Editor..."
    Start-Process -FilePath $Editor -ArgumentList "`"$UProject`""
}
