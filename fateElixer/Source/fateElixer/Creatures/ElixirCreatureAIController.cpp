// Copyright Epic Games, Inc. All Rights Reserved.

#include "ElixirCreatureAIController.h"
#include "ElixirCreature.h"
#include "BehaviorTree/BehaviorTree.h"

void AElixirCreatureAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const AElixirCreature* Creature = Cast<AElixirCreature>(InPawn);
	if (!Creature)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elixir] AIController possessed %s, which is not an AElixirCreature -- no Behavior Tree started."), *GetNameSafe(InPawn));
		return;
	}
	if (!Creature->BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elixir] %s has no BehaviorTreeAsset assigned -- nothing to run."), *GetNameSafe(Creature));
		return;
	}

	const bool bStarted = RunBehaviorTree(Creature->BehaviorTreeAsset);
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] RunBehaviorTree(%s) on %s -- started=%d"), *Creature->BehaviorTreeAsset->GetName(), *GetNameSafe(Creature), bStarted);
}
