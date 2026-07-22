# Data pipeline

Commands run from `tools/geo_pipeline`:

```text
python -m sf_racer_geo.cli download-roads [--force-download]
python -m sf_racer_geo.cli download-buildings [--force-download] [--osm-fallback]
python -m sf_racer_geo.cli normalize
python -m sf_racer_geo.cli generate-routes
python -m sf_racer_geo.cli validate
python -m sf_racer_geo.cli export-unreal
python -m sf_racer_geo.cli preview
python -m sf_racer_geo.cli build-all
```

Roads come from OpenStreetMap through OSMnx. Preferred building footprints come from
Overture; OSM is an explicit fallback. Raw files and service caches are local and are
excluded from Git.

Coordinates follow one path: WGS84 longitude/latitude → EPSG:32610 meters → subtract
the projected world origin → local meters. Unreal conversion is `(X*100, -Y*100,
elevation*100)` and exists only in `coordinates.py`.

Exports use schema `0.1.0`. Geometry and inferred attributes are deterministic.
Generation time records the real UTC export time; set `SOURCE_DATE_EPOCH` when a
bit-for-bit reproducible manifest timestamp is required.
