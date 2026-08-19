"""
Registers Elixir's custom MCP toolset (UElixirEditorToolset, in the fateElixerEditorTools module)
with the Toolset Registry on editor startup, so its tools show up over the Unreal Editor MCP bridge
(list_toolsets / describe_toolset / call_tool) automatically -- no manual step needed each session.

Unreal runs any Content/Python/init_unreal.py it finds after editor startup, once GEditor and its
subsystems are ready, so this is the right place for the registration call (rather than C++ module
StartupModule, which runs too early for GEditor to be guaranteed available).
"""

import unreal

_ELIXIR_TOOLSET_PATH = "/Script/fateElixerEditorTools.ElixirEditorToolset"


def _register_elixir_toolset():
    toolset_class = unreal.load_class(None, _ELIXIR_TOOLSET_PATH)
    if toolset_class is None:
        unreal.log_error(
            f"[Elixir] Could not load {_ELIXIR_TOOLSET_PATH} -- "
            "has the fateElixerEditorTools module been built?"
        )
        return

    if unreal.ToolsetRegistry.is_toolset_class_registered(toolset_class):
        unreal.log("[Elixir] ElixirEditorToolset is already registered.")
        return

    unreal.ToolsetRegistry.register_toolset_class(toolset_class)
    unreal.log("[Elixir] Registered ElixirEditorToolset with the MCP Toolset Registry.")


_register_elixir_toolset()
