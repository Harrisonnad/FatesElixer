// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ElixirTestWorld.h"
#include "../Creatures/ElixirCreature.h"
#include "../fateElixerCharacter.h"
#include "../Combat/ElixirDamageable.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_CanTakeDamageBeforeDeath, "Elixir.Combat.Creature.CanTakeDamageIsTrueBeforeDeath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_CanTakeDamageBeforeDeath::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature))
	{
		return false;
	}

	TestTrue(TEXT("A freshly spawned creature can take damage"), IElixirDamageable::Execute_CanTakeDamage(Creature));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_ApplyDamageDiesAtZeroHealth, "Elixir.Combat.Creature.ApplyDamageDiesAtZeroHealth", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_ApplyDamageDiesAtZeroHealth::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature))
	{
		return false;
	}
	TestTrue(TEXT("A newly spawned actor has authority by default"), Creature->HasAuthority());

	IElixirDamageable::Execute_ApplyDamage(Creature, Creature->MaxHealth, nullptr);

	TestTrue(TEXT("Lethal damage kills the creature"), Creature->bIsDead);
	TestFalse(TEXT("A dead creature can no longer take damage"), IElixirDamageable::Execute_CanTakeDamage(Creature));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_ApplyDamageNoOpWithoutAuthority, "Elixir.Combat.Creature.ApplyDamageNoOpWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_ApplyDamageNoOpWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature))
	{
		return false;
	}
	Creature->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	IElixirDamageable::Execute_ApplyDamage(Creature, Creature->MaxHealth, nullptr);

	TestFalse(TEXT("A non-authoritative damage application must not kill the creature"), Creature->bIsDead);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_TryAttackSucceedsInRangeOffCooldown, "Elixir.Combat.Creature.TryAttackSucceedsInRangeOffCooldown", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_TryAttackSucceedsInRangeOffCooldown::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	AfateElixerCharacter* Target = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature) || !TestNotNull(TEXT("Target spawned"), Target))
	{
		return false;
	}
	// Same location as the creature (default spawn), so well within AttackRange.
	// Health set below MeleeDamage so a single successful hit is directly observable as a downed player.
	Target->CurrentHealth = Creature->MeleeDamage - 1.f;

	const bool bAttacked = Creature->TryAttack(Target);

	TestTrue(TEXT("Attack succeeds when target is in range and off cooldown"), bAttacked);
	TestTrue(TEXT("A successful attack actually applied damage"), Target->bIsDowned);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_TryAttackFailsOutOfRange, "Elixir.Combat.Creature.TryAttackFailsOutOfRange", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_TryAttackFailsOutOfRange::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	AfateElixerCharacter* Target = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature) || !TestNotNull(TEXT("Target spawned"), Target))
	{
		return false;
	}
	Target->SetActorLocation(FVector(Creature->AttackRange * 10.f, 0.f, 0.f)); // far outside AttackRange

	const bool bAttacked = Creature->TryAttack(Target);

	TestFalse(TEXT("Attack fails when target is out of range"), bAttacked);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_TryAttackFailsOnCooldown, "Elixir.Combat.Creature.TryAttackFailsOnCooldown", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_TryAttackFailsOnCooldown::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	AfateElixerCharacter* Target = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature) || !TestNotNull(TEXT("Target spawned"), Target))
	{
		return false;
	}

	const bool bFirstAttack = Creature->TryAttack(Target);
	const bool bSecondAttack = Creature->TryAttack(Target); // immediately after -- still on cooldown

	TestTrue(TEXT("First attack succeeds"), bFirstAttack);
	TestFalse(TEXT("Second immediate attack is blocked by AttackCooldown"), bSecondAttack);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_TryAttackFailsWithoutAuthority, "Elixir.Combat.Creature.TryAttackFailsWithoutAuthority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_TryAttackFailsWithoutAuthority::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	AfateElixerCharacter* Target = TestWorld.Get()->SpawnActor<AfateElixerCharacter>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature) || !TestNotNull(TEXT("Target spawned"), Target))
	{
		return false;
	}
	Creature->SetRole(ROLE_SimulatedProxy); // simulate a client's view of a remote actor

	const bool bAttacked = Creature->TryAttack(Target);

	TestFalse(TEXT("A non-authoritative creature must not be able to attack"), bAttacked);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCreature_TryAttackFailsAgainstDeadTarget, "Elixir.Combat.Creature.TryAttackFailsAgainstDeadTarget", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FCreature_TryAttackFailsAgainstDeadTarget::RunTest(const FString& Parameters)
{
	FElixirTestWorld TestWorld;
	AElixirCreature* Creature = TestWorld.Get()->SpawnActor<AElixirCreature>();
	AElixirCreature* DeadTarget = TestWorld.Get()->SpawnActor<AElixirCreature>();
	if (!TestNotNull(TEXT("Creature spawned"), Creature) || !TestNotNull(TEXT("Target creature spawned"), DeadTarget))
	{
		return false;
	}
	IElixirDamageable::Execute_ApplyDamage(DeadTarget, DeadTarget->MaxHealth, nullptr);
	TestTrue(TEXT("Precondition: target is dead"), DeadTarget->bIsDead);

	const bool bAttacked = Creature->TryAttack(DeadTarget);

	TestFalse(TEXT("Attacking an already-dead target fails"), bAttacked);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
