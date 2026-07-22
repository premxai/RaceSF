"""Normalize building polygons and infer deterministic graybox heights."""

from __future__ import annotations

import hashlib
import json
import math
from pathlib import Path
from typing import Any

import geopandas as gpd
from shapely.geometry import MultiPolygon, Point, Polygon
from shapely.ops import transform

from .coordinates import CoordinateConverter
from .settings import PROJECT_ROOT, Building, load_map_config


def deterministic_height(
    building_id: str, area_m2: float, category: str, downtown_distance_m: float
) -> float:
    unit = int(hashlib.sha256(building_id.encode()).hexdigest()[:8], 16) / 0xFFFFFFFF
    if category in {"commercial", "office", "retail"} and downtown_distance_m < 1800:
        low, high = 18.0, 65.0
    elif area_m2 >= 1500:
        low, high = 12.0, 35.0
    elif category in {"industrial", "warehouse"}:
        low, high = 6.0, 18.0
    elif area_m2 < 60:
        low, high = 3.0, 8.0
    else:
        low, high = 7.0, 22.0
    return round(low + unit * (high - low), 2)


def tile_id_for_point(x_m: float, y_m: float, tile_size: float) -> str:
    return f"tile_{math.floor(x_m / tile_size)}_{math.floor(y_m / tile_size)}"


def _number(value: Any) -> float | None:
    try:
        result = float(value)
        return result if result > 0 else None
    except (TypeError, ValueError):
        return None


def normalize_buildings(source_path: Path, output_path: Path | None = None) -> list[Building]:
    config = load_map_config()
    converter = CoordinateConverter(config)
    frame = gpd.read_file(source_path).to_crs("EPSG:4326")
    buildings: list[Building] = []
    rejected: list[dict[str, str]] = []
    for index, row in frame.iterrows():
        source_id = str(row.get("id") or row.get("@id") or f"feature-{index}")
        geometry = row.geometry
        if isinstance(geometry, MultiPolygon):
            geometry = max(geometry.geoms, key=lambda polygon: polygon.area)
        if not isinstance(geometry, Polygon) or not geometry.is_valid:
            rejected.append({"id": source_id, "reason": "invalid or non-polygon geometry"})
            continue
        local = transform(
            lambda x, y, z=None: converter.geographic_to_local(x, y), geometry
        ).simplify(0.35, preserve_topology=True)
        if local.area < 20 or not local.is_valid:
            rejected.append({"id": source_id, "reason": "tiny or invalid simplified footprint"})
            continue
        levels = _number(row.get("num_floors") or row.get("building:levels") or row.get("levels"))
        source_height = _number(row.get("height"))
        category = str(
            row.get("subtype") or row.get("class") or row.get("building") or "residential"
        )
        height = source_height or (levels * 3.0 if levels else None)
        if height is None:
            height = deterministic_height(
                source_id, local.area, category, local.centroid.distance(Point(0, 0))
            )
        buildings.append(
            Building(
                id=source_id,
                outer_ring=list(local.exterior.coords),
                holes=[list(interior.coords) for interior in local.interiors],
                height_m=height,
                levels=round(levels) if levels else None,
                category=category,
                tile_id=tile_id_for_point(
                    local.centroid.x, local.centroid.y, config.tile_size_meters
                ),
                source={"dataset": "OSM" if "osm" in source_path.name else "Overture"},
            )
        )
    output_path = output_path or PROJECT_ROOT / "data" / "processed" / "buildings.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        json.dumps([item.model_dump() for item in buildings], indent=2), encoding="utf-8"
    )
    (output_path.parent / "rejected_buildings.json").write_text(
        json.dumps(rejected, indent=2), encoding="utf-8"
    )
    return buildings
