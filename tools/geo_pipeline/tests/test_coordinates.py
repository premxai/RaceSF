import pytest

from sf_racer_geo.coordinates import CoordinateConverter
from sf_racer_geo.settings import load_map_config


def test_world_origin_maps_to_local_zero() -> None:
    config = load_map_config()
    converter = CoordinateConverter(config)
    x_m, y_m = converter.geographic_to_local(
        config.world_origin.longitude, config.world_origin.latitude
    )
    assert abs(x_m) < 1e-6
    assert abs(y_m) < 1e-6


def test_unreal_conversion_negates_northing_once() -> None:
    assert CoordinateConverter.local_to_unreal(12.5, 3.0, 1.25) == (1250.0, -300.0, 125.0)


def test_geographic_round_trip() -> None:
    converter = CoordinateConverter(load_map_config())
    local = converter.geographic_to_local(-122.3937, 37.7955)
    longitude, latitude = converter.local_to_geographic(*local)
    assert longitude == pytest.approx(-122.3937, abs=1e-7)
    assert latitude == pytest.approx(37.7955, abs=1e-7)
