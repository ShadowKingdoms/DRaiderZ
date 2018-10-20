// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "SkillTreeItemContainer.h"
#include "Core/EODSaveGame.h"
#include "Core/GameSingleton.h"

#include "TextBlock.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

USkillTreeItemContainer::USkillTreeItemContainer(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	ContainerType = EEODContainerType::SkillTree;
}

bool USkillTreeItemContainer::Initialize()
{
	if (!(Super::Initialize() &&
		SkillUpgradeText))
	{
		return false;
	}

	LoadSkillContainerState();
	LoadEODItemInfo();
	RefreshContainerVisuals();

	return true;
}

void USkillTreeItemContainer::NativeConstruct()
{
	Super::NativeConstruct();
}

void USkillTreeItemContainer::NativeDestruct()
{
	Super::NativeDestruct();
}

void USkillTreeItemContainer::RefreshContainerVisuals()
{
	Super::RefreshContainerVisuals();

	UpdateSkillUpgradeText();
}

FORCEINLINE void USkillTreeItemContainer::UpdateSkillUpgradeText()
{
	FString String;
	String = FString::FromInt(SkillState.CurrentUpgradeLevel) + FString("/") + FString::FromInt(SkillState.MaxUpgradeLevel);
	FText Text = FText::FromString(String);
	SkillUpgradeText->SetText(Text);
}

FORCEINLINE void USkillTreeItemContainer::LoadSkillContainerState()
{
	if (SkillGroup == FString())
	{
		return;
	}

	UGameSingleton* GameSingleton = nullptr;
	if (GEngine)
	{
		GameSingleton = Cast<UGameSingleton>(GEngine->GameSingleton);
	}

	if (!GameSingleton)
	{
		return;
	}

	UEODSaveGame* EODSaveGame = Cast<UEODSaveGame>(UGameplayStatics::LoadGameFromSlot(GameSingleton->CurrentSaveSlotName, GameSingleton->UserIndex));
	if (!EODSaveGame)
	{
		return;
	}

	if (EODSaveGame->SkillToStateMap.Contains(SkillGroup))
	{
		SkillState = EODSaveGame->SkillToStateMap[SkillGroup];
	}
}

FORCEINLINE void USkillTreeItemContainer::LoadEODItemInfo()
{
	if (SkillGroup == FString())
	{
		return;
	}

	FString SkillID;
	if (SkillState.CurrentUpgradeLevel > 0)
	{
		SkillID = SkillGroup + FString("_") + FString::FromInt(SkillState.CurrentUpgradeLevel);
	}
	else
	{
		SkillID = SkillGroup + FString("_1");
	}

	// FPlayerSkillTableRow* Skill = UCharacterLibrary::GetPlayerSkill(FName(*SkillID), FString("USkillTreeItemContainer::LoadEODItemInfo(), looking for player skill"));
	FSkillTableRow* Skill = nullptr;
	if (!Skill)
	{
		return;
	}

	EODItemInfo.ItemID = FName(*SkillID);
	if (Skill->bPassiveSkill)
	{
		EODItemInfo.EODItemType = EEODItemType::PassiveSkill;
	}
	else
	{
		EODItemInfo.EODItemType = EEODItemType::ActiveSkill;
	}
	EODItemInfo.Icon = Skill->Icon;
	EODItemInfo.Description = Skill->Description;
	EODItemInfo.InGameName = Skill->InGameName;
	EODItemInfo.StackCount = 1;

	SkillState.MaxUpgradeLevel = Skill->MaxUpgrades;
}
