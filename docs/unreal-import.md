# Unreal import

## Export location

The offline pipeline writes:

```text
data/exports/sf_mvp/manifest.json
```

`USFMapDataSubsystem` resolves that directory relative to the Unreal project, or falls
back to `Content/Maps/sf_mvp` / `Content/TestData/sf_mvp_fixture` for local tests.

## Load rules

1. Reject any schema version other than `0.1.0`.
2. Validate duplicate IDs, missing graph nodes, missing tile references, missing
   landmark spawns, and non-driveable route edges.
3. Fail before spawning map actors when validation fails.
4. Log through `LogSFGeo`, `LogSFRouting`, `LogSFMapGeneration`, and `LogSFRace`.

## Coordinate conversion

Local meters become Unreal centimeters with:

```text
X = easting × 100
Y = -northing × 100
Z = elevation × 100
```

Implemented once in `USFGeoCoordinateLibrary`.

## Editor import

`Tools → Import SF Route Racer Map Export` validates the default export directory and
shows a notification with road/building/race counts.
