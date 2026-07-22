"""Milestone 1 data validation and acceptance gates."""

from __future__ import annotations

import json
from dataclasses import asdict, dataclass, field
from pathlib import Path

import networkx as nx

from .settings import PROJECT_ROOT, Landmark, RaceDefinition, RoadEdge


@dataclass
class ValidationReport:
    valid: bool = True
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)
    metrics: dict[str, int] = field(default_factory=dict)

    def error(self, message: str) -> None:
        self.valid = False
        self.errors.append(message)


def validate_data(
    graph: nx.DiGraph,
    edges: dict[str, RoadEdge],
    landmarks: list[Landmark],
    races: list[RaceDefinition],
    rejected_count: int = 0,
) -> ValidationReport:
    report = ValidationReport(
        metrics={
            "nodes": graph.number_of_nodes(),
            "edges": graph.number_of_edges(),
            "connected_components": nx.number_weakly_connected_components(graph),
            "one_way_edges": sum(edge.one_way for edge in edges.values()),
            "missing_source_lane_count": sum("lanes" not in edge.source for edge in edges.values()),
            "missing_source_speed_count": sum(
                "maxspeed" not in edge.source for edge in edges.values()
            ),
            "bridges": sum(edge.bridge for edge in edges.values()),
            "tunnels": sum(edge.tunnel for edge in edges.values()),
            "invalid_geometry": rejected_count,
            "rejected_features": rejected_count,
            "landmarks": len(landmarks),
            "races": len(races),
        }
    )
    if len(landmarks) != 6:
        report.error(f"Expected six configured landmarks, found {len(landmarks)}")
    for landmark in landmarks:
        if landmark.enabled and landmark.spawn is None:
            report.error(f"Enabled landmark {landmark.id} has no snapped spawn")
    enabled = [landmark for landmark in landmarks if landmark.enabled and landmark.spawn]
    for start in enabled:
        for destination in enabled:
            if start.id == destination.id:
                continue
            if not nx.has_path(graph, start.spawn.node_id, destination.spawn.node_id):
                report.error(f"No directed route from {start.id} to {destination.id}")
    flagship = next(
        (race for race in races if race.race_id == "ferry_building_to_chase_center"), None
    )
    if flagship is None:
        report.error("Flagship Ferry Building to Chase Center race is missing")
    elif len(flagship.routes) < 3:
        report.error("Flagship race has fewer than three route suggestions")
    edge_ids = set(edges)
    for race in races:
        fastest_distance = min(route.distance_m for route in race.routes)
        if not 750 <= fastest_distance <= 7000:
            report.error(f"Race {race.race_id} is outside supported road-distance bounds")
        for route in race.routes:
            missing = set(route.edge_ids) - edge_ids
            if missing:
                report.error(f"Route {race.race_id}/{route.profile} references missing edges")
                continue
            rejected = [edge_id for edge_id in route.edge_ids if not edges[edge_id].driveable]
            if rejected:
                report.error(f"Route {race.race_id}/{route.profile} uses rejected edges")
            if route.distance_m > fastest_distance * 1.6:
                report.error(f"Route {race.race_id}/{route.profile} exceeds distance ratio")
            if route.profile != "fastest" and route.overlap_with_fastest > 0.8:
                report.error(f"Route {race.race_id}/{route.profile} exceeds overlap limit")
            if len(route.node_ids) != len(set(route.node_ids)):
                report.error(f"Route {race.race_id}/{route.profile} repeats graph nodes")
    return report


def save_report(report: ValidationReport, path: Path | None = None) -> Path:
    path = path or PROJECT_ROOT / "data" / "processed" / "validation_report.json"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(asdict(report), indent=2), encoding="utf-8")
    return path
