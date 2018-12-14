// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/Overlay.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/UserWidget.h"
#include "StatusIndicatorWidget.generated.h"

class UImage;
class USpacer;
class UOverlay;
/**
 * 
 */
UCLASS()
class EOD_API UStatusIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UStatusIndicatorWidget(const FObjectInitializer& ObjectInitializer);

	bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* HealthBarEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* HealthBarFill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* HealthBarBlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* HealthOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USpacer* HealthSpacer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ManaBarEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ManaBarFill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ManaBarBlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* ManaOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USpacer* ManaSpacer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StaminaBarEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StaminaBarFill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StaminaBarBlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* StaminaOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USpacer* StaminaSpacer;

	UFUNCTION()
	void UpdateHealthBar(int32 BaseHealth, int32 MaxHealth, int32 CurrentHealth);

	UFUNCTION()
	void UpdateManaBar(int32 BaseMana, int32 MaxMana, int32 CurrentMana);

	UFUNCTION()
	void UpdateStaminaBar(int32 BaseStamina, int32 MaxStamina, int32 CurrentStamina);

private:
	FORCEINLINE void SetHorizontalSlotSize(class UHorizontalBoxSlot* HBSlot, float FillValue);

	FORCEINLINE void UpdateHealthBar_Internal(int32 BaseHealth, int32 MaxHealth, int32 CurrentHealth);

	FORCEINLINE void UpdateManaBar_Internal(int32 BaseMana, int32 MaxMana, int32 CurrentMana);

	FORCEINLINE void UpdateStaminaBar_Internal(int32 BaseStamina, int32 MaxStamina, int32 CurrentStamina);

};

FORCEINLINE void UStatusIndicatorWidget::UpdateHealthBar_Internal(int32 BaseHealth, int32 MaxHealth, int32 CurrentHealth)
{
	float MBHratio = (float)MaxHealth / (float)BaseHealth;
	MBHratio = MBHratio > 1.0 ? 1.0 : MBHratio;

	UHorizontalBoxSlot* HealthOverlayHSlot = Cast<UHorizontalBoxSlot>(HealthOverlay->Slot);
	UHorizontalBoxSlot* HealthBlockedHSlot = Cast<UHorizontalBoxSlot>(HealthBarBlocked->Slot);

	SetHorizontalSlotSize(HealthOverlayHSlot, MBHratio);
	SetHorizontalSlotSize(HealthBlockedHSlot, 1 - MBHratio);

	float CMHratio = (float)CurrentHealth / (float)MaxHealth;
	CMHratio = CMHratio > 1.0 ? 1.0 : CMHratio;

	UHorizontalBoxSlot* HealthBarFillHSlot = Cast<UHorizontalBoxSlot>(HealthBarFill->Slot);
	UHorizontalBoxSlot* HealthSpacerHSlot = Cast<UHorizontalBoxSlot>(HealthSpacer->Slot);

	SetHorizontalSlotSize(HealthBarFillHSlot, CMHratio);
	SetHorizontalSlotSize(HealthSpacerHSlot, 1 - CMHratio);
}

FORCEINLINE void UStatusIndicatorWidget::UpdateManaBar_Internal(int32 BaseMana, int32 MaxMana, int32 CurrentMana)
{
	float MBMratio = (float)MaxMana / (float)BaseMana;
	MBMratio = MBMratio > 1.0 ? 1.0 : MBMratio;

	UHorizontalBoxSlot* ManaOverlayHSlot = Cast<UHorizontalBoxSlot>(ManaOverlay->Slot);
	UHorizontalBoxSlot* ManaBlockedHSlot = Cast<UHorizontalBoxSlot>(ManaBarBlocked->Slot);

	SetHorizontalSlotSize(ManaOverlayHSlot, MBMratio);
	SetHorizontalSlotSize(ManaBlockedHSlot, 1 - MBMratio);

	float CMMratio = (float)CurrentMana / (float)MaxMana;
	CMMratio = CMMratio > 1.0 ? 1.0 : CMMratio;

	UHorizontalBoxSlot* ManaBarFillHSlot = Cast<UHorizontalBoxSlot>(ManaBarFill->Slot);
	UHorizontalBoxSlot* ManaSpacerHSlot = Cast<UHorizontalBoxSlot>(ManaSpacer->Slot);

	SetHorizontalSlotSize(ManaBarFillHSlot, CMMratio);
	SetHorizontalSlotSize(ManaSpacerHSlot, 1 - CMMratio);
}

FORCEINLINE void UStatusIndicatorWidget::UpdateStaminaBar_Internal(int32 BaseStamina, int32 MaxStamina, int32 CurrentStamina)
{
	float MBSratio = (float)MaxStamina / (float)BaseStamina;
	MBSratio = MBSratio > 1.0 ? 1.0 : MBSratio;

	UHorizontalBoxSlot* StaminaOverlayHSlot = Cast<UHorizontalBoxSlot>(StaminaOverlay->Slot);
	UHorizontalBoxSlot* StaminaBlockedHSlot = Cast<UHorizontalBoxSlot>(StaminaBarBlocked->Slot);

	SetHorizontalSlotSize(StaminaOverlayHSlot, MBSratio);
	SetHorizontalSlotSize(StaminaBlockedHSlot, 1 - MBSratio);

	float CMSratio = (float)CurrentStamina / (float)MaxStamina;
	CMSratio = CMSratio > 1.0 ? 1.0 : CMSratio;

	UHorizontalBoxSlot* StaminaBarFillHSlot = Cast<UHorizontalBoxSlot>(StaminaBarFill->Slot);
	UHorizontalBoxSlot* StaminaSpacerHSlot = Cast<UHorizontalBoxSlot>(StaminaSpacer->Slot);

	SetHorizontalSlotSize(StaminaBarFillHSlot, CMSratio);
	SetHorizontalSlotSize(StaminaSpacerHSlot, 1 - CMSratio);
}

FORCEINLINE void UStatusIndicatorWidget::SetHorizontalSlotSize(UHorizontalBoxSlot* HBSlot, float FillValue)
{
	FSlateChildSize StaminaOverlaySize;
	StaminaOverlaySize.SizeRule = ESlateSizeRule::Fill;
	StaminaOverlaySize.Value = FillValue;
	HBSlot->SetSize(StaminaOverlaySize);
}
