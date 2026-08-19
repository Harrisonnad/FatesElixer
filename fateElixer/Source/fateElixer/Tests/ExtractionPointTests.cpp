// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ElixirTestWorld.h"
#include "../World/ExtractionPoint.h"
#include "../Interaction/ElixirInteractable.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractionPoint_CanInteractBeforeRunEnds, "Elixir.World.ExtractionPoint.CanInteractIsTrueBeforeRunEnds", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FExtractionPoint_CanInteractBeforeRunEnds::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AExtractionPoint* Extraction = TestWorld.Get()->SpawnActor<AExtractionPoint>();
	if (!TestNotNull(TEXT("ExtractionPoint spawned"), Extraction))
	{
		return false;
	}

	TestTrue(TEXT("A freshly spawned extraction point can be interacted with"), IElixirInteractable::Execute_CanInteract(Extraction, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractionPoint_OnInteractEndsRunWithAuthority, "Elixir.World.ExtractionPoint.OnInteractEndsRunWithAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FExtractionPoint_OnInteractEndsRunWithAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AExtractionPoint* Extraction = TestWorld.Get()->SpawnActor<AExtractionPoint>();
	if (!TestNotNull(TEXT("ExtractionPoint spawned"), Extraction))
	{
		return false;
	}
	TestTrue(TEXT("A newly spawned actor has authority by default"), Extraction->HasAuthority());

	IElixirInteractable::Execute_OnInteract(Extraction, nullptr);

	TestFalse(TEXT("Extracting with authority ends the run"), IElixirInteractable::Execute_CanInteract(Extraction, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractionPoint_OnInteractIsIdempotent, "Elixir.World.ExtractionPoint.OnInteractIsIdempotent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FExtractionPoint_OnInteractIsIdempotent::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AExtractionPoint* Extraction = TestWorld.Get()->SpawnActor<AExtractionPoint>();
	if (!TestNotNull(TEXT("ExtractionPoint spawned"), Extraction))
	{
		return false;
	}

	IElixirInteractable::Execute_OnInteract(Extraction, nullptr);
	IElixirInteractable::Execute_OnInteract(Extraction, nullptr); // a second extractor must not re-trigger the run end

	TestFalse(TEXT("A second interact after the run already ended stays ended"), IElixirInteractable::Execute_CanInteract(Extraction, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExtractionPoint_OnInteractNoOpWithoutAuthority, "Elixir.World.ExtractionPoint.OnInteractNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FExtractionPoint_OnInteractNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AExtractionPoint* Extraction = TestWorld.Get()->SpawnActor<AExtractionPoint>();
	if (!TestNotNull(TEXT("ExtractionPoint spawned"), Extraction))
	{
		return false;
	}
	Extraction->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirInteractable::Execute_OnInteract(Extraction, nullptr);

	TestTrue(TEXT("A non-authoritative interact must not end the run"), IElixirInteractable::Execute_CanInteract(Extraction, nullptr));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
