// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTTask_MeleeAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ElixirCreature.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MeleeAttack, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AElixirCreature* Creature = AIController ? Cast<AElixirCreature>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Creature || !Blackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elixir] MeleeAttack task: no Creature or Blackboard -- failing."));
		return EBTNodeResult::Failed;
	}

	// If this logs "None", the task's Target Actor Key was never bound to TargetActor in the BT editor.
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elixir] MeleeAttack task: no target on key '%s' -- failing."), *TargetActorKey.SelectedKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	const bool bAttacked = Creature->TryAttack(Target);
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] MeleeAttack task: TryAttack(%s) -> %d"), *GetNameSafe(Target), bAttacked);
	return EBTNodeResult::Succeeded;
}
