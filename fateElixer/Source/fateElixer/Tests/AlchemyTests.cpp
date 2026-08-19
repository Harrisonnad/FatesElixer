// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ElixirTestWorld.h"
#include "../fateElixerCharacter.h"
#include "../Alchemy/AlchemyComponent.h"
#include "../Alchemy/BrewingStation.h"
#include "../Alchemy/ElixirAlchemyTypes.h"
#include "../Interaction/ElixirInteractable.h"
#include "Engine/DataTable.h"

namespace
{
	const FName TestRecipeID(TEXT("R_Test"));
	const FName TestNodeID(TEXT("N_Test"));

	UDataTable* MakeTestRecipeTable(int32 FermentDustCost = 4)
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FElixirPotionRecipe::StaticStruct();
		FElixirPotionRecipe Row;
		Row.FermentDustCost = FermentDustCost;
		Row.UnlocksNodeID = TestNodeID;
		Table->AddRow(TestRecipeID, Row);
		return Table;
	}

	// Seeds a fresh player's AlchemyComponent for testing without touching real save files on disk
	// (Debug_InitializeForTest marks the component already-loaded, bypassing EnsureLoaded/LoadProgress).
	UAlchemyComponent* SetUpAlchemy(AfateElixerCharacter* Player, int32 FermentDust, UDataTable* RecipeTable)
	{
		UAlchemyComponent* Alchemy = Player->GetAlchemyComponent();
		Alchemy->PotionRecipeTable = RecipeTable;
		Alchemy->Debug_InitializeForTest(FermentDust, { TestRecipeID });
		return Alchemy;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_BrewConsumesReagentsAndAddsPotion, "Elixir.Alchemy.Brew.ConsumesReagentsAndAddsPotion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_BrewConsumesReagentsAndAddsPotion::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));

	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);
	const bool bBrewed = Alchemy->TryBrewSelectedRecipe();

	TestTrue(TEXT("Brewing an affordable, selected recipe succeeds"), bBrewed);
	TestEqual(TEXT("Reagent cost is deducted"), Alchemy->FermentDust, 6);
	TestEqual(TEXT("One potion is added to inventory"), Alchemy->PotionInventory.Num(), 1);
	TestTrue(TEXT("The added potion is the brewed recipe"), Alchemy->PotionInventory.Contains(TestRecipeID));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_BrewFailsWhenUnaffordable, "Elixir.Alchemy.Brew.FailsWhenUnaffordable", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_BrewFailsWhenUnaffordable::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 2, MakeTestRecipeTable(4)); // cost 4, only have 2

	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);
	const bool bBrewed = Alchemy->TryBrewSelectedRecipe();

	TestFalse(TEXT("Brewing an unaffordable recipe fails"), bBrewed);
	TestEqual(TEXT("Reagents are untouched on failure"), Alchemy->FermentDust, 2);
	TestEqual(TEXT("No potion is added on failure"), Alchemy->PotionInventory.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_BrewFailsWithoutSelection, "Elixir.Alchemy.Brew.FailsWithoutSelection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_BrewFailsWithoutSelection::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));

	const bool bBrewed = Alchemy->TryBrewSelectedRecipe(); // never selected a recipe

	TestFalse(TEXT("Brewing with nothing selected fails"), bBrewed);
	TestEqual(TEXT("Reagents are untouched"), Alchemy->FermentDust, 10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_SelectUnknownRecipeIsRejected, "Elixir.Alchemy.Brew.SelectUnknownRecipeIsRejected", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_SelectUnknownRecipeIsRejected::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));

	Alchemy->ServerSelectRecipe_Implementation(FName("R_NotKnown"));
	const bool bBrewed = Alchemy->TryBrewSelectedRecipe();

	TestFalse(TEXT("A recipe the player doesn't know cannot be selected or brewed"), bBrewed);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_DrinkPotionUnlocksNode, "Elixir.Alchemy.Drink.UnlocksNode", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_DrinkPotionUnlocksNode::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));
	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);
	TestTrue(TEXT("Precondition: brew succeeds"), Alchemy->TryBrewSelectedRecipe());

	Alchemy->ServerDrinkPotion_Implementation(TestRecipeID);

	TestTrue(TEXT("Drinking the potion unlocks its tree node"), Alchemy->UnlockedNodeIDs.Contains(TestNodeID));
	TestEqual(TEXT("The drunk potion is removed from inventory"), Alchemy->PotionInventory.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_DrinkPotionNoOpWithoutInventory, "Elixir.Alchemy.Drink.NoOpWithoutInventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_DrinkPotionNoOpWithoutInventory::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4)); // never brewed

	Alchemy->ServerDrinkPotion_Implementation(TestRecipeID);

	TestFalse(TEXT("Drinking a potion the player doesn't hold unlocks nothing"), Alchemy->UnlockedNodeIDs.Contains(TestNodeID));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_DrinkingTwicePreservesSingleUnlock, "Elixir.Alchemy.Drink.DrinkingTwicePreservesSingleUnlock", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_DrinkingTwicePreservesSingleUnlock::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));
	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);
	Alchemy->TryBrewSelectedRecipe();
	Alchemy->TryBrewSelectedRecipe(); // two potions brewed (8 reagents spent, 2 left)

	Alchemy->ServerDrinkPotion_Implementation(TestRecipeID);
	Alchemy->ServerDrinkPotion_Implementation(TestRecipeID);

	TestEqual(TEXT("The node appears exactly once even after drinking the same recipe twice"), Alchemy->UnlockedNodeIDs.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_TwoPlayersBrewIndependently, "Elixir.Alchemy.Brew.TwoPlayersBrewIndependently", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_TwoPlayersBrewIndependently::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* PlayerA = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	AfateElixerCharacter* PlayerB = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player A spawned"), PlayerA) || !TestNotNull(TEXT("Player B spawned"), PlayerB))
	{
		return false;
	}
	UDataTable* SharedTable = MakeTestRecipeTable(4); // same station/table, distinct per-player state
	UAlchemyComponent* AlchemyA = SetUpAlchemy(PlayerA, 10, SharedTable);
	UAlchemyComponent* AlchemyB = SetUpAlchemy(PlayerB, 10, SharedTable);

	AlchemyA->ServerSelectRecipe_Implementation(TestRecipeID);
	AlchemyA->TryBrewSelectedRecipe();

	TestEqual(TEXT("Player A's reagents are spent"), AlchemyA->FermentDust, 6);
	TestEqual(TEXT("Player B's reagents are untouched by Player A's brew"), AlchemyB->FermentDust, 10);
	TestEqual(TEXT("Player B has no potion from Player A's brew"), AlchemyB->PotionInventory.Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_BrewingStationOnInteractBrewsSelectedRecipe, "Elixir.Alchemy.BrewingStation.OnInteractBrewsSelectedRecipe", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_BrewingStationOnInteractBrewsSelectedRecipe::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	ABrewingStation* Station = TestWorld.Get()->SpawnActor<ABrewingStation>();
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Station spawned"), Station))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));
	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);

	TestTrue(TEXT("A player with an AlchemyComponent can interact with the station"), IElixirInteractable::Execute_CanInteract(Station, Player));
	IElixirInteractable::Execute_OnInteract(Station, Player);

	TestEqual(TEXT("Interacting with the station brews the selected recipe"), Alchemy->FermentDust, 6);
	TestEqual(TEXT("One potion is produced"), Alchemy->PotionInventory.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAlchemy_BrewingStationOnInteractNoOpWithoutAuthority, "Elixir.Alchemy.BrewingStation.OnInteractNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FAlchemy_BrewingStationOnInteractNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	ABrewingStation* Station = TestWorld.Get()->SpawnActor<ABrewingStation>();
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Station spawned"), Station))
	{
		return false;
	}
	UAlchemyComponent* Alchemy = SetUpAlchemy(Player, 10, MakeTestRecipeTable(4));
	Alchemy->ServerSelectRecipe_Implementation(TestRecipeID);
	Station->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirInteractable::Execute_OnInteract(Station, Player);

	TestEqual(TEXT("A non-authoritative interact must not brew anything"), Alchemy->FermentDust, 10);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
