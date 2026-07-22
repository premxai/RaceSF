# SF Route Racer — Project State

Last updated: 2026-07-22

Status values: Not started · In progress · Complete · Blocked

## Milestone 1 — Data proof

- Complete — Offline SF data pipeline (`8de34c0`)

## Milestone 2 — Unreal graybox

- Complete — C++ graybox modules (`8d38564`)
- Blocked — UE 5.6 compile not available here

## Milestone 3 — Race loop

- Complete — Countdown, finish, results, save, choose/quick/daily (`44fa98f`)

## Milestone 4 — Navigation

- Complete — Highlights, minimap capture, throttled rerouting (`c77c2fe`)

## Milestone 5 — Presentation

- Complete — Audio subsystem placeholders (countdown/go/destination/reroute/UI)
- Complete — Boundary barrier actor with MVP signage hook
- Complete — Credits/attribution widget data from manifest
- Complete — Road edge markings on wider roads
- Complete — Out-of-bounds vehicle reset helper
- Complete — Packaging + profiling documentation
- Blocked — Packaged build and measured 60 FPS result (requires UE 5.6 machine)

## Verification log

| Item | Result |
|------|--------|
| Python tests | 11 passed (earlier in session) |
| GitHub remote | https://github.com/premxai/RaceSF.git |
| Unreal compile | Unverified — UE 5.6 not installed |
| Packaged build | Not run |

## Known limitations

- No editor-authored materials, Chaos vehicle Blueprint, or World Partition `.umap` yet
- Audio cues log placeholders until licensed assets are bound
- Explorer route still often omitted by overlap gating
- Full PIE / package validation still pending a UE 5.6 workstation

## Exact next step

On a UE 5.6 machine: generate project files, compile, create the World Partition map and
vehicle/input Blueprints, run automation tests, package a Development build, then profile
a Ferry Building → Chase Center run with `stat unit` / Insights.
