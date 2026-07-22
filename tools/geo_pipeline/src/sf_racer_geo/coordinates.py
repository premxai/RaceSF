"""The single authoritative geographic/local/Unreal coordinate conversion."""

from __future__ import annotations

from dataclasses import dataclass

from pyproj import Transformer

from .settings import MapConfiguration


@dataclass(frozen=True)
class CoordinateConverter:
    """Convert WGS84 points into origin-relative UTM and Unreal coordinates."""

    config: MapConfiguration

    def __post_init__(self) -> None:
        object.__setattr__(
            self,
            "_project",
            Transformer.from_crs("EPSG:4326", self.config.projection, always_xy=True),
        )
        object.__setattr__(
            self,
            "_unproject",
            Transformer.from_crs(self.config.projection, "EPSG:4326", always_xy=True),
        )
        origin = self._project.transform(
            self.config.world_origin.longitude, self.config.world_origin.latitude
        )
        object.__setattr__(self, "_origin_easting", origin[0])
        object.__setattr__(self, "_origin_northing", origin[1])

    def geographic_to_local(self, longitude: float, latitude: float) -> tuple[float, float]:
        easting, northing = self._project.transform(longitude, latitude)
        return easting - self._origin_easting, northing - self._origin_northing

    def local_to_geographic(self, x_m: float, y_m: float) -> tuple[float, float]:
        return self._unproject.transform(x_m + self._origin_easting, y_m + self._origin_northing)

    @staticmethod
    def local_to_unreal(
        x_m: float, y_m: float, elevation_m: float = 0.0
    ) -> tuple[float, float, float]:
        """Map meters to centimeters; Unreal Y deliberately negates northing."""
        return x_m * 100.0, -y_m * 100.0, elevation_m * 100.0
