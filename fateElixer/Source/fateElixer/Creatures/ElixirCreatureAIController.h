// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ElixirCreatureAIController.generated.h"

/** Runs the possessed AElixirCreature's BehaviorTreeAsset on possession. */
UCLASS()
class AElixirCreatureAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
