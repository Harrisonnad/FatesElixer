// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ElixirAlchemyTypes.generated.h"

/** The three alchemy tree pathways from 01-systems-spec.md -- unchanged in role across phases. */
UENUM(BlueprintType)
enum class EElixirPathway : uint8
{
	Transmutation,
	Warding,
	Extraction
};

/**
 * A brewable potion recipe. Row name is the RecipeID referenced elsewhere (AlchemyComponent's
 * KnownRecipeIDs/PotionInventory, FElixirTreeNode::RecipeID). Backs DT_Potions.
 */
USTRUCT(BlueprintType)
struct FElixirPotionRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	EElixirPathway Pathway = EElixirPathway::Transmutation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	int32 SequenceTier = 9;

	/** Ferment Dust required to brew one potion. Phase 2 uses a single placeholder reagent type. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	int32 FermentDustCost = 4;

	/** Hold duration (seconds) at a brewing station to produce this potion -- see BrewHoldDuration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	float BrewTime = 3.f;

	/** Tree node this potion unlocks when drunk. None if this recipe doesn't unlock a node. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	FName UnlocksNodeID;
};

/**
 * A slot on a player's alchemy tree. Row name is the NodeID. Backs DT_AlchemyTree. Sequence 6/3/0
 * rows exist with no RecipeID this phase -- locked, no path to unlock until later phases add the
 * reagents/fragments those recipes depend on.
 */
USTRUCT(BlueprintType)
struct FElixirTreeNode : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	EElixirPathway Pathway = EElixirPathway::Transmutation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	int32 SequenceTier = 9;

	/** Recipe that unlocks this node when drunk. None if not yet implemented (locked, no recipe). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	FName RecipeID;

	/** Prerequisite node this one is chained after. None for each pathway's Sequence 9 entry point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Alchemy")
	FName PrerequisiteNodeID;
};
