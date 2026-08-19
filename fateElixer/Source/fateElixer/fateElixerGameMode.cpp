// Copyright Epic Games, Inc. All Rights Reserved.

#include "fateElixerGameMode.h"
#include "fateElixerCharacter.h"
#include "UObject/ConstructorHelpers.h"

#if !UE_BUILD_SHIPPING
#include "Creatures/ElixirCreature.h"
#endif

AfateElixerGameMode::AfateElixerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

#if !UE_BUILD_SHIPPING
void AfateElixerGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirSpawnTestCreature")))
	{
		GetWorld()->SpawnActor<AElixirCreature>(FVector(300.f, 0.f, 100.f), FRotator::ZeroRotator);
	}
}
#endif
