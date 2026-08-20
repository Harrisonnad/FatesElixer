// Copyright Epic Games, Inc. All Rights Reserved.

#include "TravelPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ATravelPoint::ATravelPoint()
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

bool ATravelPoint::CanInteract_Implementation(APawn* Interactor) const
{
	return !DestinationLevel.IsNone();
}

void ATravelPoint::OnInteract_Implementation(APawn* Interactor)
{
	if (!HasAuthority() || DestinationLevel.IsNone())
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] TravelPoint %s -- traveling all connected players to %s"), *GetName(), *DestinationLevel.ToString());
	UGameplayStatics::OpenLevel(this, DestinationLevel, true);
}
