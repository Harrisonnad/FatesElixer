// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlchemyHubWidget.generated.h"

class UAlchemyComponent;
class UVerticalBox;
class UTextBlock;
class UButton;

/**
 * Minimal gray-box hub UI: built entirely in C++ (no WidgetBlueprint/Designer step) since this
 * phase's job is proving the brew/drink/unlock loop is actually playable, not visual polish -- a
 * real UMG pass is technical-artist's follow-up. Shows Ferment Dust, the 3 known recipes (select),
 * held potions (drink), and the 12-node tree's lock state. Always on screen for the locally
 * controlled player -- no toggle input exists yet this phase.
 *
 * Hardcoded to exactly 3 recipe/drink slots because Phase 2 only ever grants the 3 starter
 * recipes (see AlchemyComponent's StarterRecipeIDs) -- revisit if a later phase adds more.
 */
UCLASS()
class UAlchemyHubWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY()
	TObjectPtr<UAlchemyComponent> BoundAlchemy;

	UPROPERTY()
	TObjectPtr<UTextBlock> FermentDustText;

	UPROPERTY()
	TObjectPtr<UButton> RecipeButtons[3];

	UPROPERTY()
	TObjectPtr<UTextBlock> RecipeButtonLabels[3];

	UPROPERTY()
	TObjectPtr<UButton> DrinkButtons[3];

	UPROPERTY()
	TObjectPtr<UTextBlock> DrinkButtonLabels[3];

	UPROPERTY()
	TObjectPtr<UVerticalBox> TreeListBox;

	void BuildLayout();

	// UFUNCTION so it can bind to UAlchemyComponent::OnAlchemyStateChanged via AddDynamic.
	UFUNCTION()
	void RefreshFromAlchemy();

	UFUNCTION()
	void OnSelectRecipe0Clicked();
	UFUNCTION()
	void OnSelectRecipe1Clicked();
	UFUNCTION()
	void OnSelectRecipe2Clicked();
	UFUNCTION()
	void OnDrinkRecipe0Clicked();
	UFUNCTION()
	void OnDrinkRecipe1Clicked();
	UFUNCTION()
	void OnDrinkRecipe2Clicked();

	void SelectRecipeBySlot(int32 SlotIndex);
	void DrinkRecipeBySlot(int32 SlotIndex);
};
