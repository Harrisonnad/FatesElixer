// Copyright Epic Games, Inc. All Rights Reserved.

#include "fateElixerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Interaction/InteractionComponent.h"
#include "Alchemy/AlchemyComponent.h"
#include "Alchemy/AlchemyHubWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"
#include "World/ExtractionPoint.h"

#if !UE_BUILD_SHIPPING
#include "World/ReagentNode.h"
#include "World/TravelPoint.h"
#include "Creatures/ElixirCreature.h"
#include "Alchemy/BrewingStation.h"
#endif

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AfateElixerCharacter

AfateElixerCharacter::AfateElixerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Hold-to-interact for reagent nodes / extraction points (Phase 0) and hub stations / gated creatures (later phases)
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	// Per-player alchemy state: recipes, reagents, potions, unlocked tree nodes (Phase 2)
	AlchemyComponent = CreateDefaultSubobject<UAlchemyComponent>(TEXT("AlchemyComponent"));

	CurrentHealth = MaxHealth;

#if !UE_BUILD_SHIPPING
	// Test-only: -ElixirDownedDurationOverride=<seconds> shortens the real 30s wait so the auto-respawn
	// fallback (and solo completability -- no one around to revive you) can be tested in seconds, not
	// minutes. Parsed here rather than gated behind IsLocallyControlled(), since DownedDuration is used
	// server-side in EnterDownedState() regardless of which client is watching.
	float Debug_DownedDurationOverride = 0.f;
	if (FParse::Value(FCommandLine::Get(), TEXT("ElixirDownedDurationOverride="), Debug_DownedDurationOverride) && Debug_DownedDurationOverride > 0.f)
	{
		DownedDuration = Debug_DownedDurationOverride;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// Input

void AfateElixerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Gray-box hub UI (Phase 2): always on screen for the locally controlled player -- no toggle
		// input exists yet this phase, and it's small/non-intrusive enough not to need one.
		if (PlayerController->IsLocalController() && !HubWidgetInstance)
		{
			HubWidgetInstance = CreateWidget<UAlchemyHubWidget>(PlayerController, UAlchemyHubWidget::StaticClass());
			if (HubWidgetInstance)
			{
				HubWidgetInstance->AddToViewport();
			}
		}
	}

#if !UE_BUILD_SHIPPING
	Debug_MaybeScheduleAutoInteract();
#endif
}

void AfateElixerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AfateElixerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AfateElixerCharacter::Look);

		// Interacting (reagent nodes, extraction points, reviving a downed teammate)
		if (InteractionComponent)
		{
			InteractionComponent->BindInput(PlayerInputComponent, InteractAction);
		}

		// Melee attack
		EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &AfateElixerCharacter::MeleeAttack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AfateElixerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDowned)
	{
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AfateElixerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//////////////////////////////////////////////////////////////////////////
// Combat (Phase 1)

void AfateElixerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AfateElixerCharacter, CurrentHealth);
	DOREPLIFETIME(AfateElixerCharacter, bIsDowned);
}

void AfateElixerCharacter::MeleeAttack(const FInputActionValue& Value)
{
	if (bIsDowned)
	{
		return;
	}
	ServerMeleeAttack();
}

void AfateElixerCharacter::ServerMeleeAttack_Implementation()
{
	if (bIsDowned)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastMeleeAttackTime < MeleeCooldown)
	{
		return;
	}
	LastMeleeAttackTime = Now;

	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// A simple sphere in front of the character -- no animation notify exists yet this phase, so the
	// attack resolves instantly on input rather than syncing to a swing anim.
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation() + GetActorForwardVector() * (MeleeRange * 0.5f),
		MeleeRange,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OverlappingActors
	);

	for (AActor* Candidate : OverlappingActors)
	{
		if (!Candidate || !Candidate->Implements<UElixirDamageable>())
		{
			continue;
		}
		if (!IElixirDamageable::Execute_CanTakeDamage(Candidate))
		{
			continue;
		}
		IElixirDamageable::Execute_ApplyDamage(Candidate, MeleeDamage, this);
	}
}

// Declarations are unconditional (UHT rejects UFUNCTION inside #if !UE_BUILD_SHIPPING); only the
// calling wrappers (Debug_ForceMeleeAttack/Debug_ForceSelfDown) are test-only.
void AfateElixerCharacter::ServerDebugMeleeAttackTarget_Implementation(AActor* Target)
{
	if (!Target || !Target->Implements<UElixirDamageable>())
	{
		return;
	}
	if (!IElixirDamageable::Execute_CanTakeDamage(Target))
	{
		return;
	}
	IElixirDamageable::Execute_ApplyDamage(Target, MeleeDamage, this);
}

void AfateElixerCharacter::ServerDebugSelfDown_Implementation()
{
	IElixirDamageable::Execute_ApplyDamage(this, MaxHealth, nullptr);
}

void AfateElixerCharacter::ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator)
{
	if (!HasAuthority() || bIsDowned)
	{
		return;
	}
	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);
	OnRep_CurrentHealth(); // OnRep isn't called locally on the server -- call explicitly for the host's own view.
	if (CurrentHealth <= 0.f)
	{
		EnterDownedState();
	}
}

bool AfateElixerCharacter::CanTakeDamage_Implementation() const
{
	return !bIsDowned;
}

bool AfateElixerCharacter::CanInteract_Implementation(APawn* Interactor) const
{
	// A downed player becomes a valid hold-to-interact target -- see InteractionComponent's
	// ReviveHoldDuration for why reviving takes longer than gathering a reagent node.
	return bIsDowned;
}

void AfateElixerCharacter::OnInteract_Implementation(APawn* Interactor)
{
	if (!HasAuthority() || !bIsDowned || Interactor == this)
	{
		return;
	}
	Revive();
}

void AfateElixerCharacter::OnRep_CurrentHealth()
{
	// Observable proof, on every connected process, that the server-authoritative HP change replicated.
	UE_LOG(LogTemplateCharacter, Warning, TEXT("[Elixir] %s health=%.0f (replicated)"), *GetName(), CurrentHealth);
}

void AfateElixerCharacter::OnRep_Downed()
{
	UE_LOG(LogTemplateCharacter, Warning, TEXT("[Elixir] %s downed=%d (replicated)"), *GetName(), bIsDowned);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		if (bIsDowned)
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}
		else
		{
			Movement->SetMovementMode(MOVE_Walking);
		}
	}
}

void AfateElixerCharacter::EnterDownedState()
{
	bIsDowned = true;
	OnRep_Downed(); // OnRep isn't called locally on the server -- call explicitly for the host's own view.
	GetWorldTimerManager().SetTimer(DownedTimerHandle, this, &AfateElixerCharacter::AutoRespawn, DownedDuration, false);
}

void AfateElixerCharacter::Revive()
{
	GetWorldTimerManager().ClearTimer(DownedTimerHandle);
	CurrentHealth = MaxHealth;
	bIsDowned = false;
	OnRep_CurrentHealth();
	OnRep_Downed();
}

void AfateElixerCharacter::AutoRespawn()
{
	if (!bIsDowned)
	{
		return;
	}
	// Cut line per 03-roadmap.md: respawn-at-extraction rather than a full bleed-out/permadeath system.
	if (AActor* ExtractionPointActor = UGameplayStatics::GetActorOfClass(GetWorld(), AExtractionPoint::StaticClass()))
	{
		SetActorLocation(ExtractionPointActor->GetActorLocation());
	}
	CurrentHealth = MaxHealth;
	bIsDowned = false;
	OnRep_CurrentHealth();
	OnRep_Downed();
}

#if !UE_BUILD_SHIPPING
void AfateElixerCharacter::Debug_MaybeScheduleAutoInteract()
{
	// Only the pawn this process actually controls should act on this process's command line --
	// otherwise every simulated proxy of every OTHER connected player would also try to fire.
	if (bDebug_AutoInteractScheduled || !IsLocallyControlled())
	{
		return;
	}
	bDebug_AutoInteractScheduled = true;

	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoInteractReagent")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoInteractReagentTimer, this, &AfateElixerCharacter::Debug_AutoInteractReagent, 5.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoInteractExtraction")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoInteractExtractionTimer, this, &AfateElixerCharacter::Debug_AutoInteractExtraction, 8.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoMeleeAttackCreature")))
	{
		// 5 hits at MeleeDamage=25 covers AElixirCreature's default MaxHealth=75 with room to spare.
		Debug_AutoMeleeAttackCreatureHitsRemaining = 5;
		GetWorldTimerManager().SetTimer(Debug_AutoMeleeAttackCreatureTimer, this, &AfateElixerCharacter::Debug_AutoMeleeAttackCreature, 1.f, true, 6.f);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoSelfDown")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoSelfDownTimer, this, &AfateElixerCharacter::Debug_AutoSelfDown, 6.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoReviveTeammate")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoReviveTeammateTimer, this, &AfateElixerCharacter::Debug_AutoReviveTeammate, 10.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoBrewPotion")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoBrewPotionTimer, this, &AfateElixerCharacter::Debug_AutoBrewPotion, 5.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoDrinkPotion")))
	{
		// Fires after AutoBrewPotion's 5s so there's actually a potion to drink when this runs.
		GetWorldTimerManager().SetTimer(Debug_AutoDrinkPotionTimer, this, &AfateElixerCharacter::Debug_AutoDrinkPotion, 7.f, false);
	}
	if (FParse::Param(FCommandLine::Get(), TEXT("ElixirAutoTravel")))
	{
		GetWorldTimerManager().SetTimer(Debug_AutoTravelTimer, this, &AfateElixerCharacter::Debug_AutoTravel, 5.f, false);
	}
}

void AfateElixerCharacter::Debug_AutoInteractReagent()
{
	if (AActor* Node = UGameplayStatics::GetActorOfClass(GetWorld(), AReagentNode::StaticClass()))
	{
		if (InteractionComponent)
		{
			InteractionComponent->Debug_ForceInteract(Node);
		}
	}
}

void AfateElixerCharacter::Debug_AutoInteractExtraction()
{
	if (AActor* Point = UGameplayStatics::GetActorOfClass(GetWorld(), AExtractionPoint::StaticClass()))
	{
		if (InteractionComponent)
		{
			InteractionComponent->Debug_ForceInteract(Point);
		}
	}
}

void AfateElixerCharacter::Debug_AutoMeleeAttackCreature()
{
	if (--Debug_AutoMeleeAttackCreatureHitsRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(Debug_AutoMeleeAttackCreatureTimer);
	}
	if (AActor* Creature = UGameplayStatics::GetActorOfClass(GetWorld(), AElixirCreature::StaticClass()))
	{
		Debug_ForceMeleeAttack(Creature);
	}
}

void AfateElixerCharacter::Debug_AutoSelfDown()
{
	Debug_ForceSelfDown();
}

void AfateElixerCharacter::Debug_ForceInteractWith(AActor* Target)
{
	if (InteractionComponent)
	{
		InteractionComponent->Debug_ForceInteract(Target);
	}
}

void AfateElixerCharacter::Debug_AutoBrewPotion()
{
	// The Sequence 9 Transmutation recipe -- any of the 3 starter recipes would do, this one just
	// picks arbitrarily. Select then force-interact are both Reliable Server RPCs on the same
	// connection, so ordering (select lands before the brew attempt reads it) is guaranteed.
	static const FName RecipeID(TEXT("R_TransmutationSeq9"));
	if (AlchemyComponent)
	{
		AlchemyComponent->ServerSelectRecipe(RecipeID);
	}
	if (AActor* Station = UGameplayStatics::GetActorOfClass(GetWorld(), ABrewingStation::StaticClass()))
	{
		Debug_ForceInteractWith(Station);
	}
}

void AfateElixerCharacter::Debug_AutoDrinkPotion()
{
	static const FName RecipeID(TEXT("R_TransmutationSeq9"));
	if (AlchemyComponent)
	{
		AlchemyComponent->ServerDrinkPotion(RecipeID);
	}
}

void AfateElixerCharacter::Debug_AutoTravel()
{
	if (AActor* Travel = UGameplayStatics::GetActorOfClass(GetWorld(), ATravelPoint::StaticClass()))
	{
		Debug_ForceInteractWith(Travel);
	}
}

void AfateElixerCharacter::Debug_AutoReviveTeammate()
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AfateElixerCharacter::StaticClass(), Players);
	for (AActor* PlayerActor : Players)
	{
		AfateElixerCharacter* Other = Cast<AfateElixerCharacter>(PlayerActor);
		if (Other && Other != this && Other->bIsDowned && InteractionComponent)
		{
			InteractionComponent->Debug_ForceInteract(Other);
			return;
		}
	}
}

#endif
