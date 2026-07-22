# Troubleshooting

- Road or building download fails: keep existing cache, verify network access, retry
  without `--force-download`, and inspect the command's chained error in verbose mode.
- Overture client unavailable: install the `buildings` extra. Use `--osm-fallback`
  only when the fallback is acceptable and can be attributed.
- Empty graph: verify the configured bbox and Overpass availability; do not continue
  with an empty export.
- Landmark cannot snap: inspect rejected roads and the preview. Do not silently move
  the anchor or weaken driveability rules.
- Fewer than three routes: inspect connectivity and overlap constraints. The flagship
  acceptance gate must remain failed until three meaningful alternatives exist.
- Native geospatial package installation fails on Windows: use Python 3.12 x64 and
  current pip wheels in a clean virtual environment.
