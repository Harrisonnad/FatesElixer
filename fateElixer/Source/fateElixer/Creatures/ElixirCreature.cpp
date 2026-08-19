// Copyright Epic Games, Inc. All Rights Reserved.

#include "ElixirCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "BrainComponent.h"

AElixirCreature::AElixirCreature()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AElixirCreature::BeginPlay()
{
	Super::BeginPlay();
	SpawnLocation = GetActorLocation();
}

void AElixirCreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AElixirCreature, bIsDead);
}

void AElixirCreature::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator)
{
	if (!HasAuthority() || bIsDead)
	{
		return;
	}
	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);
	if (CurrentHealth <= 0.f)
	{
		Die();
	}
}

bool AElixirCreature::CanTakeDamage_Implementation() const
{
	return !bIsDead;
}

bool AElixirCreature::TryAttack(AActor* Target)
{
	if (!HasAuthority() || bIsDead || !Target)
	{
		return false;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return false;
	}
	if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) > AttackRange)
	{
		return false;
	}
	if (!Target->Implements<UElixirDamageable>() || !IElixirDamageable::Execute_CanTakeDamage(Target))
	{
		return false;
	}

	LastAttackTime = Now;
	IElixirDamageable::Execute_ApplyDamage(Target, MeleeDamage, this);
	return true;
}

void AElixirCreature::Die()
{
	bIsDead = true;
	OnRep_Dead(); // OnRep isn't called locally on the server -- call explicitly for the host's own view.

	// A dead creature can't deal damage (TryAttack's own bIsDead guard already blocks that), but
	// without this the Behavior Tree would keep pathfinding a hidden, collision-disabled corpse
	// toward players until respawn -- wasted work with no gameplay effect. Stop it cleanly instead.
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Creature died"));
		}
	}

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AElixirCreature::RespawnCreature, RespawnDelay, false);
}

void AElixirCreature::RespawnCreature()
{
	CurrentHealth = MaxHealth;
	bIsDead = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately(); // clear any leftover velocity from the moment it died
	}
	SetActorLocation(SpawnLocation);
	OnRep_Dead();

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->RestartLogic(); // fresh tree run rather than resuming mid-MoveTo from wherever it died
		}
	}
}

void AElixirCreature::OnRep_Dead()
{
	// Observable proof, on every connected process, that the server-authoritative death state replicated.
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] Creature %s dead=%d (replicated)"), *GetName(), bIsDead);
	SetDeadVisuals(bIsDead);
}

void AElixirCreature::SetDeadVisuals(bool bDead)
{
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetVisibility(!bDead);
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(bDead ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	}
	SetActorEnableCollision(!bDead);
}
