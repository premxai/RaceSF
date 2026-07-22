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

## Later milestones

- Not started — Milestone 2: Unreal graybox
- Not started — Milestone 3: Race loop
- Not started — Milestone 4: Navigation
- Not started — Milestone 5: Presentation

## Phase checklist

1. Complete configuration, schemas, and coordinate conversion.
2. Download and cache roads; normalize a directed game graph.
3. Snap six curated landmarks and validate connectivity.
4. Generate, score, and deduplicate route alternatives.
5. Acquire and normalize Overture buildings, with explicit OSM fallback.
6. Tile and export versioned data for Unreal.
7. Produce the HTML preview and pass offline tests and acceptance checks.
8. Begin Unreal integration only after all Milestone 1 gates pass.

## Verification log

- `py -3.12 --version` — Python 3.12.9
- `pip install -e ".[dev,buildings]"` — complete
- `python -m ruff format .` — complete
- `python -m ruff check .` — all checks passed
- `python -m pytest` — 11 passed
- `python -m sf_racer_geo.cli download-roads --force-download --verbose` — complete
- `python -m sf_racer_geo.cli download-buildings --verbose` — complete
- `python -m sf_racer_geo.cli build-all --verbose` — complete from cached sources

Validated export summary:

- 1,910 driveable graph nodes
- 3,892 runtime driveable edges; 3,944 normalized road records
- 5,638 normalized Overture buildings; 79 rejected invalid/tiny footprints
- Six snapped landmarks; all 30 directed landmark pairs connected
- Three flagship suggestions: Fastest, Balanced, and Scenic
- 164 tile files; no missing manifest references
- Schema version `0.1.0`

## Known limitations

- Unreal integration intentionally has not started.
- Unreal compilation is unverified.
- The first 20-path candidate pool produced three valid flagship routes. An Explorer
  route did not meet the 80% overlap gate and is correctly omitted.
- The full normalized road dataset has six weak components, but all supported landmark
  pairs are in the same directed routable component.
- Elevation is flattened and 18 tunnel records are retained as non-driveable metadata.
- Source lane and speed tags are sparse; documented class defaults fill 1,966 lane and
  2,884 speed values.

## Exact next step

Begin Milestone 2 by creating the Unreal Engine 5.6 C++ project and the three module
skeletons, then implement schema-checked manifest/graph loading and its automation
fixtures before generating any road geometry.
