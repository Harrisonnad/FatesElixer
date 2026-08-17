// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/ElixirInteractable.h"
#include "ReagentNode.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/**
 * Phase 0 gray-box reagent node: any player can gather it once, and the depleted state replicates
 * identically to all connected clients. Real reagent data (type, yield, rarity) is a later-phase concern
 * -- this proves the replication pattern the real reagent system builds on.
 */
UCLASS()
class AReagentNode : public AActor, public IElixirInteractable
{
	GENERATED_BODY()

public:
	AReagentNode();

	virtual void OnInteract_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(APawn* Interactor) const override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionVolume;

	/** True once this node has been gathered by any player. Replicated so all connected clients agree. */
	UPROPERTY(ReplicatedUsing = OnRep_Gathered)
	bool bGathered = false;

	UFUNCTION()
	void OnRep_Gathered();

	void SetGatheredVisuals(bool bIsGathered);
};
