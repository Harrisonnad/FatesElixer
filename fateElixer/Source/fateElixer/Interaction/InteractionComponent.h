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

	/** Call from the owning pawn's SetupPlayerInputComponent. */
	void BindInput(UInputComponent* PlayerInputComponent, UInputAction* InteractAction);

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
