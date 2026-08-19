// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "fateElixerGameMode.generated.h"

UCLASS(minimalapi)
class AfateElixerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AfateElixerGameMode();

#if !UE_BUILD_SHIPPING
protected:
	/**
	 * Test-only: reads -ElixirSpawnTestCreature and, if present, spawns a bare AElixirCreature (no
	 * BehaviorTreeAsset, no AI -- just the plain C++ actor) so the headless replication harness can
	 * test melee-kills-creature, downed/revive/auto-respawn, etc. without depending on the Behavior
	 * Tree asset being built. See Tests/ReplicationHarness/ for the Phase 0 precedent this follows.
	 */
	virtual void BeginPlay() override;
#endif
};



