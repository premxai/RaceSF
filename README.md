# SF Route Racer

Playable open-city route-racing MVP for Unreal Engine 5.6+. Players race between curated
San Francisco landmarks while remaining free to abandon any suggested route.

The repository currently implements Milestone 1: an offline, deterministic Python pipeline
for the bounded SF waterfront district. Unreal work is gated on validated real data.

## Quick start

Requires Python 3.12 and native geospatial wheels supported by the selected platform.

```powershell
cd tools/geo_pipeline
py -3.12 -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
pip install -e ".[dev,buildings]"
pytest
python -m sf_racer_geo.cli build-all
```

If Overture is temporarily unavailable, explicitly opt into OSM building footprints:

```powershell
python -m sf_racer_geo.cli build-all --osm-fallback
```

Downloads are cached under `data/cache`; generated Unreal inputs are written to
`data/exports/sf_mvp`. The game never needs a live map API.

See `docs/setup.md`, `docs/data-pipeline.md`, and `PROJECT_STATE.md` for details and
verified project status.
