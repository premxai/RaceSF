"""Normalize OSMnx graphs into deterministic game-oriented records."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path
from typing import Any

import networkx as nx
from shapely.geometry import LineString

from .coordinates import CoordinateConverter
from .settings import PROJECT_ROOT, RoadEdge, RoadNode, load_json, load_map_config


def stable_id(*parts: object) -> str:
    payload = "|".join(str(part) for part in parts).encode()
    return hashlib.sha256(payload).hexdigest()[:20]


def scalar(value: Any) -> Any:
    return value[0] if isinstance(value, list) and value else value


def road_class(data: dict[str, Any]) -> str:
    return str(scalar(data.get("highway", "unclassified"))).split("_link")[0]


def parse_speed_kph(value: Any, default: int) -> int:
    value = scalar(value)
    if value is None:
        return default
    text = str(value).lower().strip()
    numbers = [float(number) for number in re.findall(r"\d+(?:\.\d+)?", text)]
    if not numbers:
        return default
    speed = min(numbers)
    if "mph" in text:
        speed *= 1.609344
    return max(5, min(130, round(speed)))


def infer_lanes(data: dict[str, Any], road_type: str, defaults: dict[str, Any]) -> int:
    raw = scalar(data.get("lanes"))
    if raw is not None:
        match = re.search(r"\d+", str(raw))
        if match and int(match.group()) > 0:
            return int(match.group())
    total = int(defaults["total_lanes"].get(road_type, 2))
    return max(1, (total + 1) // 2) if bool(data.get("oneway")) else total


def normalize_graph(graph: nx.MultiDiGraph) -> tuple[list[RoadNode], list[RoadEdge], list[dict]]:
    config = load_map_config()
    converter = CoordinateConverter(config)
    defaults = load_json(PROJECT_ROOT / "config" / "road_defaults.json")
    nodes: list[RoadNode] = []
    for node_id, data in sorted(graph.nodes(data=True), key=lambda item: str(item[0])):
        x_m, y_m = converter.geographic_to_local(float(data["x"]), float(data["y"]))
        nodes.append(RoadNode(id=str(node_id), x_m=x_m, y_m=y_m))

    edges: list[RoadEdge] = []
    rejected: list[dict] = []
    for from_id, to_id, key, data in sorted(
        graph.edges(keys=True, data=True), key=lambda item: tuple(map(str, item[:3]))
    ):
        try:
            kind = road_class(data)
            geometry = data.get("geometry")
            if geometry is None:
                start, end = graph.nodes[from_id], graph.nodes[to_id]
                geometry = LineString([(start["x"], start["y"]), (end["x"], end["y"])])
            points = [
                (*converter.geographic_to_local(float(lon), float(lat)), 0.0)
                for lon, lat in geometry.coords
            ]
            length_m = float(data.get("length") or LineString(points).length)
            if length_m <= 0 or len(points) < 2:
                raise ValueError("empty or zero-length geometry")
            lanes = infer_lanes(data, kind, defaults)
            lane_width = float(defaults["lane_width_m"].get(kind, 3.0))
            speed = parse_speed_kph(
                data.get("maxspeed"), int(defaults["speed_limit_kph"].get(kind, 30))
            )
            edges.append(
                RoadEdge(
                    id=stable_id(scalar(data.get("osmid", "")), from_id, to_id, key),
                    **{"from": str(from_id), "to": str(to_id)},
                    points=points,
                    length_m=length_m,
                    travel_time_s=length_m / (speed / 3.6),
                    road_class=kind,
                    lane_count=lanes,
                    lane_width_m=lane_width,
                    total_width_m=lanes * lane_width + float(defaults["surface_margin_m"]),
                    speed_limit_kph=speed,
                    one_way=bool(data.get("oneway", False)),
                    bridge=bool(data.get("bridge")),
                    tunnel=bool(data.get("tunnel")),
                    layer=int(scalar(data.get("layer", 0)) or 0),
                    driveable=not bool(data.get("tunnel")),
                    source={
                        field: data[field]
                        for field in (
                            "osmid",
                            "name",
                            "access",
                            "service",
                            "junction",
                            "lanes",
                            "maxspeed",
                            "width",
                        )
                        if field in data
                    },
                )
            )
        except (TypeError, ValueError, KeyError) as exc:
            rejected.append(
                {"from": str(from_id), "to": str(to_id), "key": str(key), "reason": str(exc)}
            )
    return nodes, edges, rejected


def normalize_file(
    graph_path: Path | None = None, output_dir: Path | None = None
) -> tuple[list[RoadNode], list[RoadEdge]]:
    import osmnx as ox

    graph_path = graph_path or PROJECT_ROOT / "data" / "raw" / "sf_drive.graphml"
    output_dir = output_dir or PROJECT_ROOT / "data" / "processed"
    output_dir.mkdir(parents=True, exist_ok=True)
    graph = ox.load_graphml(graph_path)
    nodes, edges, rejected = normalize_graph(graph)
    payload = {
        "schema_version": "0.1.0",
        "nodes": [node.model_dump(by_alias=True) for node in nodes],
        "edges": [edge.model_dump(by_alias=True) for edge in edges],
    }
    (output_dir / "graph.json").write_text(json.dumps(payload, indent=2), encoding="utf-8")
    (output_dir / "rejected_roads.json").write_text(
        json.dumps(rejected, indent=2), encoding="utf-8"
    )
    return nodes, edges
