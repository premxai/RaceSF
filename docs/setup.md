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

## Unreal

Unreal project creation is blocked by the Milestone 1 acceptance gate. Once the
exports validate, install Unreal Engine 5.6 with C++ tooling, Visual Studio 2022,
the Game development with C++ workload, and a Windows SDK. Exact project generation
and build commands will be added in Milestone 2.
