# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestones 1–5

- Complete (source) — Data pipeline through presentation hooks
- Complete — UE 5.8 `SFRouteRacerEditor` Development Win64 compile on this machine

## Post-milestone playability pass

- Complete — `scripts/setup_unreal.ps1` (finds UE 5.8, generates project files, builds)
- Complete — `scripts/sync_map_export.ps1` (copies export into Content/Maps/sf_mvp)
- Complete — Editor Python validator `Content/Python/validate_sf_export.py`
- Complete — Ghost opponent along an alternate suggested route
- Complete — MVP boundary barrier generator from map extents
- Complete — Optional debug draw for graph/landmarks/tiles/destination radius
- Complete — Retargeted to EngineAssociation 5.8; BuildSettingsVersion V7
- Not started — Chaos vehicle Blueprint, Enhanced Input assets, `SFWaterfrontMVP` map
- Not started — PIE flagship race / packaged build

## Verification log

- Toolchain: VS Build Tools 2026, MSVC 14.51.36231, Windows SDK 10.0.22621.0
- `Build.bat SFRouteRacerEditor Win64 Development` → Result: Succeeded (2026-07-22)
- Map export synced locally under `Content/Maps/sf_mvp` (gitignored)
- Remote: https://github.com/premxai/RaceSF.git

## Exact next step

Open `unreal/SFRouteRacer/SFRouteRacer.uproject` in UE 5.8, create Chaos vehicle +
Enhanced Input assets, author `SFWaterfrontMVP`, and play Ferry Building → Chase Center
against the scenic ghost.
