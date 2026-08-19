// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInputAction;
class UInputComponent;
struct FInputActionValue;

/**
 * Hold-to-interact component for the player pawn. Finds the nearest IElixirInteractable in range while
 * the interact input is held, and asks the server to resolve the interaction once held long enough.
 * Add this to a pawn and call BindInput() from SetupPlayerInputComponent with the pawn's Interact
 * UInputAction (created/assigned in-editor, per this project's Enhanced Input convention).
 */
UCLASS(ClassGroup = (Elixir), meta = (BlueprintSpawnableComponent))
class UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	/** How far, in cm, the owner can be from an interactable to start interacting with it. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractRange = 200.f;

	/** How long the interact input must be held before the interaction resolves. */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float HoldDuration = 1.f;

	/**
	 * Hold duration used instead of HoldDuration when the target is a Pawn (Phase 1: reviving a downed
	 * teammate) rather than a world interactable like a reagent node -- reviving is meant to take
	 * noticeably longer than gathering.
	 */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ReviveHoldDuration = 3.f;

	/**
	 * Hold duration used instead of HoldDuration when the target is a brewing station (Phase 2) --
	 * represents that recipe's brew time, the same way ReviveHoldDuration represents revive time.
	 */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float BrewHoldDuration = 3.f;

	/** Call from the owning pawn's SetupPlayerInputComponent. */
	void BindInput(UInputComponent* PlayerInputComponent, UInputAction* InteractAction);

#if !UE_BUILD_SHIPPING
	/**
	 * Test-only hook driving the exact same Server RPC a real hold-to-interact input would, without
	 * needing simulated player input. Used by the headless multi-process replication test harness
	 * (Tests/ReplicationHarness/) to prove interaction state replicates correctly across real
	 * client-server connections, not just in-process logic.
	 */
	void Debug_ForceInteract(AActor* Target) { ServerTryInteract(Target); }
#endif

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void OnInteractPressed(const FInputActionValue& Value);
	void OnInteractReleased(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerTryInteract(AActor* Target);

	AActor* FindBestInteractable() const;

	bool bHoldingInteract = false;
	float HeldTime = 0.f;
	TWeakObjectPtr<AActor> CurrentTarget;
};
