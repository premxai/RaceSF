# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestone 1 — Data proof

- Complete — Offline SF data pipeline, preview, tests
- Complete — Pushed (`8de34c0`)

## Milestone 2 — Unreal graybox

- Complete — Unreal C++ modules, JSON loaders, road/building graybox, vehicle foundation
- Complete — Pushed (`8d38564`)
- Blocked — Full Unreal compile (UE 5.6 not installed here)

## Milestone 3 — Race loop

- Complete — Race state machine (menu → selection → loading → spawn → countdown → racing → results)
- Complete — Countdown 3-2-1-GO and race timer
- Complete — Destination overlap finish (25 m radius)
- Complete — Results metrics + local best-time save game
- Complete — Choose Race / Quick Race / Daily Run (local prototype) helpers
- Complete — Multi-pair route generation for Choose/Quick Race data
- Complete — Automation tests for race state transitions
- Blocked — In-editor UMG Blueprint widgets and live play verification without UE 5.6

## Later milestones

- Not started — Milestone 4: Navigation
- Not started — Milestone 5: Presentation

## Verification log

- `python -m pytest` — 11 passed
- `python -m sf_racer_geo.cli generate-routes` / `validate` / `export-unreal` — complete after multi-pair race generation
- Unreal compile — unverified (no UE 5.6 install)

## Known limitations

- UE 5.6 compile and PIE verification remain unverified
- HUD/results Blueprint widgets still need editor authoring on top of C++ bases
- World Partition map asset still needs editor creation
- Explorer route still often omitted by overlap gate

## Exact next step

Milestone 4: route cards UI wiring, minimap SceneCapture, route highlighting, destination
distance/guidance, and throttled runtime rerouting in `USFRoutingSubsystem`.
