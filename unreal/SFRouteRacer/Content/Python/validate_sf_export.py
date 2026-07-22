# Unreal Editor Python helper: validate and report the SF MVP export.
# Enable Edit ▸ Plugins ▸ Python Editor Script Plugin, then:
#   File ▸ Execute Python Script ▸ this file

import unreal
import json
import os


def _project_dir():
    return unreal.Paths.project_dir()


def _candidates():
    root = _project_dir()
    return [
        os.path.normpath(os.path.join(root, "..", "..", "data", "exports", "sf_mvp")),
        os.path.normpath(os.path.join(root, "Content", "Maps", "sf_mvp")),
        os.path.normpath(os.path.join(root, "Content", "TestData", "sf_mvp_fixture")),
    ]


def main():
    unreal.log("SF Route Racer: scanning map exports...")
    found = None
    for path in _candidates():
        manifest = os.path.join(path, "manifest.json")
        if os.path.isfile(manifest):
            found = path
            break

    if not found:
        unreal.log_error("No sf_mvp manifest found. Run scripts/sync_map_export.ps1 or the geo pipeline.")
        return

    with open(os.path.join(found, "manifest.json"), "r", encoding="utf-8") as handle:
        data = json.load(handle)

    schema = data.get("schema_version")
    if schema != "0.1.0":
        unreal.log_error(f"Unsupported schema_version: {schema}")
        return

    unreal.log(
        "Loaded manifest from {} | roads={} buildings={} landmarks={} races={} tiles={}".format(
            found,
            data.get("road_count"),
            data.get("building_count"),
            data.get("landmark_count"),
            data.get("race_count"),
            data.get("tile_count"),
        )
    )
    for line in data.get("attribution", []):
        unreal.log(f"Attribution: {line}")


if __name__ == "__main__":
    main()
