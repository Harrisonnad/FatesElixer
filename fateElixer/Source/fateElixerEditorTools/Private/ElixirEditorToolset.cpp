// Copyright Epic Games, Inc. All Rights Reserved.

#include "ElixirEditorToolset.h"
#include "IPythonScriptPlugin.h"
#include "PythonScriptTypes.h"

namespace
{
	FString RunPython(const FString& Command, EPythonCommandExecutionMode Mode)
	{
		FPythonCommandEx PythonCommand;
		PythonCommand.ExecutionMode = Mode;
		// Public scope: share the console's globals/locals so imports and variables persist between
		// calls, exactly as if the script had been typed into the editor's Python console by hand.
		PythonCommand.FileExecutionScope = EPythonFileExecutionScope::Public;
		PythonCommand.Command = Command;

		const bool bSuccess = IPythonScriptPlugin::Get()->ExecPythonCommandEx(PythonCommand);

		TStringBuilder<512> Result;
		for (const FPythonLogOutputEntry& Entry : PythonCommand.LogOutput)
		{
			Result.Append(Entry.Output);
			Result.Append(TEXT("\n"));
		}

		if (!bSuccess)
		{
			// On failure CommandResult holds the Python exception trace.
			Result.Append(TEXT("ERROR: "));
			Result.Append(PythonCommand.CommandResult);
		}
		else if (Mode == EPythonCommandExecutionMode::EvaluateStatement)
		{
			Result.Append(PythonCommand.CommandResult);
		}

		return Result.ToString();
	}
}

FString UElixirEditorToolset::ExecutePythonScript(const FString& Script)
{
	return RunPython(Script, EPythonCommandExecutionMode::ExecuteFile);
}

FString UElixirEditorToolset::EvaluatePythonExpression(const FString& Expression)
{
	return RunPython(Expression, EPythonCommandExecutionMode::EvaluateStatement);
}
