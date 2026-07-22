"""Route metrics and length-weighted overlap."""

from __future__ import annotations

import networkx as nx

from .graph import edge_ids_for_nodes, path_metric


def route_overlap(
    route_edge_ids: list[str], other_edge_ids: list[str], edge_lengths: dict[str, float]
) -> float:
    """Return shared directed edge length divided by the candidate route length."""
    route = set(route_edge_ids)
    other = set(other_edge_ids)
    total = sum(edge_lengths[edge_id] for edge_id in route)
    if total <= 0:
        return 0.0
    return sum(edge_lengths[edge_id] for edge_id in route & other) / total


def immediate_uturn(graph: nx.DiGraph, node_ids: list[str]) -> bool:
    return any(a == c for a, _, c in zip(node_ids, node_ids[1:], node_ids[2:], strict=False))


def route_metrics(graph: nx.DiGraph, node_ids: list[str]) -> dict[str, object]:
    edge_ids = edge_ids_for_nodes(graph, node_ids)
    return {
        "node_ids": node_ids,
        "edge_ids": edge_ids,
        "distance_m": path_metric(graph, node_ids, "length_m"),
        "estimated_time_s": path_metric(graph, node_ids, "travel_time_s"),
        "turn_count": max(0, len(node_ids) - 2),
    }
