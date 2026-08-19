// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReagentNode.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"

AReagentNode::AReagentNode()
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
	InteractionVolume->SetSphereRadius(120.f);
	InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AReagentNode::BeginPlay()
{
	Super::BeginPlay();
	SetGatheredVisuals(bGathered);
}

void AReagentNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AReagentNode, bGathered);
}

bool AReagentNode::CanInteract_Implementation(APawn* Interactor) const
{
	return !bGathered;
}

void AReagentNode::OnInteract_Implementation(APawn* Interactor)
{
	// Server-authoritative: only the server sets state. Every client (including a listen-server host's
	// own local view) picks up the change through OnRep_Gathered.
	if (!HasAuthority() || bGathered)
	{
		return;
	}
	bGathered = true;
	OnRep_Gathered(); // OnRep isn't called locally on the server -- call it explicitly for the host's own view.
}

void AReagentNode::OnRep_Gathered()
{
	// Observable proof, on every connected process, that the server-authoritative gather state replicated.
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] ReagentNode %s gathered=%d (replicated)"), *GetName(), bGathered);
	SetGatheredVisuals(bGathered);
}

void AReagentNode::SetGatheredVisuals(bool bIsGathered)
{
	Mesh->SetVisibility(!bIsGathered);
	Mesh->SetCollisionEnabled(bIsGathered ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}
