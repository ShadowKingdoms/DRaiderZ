// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "AICharacterBase.h"
#include "GameSingleton.h"
#include "AIStatsComponent.h"
#include "EODAIControllerBase.h"
#include "EODWidgetComponent.h"
#include "AttackDodgedEvent.h"
#include "EODBlueprintFunctionLibrary.h"
#include "EODPlayerController.h"
#include "EODGameInstance.h"
#include "FloatingHealthBarWidget.h"
#include "AISkillsComponent.h"
#include "EODCharacterMovementComponent.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GenericPlatform/GenericPlatformTime.h"
#include "GameFramework/CharacterMovementComponent.h"


const FName AAICharacterBase::AggroWidgetCompName(TEXT("Aggro Indicator"));
const FName AAICharacterBase::HealthWidgetCompName(TEXT("Health Indicator"));

AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAISkillsComponent>(AEODCharacterBase::GameplaySkillsComponentName))
{
	// Mob characters don't have strafe animations and so they must be rotated in the direction of their movement.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	AggroWidgetComp = ObjectInitializer.CreateDefaultSubobject<UEODWidgetComponent>(this, AAICharacterBase::AggroWidgetCompName);
	if (AggroWidgetComp)
	{
		AggroWidgetComp->SetupAttachment(RootComponent);
	}

	HealthWidgetComp = ObjectInitializer.CreateDefaultSubobject<UEODWidgetComponent>(this, AAICharacterBase::HealthWidgetCompName);
	if (HealthWidgetComp)
	{
		HealthWidgetComp->SetupAttachment(RootComponent);
	}

	AggroWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AggroWidgetComp->SetCollisionProfileName(CollisionProfileNames::NoCollision);
	HealthWidgetComp->SetCollisionProfileName(CollisionProfileNames::NoCollision);
}

void AAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetGameplaySkillsComponent())
	{
		GetGameplaySkillsComponent()->InitializeSkills(this);
	}
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetInCombat(false);
	UpdateHealthWidget();
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller && Controller->IsLocalController())
	{
		bool bCanGuardAgainstAttacks = CanGuardAgainstAttacks();
		// If character wants to guard but it's guard is not yet active
		if (bWantsToGuard && !IsBlocking() && bCanGuardAgainstAttacks)
		{
			StartBlockingAttacks();
		}
		// If the character guard is active but it doesn't want to guard anymore
		else if (!bWantsToGuard && IsBlocking())
		{
			StopBlockingAttacks();
		}

		UpdateMovement(DeltaTime);
		UpdateRotation(DeltaTime);
	}
}

void AAICharacterBase::Destroyed()
{
	Super::Destroyed();
}

bool AAICharacterBase::CCEFlinch_Implementation(const float BCAngle)
{
	if (CanFlinch() && FlinchMontage)
	{
		if (BCAngle <= 90)
		{
			PlayAnimMontage(FlinchMontage, 1.f, UCharacterLibrary::SectionName_ForwardFlinch);
		}
		else
		{
			PlayAnimMontage(FlinchMontage, 1.f, UCharacterLibrary::SectionName_BackwardFlinch);
		}

		return true;
	}

	return false;
}

bool AAICharacterBase::CCEInterrupt_Implementation(const float BCAngle)
{
	if (CanInterrupt() && InterruptMontage)
	{
		if (IsUsingAnySkill())
		{
			UGameplaySkillsComponent* SkillsComponent = GetGameplaySkillsComponent();
			check(SkillsComponent);
			SkillsComponent->CancelAllActiveSkills();
		}

		CharacterStateInfo = FCharacterStateInfo(ECharacterState::GotHit);

		if (BCAngle <= 90)
		{
			PlayAnimMontage(InterruptMontage, 1.f, UCharacterLibrary::SectionName_ForwardInterrupt);
		}
		else
		{
			PlayAnimMontage(InterruptMontage, 1.f, UCharacterLibrary::SectionName_BackwardInterrupt);
		}

		return true;
	}

	return false;
}

bool AAICharacterBase::CCEStun_Implementation(const float Duration)
{
	if (CanStun())
	{
		if (IsUsingAnySkill())
		{
			UGameplaySkillsComponent* SkillsComponent = GetGameplaySkillsComponent();
			check(SkillsComponent);
			SkillsComponent->CancelAllActiveSkills();
		}

		CharacterStateInfo = FCharacterStateInfo(ECharacterState::GotHit);

		PlayStunAnimation();

		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &AEODCharacterBase::CCERemoveStun, Duration, false);
		return true;
	}

	return false;
}

void AAICharacterBase::CCERemoveStun_Implementation()
{
	StopStunAnimation();

	ResetState();

	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().ClearTimer(CrowdControlTimerHandle);
	// @todo Restore character state to IdleWalkRun if necessary (if OnMontageBlendingOut event doesn't restore character state to IdleWalkRun)
}

bool AAICharacterBase::CCEFreeze_Implementation(const float Duration)
{
	if (CanFreeze() && GetMesh())
	{
		if (IsUsingAnySkill())
		{
			UGameplaySkillsComponent* SkillsComponent = GetGameplaySkillsComponent();
			check(SkillsComponent);
			SkillsComponent->CancelAllActiveSkills();
		}

		CharacterStateInfo = FCharacterStateInfo(ECharacterState::GotHit);
		GetMesh()->GlobalAnimRateScale = 0.f;

		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &AAICharacterBase::CCEUnfreeze, Duration, false);
		return true;
	}

	return false;
}

void AAICharacterBase::CCEUnfreeze_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->GlobalAnimRateScale = 1.f;
	}

	ResetState();

	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().ClearTimer(CrowdControlTimerHandle);
}

bool AAICharacterBase::CCEKnockdown_Implementation(const float Duration)
{
	if (CanKnockdown() && KnockdownMontage)
	{
		if (IsUsingAnySkill())
		{
			UGameplaySkillsComponent* SkillsComponent = GetGameplaySkillsComponent();
			check(SkillsComponent);
			SkillsComponent->CancelAllActiveSkills();
		}

		CharacterStateInfo = FCharacterStateInfo(ECharacterState::GotHit);

		PlayAnimMontage(KnockdownMontage, 1.f, UCharacterLibrary::SectionName_KnockdownStart);
		GetWorld()->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &AEODCharacterBase::CCEEndKnockdown, Duration, false);
		// PushBack(ImpulseDirection);
		return true;
	}

	return false;
}

void AAICharacterBase::CCEEndKnockdown_Implementation()
{
	PlayAnimMontage(KnockdownMontage, 1.f, UCharacterLibrary::SectionName_KnockdownEnd);
}

bool AAICharacterBase::CCEKnockback_Implementation(const float Duration, const FVector & ImpulseDirection)
{
	if (CanKnockdown() && KnockdownMontage)
	{
		if (IsUsingAnySkill())
		{
			UGameplaySkillsComponent* SkillsComponent = GetGameplaySkillsComponent();
			check(SkillsComponent);
			SkillsComponent->CancelAllActiveSkills();
		}

		CharacterStateInfo = FCharacterStateInfo(ECharacterState::GotHit);

		PlayAnimMontage(KnockdownMontage, 1.f, UCharacterLibrary::SectionName_KnockdownStart);

		UWorld* World = GetWorld();
		check(World);
		World->GetTimerManager().SetTimer(CrowdControlTimerHandle, this, &AEODCharacterBase::CCEEndKnockdown, Duration, false);
		PushBack(ImpulseDirection);
		return true;
	}

	return false;
}

void AAICharacterBase::SetInCombat(bool bValue)
{
	bInCombat = bValue;
	SetIsRunning(bInCombat);

	if (bInCombat)
	{
		UUserWidget* AggroWidget = AggroWidgetComp->GetUserWidgetObject();
		if (AggroWidget)
		{
			AggroWidget->SetVisibility(ESlateVisibility::Visible);
		}

		UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
		if (HealthWidget)
		{
			HealthWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UUserWidget* AggroWidget = AggroWidgetComp->GetUserWidgetObject();
		if (AggroWidget)
		{
			AggroWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
		if (HealthWidget)
		{
			HealthWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AAICharacterBase::OnMontageBlendingOut(UAnimMontage* AnimMontage, bool bInterrupted)
{
	// if (AnimMontage == FlinchAnimMontage || bUsingUniqueSkill)
	if (AnimMontage == FlinchMontage)
	{
		return;
	}

	//~ @todo
	/*
	if (GetCurrentActiveSkill() && GetCurrentActiveSkill()->AnimMontage.Get() == AnimMontage)
	{
		GetLastUsedSkill().LastUsedSkillID = GetCurrentActiveSkillID();
		GetLastUsedSkill().bInterrupted = bInterrupted;

		SetCurrentActiveSkillID(NAME_None);
		SetCurrentActiveSkill(nullptr);
	}
	*/

	//~ @todo
	/*
	if (!bInterrupted && GetCharacterState() != ECharacterState::Dead)
	{
		SetCharacterState(ECharacterState::IdleWalkRun);
	}
	*/
}

void AAICharacterBase::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	// @todo
}

bool AAICharacterBase::CanUseAnySkill() const
{
	return false;
}

bool AAICharacterBase::CanUseSkill(FName SkillID, UGameplaySkillBase* Skill)
{
	bool bCanUseSkill = false;
	if (GetGameplaySkillsComponent())
	{
		uint8 SkillIndex = GetGameplaySkillsComponent()->GetSkillIndexForSkillGroup(SkillID);
		bCanUseSkill = GetGameplaySkillsComponent()->CanUseSkill(SkillIndex, Skill);
	}
	return bCanUseSkill;
}

bool AAICharacterBase::UseSkill_Implementation(FName SkillID, UGameplaySkillBase* Skill)
{
	if (CanUseSkill(SkillID, Skill))
	{
		uint8 SkillIndex = GetGameplaySkillsComponent()->GetSkillIndexForSkillGroup(SkillID);
		GetGameplaySkillsComponent()->TriggerSkill(SkillIndex, Skill);
		return true;
	}

	return false;
}

EEODTaskStatus AAICharacterBase::CheckSkillStatus(FName SkillID)
{
	if (GetCurrentActiveSkillID() == SkillID)
	{
		return EEODTaskStatus::Active;
	}
	
	if (GetLastUsedSkill().LastUsedSkillID != SkillID)
	{
		return EEODTaskStatus::Inactive;
	}

	if (GetLastUsedSkill().bInterrupted)
	{
		return EEODTaskStatus::Aborted;
	}
	else
	{
		return EEODTaskStatus::Finished;
	}
}

FName AAICharacterBase::GetMostWeightedMeleeSkillID(const AEODCharacterBase* TargetCharacter) const
{
	FName MostWeightedSkillID = NAME_None;
	UAISkillsComponent* SkillsComp = Cast<UAISkillsComponent>(GetGameplaySkillsComponent());
	if (SkillsComp)
	{
		MostWeightedSkillID = SkillsComp->GetMostWeightedMeleeSkillID(TargetCharacter);
	}
	return MostWeightedSkillID;
}

void AAICharacterBase::OnSkillActivated(uint8 SkillIndex, FName SkillGroup, UGameplaySkillBase* Skill)
{
	SetLastUsedSkill(FLastUsedSkillInfo());
}

bool AAICharacterBase::CanAssistAlly_Implementation()
{
	return false;
}

void AAICharacterBase::AssistanceRequested_Implementation(const AAICharacterBase* Requestor)
{
}

void AAICharacterBase::UpdateMovement(float DeltaTime)
{
	if (!bCharacterStateAllowsMovement)
	{
		return;
	}

	float NewSpeed = 0.f;
	if (IsBlocking())
	{
		NewSpeed = DefaultWalkSpeedWhileBlocking * MovementSpeedModifier;
	}
	else
	{
		float DesiredSpeed = IsRunning() ? DefaultRunSpeed : DefaultWalkSpeed;
		NewSpeed = DesiredSpeed * MovementSpeedModifier;
	}

	SetWalkSpeed(NewSpeed);

	float CurrentSpeed = GetVelocity().Size();
	if (CurrentSpeed > 0.f)
	{
		SetCharacterMovementDirection(ECharMovementDirection::F);
	}
}

void AAICharacterBase::UpdateRotation(float DeltaTime)
{
}

void AAICharacterBase::UpdateHealthWidget()
{
	if (HealthWidgetComp)
	{
		UFloatingHealthBarWidget* HealthWidget = Cast<UFloatingHealthBarWidget>(HealthWidgetComp->GetUserWidgetObject());
		if (HealthWidget)
		{
			HealthWidget->UpdateHealth(Health.MaxValue, Health.CurrentValue);
		}
	}
}

void AAICharacterBase::UpdateHealth(int32 MaxHealth, int32 CurrentHealth)
{
	Super::UpdateHealth(MaxHealth, CurrentHealth);
	UpdateHealthWidget();
}

void AAICharacterBase::OnRep_InCombat()
{
	SetInCombat(IsInCombat());
}

void AAICharacterBase::OnRep_Health(FCharacterStat& OldHealth)
{
	UpdateHealthWidget();
}
