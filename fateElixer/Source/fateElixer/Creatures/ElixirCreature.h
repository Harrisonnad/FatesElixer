// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Combat/ElixirDamageable.h"
#include "ElixirCreature.generated.h"

class UBehaviorTree;

/**
 * Phase 1 fodder-tier melee creature: idles until a player enters AggroRange (found by
 * BTService_FindNearestPlayer), chases via the Behavior Tree's stock MoveTo task, and attacks via
 * BTTask_MeleeAttack -> TryAttack(). Death and the test-loop respawn replicate identically to every
 * connected client, the same discipline Phase 0 applied to the reagent node.
 */
UCLASS()
class AElixirCreature : public ACharacter, public IElixirDamageable
{
	GENERATED_BODY()

public:
	AElixirCreature();

	/** Assigned in-editor to the creature's Behavior Tree asset. */
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, Category = Combat)
	float MaxHealth = 75.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float MeleeDamage = 15.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackCooldown = 1.2f;

	/** How close the creature must be to Target before TryAttack succeeds. Also used as the Behavior
	 * Tree's MoveTo acceptable radius, so "in range to attack" and "done chasing" mean the same thing. */
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AggroRange = 800.f;

	/** Test-loop only, not final spawn logic -- see milestone-1-combat-and-creature.md's Data table. */
	UPROPERTY(EditAnywhere, Category = Combat)
	float RespawnDelay = 15.f;

	/** True once HP reaches 0. Replicated so death/respawn is identical on every connected client. */
	UPROPERTY(ReplicatedUsing = OnRep_Dead, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bIsDead = false;

	//~ Begin IElixirDamageable
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator) override;
	virtual bool CanTakeDamage_Implementation() const override;
	//~ End IElixirDamageable

	/**
	 * Server-only: called by BTTask_MeleeAttack. No-ops (returns false) if Target is out of AttackRange,
	 * still on cooldown, or can't currently take damage. Returns whether an attack was actually made.
	 */
	bool TryAttack(AActor* Target);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Dead();

private:
	float CurrentHealth = 75.f;
	float LastAttackTime = -1000.f;
	FVector SpawnLocation = FVector::ZeroVector;
	FTimerHandle RespawnTimerHandle;

	void Die();
	void RespawnCreature();
	void SetDeadVisuals(bool bDead);
};
