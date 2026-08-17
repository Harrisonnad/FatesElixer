// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtractionPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"

AExtractionPoint::AExtractionPoint()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetSphereRadius(150.f);
	InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

bool AExtractionPoint::CanInteract_Implementation(APawn* Interactor) const
{
	return !bRunEnded;
}

void AExtractionPoint::OnInteract_Implementation(APawn* Interactor)
{
	if (!HasAuthority() || bRunEnded)
	{
		return;
	}
	bRunEnded = true;
	MulticastOnRunExtracted(Interactor);
}

void AExtractionPoint::MulticastOnRunExtracted_Implementation(APawn* ExtractedBy)
{
	// Phase 0 has no run/score system yet -- this is the observable proof that extraction ends the run
	// for everyone. Real run-end logic (turn-in, rewards, return to hub) is a later-phase concern.
	const FString ExtractorName = ExtractedBy ? ExtractedBy->GetName() : TEXT("Unknown");
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] Run extracted by %s -- ending run for all connected players."), *ExtractorName);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, FString::Printf(TEXT("RUN EXTRACTED by %s"), *ExtractorName));
	}
}
