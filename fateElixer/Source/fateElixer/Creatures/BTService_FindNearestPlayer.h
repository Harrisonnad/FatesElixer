// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * Writes the nearest player within the controlled AElixirCreature's AggroRange to the selected
 * Blackboard key (clearing it if none are in range). Uses a plain distance scan rather than
 * AIPerceptionComponent, matching the project's existing sphere-overlap detection pattern
 * (InteractionComponent) instead of adding sense-config plugin surface this phase doesn't need.
 */
UCLASS()
class UBTService_FindNearestPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FindNearestPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// Diagnostic only: logs on change rather than every tick. Shared across instances of this service
	// (BT service nodes aren't per-instance by default) -- fine while Phase 1 only ever has one
	// creature; would need real per-instance state (NodeMemory) if multiple creatures run this
	// simultaneously later.
	TWeakObjectPtr<AActor> Debug_LastLoggedTarget;
};
