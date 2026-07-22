"""Generate genuinely distinct route suggestions from a directed road graph."""

from __future__ import annotations

import json
from itertools import islice
from pathlib import Path

import networkx as nx

from .route_scoring import immediate_uturn, route_metrics, route_overlap
from .settings import (
    PROJECT_ROOT,
    Landmark,
    RaceDefinition,
    RoadEdge,
    SuggestedRoute,
    load_json,
)

PROFILE_ORDER = ("fastest", "balanced", "scenic", "explorer")


class RouteGenerationError(RuntimeError):
    pass


def _candidate_paths(
    graph: nx.DiGraph, start: str, destination: str, count: int
) -> list[list[str]]:
    try:
        generator = nx.shortest_simple_paths(graph, start, destination, weight="travel_time_s")
        return list(islice(generator, count))
    except (nx.NetworkXNoPath, nx.NodeNotFound) as exc:
        raise RouteGenerationError(f"No route from {start} to {destination}") from exc


def generate_route_options(
    graph: nx.DiGraph,
    edges: dict[str, RoadEdge],
    start: str,
    destination: str,
    candidate_count: int = 20,
    max_overlap: float = 0.8,
    max_distance_ratio: float = 1.6,
) -> list[SuggestedRoute]:
    raw = _candidate_paths(graph, start, destination, candidate_count)
    metrics = [route_metrics(graph, path) for path in raw if len(path) == len(set(path))]
    metrics = [
        item
        for item in metrics
        if not immediate_uturn(graph, item["node_ids"])  # type: ignore[arg-type]
    ]
    if not metrics:
        raise RouteGenerationError("Candidate generation returned no valid simple paths")
    metrics.sort(key=lambda item: (item["estimated_time_s"], item["distance_m"]))
    fastest = metrics[0]
    max_distance = float(fastest["distance_m"]) * max_distance_ratio
    eligible = [item for item in metrics if float(item["distance_m"]) <= max_distance]
    edge_lengths = {edge_id: edge.length_m for edge_id, edge in edges.items()}
    min_x = min(float(data["x_m"]) for _, data in graph.nodes(data=True))
    max_x = max(float(data["x_m"]) for _, data in graph.nodes(data=True))
    x_span = max(1.0, max_x - min_x)

    def features(item: dict[str, object]) -> tuple[float, float, float, float]:
        edge_ids = item["edge_ids"]
        node_ids = item["node_ids"]
        overlap = route_overlap(edge_ids, fastest["edge_ids"], edge_lengths)  # type: ignore[arg-type]
        average_x = sum(float(graph.nodes[node]["x_m"]) for node in node_ids) / len(node_ids)  # type: ignore[arg-type]
        waterfront = (average_x - min_x) / x_span
        main_road_length = sum(
            edges[edge_id].length_m
            for edge_id in edge_ids  # type: ignore[union-attr]
            if edges[edge_id].road_class in {"motorway", "trunk", "primary", "secondary"}
        )
        main_road_ratio = main_road_length / float(item["distance_m"])
        return 1.0 - overlap, waterfront, main_road_ratio, float(item["turn_count"])

    selected: list[tuple[str, dict[str, object]]] = [("fastest", fastest)]

    def choose(profile: str) -> None:
        candidates: list[tuple[float, dict[str, object]]] = []
        selected_items = [item for _, item in selected]
        for item in eligible:
            if item in selected_items:
                continue
            overlaps = [
                route_overlap(item["edge_ids"], previous["edge_ids"], edge_lengths)  # type: ignore[arg-type]
                for previous in selected_items
            ]
            if max(overlaps) > max_overlap:
                continue
            novelty, waterfront, main_roads, turns = features(item)
            time_ratio = float(item["estimated_time_s"]) / float(fastest["estimated_time_s"])
            distance_ratio = float(item["distance_m"]) / float(fastest["distance_m"])
            if profile == "balanced":
                score = novelty * 0.45 + main_roads * 0.25 - time_ratio * 0.2 - turns * 0.002
            elif profile == "scenic":
                score = waterfront * 0.6 + main_roads * 0.2 + novelty * 0.3 - time_ratio * 0.1
            else:
                score = novelty * 0.8 + turns * 0.01 - distance_ratio * 0.15
            candidates.append((score, item))
        if candidates:
            selected.append((profile, max(candidates, key=lambda pair: pair[0])[1]))

    for profile in PROFILE_ORDER[1:]:
        choose(profile)

    if len(selected) < 3:
        raise RouteGenerationError(
            f"Only {len(selected)} routes met overlap and distance constraints; need at least 3"
        )
    routes: list[SuggestedRoute] = []
    for profile, item in selected:
        routes.append(
            SuggestedRoute(
                profile=profile,
                distance_m=float(item["distance_m"]),
                estimated_time_s=float(item["estimated_time_s"]),
                overlap_with_fastest=route_overlap(
                    item["edge_ids"],
                    fastest["edge_ids"],
                    edge_lengths,  # type: ignore[arg-type]
                ),
                edge_ids=item["edge_ids"],  # type: ignore[arg-type]
                node_ids=item["node_ids"],  # type: ignore[arg-type]
            )
        )
    return routes


def generate_flagship(
    graph: nx.DiGraph,
    edges: dict[str, RoadEdge],
    landmarks: list[Landmark],
    output_path: Path | None = None,
) -> RaceDefinition:
    by_id = {landmark.id: landmark for landmark in landmarks}
    start = by_id["ferry_building"].spawn
    destination = by_id["chase_center"].spawn
    if start is None or destination is None:
        raise RouteGenerationError("Flagship landmarks must be snapped before route generation")
    settings = load_json(PROJECT_ROOT / "config" / "route_profiles.json")
    race = RaceDefinition(
        race_id="ferry_building_to_chase_center",
        start_landmark_id="ferry_building",
        destination_landmark_id="chase_center",
        routes=generate_route_options(
            graph,
            edges,
            start.node_id,
            destination.node_id,
            int(settings["candidate_count"]),
            float(settings["max_overlap_ratio"]),
            float(settings["max_distance_ratio"]),
        ),
    )
    output_path = output_path or PROJECT_ROOT / "data" / "processed" / "races.json"
    output_path.write_text(json.dumps([race.model_dump()], indent=2), encoding="utf-8")
    return race
