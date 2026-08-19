// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

// This module exists to host UElixirEditorToolset (see ElixirEditorToolset.h) -- registration with the
// Toolset Registry happens from Content/Python/init_unreal.py once the editor is fully started, so no
// custom startup/shutdown logic is needed here.
IMPLEMENT_MODULE(FDefaultModuleImpl, fateElixerEditorTools);
