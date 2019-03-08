// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterLibrary.h"
#include "GameFramework/Info.h"
#include "PlayerSkillTreeManager.generated.h"

class UGameplaySkillBase;

/** Struct containing skill tree slot information */
USTRUCT(BlueprintType)
struct EOD_API FSkillTreeSlot
{
	GENERATED_USTRUCT_BODY()

	/** Determines if this slot is unlocked by default */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUnlockedByDefault;

	/** Vocation that this slot belongs to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EVocations Vocation;

	/** Player skill associated with this slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplaySkillBase> PlayerSkill;

	/** The skill tree column that this slot should be placed in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ColumnPosition;

	/** The skill tree row that this slot should be placed in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RowPosition;

	FSkillTreeSlot()
	{
		bUnlockedByDefault = true;
		Vocation = EVocations::Berserker;
		PlayerSkill = NULL;
		ColumnPosition = -1;
		RowPosition = -1;
	}
};

/**
 * Skill tree manager class for locally controlled player
 */
UCLASS(BlueprintType, Blueprintable)
class EOD_API APlayerSkillTreeManager : public AInfo
{
	GENERATED_BODY()

public:
	APlayerSkillTreeManager(const FObjectInitializer& ObjectInitializer);
	
	/** A map of skill group to it's skill tree slot info */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Layout")
	TMap<FName, FSkillTreeSlot> SkillTreeLayout;

	/** Classes of all the player skills that this skill tree contains */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Tree|Skill Layout")
	// TArray<TSubclassOf<UGameplaySkillBase>> PlayerSkillClasses;





};