# SF Route Racer

Playable open-city route-racing MVP for Unreal Engine 5.6+. Players race between curated
San Francisco landmarks while remaining free to abandon any suggested route.

## Current status

- Milestone 1 complete: offline OSM/Overture data pipeline, flagship routes, preview
- Milestone 2 complete (source): Unreal graybox modules; compile unverified without UE 5.6
- Milestone 3 complete (source): race loop, save game, choose/quick/daily race helpers

## Quick start (data)

Requires Python 3.12.

```powershell
cd tools/geo_pipeline
py -3.12 -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
pip install -e ".[dev,buildings]"
pytest
python -m sf_racer_geo.cli build-all
```

If Overture is temporarily unavailable:

```powershell
python -m sf_racer_geo.cli build-all --osm-fallback
```

## Quick start (Unreal)

Open `unreal/SFRouteRacer/SFRouteRacer.uproject` in Unreal Engine 5.6. Compilation has
not been verified in environments without UE 5.6 installed. See `docs/setup.md`.

Remote: https://github.com/premxai/RaceSF.git

See `PROJECT_STATE.md` for the live checklist.
