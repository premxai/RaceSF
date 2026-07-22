import networkx as nx

from sf_racer_geo.graph import build_graph
from sf_racer_geo.normalize_roads import infer_lanes, parse_speed_kph, stable_id
from sf_racer_geo.settings import RoadEdge, RoadNode


def test_stable_ids_are_repeatable_and_sensitive() -> None:
    assert stable_id(123, 1, 2, 0) == stable_id(123, 1, 2, 0)
    assert stable_id(123, 1, 2, 0) != stable_id(123, 2, 1, 0)


def test_lane_and_speed_inference() -> None:
    defaults = {"total_lanes": {"primary": 2}}
    assert infer_lanes({}, "primary", defaults) == 2
    assert infer_lanes({"oneway": True}, "primary", defaults) == 1
    assert infer_lanes({"lanes": "3;2"}, "primary", defaults) == 3
    assert parse_speed_kph("25 mph", 30) == 40
    assert parse_speed_kph("signals", 30) == 30


def test_build_graph_excludes_non_driveable_edges() -> None:
    nodes = [RoadNode(id="a", x_m=0, y_m=0), RoadNode(id="b", x_m=10, y_m=0)]
    common = {
        "from": "a",
        "to": "b",
        "points": [(0, 0, 0), (10, 0, 0)],
        "length_m": 10,
        "travel_time_s": 1,
        "road_class": "residential",
        "lane_count": 2,
        "lane_width_m": 3,
        "total_width_m": 7,
        "speed_limit_kph": 30,
        "one_way": True,
    }
    graph = build_graph(nodes, [RoadEdge(id="edge", driveable=False, **common)])
    assert isinstance(graph, nx.DiGraph)
    assert graph.number_of_edges() == 0
