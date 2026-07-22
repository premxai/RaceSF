# Profiling

Desktop MVP target: **60 FPS at 1080p on a mid-range gaming PC** (target, not a measured result).

## In-editor / PIE commands

```text
stat unit
stat gpu
stat rhi
stat scenerendering
stat memory
```

## Unreal Insights

1. Enable Unreal Insights from the editor or standalone.
2. Capture a ferry_building → chase_center run.
3. Inspect Game / Render / GPU timelines around map bootstrap and dense SoMa tiles.

## Counts to watch

- Actor count after graybox bootstrap
- Primitive / draw call counts with all building sections loaded
- Procedural mesh section counts for roads + buildings
- Working set / streaming memory with World Partition enabled

## Expected hotspots

- Combined building procedural sections on first load
- Road network mesh creation during bootstrap
- Minimap SceneCapture every frame

Mitigations already in code direction: no per-building Tick actors, async mesh cooking flags,
collision disabled on decorative buildings by default, and tile-oriented generation.
