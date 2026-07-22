# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestone: Playable graybox (2026-07-22)

**Status: Complete** — Ferry Building graybox is driveable in PIE.

Confirmed working:
- Map export loads (3944 roads / 6 landmarks / 26 races)
- Roads + buildings spawn; race reaches Racing with scenic ghost
- Player spawns at Ferry Building with chase camera
- Unlit gray city + orange car are visible without lightmap rebuild
- Arcade controls (WASD / Space / R / C) feel usable; camera pitch locked

Key fixes that got us here:
- Dynamic lighting + auto-exposure (no black unbuilt void)
- Remove ProceduralMesh `SectionIndex + 100000` densification
- Unlit materials: `M_SFGrayboxUnlit` (city) / `M_SFCarUnlit` (player)
- Locked chase pitch; mouse yaw peek only; smoother ground follow + steering

## Exact next step
Polish destination race UX (HUD, route choice, clearer landmarks) and keep iterating vehicle feel.
