"""
Phase 0 -- gray-box test level.

Creates L_Phase0_VerticalSlice: a flat floor, one PlayerStart, one AReagentNode,
and one AExtractionPoint, spaced out so distance-to-extraction is a real (if
trivial) choice even in this placeholder box. Matches the milestone-0 spec's
"one replicated reagent node, one extraction point ending the run" goal.

Run from the Unreal Editor:
  Tools -> Execute Python Script... -> select this file
  OR paste into the Output Log's Python console:
    exec(open(r"C:\Users\blitz\projects\fatesElixer\fateElixer\EditorScripts\phase0_graybox_level.py").read())

After running, open L_Phase0_VerticalSlice and set it as the startup map
(Project Settings -> Maps & Modes -> Editor/Game Default Map) or launch it
directly for PIE testing.
"""

import unreal

LEVEL_PATH = "/Game/Levels/L_Phase0_VerticalSlice"
CUBE_MESH_PATH = "/Engine/BasicShapes/Cube.Cube"

editor_asset_lib = unreal.EditorAssetLibrary
level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# 1. Create (or open) the level.
if editor_asset_lib.does_asset_exist(LEVEL_PATH):
    level_subsystem.load_level(LEVEL_PATH)
    print(f"[phase0] {LEVEL_PATH} already exists, opened it for editing.")
else:
    level_subsystem.new_level(LEVEL_PATH)
    print(f"[phase0] Created {LEVEL_PATH}.")

# 2. Floor: a scaled-up cube acting as ground plane.
cube_mesh = unreal.load_asset(CUBE_MESH_PATH)
floor = actor_subsystem.spawn_actor_from_class(
    unreal.StaticMeshActor, unreal.Vector(0, 0, -50), unreal.Rotator(0, 0, 0)
)
floor.set_actor_label("Floor")
floor.static_mesh_component.set_static_mesh(cube_mesh)
floor.set_actor_scale3d(unreal.Vector(40, 40, 1))

# 3. PlayerStart at the origin.
player_start = actor_subsystem.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(0, 0, 100), unreal.Rotator(0, 0, 0)
)
player_start.set_actor_label("PlayerStart_Phase0")

# 4. One AReagentNode, placed away from spawn so gathering it is a real trip.
reagent_class = unreal.load_class(None, "/Script/fateElixer.ReagentNode")
if reagent_class is None:
    raise RuntimeError(
        "[phase0] Could not load /Script/fateElixer.ReagentNode -- "
        "make sure the project has been built since ReagentNode.cpp was added."
    )
reagent_node = actor_subsystem.spawn_actor_from_class(
    reagent_class, unreal.Vector(800, 0, 100), unreal.Rotator(0, 0, 0)
)
reagent_node.set_actor_label("ReagentNode_Phase0")

# 5. One AExtractionPoint, further out so distance-to-extraction is felt.
extraction_class = unreal.load_class(None, "/Script/fateElixer.ExtractionPoint")
if extraction_class is None:
    raise RuntimeError(
        "[phase0] Could not load /Script/fateElixer.ExtractionPoint -- "
        "make sure the project has been built since ExtractionPoint.cpp was added."
    )
extraction_point = actor_subsystem.spawn_actor_from_class(
    extraction_class, unreal.Vector(1600, 0, 100), unreal.Rotator(0, 0, 0)
)
extraction_point.set_actor_label("ExtractionPoint_Phase0")

# 6. Save.
level_subsystem.save_current_level()
print(
    f"[phase0] Gray-box level ready at {LEVEL_PATH}: "
    "Floor, PlayerStart_Phase0, ReagentNode_Phase0 (800,0,100), "
    "ExtractionPoint_Phase0 (1600,0,100)."
)
