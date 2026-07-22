# Architecture

The Python pipeline owns source acquisition, projection, normalization, landmark
snapping, route cards, validation, tiling, and versioned JSON export. All coordinates
are projected to EPSG:32610, made origin-relative in meters, then converted to Unreal
centimeters at the runtime boundary.

The future Unreal project has three modules:

- `SFGeoRuntime`: contracts, JSON loading, coordinate conversion, tiles, graph routing,
  and road/building generation abstractions.
- `SFRouteRacer`: vehicle, race flow, player systems, save game, and UMG.
- `SFRouteRacerEditor`: import validation, map baking, and editor-only tooling.

The road graph is authoritative for navigation. Rendered geometry is replaceable and
must never encode routing state. Precomputed route cards seed navigation; Unreal A*
will reroute freely to the unchanged destination.
