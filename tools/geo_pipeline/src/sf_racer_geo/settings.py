"""Configuration loading and versioned data contracts."""

from __future__ import annotations

import json
from datetime import datetime
from pathlib import Path
from typing import Literal

from pydantic import BaseModel, ConfigDict, Field, model_validator

SCHEMA_VERSION = "0.1.0"
PROJECT_ROOT = Path(__file__).resolve().parents[4]


class StrictModel(BaseModel):
    model_config = ConfigDict(extra="forbid")


class BoundingBox(StrictModel):
    west: float
    south: float
    east: float
    north: float

    @model_validator(mode="after")
    def validate_bounds(self) -> BoundingBox:
        if self.west >= self.east or self.south >= self.north:
            raise ValueError("Bounding box west/south must be below east/north")
        return self


class GeoPoint(StrictModel):
    latitude: float
    longitude: float


class MapConfiguration(StrictModel):
    name: str
    bbox: BoundingBox
    projection: str = "EPSG:32610"
    tile_size_meters: int = Field(gt=0)
    world_origin: GeoPoint


class RoadNode(StrictModel):
    id: str
    x_m: float
    y_m: float
    elevation_m: float = 0.0


class RoadEdge(StrictModel):
    id: str
    from_node: str = Field(alias="from")
    to_node: str = Field(alias="to")
    points: list[tuple[float, float, float]]
    length_m: float = Field(gt=0)
    travel_time_s: float = Field(gt=0)
    road_class: str
    lane_count: int = Field(gt=0)
    lane_width_m: float = Field(gt=0)
    total_width_m: float = Field(gt=0)
    speed_limit_kph: int = Field(gt=0)
    one_way: bool
    bridge: bool = False
    tunnel: bool = False
    layer: int = 0
    driveable: bool = True
    source: dict[str, object] = Field(default_factory=dict)


class Building(StrictModel):
    id: str
    outer_ring: list[tuple[float, float]]
    holes: list[list[tuple[float, float]]] = Field(default_factory=list)
    height_m: float = Field(gt=0)
    levels: int | None = Field(default=None, gt=0)
    category: str
    tile_id: str
    source: dict[str, object] = Field(default_factory=dict)


class SpawnPoint(StrictModel):
    edge_id: str
    node_id: str
    x_m: float
    y_m: float
    heading_degrees: float


class Landmark(StrictModel):
    id: str
    name: str
    latitude: float
    longitude: float
    spawn: SpawnPoint | None = None
    enabled: bool = True


class SuggestedRoute(StrictModel):
    profile: Literal["fastest", "balanced", "scenic", "explorer"]
    distance_m: float = Field(gt=0)
    estimated_time_s: float = Field(gt=0)
    overlap_with_fastest: float = Field(ge=0, le=1)
    edge_ids: list[str]
    node_ids: list[str]


class RaceDefinition(StrictModel):
    race_id: str
    start_landmark_id: str
    destination_landmark_id: str
    routes: list[SuggestedRoute] = Field(min_length=3, max_length=4)


class MapTile(StrictModel):
    schema_version: str = SCHEMA_VERSION
    tile_id: str
    roads: list[str] = Field(default_factory=list)
    buildings: list[Building] = Field(default_factory=list)


class ExportManifest(StrictModel):
    schema_version: str = SCHEMA_VERSION
    source_dataset_versions: dict[str, str]
    generation_timestamp: datetime
    projection: str
    world_origin: GeoPoint
    bbox: BoundingBox
    tile_size_meters: int
    tile_count: int
    road_count: int
    building_count: int
    landmark_count: int
    race_count: int
    graph_file: str
    landmarks_file: str
    races_file: str
    tile_files: list[str]
    attribution: list[str]


def load_json(path: Path) -> object:
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


def load_map_config(path: Path | None = None) -> MapConfiguration:
    config_path = path or PROJECT_ROOT / "config" / "sf_mvp_region.json"
    return MapConfiguration.model_validate(load_json(config_path))


def load_landmarks(path: Path | None = None) -> list[Landmark]:
    config_path = path or PROJECT_ROOT / "config" / "landmarks.json"
    return [Landmark.model_validate(item) for item in load_json(config_path)]
