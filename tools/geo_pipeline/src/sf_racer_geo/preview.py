"""Generate a dependency-free interactive SVG data preview."""
# ruff: noqa: E501

from __future__ import annotations

import html
import math
from pathlib import Path

from .coordinates import CoordinateConverter
from .settings import PROJECT_ROOT, Landmark, RaceDefinition, RoadEdge, load_map_config

COLORS = {"fastest": "#20d9ff", "balanced": "#33d17a", "scenic": "#f6d32d", "explorer": "#a56de2"}


def generate_preview(
    edges: dict[str, RoadEdge],
    landmarks: list[Landmark],
    races: list[RaceDefinition],
    output_path: Path | None = None,
) -> Path:
    output_path = output_path or PROJECT_ROOT / "data" / "exports" / "sf_mvp" / "preview.html"
    config = load_map_config()
    converter = CoordinateConverter(config)
    all_points = [(x, y) for edge in edges.values() for x, y, _ in edge.points]
    min_x, max_x = min(x for x, _ in all_points), max(x for x, _ in all_points)
    min_y, max_y = min(y for _, y in all_points), max(y for _, y in all_points)
    width, height = max_x - min_x, max_y - min_y

    def point(value: tuple[float, float]) -> str:
        x, y = value
        return f"{x - min_x:.1f},{max_y - y:.1f}"

    roads = "\n".join(
        f'<polyline class="road{" oneway" if edge.one_way else ""}" '
        f'points="{" ".join(point((x, y)) for x, y, _ in edge.points)}"/>'
        for edge in edges.values()
        if edge.driveable
    )
    excluded = "\n".join(
        f'<polyline class="excluded" points="{" ".join(point((x, y)) for x, y, _ in edge.points)}"/>'
        for edge in edges.values()
        if not edge.driveable
    )
    verticals = range(
        math.floor(min_x / config.tile_size_meters),
        math.ceil(max_x / config.tile_size_meters) + 1,
    )
    horizontals = range(
        math.floor(min_y / config.tile_size_meters),
        math.ceil(max_y / config.tile_size_meters) + 1,
    )
    tiles = "".join(
        f'<line class="tile" x1="{x * config.tile_size_meters - min_x}" y1="0" '
        f'x2="{x * config.tile_size_meters - min_x}" y2="{height}"/>'
        for x in verticals
    ) + "".join(
        f'<line class="tile" x1="0" y1="{max_y - y * config.tile_size_meters}" '
        f'x2="{width}" y2="{max_y - y * config.tile_size_meters}"/>'
        for y in horizontals
    )
    routes: list[str] = []
    flagship = next(
        (race for race in races if race.race_id == "ferry_building_to_chase_center"), None
    )
    if flagship:
        for route in flagship.routes:
            route_points = [
                (x, y) for edge_id in route.edge_ids for x, y, _ in edges[edge_id].points
            ]
            routes.append(
                f'<polyline class="route" data-profile="{route.profile}" '
                f'stroke="{COLORS[route.profile]}" points="{" ".join(map(point, route_points))}"/>'
            )
    markers = "\n".join(
        f'<g><circle class="original" cx="{point(converter.geographic_to_local(landmark.longitude, landmark.latitude)).split(",")[0]}" '
        f'cy="{point(converter.geographic_to_local(landmark.longitude, landmark.latitude)).split(",")[1]}" r="8"/>'
        f'<circle class="spawn" cx="{point((landmark.spawn.x_m, landmark.spawn.y_m)).split(",")[0]}" '
        f'cy="{point((landmark.spawn.x_m, landmark.spawn.y_m)).split(",")[1]}" r="10"/>'
        f'<text x="{point((landmark.spawn.x_m, landmark.spawn.y_m)).split(",")[0]}" '
        f'y="{float(point((landmark.spawn.x_m, landmark.spawn.y_m)).split(",")[1]) - 14}">'
        f"{html.escape(landmark.name)}</text></g>"
        for landmark in landmarks
        if landmark.spawn
    )
    legend = "".join(
        f'<label><input type="checkbox" checked data-toggle="{name}">'
        f'<span style="color:{color}">●</span> {name.title()}</label>'
        for name, color in COLORS.items()
    )
    document = f"""<!doctype html>
<html><head><meta charset="utf-8"><title>SF Route Racer data preview</title>
<style>
body{{margin:0;background:#15191f;color:#eef;font:14px system-ui}} aside{{position:fixed;z-index:2;padding:14px;background:#202630dd}}
label{{display:block;margin:6px}} svg{{width:100vw;height:100vh}} .road{{fill:none;stroke:#69717d;stroke-width:2}}
.oneway{{stroke:#9aa3b0;stroke-dasharray:8 5}} .excluded{{fill:none;stroke:#e01b24;stroke-width:5}}
.tile{{stroke:#5e5c64;stroke-width:1}} .route{{fill:none;stroke-width:10;opacity:.8}}
.original{{fill:none;stroke:white;stroke-width:3}} .spawn{{fill:#ffb000;stroke:white;stroke-width:3}}
text{{fill:white;font-size:22px;paint-order:stroke;stroke:#111;stroke-width:4}}
</style></head><body><aside><strong>SF Waterfront MVP</strong>{legend}<p>Wheel: zoom · drag: pan<br>Dashed roads: one-way</p></aside>
<svg viewBox="0 0 {width:.1f} {height:.1f}"><g id="map">{tiles}{roads}{excluded}{"".join(routes)}{markers}</g></svg>
<script>
const svg=document.querySelector('svg');let box=[0,0,{width},{height}],drag=null;
function render(){{svg.setAttribute('viewBox',box.join(' '))}}
svg.onwheel=e=>{{e.preventDefault();const f=e.deltaY>0?1.12:.88;box[2]*=f;box[3]*=f;render()}};
svg.onpointerdown=e=>drag=[e.clientX,e.clientY,...box];svg.onpointermove=e=>{{if(!drag)return;
box[0]=drag[2]-(e.clientX-drag[0])*box[2]/innerWidth;box[1]=drag[3]-(e.clientY-drag[1])*box[3]/innerHeight;render()}};
svg.onpointerup=()=>drag=null;
document.querySelectorAll('[data-toggle]').forEach(x=>x.onchange=()=>document.querySelectorAll(`[data-profile="${{x.dataset.toggle}}"]`).forEach(r=>r.style.display=x.checked?'':'none'));
</script></body></html>"""
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(document, encoding="utf-8")
    return output_path
