// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ElixirDamageable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UElixirDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything that can take combat damage -- players and the Phase 1 melee creature.
 * Mirrors IElixirInteractable's BlueprintNativeEvent + server-authoritative shape from Phase 0.
 */
class IElixirDamageable
{
	GENERATED_BODY()

public:
	/** Server-authoritative: apply damage from DamageInstigator. Implementations must no-op without authority. */
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void ApplyDamage(float DamageAmount, AActor* DamageInstigator);

	/** Server-authoritative check for whether this can currently take damage (false once dead/downed). */
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	bool CanTakeDamage() const;
};
