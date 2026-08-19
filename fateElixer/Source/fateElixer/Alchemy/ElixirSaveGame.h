// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ElixirSaveGame.generated.h"

/** Per-player persistent alchemy progress. Written/read by UAlchemyComponent, one slot per player
 * (see UAlchemyComponent::MakeSaveSlotName -- keyed by PlayerState unique id, or PlayerId as an
 * offline/PIE-testing fallback, so simultaneous local test clients don't collide on one save file). */
UCLASS()
class UElixirSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 FermentDust = 0;

	UPROPERTY()
	TArray<FName> KnownRecipeIDs;

	UPROPERTY()
	TArray<FName> UnlockedNodeIDs;

	/** One entry per potion held (a multiset, not counts) -- matches UAlchemyComponent's replicated shape. */
	UPROPERTY()
	TArray<FName> PotionInventory;
};
