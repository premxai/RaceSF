"""Create the MVP startup map if missing.

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


def create_map() -> bool:
    ensure_maps_folder()
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.log(f"SF playable bootstrap: map already exists at {MAP_PATH}")
        return True

    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem is None:
        unreal.log_error("SF playable bootstrap: LevelEditorSubsystem unavailable")
        return False

    # Prefer a simple empty level; World Partition can be enabled later in-editor.
    created = False
    try:
        created = bool(level_subsystem.new_level(MAP_PATH))
    except Exception as exc:  # noqa: BLE001 - editor API variance across UE versions
        unreal.log_warning(f"SF playable bootstrap: new_level failed ({exc}); trying asset factory")

    if not created and not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        factory = unreal.WorldFactory()
        world = asset_tools.create_asset(MAP_NAME, MAP_DIR, unreal.World, factory)
        created = world is not None

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.EditorAssetLibrary.save_asset(MAP_PATH)
        unreal.log(f"SF playable bootstrap: created {MAP_PATH}")
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
