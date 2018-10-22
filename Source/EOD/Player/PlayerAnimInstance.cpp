// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer & ObjectInitializer): Super(ObjectInitializer)
{
	MasterStateMachine_AnimationsBlendTime = 0.f;
	IdleWalkRun_AnimationsBlendTime = 0.2f;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPlayer = CastPawnOwnerToPlayerCharacter();
	
	FScriptDelegate OutDelegate;
	OutDelegate.BindUFunction(this, FName("HandleMontageBlendingOut"));
	OnMontageBlendingOut.AddUnique(OutDelegate);
	
	FScriptDelegate EndDelegate;
	EndDelegate.BindUFunction(this, FName("HandleMontageEnded"));
	OnMontageEnded.AddUnique(EndDelegate);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds); 
}

void UPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void UPlayerAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

bool UPlayerAnimInstance::IsBlocking() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->IsBlocking();
	}

	return false;
}

bool UPlayerAnimInstance::IsFastRunning() const
{
	if (OwningPlayer)
	{
		// return OwningPlayer->
	}

	return false;
}

ECharMovementDirection UPlayerAnimInstance::GetIWRCharMovementDir() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->IWR_CharacterMovementDirection;
	}

	return ECharMovementDirection::None;
}

float UPlayerAnimInstance::GetMovementSpeed() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->GetVelocity().Size();
	}

	return 0.0f;
}

float UPlayerAnimInstance::GetBlockMovementDirectionYaw() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->BlockMovementDirectionYaw;
	}

	return 0.f;
}

EWeaponType UPlayerAnimInstance::GetWeaponAnimationType() const
{
	if (OwningPlayer && !OwningPlayer->IsWeaponSheathed())
	{
		return OwningPlayer->GetEquippedWeaponType();
	}

	return EWeaponType::None;
}

/*
EWeaponAnimationType UPlayerAnimInstance::GetWeaponAnimationType() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->CurrentWeaponAnimationToUse;
	}

	return EWeaponAnimationType();
}
*/

void UPlayerAnimInstance::HandleMontageBlendingOut(UAnimMontage * AnimMontage, bool bInterrupted)
{
	OwningPlayer->OnMontageBlendingOut(AnimMontage, bInterrupted);

	/*
	if(!bInterrupted)
	{
		if (AnimMontage == OwningPlayer->GetActiveAnimationReferences()->AnimationMontage_Jump)
		{
			// OwningPlayer->GetCharacterState() == ECharacterState::IdleWalkRun;
		}
		else if (AnimMontage == OwningPlayer->GetActiveAnimationReferences()->AnimationMontage_Dodge)
		{
			// OwningPlayer->GetCharacterState() == ECharacterState::IdleWalkRun;
			// @todo
			// OwningPlayer-> UpdateMovementAnimation();
		}
		else
		{
			// OwningPlayer->GetCharacterState() == ECharacterState::IdleWalkRun;
		}
	}
	*/
}

void UPlayerAnimInstance::HandleMontageEnded(UAnimMontage * AnimMontage, bool bInterrupted)
{
	OwningPlayer->OnMontageEnded(AnimMontage, bInterrupted);
}

APlayerCharacter * UPlayerAnimInstance::CastPawnOwnerToPlayerCharacter()
{
	APlayerCharacter* PlayerChar = nullptr;

	if (TryGetPawnOwner())
	{
		PlayerChar = Cast<APlayerCharacter>(TryGetPawnOwner());
	}

	return PlayerChar;
}
