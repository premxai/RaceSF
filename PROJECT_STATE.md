# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Diagnosis (2026-07-22 evening)

Gameplay bootstrap is healthy. Latest PIE log showed:
- map export loaded (3944 roads / 6 landmarks / 26 races)
- roads + buildings built
- race `ferry_building_to_chase_center` reached Racing
- scenic ghost started

What looked “broken” was the viewport:
1. Empty map with unbuilt/static lighting → black screen + “LIGHTING NEEDS TO BE REBUILT”
2. `r.DefaultFeature.AutoExposure=False` → crushed exposure / black scene
3. City graybox only exists after Play (spawned in GameMode), ~1.5 km from origin
4. Double road/building build slowed first PIE to ~35s of black loading

## Fixes applied
- Dynamic lighting only (`r.AllowStaticLighting=False`, Force No Precomputed Lighting)
- AutoExposure re-enabled + unbound post-process exposure boost
- Brighter BasicShape materials on roads/buildings/vehicle
- Disable BeginPlay auto-rebuild (GameMode builds once)

## Exact next step
Press Play, wait for bootstrap (~10–20s after fix), confirm yellow/checker graybox city at Ferry Building.
