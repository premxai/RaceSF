"""Command-line interface for the complete deterministic data pipeline."""

from __future__ import annotations

import json
import logging
from typing import Annotated

import typer
from rich.console import Console

from .download_buildings import download_buildings as acquire_buildings
from .download_roads import download_roads as acquire_roads
from .export_unreal import export_unreal as write_unreal_export
from .graph import build_graph
from .landmark_snapping import snap_all
from .normalize_buildings import normalize_buildings
from .normalize_roads import normalize_file
from .preview import generate_preview
from .route_generation import generate_flagship
from .settings import PROJECT_ROOT, Building, Landmark, RaceDefinition, RoadEdge, RoadNode
from .validation import save_report, validate_data

app = typer.Typer(no_args_is_help=True, help="Build SF Route Racer's offline map data.")
console = Console()
VerboseOption = Annotated[bool, typer.Option("--verbose", "-v")]


@app.callback()
def configure(verbose: Annotated[bool, typer.Option("--verbose", "-v")] = False) -> None:
    logging.basicConfig(level=logging.DEBUG if verbose else logging.INFO)


def _set_verbose(verbose: bool) -> None:
    if verbose:
        logging.getLogger().setLevel(logging.DEBUG)


def _processed() -> tuple[
    list[RoadNode], list[RoadEdge], list[Landmark], list[RaceDefinition], list[Building]
]:
    directory = PROJECT_ROOT / "data" / "processed"
    graph = json.loads((directory / "graph.json").read_text(encoding="utf-8"))
    nodes = [RoadNode.model_validate(item) for item in graph["nodes"]]
    edges = [RoadEdge.model_validate(item) for item in graph["edges"]]
    landmarks = [
        Landmark.model_validate(item)
        for item in json.loads((directory / "landmarks.json").read_text(encoding="utf-8"))
    ]
    races = [
        RaceDefinition.model_validate(item)
        for item in json.loads((directory / "races.json").read_text(encoding="utf-8"))
    ]
    building_path = directory / "buildings.json"
    buildings = (
        [
            Building.model_validate(item)
            for item in json.loads(building_path.read_text(encoding="utf-8"))
        ]
        if building_path.exists()
        else []
    )
    return nodes, edges, landmarks, races, buildings


def _run(label: str, operation):
    console.print(f"[cyan]{label}[/cyan]")
    try:
        result = operation()
        console.print("[green]Complete[/green]")
        return result
    except Exception as exc:
        console.print(f"[red]Failed:[/red] {exc}")
        raise typer.Exit(code=1) from exc


@app.command("download-roads")
def download_roads_command(
    force_download: Annotated[bool, typer.Option("--force-download")] = False,
    verbose: VerboseOption = False,
) -> None:
    _set_verbose(verbose)
    _run("Downloading or reusing cached OSM roads", lambda: acquire_roads(force_download))


@app.command("download-buildings")
def download_buildings_command(
    force_download: Annotated[bool, typer.Option("--force-download")] = False,
    osm_fallback: Annotated[bool, typer.Option("--osm-fallback")] = False,
    verbose: VerboseOption = False,
) -> None:
    _set_verbose(verbose)
    _run(
        "Downloading or reusing cached building footprints",
        lambda: acquire_buildings(force_download, osm_fallback),
    )


@app.command("normalize")
def normalize_command(verbose: VerboseOption = False) -> None:
    _set_verbose(verbose)

    def operation() -> None:
        _, edges = normalize_file()
        snap_all(edges)
        overture = PROJECT_ROOT / "data" / "raw" / "sf_buildings_overture.geojson"
        fallback = PROJECT_ROOT / "data" / "raw" / "sf_buildings_osm.geojson"
        if overture.exists() or fallback.exists():
            normalize_buildings(overture if overture.exists() else fallback)

    _run("Normalizing roads, landmarks, and available buildings", operation)


@app.command("generate-routes")
def generate_routes_command(verbose: VerboseOption = False) -> None:
    _set_verbose(verbose)

    def operation() -> None:
        nodes, edges, landmarks, _, _ = _processed_for_routes()
        generate_flagship(build_graph(nodes, edges), {edge.id: edge for edge in edges}, landmarks)

    _run("Generating supported race route cards", operation)


def _processed_for_routes() -> tuple[list[RoadNode], list[RoadEdge], list[Landmark], list, list]:
    directory = PROJECT_ROOT / "data" / "processed"
    graph = json.loads((directory / "graph.json").read_text(encoding="utf-8"))
    landmarks = json.loads((directory / "landmarks.json").read_text(encoding="utf-8"))
    return (
        [RoadNode.model_validate(item) for item in graph["nodes"]],
        [RoadEdge.model_validate(item) for item in graph["edges"]],
        [Landmark.model_validate(item) for item in landmarks],
        [],
        [],
    )


@app.command("validate")
def validate_command(verbose: VerboseOption = False) -> None:
    _set_verbose(verbose)

    def operation() -> None:
        nodes, edges, landmarks, races, _ = _processed()
        rejected_path = PROJECT_ROOT / "data" / "processed" / "rejected_roads.json"
        rejected_count = (
            len(json.loads(rejected_path.read_text(encoding="utf-8")))
            if rejected_path.exists()
            else 0
        )
        report = validate_data(
            build_graph(nodes, edges),
            {edge.id: edge for edge in edges},
            landmarks,
            races,
            rejected_count,
        )
        path = save_report(report)
        console.print(f"Report: {path}")
        if not report.valid:
            raise RuntimeError("; ".join(report.errors))

    _run("Validating Milestone 1 acceptance gates", operation)


@app.command("preview")
def preview_command(verbose: VerboseOption = False) -> None:
    _set_verbose(verbose)
    _run(
        "Generating interactive HTML preview",
        lambda: generate_preview(
            {edge.id: edge for edge in _processed()[1]}, _processed()[2], _processed()[3]
        ),
    )


@app.command("export-unreal")
def export_unreal_command(verbose: VerboseOption = False) -> None:
    _set_verbose(verbose)

    def operation() -> None:
        nodes, edges, landmarks, races, buildings = _processed()
        write_unreal_export(nodes, edges, buildings, landmarks, races)

    _run("Exporting versioned Unreal JSON", operation)


@app.command("build-all")
def build_all(
    force_download: Annotated[bool, typer.Option("--force-download")] = False,
    osm_fallback: Annotated[bool, typer.Option("--osm-fallback")] = False,
    verbose: VerboseOption = False,
) -> None:
    _set_verbose(verbose)
    _run("Downloading or reusing cached OSM roads", lambda: acquire_roads(force_download))
    _run(
        "Downloading or reusing cached building footprints",
        lambda: acquire_buildings(force_download, osm_fallback),
    )
    normalize_command(verbose)
    generate_routes_command(verbose)
    validate_command(verbose)
    export_unreal_command(verbose)
    preview_command(verbose)


if __name__ == "__main__":
    app()
