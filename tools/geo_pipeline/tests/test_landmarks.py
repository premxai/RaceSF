from sf_racer_geo.coordinates import CoordinateConverter
from sf_racer_geo.landmark_snapping import snap_landmark
from sf_racer_geo.normalize_buildings import deterministic_height, tile_id_for_point
from sf_racer_geo.settings import Landmark, RoadEdge, load_map_config


def edge() -> RoadEdge:
    return RoadEdge(
        id="safe-road",
        **{"from": "a", "to": "b"},
        points=[(-100, 0, 0), (100, 0, 0)],
        length_m=200,
        travel_time_s=20,
        road_class="residential",
        lane_count=2,
        lane_width_m=3,
        total_width_m=7,
        speed_limit_kph=30,
        one_way=False,
    )


def test_landmark_snaps_to_safe_road_position() -> None:
    config = load_map_config()
    landmark = Landmark(
        id="origin",
        name="Origin",
        latitude=config.world_origin.latitude,
        longitude=config.world_origin.longitude,
    )
    snapped = snap_landmark(landmark, [edge()], CoordinateConverter(config))
    assert snapped.spawn is not None
    assert snapped.spawn.edge_id == "safe-road"
    assert abs(snapped.spawn.y_m) < 0.01
    assert snapped.spawn.heading_degrees == 0


def test_building_height_and_tile_assignment_are_deterministic() -> None:
    first = deterministic_height("building-1", 500, "residential", 2000)
    assert first == deterministic_height("building-1", 500, "residential", 2000)
    assert 7 <= first <= 22
    assert tile_id_for_point(-1, 251, 250) == "tile_-1_1"
