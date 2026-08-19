"""
Phase 0 -- Interact Input Action setup.

Creates IA_Interact, maps it to the E key in IMC_Default, and assigns it to
BP_ThirdPersonCharacter's InteractAction slot (the UInputAction* property
InteractionComponent::BindInput expects at runtime).

Run from the Unreal Editor:
  Tools -> Execute Python Script... -> select this file
  OR paste into the Output Log's Python console:
    exec(open(r"C:\Users\blitz\projects\fatesElixer\fateElixer\EditorScripts\phase0_input_setup.py").read())
"""

import unreal

ACTIONS_FOLDER = "/Game/ThirdPerson/Input/Actions"
IA_NAME = "IA_Interact"
IMC_PATH = "/Game/ThirdPerson/Input/IMC_Default"
CHARACTER_BP_PATH = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
INTERACT_KEY = "E"

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_asset_lib = unreal.EditorAssetLibrary

# 1. Create IA_Interact (skip if it already exists so this script is safe to re-run).
ia_path = f"{ACTIONS_FOLDER}/{IA_NAME}"
if editor_asset_lib.does_asset_exist(ia_path):
    interact_action = editor_asset_lib.load_asset(ia_path)
    print(f"[phase0] {ia_path} already exists, reusing it.")
else:
    factory = unreal.InputAction_Factory()
    interact_action = asset_tools.create_asset(IA_NAME, ACTIONS_FOLDER, unreal.InputAction, factory)
    print(f"[phase0] Created {ia_path}.")

if interact_action is None:
    raise RuntimeError("[phase0] Failed to create or load IA_Interact.")

editor_asset_lib.save_loaded_asset(interact_action)

# 2. Map it to E in IMC_Default (skip if already mapped).
imc = editor_asset_lib.load_asset(IMC_PATH)
if imc is None:
    raise RuntimeError(f"[phase0] Could not load {IMC_PATH}.")

already_mapped = any(
    mapping.action == interact_action and mapping.key.key_name == INTERACT_KEY
    for mapping in imc.get_editor_property("mappings")
)
if already_mapped:
    print(f"[phase0] IA_Interact is already mapped to {INTERACT_KEY} in IMC_Default.")
else:
    interact_key = unreal.Key()
    interact_key.set_editor_property("key_name", INTERACT_KEY)
    imc.map_key(interact_action, interact_key)
    print(f"[phase0] Mapped IA_Interact to {INTERACT_KEY} in IMC_Default.")

editor_asset_lib.save_loaded_asset(imc)

# 3. Assign IA_Interact to BP_ThirdPersonCharacter's InteractAction property (a C++ UPROPERTY
#    exposed to Blueprint defaults -- same slot the "Class Defaults" panel edits).
character_bp = editor_asset_lib.load_asset(CHARACTER_BP_PATH)
if character_bp is None:
    raise RuntimeError(f"[phase0] Could not load {CHARACTER_BP_PATH}.")

generated_class = character_bp.generated_class()
cdo = unreal.get_default_object(generated_class)
cdo.set_editor_property("InteractAction", interact_action)
editor_asset_lib.save_loaded_asset(character_bp)
print(f"[phase0] Set InteractAction on {CHARACTER_BP_PATH}'s defaults.")

print("[phase0] Input setup complete: IA_Interact created, mapped to E, assigned to the character.")
