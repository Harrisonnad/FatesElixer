// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ElixirTestWorld.h"
#include "../fateElixerCharacter.h"
#include "../Combat/ElixirDamageable.h"
#include "../Interaction/ElixirInteractable.h"
#include "../World/ReagentNode.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_CanTakeDamageBeforeDowned, "Elixir.Combat.Player.CanTakeDamageIsTrueBeforeDowned", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_CanTakeDamageBeforeDowned::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}

	TestTrue(TEXT("A freshly spawned player can take damage"), IElixirDamageable::Execute_CanTakeDamage(Player));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_ApplyDamageEntersDownedAtZeroHealth, "Elixir.Combat.Player.ApplyDamageEntersDownedAtZeroHealth", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_ApplyDamageEntersDownedAtZeroHealth::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	TestTrue(TEXT("A newly spawned actor has authority by default"), Player->HasAuthority());

	IElixirDamageable::Execute_ApplyDamage(Player, Player->MaxHealth, nullptr);

	TestTrue(TEXT("Lethal damage puts the player in a downed state"), Player->bIsDowned);
	TestFalse(TEXT("A downed player can no longer take damage"), IElixirDamageable::Execute_CanTakeDamage(Player));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_ApplyDamageNoOpWithoutAuthority, "Elixir.Combat.Player.ApplyDamageNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_ApplyDamageNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	Player->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirDamageable::Execute_ApplyDamage(Player, Player->MaxHealth, nullptr);

	TestFalse(TEXT("A non-authoritative damage application must not down the player"), Player->bIsDowned);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_ApplyDamageNoOpWhileAlreadyDowned, "Elixir.Combat.Player.ApplyDamageNoOpWhileAlreadyDowned", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_ApplyDamageNoOpWhileAlreadyDowned::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Player, Player->MaxHealth, nullptr);
	IElixirDamageable::Execute_ApplyDamage(Player, Player->MaxHealth, nullptr); // second hit while downed must not crash

	TestTrue(TEXT("Player stays downed after a second hit"), Player->bIsDowned);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_CanInteractFalseBeforeDowned, "Elixir.Combat.Player.CanInteractIsFalseBeforeDowned", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_CanInteractFalseBeforeDowned::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}

	TestFalse(TEXT("A healthy player is not a valid revive target"), IElixirInteractable::Execute_CanInteract(Player, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_CanInteractTrueWhenDowned, "Elixir.Combat.Player.CanInteractIsTrueWhenDowned", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_CanInteractTrueWhenDowned::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Player = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Player, Player->MaxHealth, nullptr);

	TestTrue(TEXT("A downed player becomes a valid revive target"), IElixirInteractable::Execute_CanInteract(Player, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_OnInteractRevivesDownedPlayer, "Elixir.Combat.Player.OnInteractRevivesDownedPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_OnInteractRevivesDownedPlayer::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Downed = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	AfateElixerCharacter* Reviver = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Downed player spawned"), Downed) || !TestNotNull(TEXT("Reviver spawned"), Reviver))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Downed, Downed->MaxHealth, nullptr);
	TestTrue(TEXT("Precondition: player is downed"), Downed->bIsDowned);

	IElixirInteractable::Execute_OnInteract(Downed, Reviver);

	TestFalse(TEXT("Reviving a teammate clears the downed state"), Downed->bIsDowned);
	TestEqual(TEXT("Reviving restores full health"), Downed->CurrentHealth, Downed->MaxHealth);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_OnInteractSelfReviveBlocked, "Elixir.Combat.Player.OnInteractSelfReviveBlocked", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_OnInteractSelfReviveBlocked::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Downed = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Player spawned"), Downed))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Downed, Downed->MaxHealth, nullptr);
	IElixirInteractable::Execute_OnInteract(Downed, Downed); // a downed player cannot revive themselves

	TestTrue(TEXT("Player stays downed after a self-revive attempt"), Downed->bIsDowned);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_OnInteractNoOpWithoutAuthority, "Elixir.Combat.Player.OnInteractNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_OnInteractNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Downed = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	AfateElixerCharacter* Reviver = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Downed player spawned"), Downed) || !TestNotNull(TEXT("Reviver spawned"), Reviver))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Downed, Downed->MaxHealth, nullptr);
	Downed->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirInteractable::Execute_OnInteract(Downed, Reviver);

	TestTrue(TEXT("A non-authoritative revive attempt must not clear the downed state"), Downed->bIsDowned);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayer_DownedPlayerCannotInteractWithWorld, "Elixir.Combat.Player.DownedPlayerCannotInteractWithWorld", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPlayer_DownedPlayerCannotInteractWithWorld::RunTest(const FString& Parameters)
{
	// Regression test: a downed player must not be able to gather reagents / trigger extraction --
	// only be revived. Move()/MeleeAttack() got a bIsDowned guard directly, but the pre-existing
	// InteractionComponent path (gather/extract/revive) needed its own fix.
	FElixirTestWorld TestWorld;
	AfateElixerCharacter* Downed = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	AReagentNode* Node = TestWorld.Get()->SpawnActor<AReagentNode>();
	if (!TestNotNull(TEXT("Player spawned"), Downed) || !TestNotNull(TEXT("ReagentNode spawned"), Node))
	{
		return false;
	}

	IElixirDamageable::Execute_ApplyDamage(Downed, Downed->MaxHealth, nullptr);
	TestTrue(TEXT("Precondition: player is downed"), Downed->bIsDowned);

	Downed->Debug_ForceInteractWith(Node);

	TestTrue(TEXT("A downed player's interact attempt must not gather the reagent node"), IElixirInteractable::Execute_CanInteract(Node, nullptr));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
