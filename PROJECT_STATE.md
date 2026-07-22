# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestone 1 — Data proof

- Complete — Repository structure and project configuration
- Complete — Typed geospatial data contracts
- Complete — Cached OSM road download
- Complete — Road graph normalization
- Complete — Six-landmark snapping and directed connectivity
- Complete — Alternative route generation and scoring
- Complete — Overture building download and normalization
- Complete — Tiled, versioned Unreal JSON export
- Complete — Interactive HTML preview
- Complete — Validation and acceptance tests
- Complete — Formatting, static checks, and tests
- Complete — Pushed to https://github.com/premxai/RaceSF.git (`8de34c0`)

## Milestone 2 — Unreal graybox

- Complete — Unreal project structure and three C++ modules
- Complete — Schema-checked JSON map loading (`FSFMapJsonLoader`)
- Complete — Coordinate conversion library (`USFGeoCoordinateLibrary`)
- Complete — Map / routing / race subsystems
- Complete — Road graybox geometry via procedural mesh builder
- Complete — Building graybox masses (combined tile meshes)
- Complete — Chaos vehicle pawn foundation and chase camera
- Complete — Destination marker, race manager stub, UMG widget bases
- Complete — Editor import menu action
- Complete — Automation fixtures under `Content/TestData/sf_mvp_fixture`
- Complete — Build/setup docs with unverified compile status
- Blocked — Full Unreal compile (UE 5.6 not installed in this environment)
- Not started — Editor-authored World Partition map asset and vehicle Blueprint assets

## Later milestones

- Not started — Milestone 3: Race loop
- Not started — Milestone 4: Navigation
- Not started — Milestone 5: Presentation

## Verification log

Milestone 1:

- `python -m pytest` — 11 passed
- `python -m sf_racer_geo.cli build-all --verbose` — complete from cached sources

Milestone 2:

- Source and configs authored under `unreal/SFRouteRacer`
- Tiny fixture export checked in for automation tests
- Unreal Engine 5.6 was not found on this machine (Epic install list has no UE_5.6)
- Unreal compilation therefore remains **unverified**
- No fabricated build success claims

## Known limitations

- UE 5.6 + Visual Studio compile has not been run
- Enhanced Input assets and Chaos vehicle Blueprint must be created in-editor
- World Partition map `SFWaterfrontMVP` is referenced but not yet baked as a `.umap`
- Building collision is disabled by default for graybox performance
- Explorer route still omitted under the 80% overlap gate from Milestone 1

## Exact next step

On a UE 5.6 machine: generate project files, compile, create the World Partition map and
vehicle Blueprint, run Session Frontend automation tests, then start Milestone 3 race loop
wiring (countdown, timer, results, save best time).
