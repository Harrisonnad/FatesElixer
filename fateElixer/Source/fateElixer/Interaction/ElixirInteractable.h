// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ElixirInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UElixirInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything a player can hold-interact with -- reagent nodes and extraction points in
 * Phase 0, hub brewing stations and gated creatures in later phases.
 */
class IElixirInteractable
{
	GENERATED_BODY()

public:
	/** Called on the server once the interactor has held the interact input for the required duration. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(APawn* Interactor);

	/** Server-authoritative check for whether this can currently be interacted with. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(APawn* Interactor) const;
};
