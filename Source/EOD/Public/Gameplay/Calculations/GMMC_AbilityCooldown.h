// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GMMC_AbilityCooldown.generated.h"

/**
 * 
 */
UCLASS()
class EOD_API UGMMC_AbilityCooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
protected:

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
