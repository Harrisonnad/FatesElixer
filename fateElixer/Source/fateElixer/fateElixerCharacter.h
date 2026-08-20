// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Interaction/ElixirInteractable.h"
#include "Combat/ElixirDamageable.h"
#include "fateElixerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInteractionComponent;
class UAlchemyComponent;
class UAlchemyHubWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AfateElixerCharacter : public ACharacter, public IElixirInteractable, public IElixirDamageable
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Interact Input Action -- hold to gather a reagent node / trigger an extraction point */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Hold-to-interact component; finds nearby IElixirInteractable actors and resolves interactions on the server */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	/** Per-player alchemy state: recipes, Ferment Dust, brewed potions, unlocked tree nodes (Phase 2). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Alchemy, meta = (AllowPrivateAccess = "true"))
	UAlchemyComponent* AlchemyComponent;

	/** Melee Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MeleeAttackAction;

public:
	AfateElixerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float MeleeDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float MeleeCooldown = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float MeleeRange = 200.f;

	/** How long a downed player waits for a revive before auto-respawning at the extraction point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float DownedDuration = 30.f;

	/** Current HP. Server-authoritative; replicated for HUD/visual purposes in later phases. */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	float CurrentHealth = 100.f;

	/** True once HP has reached 0 -- no movement, no attacking, until revived or auto-respawned. */
	UPROPERTY(ReplicatedUsing = OnRep_Downed, VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bIsDowned = false;

	//~ Begin IElixirDamageable
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator) override;
	virtual bool CanTakeDamage_Implementation() const override;
	//~ End IElixirDamageable

	//~ Begin IElixirInteractable (Phase 1: a downed player is revivable via the normal hold-to-interact flow)
	virtual void OnInteract_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(APawn* Interactor) const override;
	//~ End IElixirInteractable


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for melee attack input */
	void MeleeAttack(const FInputActionValue& Value);


protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_Downed();

private:
	UFUNCTION(Server, Reliable)
	void ServerMeleeAttack();

	/** Server-only cooldown timestamp; re-validated server-side so a client can't bypass MeleeCooldown. */
	float LastMeleeAttackTime = -1000.f;

	void EnterDownedState();
	void Revive();
	void AutoRespawn();

	/** Server-only; started on EnterDownedState, cancelled on Revive. */
	FTimerHandle DownedTimerHandle;

	/** Gray-box hub UI (Phase 2), created once for the locally controlled player in NotifyControllerChanged. */
	UPROPERTY()
	UAlchemyHubWidget* HubWidgetInstance;

	// UFUNCTION Server RPCs must be declared unconditionally (UHT rejects UFUNCTION inside
	// #if !UE_BUILD_SHIPPING) -- only the plain calling wrappers below are test-only-guarded.
	UFUNCTION(Server, Reliable)
	void ServerDebugMeleeAttackTarget(AActor* Target);

	UFUNCTION(Server, Reliable)
	void ServerDebugSelfDown();

#if !UE_BUILD_SHIPPING
private:
	/**
	 * Test-only: reads -ElixirAutoInteractReagent / -ElixirAutoInteractExtraction command-line switches
	 * and, if present, schedules this locally-controlled pawn to interact with the corresponding actor
	 * a few seconds later -- no simulated input needed. Called from NotifyControllerChanged() rather
	 * than BeginPlay(), since a networked client's pawn can run BeginPlay before possession replication
	 * finishes, making IsLocallyControlled() unreliable there. Drives the headless multi-process
	 * replication test harness (Tests/ReplicationHarness/).
	 */
	void Debug_MaybeScheduleAutoInteract();

	bool bDebug_AutoInteractScheduled = false;
	FTimerHandle Debug_AutoInteractReagentTimer;
	FTimerHandle Debug_AutoInteractExtractionTimer;
	void Debug_AutoInteractReagent();
	void Debug_AutoInteractExtraction();

	/**
	 * Test-only, Phase 1: -ElixirAutoMeleeAttackCreature / -ElixirAutoSelfDown / -ElixirAutoReviveTeammate.
	 * Same scheduling pattern as the Phase 0 hooks above, driving the extended replication harness for
	 * combat/downed/revive/auto-respawn -- deliberately independent of the Behavior Tree, so these work
	 * whether or not BT_ElixirCreature has been built yet.
	 */
	/** Repeats so cumulative damage can actually kill the creature -- a single MeleeDamage hit doesn't. */
	int32 Debug_AutoMeleeAttackCreatureHitsRemaining = 0;
	FTimerHandle Debug_AutoMeleeAttackCreatureTimer;
	FTimerHandle Debug_AutoSelfDownTimer;
	FTimerHandle Debug_AutoReviveTeammateTimer;
	void Debug_AutoMeleeAttackCreature();
	void Debug_AutoSelfDown();
	void Debug_AutoReviveTeammate();

	/**
	 * Test-only, Phase 2: -ElixirAutoBrewPotion / -ElixirAutoDrinkPotion. Exercises the brew -> drink
	 * -> unlock loop end to end (select a known recipe, force-interact the hub's brewing station, then
	 * drink the result) ahead of a real UMG brewing/tree UI existing -- same rationale as Phase 1's
	 * melee-attack debug bypass predating real input wiring.
	 */
	FTimerHandle Debug_AutoBrewPotionTimer;
	FTimerHandle Debug_AutoDrinkPotionTimer;
	void Debug_AutoBrewPotion();
	void Debug_AutoDrinkPotion();

	/** Test-only, Phase 2 polish: -ElixirAutoTravel. Force-interacts with any ATravelPoint in the
	 * current level, verifying the hub<->zone server travel actually fires. */
	FTimerHandle Debug_AutoTravelTimer;
	void Debug_AutoTravel();

	/** Bypasses MeleeRange/MeleeCooldown and attacks Target directly, through the real server RPC + damage path. */
	void Debug_ForceMeleeAttack(AActor* Target) { ServerDebugMeleeAttackTarget(Target); }

	/** Applies lethal self-damage through the real server RPC + damage path, for downed/revive/auto-respawn testing. */
	void Debug_ForceSelfDown() { ServerDebugSelfDown(); }

public:
	/** Test-only, public: drives this character's InteractionComponent at a given target, bypassing
	 * range/hold-timing -- used to unit test the incapacitated-player interaction guard. */
	void Debug_ForceInteractWith(AActor* Target);
#endif

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AlchemyComponent subobject **/
	FORCEINLINE class UAlchemyComponent* GetAlchemyComponent() const { return AlchemyComponent; }
	/** Returns InteractionComponent subobject **/
	FORCEINLINE class UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
};

