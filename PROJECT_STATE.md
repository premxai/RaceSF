# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestones 1–5

- Complete (source) — Data pipeline through presentation hooks
- Complete — UE 5.8 `SFRouteRacerEditor` Development Win64 compile on this machine

## Post-milestone playability pass

- Complete — `scripts/setup_unreal.ps1` (finds UE 5.8, generates project files, builds)
- Complete — `scripts/sync_map_export.ps1` (copies export into Content/Maps/sf_mvp)
- Complete — `scripts/bootstrap_playable.ps1` + `Content/Python/bootstrap_playable.py`
- Complete — Arcade `ASFVehiclePawn` with runtime Enhanced Input (no Chaos BP required)
- Complete — Startup map `/Game/Maps/SFWaterfrontMVP`
- Complete — Ghost opponent, boundary barriers, debug draw
- Not started — Chaos vehicle Blueprint polish / packaged Development build
- Not started — Confirmed human PIE of flagship race (open editor and press Play)

## Verification log

- Toolchain: VS Build Tools 2026, MSVC 14.51.36231, Windows SDK 10.0.22621.0
- `Build.bat SFRouteRacerEditor Win64 Development` → Result: Succeeded (arcade vehicle rewrite)
- `bootstrap_playable.ps1` → created `Content/Maps/SFWaterfrontMVP.umap`
- Remote: https://github.com/premxai/RaceSF.git

## Exact next step

Open the project and PIE `SFWaterfrontMVP`:

```powershell
.\scripts\bootstrap_playable.ps1 -OpenEditor
```

Controls: W throttle, S brake, A/D steer, Space handbrake, R reset, C camera.
