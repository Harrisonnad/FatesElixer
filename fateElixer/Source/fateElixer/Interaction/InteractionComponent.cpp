// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionComponent.h"
#include "ElixirInteractable.h"
#include "../Combat/ElixirDamageable.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

namespace
{
	/** True if OwnerPawn is currently incapacitated (Phase 1: downed) and shouldn't be able to interact. */
	bool IsIncapacitated(APawn* OwnerPawn)
	{
		return OwnerPawn && OwnerPawn->Implements<UElixirDamageable>() && !IElixirDamageable::Execute_CanTakeDamage(OwnerPawn);
	}
}

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Must be replicated for the Server RPC below to route correctly.
	SetIsReplicatedByDefault(true);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(GetOwner() && GetOwner()->GetLocalRole() != ROLE_SimulatedProxy);
}

void UInteractionComponent::BindInput(UInputComponent* PlayerInputComponent, UInputAction* InteractAction)
{
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InteractAction)
		{
			EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &UInteractionComponent::OnInteractPressed);
			EIC->BindAction(InteractAction, ETriggerEvent::Completed, this, &UInteractionComponent::OnInteractReleased);
			EIC->BindAction(InteractAction, ETriggerEvent::Canceled, this, &UInteractionComponent::OnInteractReleased);
		}
	}
}

void UInteractionComponent::OnInteractPressed(const FInputActionValue& Value)
{
	if (IsIncapacitated(Cast<APawn>(GetOwner())))
	{
		return; // e.g. a downed player cannot gather/extract/revive -- only be revived
	}
	bHoldingInteract = true;
	HeldTime = 0.f;
	CurrentTarget = FindBestInteractable();
}

void UInteractionComponent::OnInteractReleased(const FInputActionValue& Value)
{
	bHoldingInteract = false;
	HeldTime = 0.f;
	CurrentTarget = nullptr;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHoldingInteract || !CurrentTarget.IsValid())
	{
		return;
	}

	// A Pawn target means reviving a downed teammate (Phase 1), which takes noticeably longer than
	// gathering a reagent node or triggering extraction.
	const bool bTargetIsPawn = CurrentTarget.Get() && CurrentTarget.Get()->IsA<APawn>();
	const float RequiredHold = bTargetIsPawn ? ReviveHoldDuration : HoldDuration;

	HeldTime += DeltaTime;
	if (HeldTime >= RequiredHold)
	{
		ServerTryInteract(CurrentTarget.Get());
		bHoldingInteract = false;
		HeldTime = 0.f;
		CurrentTarget = nullptr;
	}
}

AActor* UInteractionComponent::FindBestInteractable() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return nullptr;
	}

	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	// Pawns (Phase 1: a downed teammate) use the Pawn collision object type, not World*.
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerPawn);

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		OwnerPawn->GetActorLocation(),
		InteractRange,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OverlappingActors
	);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (AActor* Candidate : OverlappingActors)
	{
		if (!Candidate || !Candidate->Implements<UElixirInteractable>())
		{
			continue;
		}
		if (!IElixirInteractable::Execute_CanInteract(Candidate, OwnerPawn))
		{
			continue;
		}
		const float DistSq = FVector::DistSquared(OwnerPawn->GetActorLocation(), Candidate->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate;
		}
	}
	return Best;
}

void UInteractionComponent::ServerTryInteract_Implementation(AActor* Target)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !Target || !Target->Implements<UElixirInteractable>())
	{
		return;
	}
	// Server-authoritative: client-side gating in OnInteractPressed is just responsiveness, this is
	// the real guard -- a downed player must not be able to gather/extract/revive via a stale RPC.
	if (IsIncapacitated(OwnerPawn))
	{
		return;
	}
	if (!IElixirInteractable::Execute_CanInteract(Target, OwnerPawn))
	{
		return;
	}
	IElixirInteractable::Execute_OnInteract(Target, OwnerPawn);
}
