from sf_racer_geo.graph import build_graph
from sf_racer_geo.route_generation import generate_route_options
from sf_racer_geo.route_scoring import route_overlap
from sf_racer_geo.settings import RoadEdge, RoadNode


def make_edge(edge_id: str, start: str, end: str, length: float) -> RoadEdge:
    return RoadEdge(
        id=edge_id,
        **{"from": start, "to": end},
        points=[(0, 0, 0), (length, 0, 0)],
        length_m=length,
        travel_time_s=length / 10,
        road_class="residential",
        lane_count=2,
        lane_width_m=3,
        total_width_m=7,
        speed_limit_kph=36,
        one_way=True,
    )


def fixture_graph():
    nodes = [
        RoadNode(id=node_id, x_m=index * 10, y_m=0)
        for index, node_id in enumerate(("s", "a", "b", "c", "d", "t"))
    ]
    edges = [
        make_edge("sa", "s", "a", 100),
        make_edge("at", "a", "t", 100),
        make_edge("sb", "s", "b", 105),
        make_edge("bt", "b", "t", 105),
        make_edge("sc", "s", "c", 110),
        make_edge("ct", "c", "t", 110),
        make_edge("sd", "s", "d", 115),
        make_edge("dt", "d", "t", 115),
    ]
    return build_graph(nodes, edges), {edge.id: edge for edge in edges}


def test_length_weighted_overlap() -> None:
    lengths = {"a": 90, "b": 10, "c": 50}
    assert route_overlap(["a", "b"], ["b", "c"], lengths) == 0.1


def test_generation_returns_distinct_profiled_routes() -> None:
    graph, edges = fixture_graph()
    routes = generate_route_options(graph, edges, "s", "t")
    assert len(routes) == 4
    assert [route.profile for route in routes] == ["fastest", "balanced", "scenic", "explorer"]
    assert all(route.distance_m <= routes[0].distance_m * 1.6 for route in routes)
    assert all(route.overlap_with_fastest <= 0.8 for route in routes[1:])
