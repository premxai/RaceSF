"""Create or dress the MVP startup map with fully dynamic lighting (no lightmaps).

Run via:
  UnrealEditor-Cmd.exe SFRouteRacer.uproject -ExecutePythonScript=Content/Python/bootstrap_playable.py
"""

from __future__ import annotations

import unreal

MAP_DIR = "/Game/Maps"
MAP_NAME = "SFWaterfrontMVP"
MAP_PATH = f"{MAP_DIR}/{MAP_NAME}"


def ensure_maps_folder() -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(MAP_DIR):
        unreal.EditorAssetLibrary.make_directory(MAP_DIR)


def _spawn_actor(actor_class, location=unreal.Vector(0.0, 0.0, 0.0), rotation=unreal.Rotator(0.0, 0.0, 0.0)):
    return unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, rotation)


def _destroy_labeled(labels: set[str]) -> None:
    for actor in list(unreal.EditorLevelLibrary.get_all_level_actors()):
        if actor.get_actor_label() in labels:
            unreal.EditorLevelLibrary.destroy_actor(actor)


def _set_movable_light(actor, intensity: float | None = None) -> None:
    if actor is None:
        return
    for cls in (unreal.DirectionalLightComponent, unreal.SkyLightComponent, unreal.LightComponent):
        comp = actor.get_component_by_class(cls)
        if comp is None:
            continue
        try:
            comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
        except Exception:  # noqa: BLE001
            pass
        if intensity is not None:
            try:
                comp.set_editor_property("intensity", intensity)
            except Exception:  # noqa: BLE001
                pass
        if cls is unreal.SkyLightComponent:
            try:
                comp.set_editor_property("real_time_capture", True)
            except Exception:  # noqa: BLE001
                pass
        break


def dress_current_level() -> None:
    """Replace lights with Movable ones and disable lightmap baking requirements."""
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world is not None:
        settings = world.get_world_settings()
        if settings is not None:
            try:
                settings.set_editor_property("force_no_precomputed_lighting", True)
            except Exception as exc:  # noqa: BLE001
                unreal.log_warning(f"SF playable bootstrap: could not set ForceNoPrecomputedLighting ({exc})")

    # Recreate our labeled lighting so mobility is definitely Movable.
    _destroy_labeled({"SF_Sun", "SF_SkyLight", "SF_SkyAtmosphere", "SF_Fog", "SF_PreviewFloor", "SF_PlayerStart"})

    sun = _spawn_actor(
        unreal.DirectionalLight,
        unreal.Vector(0.0, 0.0, 400.0),
        unreal.Rotator(-50.0, 35.0, 0.0),
    )
    if sun:
        sun.set_actor_label("SF_Sun")
        _set_movable_light(sun, intensity=20.0)
        light = sun.get_component_by_class(unreal.DirectionalLightComponent)
        if light:
            try:
                light.set_editor_property("atmosphere_sun_light", True)
            except Exception:  # noqa: BLE001
                pass

    sky = _spawn_actor(unreal.SkyLight)
    if sky:
        sky.set_actor_label("SF_SkyLight")
        _set_movable_light(sky, intensity=2.5)

    atmosphere = _spawn_actor(unreal.SkyAtmosphere)
    if atmosphere:
        atmosphere.set_actor_label("SF_SkyAtmosphere")

    fog = _spawn_actor(unreal.ExponentialHeightFog)
    if fog:
        fog.set_actor_label("SF_Fog")

    floor = _spawn_actor(unreal.StaticMeshActor, unreal.Vector(0.0, 0.0, -50.0))
    if floor:
        floor.set_actor_label("SF_PreviewFloor")
        floor.set_actor_scale3d(unreal.Vector(200.0, 200.0, 1.0))
        mesh = floor.static_mesh_component
        cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube")
        if mesh and cube:
            mesh.set_static_mesh(cube)
            try:
                mesh.set_editor_property("mobility", unreal.ComponentMobility.STATIC)
            except Exception:  # noqa: BLE001
                pass

    start = _spawn_actor(unreal.PlayerStart, unreal.Vector(0.0, 0.0, 100.0))
    if start:
        start.set_actor_label("SF_PlayerStart")

    try:
        unreal.EditorLevelLibrary.set_level_viewport_camera_info(
            unreal.Vector(0.0, -800.0, 400.0),
            unreal.Rotator(-20.0, 90.0, 0.0),
        )
    except Exception as exc:  # noqa: BLE001
        unreal.log_warning(f"SF playable bootstrap: could not set viewport camera ({exc})")

    unreal.EditorLevelLibrary.save_current_level()

    # One lighting build clears the "LIGHTING NEEDS TO BE REBUILT" banner after enabling
    # Force No Precomputed Lighting (no lightmaps are actually stored).
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        unreal.SystemLibrary.execute_console_command(world, "RebuildLighting")
        unreal.log("SF playable bootstrap: requested RebuildLighting")
    except Exception as exc:  # noqa: BLE001
        unreal.log_warning(f"SF playable bootstrap: RebuildLighting failed ({exc})")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("SF playable bootstrap: dressed level with dynamic lighting (no lightmaps)")


def create_map() -> bool:
    ensure_maps_folder()
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem is None:
        unreal.log_error("SF playable bootstrap: LevelEditorSubsystem unavailable")
        return False

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.log(f"SF playable bootstrap: opening existing map {MAP_PATH}")
        if not level_subsystem.load_level(MAP_PATH):
            unreal.log_error(f"SF playable bootstrap: failed to load {MAP_PATH}")
            return False
        dress_current_level()
        return True

    created = False
    try:
        created = bool(level_subsystem.new_level(MAP_PATH))
    except Exception as exc:  # noqa: BLE001
        unreal.log_warning(f"SF playable bootstrap: new_level failed ({exc}); trying asset factory")

    if not created and not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        factory = unreal.WorldFactory()
        world = asset_tools.create_asset(MAP_NAME, MAP_DIR, unreal.World, factory)
        created = world is not None
        if created:
            level_subsystem.load_level(MAP_PATH)

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        dress_current_level()
        unreal.EditorAssetLibrary.save_asset(MAP_PATH)
        unreal.log(f"SF playable bootstrap: ready {MAP_PATH}")
        return True

    unreal.log_error(f"SF playable bootstrap: failed to create {MAP_PATH}")
    return False


def main() -> None:
    ok = create_map()
    if not ok:
        raise SystemExit(1)
    unreal.log("SF playable bootstrap: done")


if __name__ == "__main__":
    main()
