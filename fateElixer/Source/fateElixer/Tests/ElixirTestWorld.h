// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * RAII helper spinning up a minimal, non-rendering UWorld so automation tests can spawn actors and
 * exercise gameplay logic without a running PIE session. Not for replication behavior -- every actor
 * spawned here defaults to ROLE_Authority; use AActor::SetRole() to test non-authoritative code paths.
 */
class FElixirTestWorld
{
public:
	FElixirTestWorld()
	{
		World = UWorld::CreateWorld(EWorldType::Game, false);
		check(World);

		WorldContext = &GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext->SetCurrentWorld(World);

		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
	}

	~FElixirTestWorld()
	{
		if (World)
		{
			GEngine->DestroyWorldContext(World);
			World->DestroyWorld(false);
		}
	}

	FElixirTestWorld(const FElixirTestWorld&) = delete;
	FElixirTestWorld& operator=(const FElixirTestWorld&) = delete;

	UWorld* Get() const { return World; }

private:
	UWorld* World = nullptr;
	FWorldContext* WorldContext = nullptr;
};

#endif // WITH_DEV_AUTOMATION_TESTS
