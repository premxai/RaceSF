"""Export versioned, tiled JSON consumed by Unreal."""

from __future__ import annotations

import json
import os
from datetime import UTC, datetime
from pathlib import Path

from .normalize_buildings import tile_id_for_point
from .settings import (
    PROJECT_ROOT,
    Building,
    ExportManifest,
    Landmark,
    MapTile,
    RaceDefinition,
    RoadEdge,
    RoadNode,
    load_map_config,
)


def _dump(path: Path, value: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True), encoding="utf-8")


def _generation_time() -> datetime:
    if "SOURCE_DATE_EPOCH" in os.environ:
        return datetime.fromtimestamp(int(os.environ["SOURCE_DATE_EPOCH"]), tz=UTC)
    return datetime.now(UTC)


def export_unreal(
    nodes: list[RoadNode],
    edges: list[RoadEdge],
    buildings: list[Building],
    landmarks: list[Landmark],
    races: list[RaceDefinition],
    output_dir: Path | None = None,
) -> Path:
    config = load_map_config()
    output_dir = output_dir or PROJECT_ROOT / "data" / "exports" / "sf_mvp"
    graph_path = output_dir / "graph.json"
    landmark_path = output_dir / "landmarks.json"
    races_path = output_dir / "races.json"
    _dump(
        graph_path,
        {
            "schema_version": "0.1.0",
            "nodes": [node.model_dump() for node in nodes],
            "edges": [edge.model_dump(by_alias=True) for edge in edges],
        },
    )
    _dump(landmark_path, [landmark.model_dump() for landmark in landmarks])
    _dump(races_path, [race.model_dump() for race in races])

    tile_roads: dict[str, set[str]] = {}
    tile_buildings: dict[str, list[Building]] = {}
    for edge in edges:
        for x_m, y_m, _ in edge.points:
            tile = tile_id_for_point(x_m, y_m, config.tile_size_meters)
            tile_roads.setdefault(tile, set()).add(edge.id)
    for building in buildings:
        tile_buildings.setdefault(building.tile_id, []).append(building)
    tile_ids = sorted(set(tile_roads) | set(tile_buildings))
    tile_files: list[str] = []
    for tile_id in tile_ids:
        tile = MapTile(
            tile_id=tile_id,
            roads=sorted(tile_roads.get(tile_id, set())),
            buildings=tile_buildings.get(tile_id, []),
        )
        relative = f"tiles/{tile_id}.json"
        _dump(output_dir / relative, tile.model_dump())
        tile_files.append(relative)

    road_metadata_path = PROJECT_ROOT / "data" / "raw" / "road_source_metadata.json"
    building_metadata_path = PROJECT_ROOT / "data" / "raw" / "building_source_metadata.json"
    road_metadata = (
        json.loads(road_metadata_path.read_text(encoding="utf-8"))
        if road_metadata_path.exists()
        else {"retrieved_at": "unknown"}
    )
    source_versions = {
        "OpenStreetMap": (
            f"{road_metadata.get('version', 'live snapshot')}; "
            f"retrieved {road_metadata['retrieved_at']}"
        )
    }
    attribution = ["© OpenStreetMap contributors, ODbL 1.0"]
    if buildings:
        building_metadata = (
            json.loads(building_metadata_path.read_text(encoding="utf-8"))
            if building_metadata_path.exists()
            else {
                "dataset": buildings[0].source.get("dataset", "unknown"),
                "retrieved_at": "unknown",
            }
        )
        source_versions[str(building_metadata["dataset"])] = (
            f"{building_metadata.get('version', 'unknown release')}; "
            f"retrieved {building_metadata['retrieved_at']}"
        )
        if building_metadata["dataset"] == "Overture Maps":
            attribution.append("Overture Maps Foundation data")
    manifest = ExportManifest(
        source_dataset_versions=source_versions,
        generation_timestamp=_generation_time(),
        projection=config.projection,
        world_origin=config.world_origin,
        bbox=config.bbox,
        tile_size_meters=config.tile_size_meters,
        tile_count=len(tile_files),
        road_count=len(edges),
        building_count=len(buildings),
        landmark_count=len(landmarks),
        race_count=len(races),
        graph_file="graph.json",
        landmarks_file="landmarks.json",
        races_file="races.json",
        tile_files=tile_files,
        attribution=attribution,
    )
    manifest_path = output_dir / "manifest.json"
    _dump(manifest_path, manifest.model_dump(mode="json"))
    return manifest_path
