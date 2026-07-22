# Gameplay

SF Route Racer is destination racing, not fixed-track racing. A route card provides
an initial suggestion—Fastest, Balanced, Scenic, or Explorer—but the only required
objective in Open Race is reaching the destination.

The flagship race runs from Ferry Building to Chase Center and must demonstrate a
waterfront route, a SoMa grid route, and a balanced central route. Suggested routes
may not exceed 1.6 times the fastest distance and are deduplicated using shared edge
length.

## Modes

- **Choose Race** — pick start and destination landmarks; unsupported pairs stay disabled
- **Quick Race** — random validated pair (≥1.5 km, ≥3 routes, avoid immediate repeat)
- **Daily Run** — local deterministic pair from the calendar date (prototype, no backend)

## Race loop

Selection → map loading → vehicle spawn → countdown (`3 2 1 GO`) → racing → destination
reached (25 m radius) → results → restart or return.

Results store completion time, distance driven, selected route profile, suggested
distance comparison, reroute count, and a local best time via `USFSaveGame`.

