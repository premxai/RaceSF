"""Create or dress the MVP startup map with lights so the editor is not a black void.

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


def dress_current_level() -> None:
    """Add sun/sky/fog/floor so the empty map is visible in the editor viewport."""
    existing = unreal.EditorLevelLibrary.get_all_level_actors()
    names = {actor.get_actor_label() for actor in existing}

    if "SF_Sun" not in names:
        sun = _spawn_actor(
            unreal.DirectionalLight,
            unreal.Vector(0.0, 0.0, 400.0),
            unreal.Rotator(-50.0, 35.0, 0.0),
        )
        if sun:
            sun.set_actor_label("SF_Sun")
            light = sun.get_component_by_class(unreal.DirectionalLightComponent)
            if light:
                light.set_editor_property("intensity", 12.0)

    if "SF_SkyLight" not in names:
        sky = _spawn_actor(unreal.SkyLight)
        if sky:
            sky.set_actor_label("SF_SkyLight")

    if "SF_SkyAtmosphere" not in names:
        atmosphere = _spawn_actor(unreal.SkyAtmosphere)
        if atmosphere:
            atmosphere.set_actor_label("SF_SkyAtmosphere")

    if "SF_Fog" not in names:
        fog = _spawn_actor(unreal.ExponentialHeightFog)
        if fog:
            fog.set_actor_label("SF_Fog")

    if "SF_PreviewFloor" not in names:
        floor = _spawn_actor(unreal.StaticMeshActor, unreal.Vector(0.0, 0.0, -50.0))
        if floor:
            floor.set_actor_label("SF_PreviewFloor")
            floor.set_actor_scale3d(unreal.Vector(200.0, 200.0, 1.0))
            mesh = floor.static_mesh_component
            cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube")
            if mesh and cube:
                mesh.set_static_mesh(cube)

    if "SF_PlayerStart" not in names:
        start = _spawn_actor(unreal.PlayerStart, unreal.Vector(0.0, 0.0, 100.0))
        if start:
            start.set_actor_label("SF_PlayerStart")

    # Nudge the editor camera so the floor is on-screen.
    try:
        unreal.EditorLevelLibrary.set_level_viewport_camera_info(
            unreal.Vector(0.0, -800.0, 400.0),
            unreal.Rotator(-20.0, 90.0, 0.0),
        )
    except Exception as exc:  # noqa: BLE001
        unreal.log_warning(f"SF playable bootstrap: could not set viewport camera ({exc})")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("SF playable bootstrap: dressed level with lighting + preview floor")


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
