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

## Unreal Engine 5.6

1. Install Unreal Engine **5.6** with C++ support.
2. Install Visual Studio 2022 with the **Game development with C++** workload and a
   current Windows 10/11 SDK.
3. Open `unreal/SFRouteRacer/SFRouteRacer.uproject`.
4. Allow the editor to generate Visual Studio project files and compile modules.
5. After first compile, create Enhanced Input assets and a Chaos vehicle Blueprint
   based on `ASFVehiclePawn`.
6. Create a World Partition map named `SFWaterfrontMVP` under `/Game/Maps`.
7. Place or spawn `ASFRoadNetworkActor` and `ASFBuildingTileActor`, or rely on
   `ASFRouteRacerGameMode` bootstrap.
8. Run automation tests from Session Frontend:

```text
SFRouteRacer.Geo.CoordinateConversion
SFRouteRacer.Geo.LoadFixtureManifest
SFRouteRacer.Geo.RejectInvalidSchema
SFRouteRacer.Race.StateTransitions
```

### Compile status

Unreal compilation is **unverified** in the current development environment because
UE 5.6 is not installed here. Source, configs, fixtures, and docs are authored for a
clean UE 5.6 machine.

### Suggested build command after UE install

```powershell
& "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat" SFRouteRacerEditor Win64 Development -Project="C:\path\to\sf-route-racer\unreal\SFRouteRacer\SFRouteRacer.uproject" -WaitMutex
```
