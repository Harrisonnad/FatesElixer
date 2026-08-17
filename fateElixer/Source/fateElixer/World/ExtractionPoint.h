// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/ElixirInteractable.h"
#include "ExtractionPoint.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/**
 * Phase 0 gray-box extraction point: any player interacting with it ends the run for the whole party,
 * observed identically by every connected client at effectively the same time. Real run/score state is
 * a later-phase concern -- this proves the replication pattern the real extraction system builds on.
 */
UCLASS()
class AExtractionPoint : public AActor, public IElixirInteractable
{
	GENERATED_BODY()

public:
	AExtractionPoint();

	virtual void OnInteract_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(APawn* Interactor) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionVolume;

	/** Server-only guard against triggering the run-end broadcast more than once. */
	bool bRunEnded = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnRunExtracted(APawn* ExtractedBy);
};
