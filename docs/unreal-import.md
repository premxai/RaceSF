# Unreal import

Unreal import has not started because Milestone 1 has not yet passed its live-data
acceptance gate.

The future importer will read `data/exports/sf_mvp/manifest.json`, reject unsupported
schema versions, validate every referenced file and ID, then load graph and tile data
through `USFMapDataSubsystem`. It must fail before actor creation when data is corrupt.

Local meters become Unreal centimeters with `X = easting × 100`, `Y = -northing × 100`,
and `Z = elevation × 100`. Runtime implementation must mirror the tested Python
conversion. Editor-only baking code belongs in `SFRouteRacerEditor`.
