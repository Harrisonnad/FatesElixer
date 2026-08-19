"""
Phase 2 -- alchemy hub gray-box level.

Creates L_Hub: a flat floor, one PlayerStart, one ABrewingStation, and the
same lighting setup (DirectionalLight + SkyLight + SkyAtmosphere) worked out
the hard way for L_Phase0_VerticalSlice -- Lumen + a real-time-capturing
SkyLight clip each other, so the SkyLight is set to a one-time RecaptureSky()
instead. No NavMeshBoundsVolume here: the hub has no AI creatures.

Run from the Unreal Editor:
  Tools -> Execute Python Script... -> select this file
  OR paste into the Output Log's Python console:
    exec(open(r"C:\\Users\\blitz\\projects\\fatesElixer\\fateElixer\\EditorScripts\\phase2_hub_level.py").read())

After running, open L_Hub for PIE testing. It is not the startup map --
L_Phase0_VerticalSlice (the combat/creature test zone) stays the default
until a real hub -> zone transition is wired up (out of scope this phase).
"""

import unreal

LEVEL_PATH = "/Game/Levels/L_Hub"
CUBE_MESH_PATH = "/Engine/BasicShapes/Cube.Cube"

editor_asset_lib = unreal.EditorAssetLibrary
level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# 1. Create (or open) the level.
if editor_asset_lib.does_asset_exist(LEVEL_PATH):
    level_subsystem.load_level(LEVEL_PATH)
    print(f"[phase2] {LEVEL_PATH} already exists, opened it for editing.")
else:
    level_subsystem.new_level(LEVEL_PATH)
    print(f"[phase2] Created {LEVEL_PATH}.")

# 2. Floor: a scaled-up cube acting as ground plane. Small room -- only needs
# to hold a brewing station and be walkable by up to 4 players at once.
cube_mesh = unreal.load_asset(CUBE_MESH_PATH)
floor = actor_subsystem.spawn_actor_from_class(
    unreal.StaticMeshActor, unreal.Vector(0, 0, -50), unreal.Rotator(0, 0, 0)
)
floor.set_actor_label("Floor")
floor.static_mesh_component.set_static_mesh(cube_mesh)
floor.set_actor_scale3d(unreal.Vector(20, 20, 1))

# 3. PlayerStart at the origin.
player_start = actor_subsystem.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(0, 0, 100), unreal.Rotator(0, 0, 0)
)
player_start.set_actor_label("PlayerStart_Hub")

# 4. One ABrewingStation, a short walk from spawn.
station_class = unreal.load_class(None, "/Script/fateElixer.BrewingStation")
if station_class is None:
    raise RuntimeError(
        "[phase2] Could not load /Script/fateElixer.BrewingStation -- "
        "make sure the project has been built since BrewingStation.cpp was added."
    )
station = actor_subsystem.spawn_actor_from_class(
    station_class, unreal.Vector(400, 0, 100), unreal.Rotator(0, 0, 0)
)
station.set_actor_label("BrewingStation_Hub")

# 5. Lighting -- same known-good recipe as L_Phase0_VerticalSlice.
sun = actor_subsystem.spawn_actor_from_class(
    unreal.DirectionalLight, unreal.Vector(0, 0, 300), unreal.Rotator(0, 0, 0)
)
sun.set_actor_label("SunLight_Hub")
sun_rot = unreal.Rotator()
sun_rot.set_editor_property("pitch", -45.0)
sun_rot.set_editor_property("yaw", 30.0)
sun_rot.set_editor_property("roll", 0.0)
sun.set_actor_rotation(sun_rot, False)
sun.get_component_by_class(unreal.DirectionalLightComponent).set_editor_property("intensity", 10.0)

sky_light = actor_subsystem.spawn_actor_from_class(
    unreal.SkyLight, unreal.Vector(0, 0, 300), unreal.Rotator(0, 0, 0)
)
sky_light.set_actor_label("SkyLight_Hub")
sky_comp = sky_light.get_component_by_class(unreal.SkyLightComponent)
sky_comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
sky_comp.set_editor_property("real_time_capture", False)
sky_comp.set_editor_property("intensity", 1.0)

sky_atmosphere = actor_subsystem.spawn_actor_from_class(
    unreal.SkyAtmosphere, unreal.Vector(0, 0, 300), unreal.Rotator(0, 0, 0)
)
sky_atmosphere.set_actor_label("SkyAtmosphere_Hub")

sky_comp.call_method("RecaptureSky", ())

# 6. Save.
level_subsystem.save_current_level()
print(
    f"[phase2] Hub level ready at {LEVEL_PATH}: "
    "Floor, PlayerStart_Hub, BrewingStation_Hub (400,0,100), "
    "SunLight_Hub / SkyLight_Hub / SkyAtmosphere_Hub."
)
