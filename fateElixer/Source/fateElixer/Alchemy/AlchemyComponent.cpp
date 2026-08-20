// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlchemyComponent.h"
#include "ElixirSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	// Milestone-2 locked decision: recipes are known from the start this phase (fragment discovery
	// needs zone content that doesn't ship until Phase 4) -- one Sequence 9 recipe per pathway.
	const TArray<FName> StarterRecipeIDs = { FName("R_TransmutationSeq9"), FName("R_WardingSeq9"), FName("R_ExtractionSeq9") };
	constexpr int32 StarterFermentDust = 10;
}

UAlchemyComponent::UAlchemyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UAlchemyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UAlchemyComponent, FermentDust, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAlchemyComponent, KnownRecipeIDs, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAlchemyComponent, UnlockedNodeIDs, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAlchemyComponent, PotionInventory, COND_OwnerOnly);
}

void UAlchemyComponent::OnRep_AlchemyState()
{
	// FermentDust/KnownRecipeIDs/UnlockedNodeIDs/PotionInventory share this OnRep, so a mutation that
	// touches several of them at once would otherwise fire this multiple times in the same frame.
	if (GFrameCounter == LastAlchemyStateBroadcastFrame)
	{
		return;
	}
	LastAlchemyStateBroadcastFrame = GFrameCounter;

	// Observable proof, on the owning process, that server-authoritative alchemy state replicated --
	// same "log on OnRep" pattern as AfateElixerCharacter::OnRep_CurrentHealth/OnRep_Downed.
	UE_LOG(LogTemp, Warning, TEXT("[Elixir] %s alchemy state (replicated): FermentDust=%d, KnownRecipeIDs=%d, PotionInventory=%d, UnlockedNodeIDs=%d"),
		*GetNameSafe(GetOwner()), FermentDust, KnownRecipeIDs.Num(), PotionInventory.Num(), UnlockedNodeIDs.Num());
	OnAlchemyStateChanged.Broadcast();
}

void UAlchemyComponent::EnsureLoaded()
{
	if (bLoaded || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	LoadProgress();
}

FString UAlchemyComponent::MakeSaveSlotName() const
{
	// PlayerId is distinct per connection (including separate PIE client processes on one machine),
	// which is what actually matters for this phase's local save/load. A real online unique net id
	// (FUniqueNetIdRepl::ToString(), stable across sessions on a given platform) would need this
	// module to depend on OnlineSubsystem/NetCore for that symbol to link -- deliberately deferred,
	// same "defer the heavy backend" call the milestone-2 spec already makes for EOS-backed saves.
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const APlayerState* PS = OwnerPawn ? OwnerPawn->GetPlayerState() : nullptr;
	return FString::Printf(TEXT("ElixirAlchemy_PlayerId%d"), PS ? PS->GetPlayerId() : -1);
}

void UAlchemyComponent::LoadProgress()
{
	bLoaded = true;
	const FString Slot = MakeSaveSlotName();
	if (UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		if (const UElixirSaveGame* Save = Cast<UElixirSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0)))
		{
			FermentDust = Save->FermentDust;
			KnownRecipeIDs = Save->KnownRecipeIDs;
			UnlockedNodeIDs = Save->UnlockedNodeIDs;
			PotionInventory = Save->PotionInventory;
			return;
		}
	}

	// Fresh player: milestone-2's starting grant.
	FermentDust = StarterFermentDust;
	KnownRecipeIDs = StarterRecipeIDs;
	UnlockedNodeIDs.Reset();
	PotionInventory.Reset();
	SaveProgress();
}

void UAlchemyComponent::SaveProgress() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetPlayerState())
	{
		// No stable per-player identity yet (e.g. automation tests spawning a bare actor, or a pawn
		// not yet possessed) -- MakeSaveSlotName would fall back to a shared "PlayerId-1" slot that
		// every such caller would collide on. Silently skip rather than write a bogus shared file.
		return;
	}

	UElixirSaveGame* Save = Cast<UElixirSaveGame>(UGameplayStatics::CreateSaveGameObject(UElixirSaveGame::StaticClass()));
	if (!Save)
	{
		return;
	}
	Save->FermentDust = FermentDust;
	Save->KnownRecipeIDs = KnownRecipeIDs;
	Save->UnlockedNodeIDs = UnlockedNodeIDs;
	Save->PotionInventory = PotionInventory;
	UGameplayStatics::SaveGameToSlot(Save, MakeSaveSlotName(), 0);
}

const FElixirPotionRecipe* UAlchemyComponent::FindRecipe(FName RecipeID) const
{
	return PotionRecipeTable ? PotionRecipeTable->FindRow<FElixirPotionRecipe>(RecipeID, TEXT("AlchemyComponent::FindRecipe")) : nullptr;
}

void UAlchemyComponent::ServerSelectRecipe_Implementation(FName RecipeID)
{
	EnsureLoaded();
	if (KnownRecipeIDs.Contains(RecipeID))
	{
		SelectedRecipeID = RecipeID;
	}
}

bool UAlchemyComponent::TryBrewSelectedRecipe()
{
	EnsureLoaded();
	if (SelectedRecipeID.IsNone() || !KnownRecipeIDs.Contains(SelectedRecipeID))
	{
		return false;
	}
	const FElixirPotionRecipe* Recipe = FindRecipe(SelectedRecipeID);
	if (!Recipe || FermentDust < Recipe->FermentDustCost)
	{
		return false; // unaffordable or unknown recipe row -- consumes nothing, per spec AC 2
	}

	FermentDust -= Recipe->FermentDustCost;
	PotionInventory.Add(SelectedRecipeID);
	SaveProgress();

	UE_LOG(LogTemp, Warning, TEXT("[Elixir] %s brewed %s -- FermentDust=%d, PotionInventory.Num=%d"),
		*GetNameSafe(GetOwner()), *SelectedRecipeID.ToString(), FermentDust, PotionInventory.Num());
	OnRep_AlchemyState(); // OnRep isn't called locally on the server -- call it explicitly for the host's own view.
	return true;
}

void UAlchemyComponent::ServerDrinkPotion_Implementation(FName RecipeID)
{
	EnsureLoaded();
	const int32 Index = PotionInventory.Find(RecipeID);
	if (Index == INDEX_NONE)
	{
		return;
	}
	const FElixirPotionRecipe* Recipe = FindRecipe(RecipeID);
	if (!Recipe)
	{
		return;
	}

	PotionInventory.RemoveAt(Index);

	if (!Recipe->UnlocksNodeID.IsNone() && !UnlockedNodeIDs.Contains(Recipe->UnlocksNodeID))
	{
		UnlockedNodeIDs.Add(Recipe->UnlocksNodeID);
	}
	SaveProgress();

	UE_LOG(LogTemp, Warning, TEXT("[Elixir] %s drank %s -- UnlockedNodeIDs.Num=%d (contains %s: %d)"),
		*GetNameSafe(GetOwner()), *RecipeID.ToString(), UnlockedNodeIDs.Num(), *Recipe->UnlocksNodeID.ToString(), UnlockedNodeIDs.Contains(Recipe->UnlocksNodeID));
	OnRep_AlchemyState(); // OnRep isn't called locally on the server -- call it explicitly for the host's own view.
}
