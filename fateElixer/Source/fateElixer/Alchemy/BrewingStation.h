// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/ElixirInteractable.h"
#include "BrewingStation.generated.h"

class UStaticMeshComponent;
class USphereComponent;

/**
 * Hub brewing station. Holding Interact here brews whichever known recipe the interactor last
 * selected in the brewing UI (UAlchemyComponent::ServerSelectRecipe) -- the hold itself (see
 * InteractionComponent::BrewHoldDuration) represents the recipe's brew time, mirroring how
 * ReviveHoldDuration represents revive time in Phase 1. Any number of players can interact with
 * the same station concurrently and independently: brewing mutates only the interactor's own
 * UAlchemyComponent, there's no shared state on the station itself to contend over.
 */
UCLASS()
class ABrewingStation : public AActor, public IElixirInteractable
{
	GENERATED_BODY()

public:
	ABrewingStation();

	virtual bool CanInteract_Implementation(APawn* Interactor) const override;
	virtual void OnInteract_Implementation(APawn* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionVolume;
};
