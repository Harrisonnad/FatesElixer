// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ElixirTestWorld.h"
#include "../World/ReagentNode.h"
#include "../Interaction/ElixirInteractable.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReagentNode_CanInteractBeforeGather, "Elixir.World.ReagentNode.CanInteractIsTrueBeforeGather", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FReagentNode_CanInteractBeforeGather::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AReagentNode* Node = TestWorld.Get()->SpawnActor<AReagentNode>();
	if (!TestNotNull(TEXT("ReagentNode spawned"), Node))
	{
		return false;
	}

	TestTrue(TEXT("A freshly spawned reagent node can be interacted with"), IElixirInteractable::Execute_CanInteract(Node, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReagentNode_OnInteractGathersWithAuthority, "Elixir.World.ReagentNode.OnInteractGathersWithAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FReagentNode_OnInteractGathersWithAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AReagentNode* Node = TestWorld.Get()->SpawnActor<AReagentNode>();
	if (!TestNotNull(TEXT("ReagentNode spawned"), Node))
	{
		return false;
	}
	TestTrue(TEXT("A newly spawned actor has authority by default"), Node->HasAuthority());

	IElixirInteractable::Execute_OnInteract(Node, nullptr);

	TestFalse(TEXT("Gathering the node with authority makes it uninteractable"), IElixirInteractable::Execute_CanInteract(Node, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReagentNode_OnInteractIsIdempotent, "Elixir.World.ReagentNode.OnInteractIsIdempotent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FReagentNode_OnInteractIsIdempotent::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AReagentNode* Node = TestWorld.Get()->SpawnActor<AReagentNode>();
	if (!TestNotNull(TEXT("ReagentNode spawned"), Node))
	{
		return false;
	}

	IElixirInteractable::Execute_OnInteract(Node, nullptr);
	IElixirInteractable::Execute_OnInteract(Node, nullptr); // second interact must be a no-op, not a crash

	TestFalse(TEXT("A second interact on an already-gathered node stays gathered"), IElixirInteractable::Execute_CanInteract(Node, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReagentNode_OnInteractNoOpWithoutAuthority, "Elixir.World.ReagentNode.OnInteractNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FReagentNode_OnInteractNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AReagentNode* Node = TestWorld.Get()->SpawnActor<AReagentNode>();
	if (!TestNotNull(TEXT("ReagentNode spawned"), Node))
	{
		return false;
	}
	Node->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirInteractable::Execute_OnInteract(Node, nullptr);

	TestTrue(TEXT("A non-authoritative interact must not gather the node"), IElixirInteractable::Execute_CanInteract(Node, nullptr));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
