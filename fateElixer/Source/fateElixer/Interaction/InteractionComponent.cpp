// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionComponent.h"
#include "ElixirInteractable.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

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

	HeldTime += DeltaTime;
	if (HeldTime >= HoldDuration)
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
	if (!IElixirInteractable::Execute_CanInteract(Target, OwnerPawn))
	{
		return;
	}
	IElixirInteractable::Execute_OnInteract(Target, OwnerPawn);
}
