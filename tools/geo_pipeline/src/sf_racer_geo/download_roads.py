"""Download and cache the vehicle-accessible SF MVP road network."""

from __future__ import annotations

import json
import time
from datetime import UTC, datetime
from pathlib import Path

from .settings import PROJECT_ROOT, load_map_config

DRIVE_FILTER = (
    '["highway"~"motorway|trunk|primary|secondary|tertiary|unclassified|'
    'residential|living_street|service"]'
    '["area"!~"yes"]["access"!~"private|no"]'
    '["service"!~"parking_aisle|driveway"]["motor_vehicle"!~"no"]'
)


class RoadDownloadError(RuntimeError):
    pass


def download_roads(force_download: bool = False, retries: int = 3) -> tuple[Path, Path]:
    """Download once, preserving GraphML types and a human-inspectable GeoJSON."""
    import osmnx as ox

    config = load_map_config()
    raw_dir = PROJECT_ROOT / "data" / "raw"
    cache_dir = PROJECT_ROOT / "data" / "cache" / "osmnx"
    graph_path = raw_dir / "sf_drive.graphml"
    geojson_path = raw_dir / "sf_roads.geojson"
    raw_dir.mkdir(parents=True, exist_ok=True)
    cache_dir.mkdir(parents=True, exist_ok=True)
    if graph_path.exists() and geojson_path.exists() and not force_download:
        metadata_path = raw_dir / "road_source_metadata.json"
        metadata = (
            json.loads(metadata_path.read_text(encoding="utf-8")) if metadata_path.exists() else {}
        )
        if "version" not in metadata:
            metadata.update(
                {
                    "dataset": "OpenStreetMap",
                    "version": "live snapshot",
                    "license": "ODbL-1.0",
                }
            )
            metadata_path.write_text(json.dumps(metadata, indent=2), encoding="utf-8")
        return graph_path, geojson_path

    ox.settings.use_cache = True
    ox.settings.cache_folder = cache_dir
    bbox = (
        config.bbox.west,
        config.bbox.south,
        config.bbox.east,
        config.bbox.north,
    )
    last_error: Exception | None = None
    for attempt in range(1, retries + 1):
        try:
            graph = ox.graph_from_bbox(
                bbox=bbox,
                custom_filter=DRIVE_FILTER,
                simplify=True,
                retain_all=False,
                truncate_by_edge=False,
            )
            if not graph.nodes or not graph.edges:
                raise RoadDownloadError("OpenStreetMap returned an empty road graph")
            ox.save_graphml(graph, graph_path)
            _, edges = ox.graph_to_gdfs(graph)
            edges.to_file(geojson_path, driver="GeoJSON")
            (raw_dir / "road_source_metadata.json").write_text(
                json.dumps(
                    {
                        "dataset": "OpenStreetMap",
                        "version": "live snapshot",
                        "retrieved_at": datetime.now(UTC).isoformat(),
                        "license": "ODbL-1.0",
                    },
                    indent=2,
                ),
                encoding="utf-8",
            )
            return graph_path, geojson_path
        except Exception as exc:  # network/client exceptions vary by OSMnx version
            last_error = exc
            if attempt < retries:
                time.sleep(2 ** (attempt - 1))
    raise RoadDownloadError(
        f"Road download failed after {retries} attempts. "
        "Check network access or reuse cached data with no --force-download."
    ) from last_error
