# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestones 1–5

- Complete (source) — Data pipeline through presentation hooks
- Blocked — UE 5.6 compile / PIE / package on this machine

## Post-milestone playability pass

- Complete — `scripts/setup_unreal.ps1` (finds UE 5.6+, generates project files, builds)
- Complete — `scripts/sync_map_export.ps1` (copies export into Content/Maps/sf_mvp)
- Complete — Editor Python validator `Content/Python/validate_sf_export.py`
- Complete — Ghost opponent along an alternate suggested route
- Complete — MVP boundary barrier generator from map extents
- Complete — Optional debug draw for graph/landmarks/tiles/destination radius
- Blocked — Live compile still requires installing Unreal Engine 5.6

## Verification log

- `scripts/setup_unreal.ps1` expected to exit 2 here until UE 5.6 is installed
- Unreal compilation remains unverified
- Remote: https://github.com/premxai/RaceSF.git

## Exact next step

Install UE 5.6, run `.\scripts\sync_map_export.ps1` then `.\scripts\setup_unreal.ps1`,
open the project, create Chaos vehicle + Enhanced Input assets, and play Ferry Building
→ Chase Center against the scenic ghost.
