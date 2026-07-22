# Architecture

The Python pipeline owns source acquisition, projection, normalization, landmark
snapping, route cards, validation, tiling, and versioned JSON export. All coordinates
are projected to EPSG:32610, made origin-relative in meters, then converted to Unreal
centimeters at the runtime boundary.

The Unreal project has three modules:

- `SFGeoRuntime`: contracts, JSON loading, coordinate conversion, tiles, graph routing,
  and road geometry builder abstractions.
- `SFRouteRacer`: vehicle, race flow stubs, player systems, graybox actors, and UMG
  widget bases.
- `SFRouteRacerEditor`: import validation menu action and future baking tools.

The road graph is authoritative for navigation. Rendered geometry is replaceable and
must never encode routing state. Precomputed route cards seed navigation; Unreal
Dijkstra/A* reroutes freely to the unchanged destination.

Milestone 2 graybox generation uses `USFProceduralRoadGeometryBuilder` and combined
tile building meshes. World Partition / Data Layer organization is configured for the
upcoming map asset pass.
