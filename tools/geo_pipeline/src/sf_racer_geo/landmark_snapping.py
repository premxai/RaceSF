"""Snap approximate landmark anchors to safe positions on driveable roads."""

from __future__ import annotations

import json
import math
from pathlib import Path

from shapely.geometry import LineString, Point

from .coordinates import CoordinateConverter
from .settings import PROJECT_ROOT, Landmark, RoadEdge, SpawnPoint, load_landmarks, load_map_config


class LandmarkSnapError(RuntimeError):
    pass


def snap_landmark(
    landmark: Landmark,
    edges: list[RoadEdge],
    converter: CoordinateConverter,
    intersection_clearance_m: float = 12.0,
) -> Landmark:
    anchor = Point(converter.geographic_to_local(landmark.longitude, landmark.latitude))
    candidates: list[tuple[float, RoadEdge, LineString, float]] = []
    for edge in edges:
        if not edge.driveable or edge.tunnel:
            continue
        line = LineString([(point[0], point[1]) for point in edge.points])
        if line.length <= intersection_clearance_m * 2:
            continue
        distance_along = line.project(anchor)
        safe_along = max(
            intersection_clearance_m, min(line.length - intersection_clearance_m, distance_along)
        )
        spawn_point = line.interpolate(safe_along)
        candidates.append((anchor.distance(spawn_point), edge, line, safe_along))
    if not candidates:
        raise LandmarkSnapError(f"No safe driveable edge found for {landmark.name}")
    distance, edge, line, position = min(candidates, key=lambda item: (item[0], item[1].id))
    if distance > 250:
        raise LandmarkSnapError(f"Nearest safe road for {landmark.name} is {distance:.0f} m away")
    point = line.interpolate(position)
    before = line.interpolate(max(0.0, position - 1.0))
    after = line.interpolate(min(line.length, position + 1.0))
    heading = math.degrees(math.atan2(after.y - before.y, after.x - before.x))
    nearest_node = edge.from_node if position <= line.length / 2 else edge.to_node
    return landmark.model_copy(
        update={
            "spawn": SpawnPoint(
                edge_id=edge.id,
                node_id=nearest_node,
                x_m=point.x,
                y_m=point.y,
                heading_degrees=heading,
            )
        }
    )


def snap_all(edges: list[RoadEdge], output_path: Path | None = None) -> list[Landmark]:
    converter = CoordinateConverter(load_map_config())
    landmarks = [snap_landmark(item, edges, converter) for item in load_landmarks()]
    output_path = output_path or PROJECT_ROOT / "data" / "processed" / "landmarks.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        json.dumps([item.model_dump() for item in landmarks], indent=2), encoding="utf-8"
    )
    return landmarks
