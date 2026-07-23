"""Create simple Unlit materials for graybox city + player car."""

from __future__ import annotations

import unreal

ASSET_DIR = "/Game/Materials"


def _create_unlit(asset_name: str, color: unreal.LinearColor) -> str:
    asset_path = f"{ASSET_DIR}/{asset_name}"
    if not unreal.EditorAssetLibrary.does_directory_exist(ASSET_DIR):
        unreal.EditorAssetLibrary.make_directory(ASSET_DIR)

    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"SF materials: already exists {asset_path}")
        return asset_path

    factory = unreal.MaterialFactoryNew()
    mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, ASSET_DIR, unreal.Material, factory
    )
    if mat is None:
        raise RuntimeError(f"Failed to create {asset_path}")

    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_OPAQUE)

    color_node = unreal.MaterialEditingLibrary.create_material_expression(
        mat, unreal.MaterialExpressionConstant3Vector, -350, 0
    )
    color_node.set_editor_property("constant", color)

    unreal.MaterialEditingLibrary.connect_material_property(
        color_node, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        color_node, "", unreal.MaterialProperty.MP_BASE_COLOR
    )

    unreal.MaterialEditingLibrary.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(asset_path)
    unreal.log(f"SF materials: created {asset_path}")
    return asset_path


def create_unlit_gray() -> str:
    return _create_unlit("M_SFGrayboxUnlit", unreal.LinearColor(0.72, 0.72, 0.7, 1.0))


def create_unlit_car() -> str:
    # Bright SF taxi / race-accent orange so the pawn reads against gray buildings.
    return _create_unlit("M_SFCarUnlit", unreal.LinearColor(1.0, 0.42, 0.08, 1.0))


def create_unlit_start() -> str:
    # Cool cyan beacon for the race start landmark.
    return _create_unlit("M_SFStartUnlit", unreal.LinearColor(0.15, 0.75, 1.0, 1.0))


def main() -> None:
    create_unlit_gray()
    create_unlit_car()
    create_unlit_start()


if __name__ == "__main__":
    main()
