import json

from sf_racer_geo.export_unreal import export_unreal
from sf_racer_geo.settings import (
    ExportManifest,
    Landmark,
    RaceDefinition,
    RoadEdge,
    RoadNode,
    SpawnPoint,
    SuggestedRoute,
)


def test_export_manifest_references_existing_files(tmp_path) -> None:
    nodes = [RoadNode(id="a", x_m=0, y_m=0), RoadNode(id="b", x_m=100, y_m=0)]
    edge = RoadEdge(
        id="ab",
        **{"from": "a", "to": "b"},
        points=[(0, 0, 0), (100, 0, 0)],
        length_m=100,
        travel_time_s=10,
        road_class="primary",
        lane_count=2,
        lane_width_m=3.5,
        total_width_m=8,
        speed_limit_kph=40,
        one_way=True,
    )
    landmarks = [
        Landmark(
            id=landmark_id,
            name=landmark_id,
            latitude=37.78,
            longitude=-122.39,
            spawn=SpawnPoint(edge_id="ab", node_id=node, x_m=x, y_m=0, heading_degrees=0),
        )
        for landmark_id, node, x in (("start", "a", 0), ("finish", "b", 100))
    ]
    routes = [
        SuggestedRoute(
            profile=profile,
            distance_m=100,
            estimated_time_s=10,
            overlap_with_fastest=1 if profile == "fastest" else 0,
            edge_ids=["ab"],
            node_ids=["a", "b"],
        )
        for profile in ("fastest", "balanced", "scenic")
    ]
    race = RaceDefinition(
        race_id="test",
        start_landmark_id="start",
        destination_landmark_id="finish",
        routes=routes,
    )
    manifest_path = export_unreal(nodes, [edge], [], landmarks, [race], tmp_path)
    payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest = ExportManifest.model_validate(payload)
    for relative in [
        manifest.graph_file,
        manifest.landmarks_file,
        manifest.races_file,
        *manifest.tile_files,
    ]:
        assert (tmp_path / relative).exists()
