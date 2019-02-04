// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EOD/Core/EODPreprocessors.h"
#include "EOD/Statics/CharacterLibrary.h"

#include "Components/ActorComponent.h"
#include "GameplaySkillsComponent.generated.h"

class AEODCharacterBase;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EOD_API UGameplaySkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGameplaySkillsComponent(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Sets up property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void OnPressingSkillKey(const int32 SkillKeyIndex);

	void OnReleasingSkillKey(const int32 SkillKeyIndex);
	
private:
	/** Returns true if skill key index is invalid */
	inline bool IsSkillKeyIndexInvalid(const int32 SkillKeyIndex) const;

	bool CanUseAnySkill() const;

	bool CanUseSkill(FName SkillID) const;

	bool CanUseSkillAtIndex(const int32 SkillKeyIndex) const;

	/**
	 * Returns the skill group that should be used when pressing a skill key.
	 * @note It won't necessarily return the skill placed at the skill key index
	 */
	FString GetSkillGroupFromSkillKeyIndex(const int32 SkillKeyIndex) const;

	TPair<int32, FString> ActiveSupersedingChainSkillGroup;

	////////////////////////////////////////////////////////////////////////////////
	// EOD
	////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(Transient)
	AEODCharacterBase* EODCharacterOwner;

	// UPROPERTY(ReplicatedUsing = OnRep_ActiveSkillID)
	FName ActiveSkillID;

	FSkillTableRow* ActiveSkill;

	/** Skill button index to skill group map (this describes which skill group is placed on which skill bar button) */
	TMap<int32, FString> SBIndexToSGMap;

	/** Skill group to skill state map */
	TMap<FString, FSkillState> SGToSSMap;

	/** A map of skill group to a boolean that determines whether the skill group is currently in cooldown or not */
	TMap<FString, bool> SGToCooldownMap;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EOD Character Skills")
	int32 MaxNumSkills;

	UPROPERTY(BlueprintReadOnly, Category = "EOD Character Skills")
	float ChainSkillResetDelay;

	/** Data table for character skills */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Skills)
	UDataTable* SkillsDataTable;

	FORCEINLINE FSkillTableRow* GetSkill(FName SkillID, const FString& ContextString = FString("AEODCharacterBase::GetSkill(), character skill lookup")) const
	{
		return SkillsDataTable ? SkillsDataTable->FindRow<FSkillTableRow>(SkillID, ContextString) : nullptr;
	}

	void UseSkill(FName SkillID);

public:
	void SetCurrentActiveSkill(const FName SkillID);

	FORCEINLINE FName GetCurrentActiveSkillID() const { return ActiveSkillID; }

	FORCEINLINE FSkillTableRow* GetCurrentActiveSkill() const { return ActiveSkill; }

	FORCEINLINE AEODCharacterBase* GetCharacterOwner() const { return EODCharacterOwner; }

	FORCEINLINE TMap<int32, FString>& GetSkillBarLayout() { return SBIndexToSGMap; }


	void LoadSkillBarLayout();

	void SaveSkillBarLayout();

	UFUNCTION()
	void AddNewSkill(int32 SkillIndex, FString SkillGroup);


	////////////////////////////////////////////////////////////////////////////////
	// NETWORK
	////////////////////////////////////////////////////////////////////////////////
private:
	// UFUNCTION()
	// void OnRep_ActiveSkillID();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCurrentActiveSkill(const FName SkillID);


};

inline bool UGameplaySkillsComponent::IsSkillKeyIndexInvalid(const int32 SkillKeyIndex) const
{
	return (SkillKeyIndex <= 0) || !SBIndexToSGMap.Contains(SkillKeyIndex);
}
