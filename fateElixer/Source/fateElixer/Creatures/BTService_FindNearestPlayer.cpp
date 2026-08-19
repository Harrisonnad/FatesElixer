// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ElixirCreature.h"
#include "../fateElixerCharacter.h"
#include "../Combat/ElixirDamageable.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
	NodeName = TEXT("Find Nearest Player");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindNearestPlayer, BlackboardKey), AActor::StaticClass());
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	AElixirCreature* Creature = AIController ? Cast<AElixirCreature>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Creature || !Blackboard)
	{
		return;
	}

	AActor* Best = nullptr;
	float BestDistSq = FMath::Square(Creature->AggroRange);

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(Creature->GetWorld(), AfateElixerCharacter::StaticClass(), Players);
	for (AActor* PlayerActor : Players)
	{
		// Skip downed players -- nothing to gain by chasing a target that can't be damaged
		// (TryAttack's own CanTakeDamage guard would just no-op every hit), and it wastes the whole
		// engagement instead of going after a live threat.
		if (PlayerActor->Implements<UElixirDamageable>() && !IElixirDamageable::Execute_CanTakeDamage(PlayerActor))
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(Creature->GetActorLocation(), PlayerActor->GetActorLocation());
		if (DistSq <= BestDistSq)
		{
			BestDistSq = DistSq;
			Best = PlayerActor;
		}
	}

	if (Debug_LastLoggedTarget.Get() != Best)
	{
		Debug_LastLoggedTarget = Best;
		UE_LOG(LogTemp, Warning, TEXT("[Elixir] FindNearestPlayer: %s -> target=%s (scanned %d players, AggroRange=%.0f, creature at %s)"),
			*GetNameSafe(Creature), *GetNameSafe(Best), Players.Num(), Creature->AggroRange, *Creature->GetActorLocation().ToString());
	}

	Blackboard->SetValueAsObject(GetSelectedBlackboardKey(), Best);
}
