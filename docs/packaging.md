# Packaging

## Development package (after UE 5.6 compile)

1. Open `unreal/SFRouteRacer/SFRouteRacer.uproject`.
2. Confirm the map export is reachable (`data/exports/sf_mvp` or copied under Content).
3. Platforms → Windows → Package Project.
4. Prefer **Development** configuration for the first packaged playtest.
5. Include the Credits screen and verify attribution strings load from the manifest.

## Suggested command line package

```powershell
& "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun `
  -project="C:\path\to\sf-route-racer\unreal\SFRouteRacer\SFRouteRacer.uproject" `
  -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development `
  -cook -allmaps -build -stage -pak -archive `
  -archivedirectory="C:\path\to\sf-route-racer\dist\windows"
```

Packaging has **not** been executed in this environment because Unreal Engine 5.6 is not installed.
