// Copyright Epic Games, Inc. All Rights Reserved.

#include "BrewingStation.h"
#include "AlchemyComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"

ABrewingStation::ABrewingStation()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetSphereRadius(150.f);
	InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

bool ABrewingStation::CanInteract_Implementation(APawn* Interactor) const
{
	// Affordability is checked at brew time (UAlchemyComponent::TryBrewSelectedRecipe), not here --
	// blocking an unaffordable brew with feedback is the brewing UI's job, per the spec's AC 2.
	return Interactor && Interactor->FindComponentByClass<UAlchemyComponent>() != nullptr;
}

void ABrewingStation::OnInteract_Implementation(APawn* Interactor)
{
	if (!HasAuthority())
	{
		return;
	}
	if (UAlchemyComponent* Alchemy = Interactor ? Interactor->FindComponentByClass<UAlchemyComponent>() : nullptr)
	{
		Alchemy->TryBrewSelectedRecipe();
	}
}
