// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/ElixirInteractable.h"
#include "TravelPoint.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/**
 * Interacting travels every connected player to DestinationLevel via a server travel
 * (UGameplayStatics::OpenLevel with bAbsolute=true) -- the standard listen-server pattern for moving
 * a whole session between levels; connected clients follow automatically over the existing connection.
 *
 * Known caveat: this is a non-seamless travel, so PlayerState/PlayerId gets recreated in the new level.
 * In practice PlayerId is assigned per-connection in a stable order, so UAlchemyComponent's PlayerId-keyed
 * save slot (see AlchemyComponent::MakeSaveSlotName) should carry over correctly for a steady set of
 * connected players -- but this isn't an engine-guaranteed identity mapping. Worth moving to a stable
 * EOS-backed unique id before this matters for real (e.g. players joining/leaving mid-travel).
 */
UCLASS()
class ATravelPoint : public AActor, public IElixirInteractable
{
	GENERATED_BODY()

public:
	ATravelPoint();

	virtual void OnInteract_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(APawn* Interactor) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionVolume;

	/** e.g. "L_Hub" or "L_Phase0_VerticalSlice". Unset (None) disables this travel point. */
	UPROPERTY(EditAnywhere, Category = "Travel")
	FName DestinationLevel;
};
