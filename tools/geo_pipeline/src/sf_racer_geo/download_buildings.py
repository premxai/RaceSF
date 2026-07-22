"""Acquire cached building footprints from Overture or an explicit OSM fallback."""

from __future__ import annotations

import json
from datetime import UTC, datetime
from pathlib import Path

from .settings import PROJECT_ROOT, load_map_config


class BuildingDownloadError(RuntimeError):
    pass


def _write_metadata(source: str, version: str | None = None) -> None:
    path = PROJECT_ROOT / "data" / "raw" / "building_source_metadata.json"
    path.write_text(
        json.dumps(
            {
                "dataset": source,
                "version": version or "live snapshot",
                "retrieved_at": datetime.now(UTC).isoformat(),
            },
            indent=2,
        ),
        encoding="utf-8",
    )


def download_buildings(force_download: bool = False, osm_fallback: bool = False) -> Path:
    output = (
        PROJECT_ROOT
        / "data"
        / "raw"
        / ("sf_buildings_osm.geojson" if osm_fallback else "sf_buildings_overture.geojson")
    )
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists() and not force_download:
        metadata_path = PROJECT_ROOT / "data" / "raw" / "building_source_metadata.json"
        metadata = (
            json.loads(metadata_path.read_text(encoding="utf-8")) if metadata_path.exists() else {}
        )
        if "version" not in metadata:
            if osm_fallback:
                _write_metadata("OpenStreetMap building fallback")
            else:
                from overturemaps import core

                _write_metadata("Overture Maps", str(core.get_latest_release()))
        return output
    config = load_map_config()
    bbox = (
        config.bbox.west,
        config.bbox.south,
        config.bbox.east,
        config.bbox.north,
    )
    if osm_fallback:
        try:
            import osmnx as ox

            buildings = ox.features_from_bbox(bbox=bbox, tags={"building": True})
            if buildings.empty:
                raise BuildingDownloadError("OSM returned no building footprints")
            buildings.to_file(output, driver="GeoJSON")
            _write_metadata("OpenStreetMap building fallback")
            return output
        except Exception as exc:
            raise BuildingDownloadError(
                "Explicit OSM building fallback failed; check network and Overpass availability"
            ) from exc
    try:
        from overturemaps import core

        buildings = core.geodataframe("building", bbox=bbox)
        if buildings.empty:
            raise BuildingDownloadError("Overture returned no building footprints")
        buildings.to_file(output, driver="GeoJSON")
        _write_metadata("Overture Maps", str(core.get_latest_release()))
        return output
    except ImportError as exc:
        raise BuildingDownloadError(
            "Overture client is unavailable. Install the 'overturemaps' package, "
            "or rerun with --osm-fallback to explicitly use OSM buildings."
        ) from exc
    except Exception as exc:
        raise BuildingDownloadError(
            "Overture building download failed. Cached data was not modified; "
            "retry later or explicitly use --osm-fallback."
        ) from exc
