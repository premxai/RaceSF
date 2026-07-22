"""Directed routing graph construction and metrics."""

from __future__ import annotations

import json
from itertools import pairwise
from pathlib import Path

import networkx as nx

from .settings import PROJECT_ROOT, RoadEdge, RoadNode


def build_graph(nodes: list[RoadNode], edges: list[RoadEdge]) -> nx.DiGraph:
    graph = nx.DiGraph()
    for node in nodes:
        graph.add_node(node.id, x_m=node.x_m, y_m=node.y_m, elevation_m=node.elevation_m)
    for edge in edges:
        if edge.driveable:
            graph.add_edge(
                edge.from_node,
                edge.to_node,
                edge_id=edge.id,
                length_m=edge.length_m,
                travel_time_s=edge.travel_time_s,
                road_class=edge.road_class,
                points=edge.points,
            )
    return graph


def load_processed_graph(path: Path | None = None) -> tuple[nx.DiGraph, dict[str, RoadEdge]]:
    path = path or PROJECT_ROOT / "data" / "processed" / "graph.json"
    payload = json.loads(path.read_text(encoding="utf-8"))
    nodes = [RoadNode.model_validate(item) for item in payload["nodes"]]
    edges = [RoadEdge.model_validate(item) for item in payload["edges"]]
    return build_graph(nodes, edges), {edge.id: edge for edge in edges}


def edge_ids_for_nodes(graph: nx.DiGraph, node_ids: list[str]) -> list[str]:
    return [graph.edges[a, b]["edge_id"] for a, b in pairwise(node_ids)]


def path_metric(graph: nx.DiGraph, node_ids: list[str], field: str) -> float:
    return sum(float(graph.edges[a, b][field]) for a, b in pairwise(node_ids))
