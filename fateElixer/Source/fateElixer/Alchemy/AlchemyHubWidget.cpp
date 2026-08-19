// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlchemyHubWidget.h"
#include "AlchemyComponent.h"
#include "ElixirAlchemyTypes.h"
#include "../fateElixerCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Types/SlateEnums.h"
#include "Engine/DataTable.h"

void UAlchemyHubWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildLayout();

	if (AfateElixerCharacter* Character = Cast<AfateElixerCharacter>(GetOwningPlayerPawn()))
	{
		BoundAlchemy = Character->GetAlchemyComponent();
	}
	if (BoundAlchemy)
	{
		BoundAlchemy->OnAlchemyStateChanged.AddDynamic(this, &UAlchemyHubWidget::RefreshFromAlchemy);
	}
	RefreshFromAlchemy();
}

void UAlchemyHubWidget::NativeDestruct()
{
	if (BoundAlchemy)
	{
		BoundAlchemy->OnAlchemyStateChanged.RemoveDynamic(this, &UAlchemyHubWidget::RefreshFromAlchemy);
	}
	Super::NativeDestruct();
}

void UAlchemyHubWidget::BuildLayout()
{
	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
	WidgetTree->RootWidget = RootBox;

	FermentDustText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FermentDustText"));
	RootBox->AddChild(FermentDustText);

	for (int32 SlotIndex = 0; SlotIndex < 3; ++SlotIndex)
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *FString::Printf(TEXT("RecipeButton%d"), SlotIndex));
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("RecipeLabel%d"), SlotIndex));
		Btn->AddChild(Label);
		RootBox->AddChild(Btn);
		RecipeButtons[SlotIndex] = Btn;
		RecipeButtonLabels[SlotIndex] = Label;
	}
	RecipeButtons[0]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnSelectRecipe0Clicked);
	RecipeButtons[1]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnSelectRecipe1Clicked);
	RecipeButtons[2]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnSelectRecipe2Clicked);

	for (int32 SlotIndex = 0; SlotIndex < 3; ++SlotIndex)
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *FString::Printf(TEXT("DrinkButton%d"), SlotIndex));
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("DrinkLabel%d"), SlotIndex));
		Btn->AddChild(Label);
		RootBox->AddChild(Btn);
		DrinkButtons[SlotIndex] = Btn;
		DrinkButtonLabels[SlotIndex] = Label;
	}
	DrinkButtons[0]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnDrinkRecipe0Clicked);
	DrinkButtons[1]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnDrinkRecipe1Clicked);
	DrinkButtons[2]->OnClicked.AddDynamic(this, &UAlchemyHubWidget::OnDrinkRecipe2Clicked);

	TreeListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TreeListBox"));
	RootBox->AddChild(TreeListBox);
}

void UAlchemyHubWidget::RefreshFromAlchemy()
{
	if (!BoundAlchemy || !FermentDustText || !TreeListBox)
	{
		return;
	}

	FermentDustText->SetText(FText::Format(NSLOCTEXT("Alchemy", "FermentDust", "Ferment Dust: {0}"), FText::AsNumber(BoundAlchemy->FermentDust)));

	for (int32 SlotIndex = 0; SlotIndex < 3; ++SlotIndex)
	{
		const bool bKnown = BoundAlchemy->KnownRecipeIDs.IsValidIndex(SlotIndex);
		RecipeButtons[SlotIndex]->SetVisibility(bKnown ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		DrinkButtons[SlotIndex]->SetVisibility(ESlateVisibility::Collapsed);
		if (!bKnown)
		{
			continue;
		}

		const FName RecipeID = BoundAlchemy->KnownRecipeIDs[SlotIndex];
		RecipeButtonLabels[SlotIndex]->SetText(FText::Format(NSLOCTEXT("Alchemy", "SelectRecipe", "Select: {0}"), FText::FromName(RecipeID)));

		if (BoundAlchemy->PotionInventory.Contains(RecipeID))
		{
			DrinkButtonLabels[SlotIndex]->SetText(FText::Format(NSLOCTEXT("Alchemy", "DrinkRecipe", "Drink: {0}"), FText::FromName(RecipeID)));
			DrinkButtons[SlotIndex]->SetVisibility(ESlateVisibility::Visible);
		}
	}

	TreeListBox->ClearChildren();
	if (BoundAlchemy->TreeNodeTable)
	{
		static const FString Context = TEXT("UAlchemyHubWidget::RefreshFromAlchemy");
		for (const FName& RowName : BoundAlchemy->TreeNodeTable->GetRowNames())
		{
			const FElixirTreeNode* Node = BoundAlchemy->TreeNodeTable->FindRow<FElixirTreeNode>(RowName, Context);
			if (!Node)
			{
				continue;
			}
			const bool bUnlocked = BoundAlchemy->UnlockedNodeIDs.Contains(RowName);
			UTextBlock* Row = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			Row->SetText(FText::Format(
				NSLOCTEXT("Alchemy", "TreeNodeRow", "[{0}] {1}"),
				bUnlocked ? NSLOCTEXT("Alchemy", "Unlocked", "UNLOCKED") : NSLOCTEXT("Alchemy", "Locked", "locked"),
				Node->DisplayName));
			TreeListBox->AddChild(Row);
		}
	}
}

void UAlchemyHubWidget::SelectRecipeBySlot(int32 SlotIndex)
{
	if (BoundAlchemy && BoundAlchemy->KnownRecipeIDs.IsValidIndex(SlotIndex))
	{
		BoundAlchemy->ServerSelectRecipe(BoundAlchemy->KnownRecipeIDs[SlotIndex]);
	}
}

void UAlchemyHubWidget::DrinkRecipeBySlot(int32 SlotIndex)
{
	if (BoundAlchemy && BoundAlchemy->KnownRecipeIDs.IsValidIndex(SlotIndex))
	{
		BoundAlchemy->ServerDrinkPotion(BoundAlchemy->KnownRecipeIDs[SlotIndex]);
	}
}

void UAlchemyHubWidget::OnSelectRecipe0Clicked() { SelectRecipeBySlot(0); }
void UAlchemyHubWidget::OnSelectRecipe1Clicked() { SelectRecipeBySlot(1); }
void UAlchemyHubWidget::OnSelectRecipe2Clicked() { SelectRecipeBySlot(2); }
void UAlchemyHubWidget::OnDrinkRecipe0Clicked() { DrinkRecipeBySlot(0); }
void UAlchemyHubWidget::OnDrinkRecipe1Clicked() { DrinkRecipeBySlot(1); }
void UAlchemyHubWidget::OnDrinkRecipe2Clicked() { DrinkRecipeBySlot(2); }
