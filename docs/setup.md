# Setup

## Python data pipeline

1. Install 64-bit Python 3.12.
2. From `tools/geo_pipeline`, create and activate a virtual environment.
3. Run `python -m pip install --upgrade pip`.
4. Run `pip install -e ".[dev,buildings]"`.
5. Run `ruff format --check .`, `ruff check .`, and `pytest`.
6. Run `python -m sf_racer_geo.cli build-all`.

Use `--force-download` only when refreshing upstream data. Use `--osm-fallback`
only when deliberately replacing preferred Overture footprints with OSM buildings.

## Unreal Engine 5.8

1. Install Unreal Engine **5.8** with C++ support (5.6+ also works).
2. Install Visual Studio / Build Tools with MSVC **14.50+** (UE 5.8 bans 14.40–14.43).
   Visual Studio Build Tools 2026 (MSVC 14.51) works; include a current Windows 10/11 SDK
   and the C++ workload / VC tools.
3. From the repo root:

```powershell
.\scripts\sync_map_export.ps1
.\scripts\setup_unreal.ps1
```

`setup_unreal.ps1` defaults to `-MaxParallelActions 4` to avoid OOM on 16 GB machines.
Pass `-MaxParallelActions 0` to use UBT's default parallelism.

4. Open `unreal/SFRouteRacer/SFRouteRacer.uproject` (or create the startup map first):

```powershell
.\scripts\bootstrap_playable.ps1 -OpenEditor
```

5. PIE on `SFWaterfrontMVP`. The C++ arcade vehicle ships with runtime Enhanced Input
   (WASD / Space / R / C). GameMode auto-loads the map JSON, builds roads/buildings,
   and starts Ferry Building → Chase Center with a scenic ghost.
6. Optional later: author a Chaos wheeled vehicle Blueprint and swap GameMode DefaultPawn,
   plus World Partition polish on `SFWaterfrontMVP`.
7. Run automation tests from Session Frontend:

```text
SFRouteRacer.Geo.CoordinateConversion
SFRouteRacer.Geo.LoadFixtureManifest
SFRouteRacer.Geo.RejectInvalidSchema
SFRouteRacer.Race.StateTransitions
SFRouteRacer.AI.GhostDefaults
```

### Suggested manual build

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" SFRouteRacerEditor Win64 Development -Project="C:\path\to\sf-route-racer\unreal\SFRouteRacer\SFRouteRacer.uproject" -WaitMutex -MaxParallelActions=4
```

Editor Python validation (with Python Editor Script Plugin enabled):

```text
Content/Python/validate_sf_export.py
```
