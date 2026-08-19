// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElixirAlchemyTypes.h"
#include "AlchemyComponent.generated.h"

/**
 * Per-player alchemy state: known recipes, Ferment Dust stock, brewed potion inventory, and
 * unlocked tree nodes. Added to AfateElixerCharacter. Server-authoritative, mirroring the
 * IElixirDamageable/IElixirInteractable pattern from Phases 0-1: only the server mutates state,
 * clients read replicated results.
 *
 * Unlike Phase 1's HP/downed (broadcast to all clients so teammates see them), a player's reagent
 * stock, recipes, and tree progress are private to that player's own UI -- replicated COND_OwnerOnly.
 */
UCLASS(ClassGroup = (Elixir), meta = (BlueprintSpawnableComponent))
class UAlchemyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAlchemyComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Alchemy")
	UDataTable* PotionRecipeTable = nullptr;

	/** Not read by component logic -- exposed for the tree UI widget to pull node display data from. */
	UPROPERTY(EditDefaultsOnly, Category = "Alchemy")
	UDataTable* TreeNodeTable = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_AlchemyState, VisibleAnywhere, BlueprintReadOnly, Category = "Alchemy")
	int32 FermentDust = 0;

	UPROPERTY(ReplicatedUsing = OnRep_AlchemyState, VisibleAnywhere, BlueprintReadOnly, Category = "Alchemy")
	TArray<FName> KnownRecipeIDs;

	UPROPERTY(ReplicatedUsing = OnRep_AlchemyState, VisibleAnywhere, BlueprintReadOnly, Category = "Alchemy")
	TArray<FName> UnlockedNodeIDs;

	/** One entry per potion held (a multiset, not counts) -- UE doesn't support replicated TMaps, and
	 * this phase only ever needs "do I have at least one of RecipeID", so a flat array is simplest. */
	UPROPERTY(ReplicatedUsing = OnRep_AlchemyState, VisibleAnywhere, BlueprintReadOnly, Category = "Alchemy")
	TArray<FName> PotionInventory;

	/** UI calls this when the player picks a recipe in the brewing panel, before holding Interact at the station. */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Alchemy")
	void ServerSelectRecipe(FName RecipeID);

	/** UI calls this directly -- drinking isn't gated by proximity to the brewing station. */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Alchemy")
	void ServerDrinkPotion(FName RecipeID);

	/** Called by ABrewingStation::OnInteract_Implementation (already server-side, via the normal
	 * hold-to-interact flow). Returns false and mutates nothing if unaffordable or nothing selected. */
	bool TryBrewSelectedRecipe();

	FName GetSelectedRecipeID() const { return SelectedRecipeID; }

#if !UE_BUILD_SHIPPING
	/** Test-only: seeds state directly and marks the component already-loaded, so automation tests
	 * never touch real save files on disk (see EnsureLoaded/LoadProgress). */
	void Debug_InitializeForTest(int32 InFermentDust, const TArray<FName>& InKnownRecipeIDs)
	{
		FermentDust = InFermentDust;
		KnownRecipeIDs = InKnownRecipeIDs;
		bLoaded = true;
	}
#endif

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_AlchemyState();

private:
	/** Server-only working state: which known recipe the brewing station will produce next.
	 * Doesn't need to replicate -- only the server reads it, inside TryBrewSelectedRecipe. */
	FName SelectedRecipeID = NAME_None;

	bool bLoaded = false;

	/** Lazily loads on first server-side alchemy action rather than BeginPlay, since a networked
	 * client's PlayerState (which MakeSaveSlotName depends on) isn't guaranteed valid that early --
	 * the same possession-timing risk already documented on AfateElixerCharacter's debug-interact hook. */
	void EnsureLoaded();

	void LoadProgress();
	void SaveProgress() const;
	FString MakeSaveSlotName() const;

	const FElixirPotionRecipe* FindRecipe(FName RecipeID) const;
};
