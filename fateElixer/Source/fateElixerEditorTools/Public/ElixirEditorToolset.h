// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ToolsetRegistry/ToolsetDefinition.h"
#include "ElixirEditorToolset.generated.h"

/**
 * Elixir's editor automation toolset, exposed over the Unreal Editor MCP bridge once registered
 * (see Content/Python/init_unreal.py). ExecutePythonScript/EvaluatePythonExpression hand an MCP
 * client the same Unreal Python editor-scripting surface EditorScripts/*.py already uses, without a
 * human needing to paste scripts into the editor console by hand -- the general-purpose escape hatch
 * this toolset is built around. Add narrower, purpose-built tools here over time as specific repeated
 * workflows emerge; Python coverage is the floor, not the ceiling.
 */
UCLASS()
class UElixirEditorToolset : public UToolsetDefinition
{
	GENERATED_BODY()

public:
	/**
	 * Runs a Python script (one or more statements) in the running editor and returns its captured
	 * output. Shares the same globals/locals as the editor's Python console, so state (imports,
	 * variables) persists between calls within a session, exactly like typing into the console by hand.
	 */
	UFUNCTION(meta = (AICallable), Category = "Elixir")
	static FString ExecutePythonScript(const FString& Script);

	/** Evaluates a single Python expression in the running editor and returns its repr. */
	UFUNCTION(meta = (AICallable), Category = "Elixir")
	static FString EvaluatePythonExpression(const FString& Expression);
};
