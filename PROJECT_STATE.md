# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestone 1 — Data proof

- Complete — Offline SF data pipeline (`8de34c0`)

## Milestone 2 — Unreal graybox

- Complete — C++ graybox modules (`8d38564`)
- Blocked — UE 5.6 compile not available in this environment

## Milestone 3 — Race loop

- Complete — Countdown, timer, finish, results, save, choose/quick/daily (`44fa98f`)

## Milestone 4 — Navigation

- Complete — Route highlight actor with profile colors
- Complete — Orthographic minimap SceneCapture actor
- Complete — Navigation subsystem with destination distance
- Complete — Throttled off-route rerouting (≤2/sec, skip when stationary)
- Complete — Temporary rerouting banner state
- Complete — Route choice card data builder for UMG
- Blocked — Live PIE verification and Blueprint HUD polish without UE 5.6

## Later milestones

- Not started — Milestone 5: Presentation

## Verification log

- Python tests still pass for route generation changes from Milestone 3
- Unreal compile remains unverified

## Exact next step

Milestone 5: lighting/atmosphere notes, road markings polish hooks, boundary props,
placeholder audio interfaces, credits/attribution screen, packaging docs.
