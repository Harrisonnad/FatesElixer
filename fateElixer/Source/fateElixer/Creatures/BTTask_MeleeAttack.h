// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

/** Instant (non-latent) task: calls the controlled AElixirCreature's TryAttack() against the actor
 * stored at TargetActorKey. AElixirCreature::TryAttack itself no-ops if out of range or on cooldown --
 * this task always reports Succeeded regardless, since "tried and it wasn't off cooldown yet" isn't
 * a task failure worth backing out of the Behavior Tree branch for. */
UCLASS()
class UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeAttack();

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector TargetActorKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
