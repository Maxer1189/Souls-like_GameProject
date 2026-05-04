// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CharacterGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CharacterFunctionLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"

// Actor info contains a bunch of parameters that are associated with the character that has the ability system, and Spec contains data such as activation info
void UCharacterGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActivationPolicy == ECharacterAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UCharacterGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActivationPolicy == ECharacterAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

FActiveGameplayEffectHandle UCharacterGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	check(ASC && InSpecHandle.IsValid());
	
	return GetCharacterAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, ASC);
}

UPawnCombatComponent* UCharacterGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UCharacterAbilitySystemComponent* UCharacterGameplayAbility::GetCharacterAbilitySystemComponentFromActorInfo() const
{
	return Cast<UCharacterAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UCharacterGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, ECharacterSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? ECharacterSuccessType::Success : ECharacterSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

void UCharacterGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults, bool FriendlyFireEnabled)
{
	if (InHitResults.IsEmpty()) return;
	
	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());
	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (FriendlyFireEnabled)
			{
				ApplyHandleToFoundTarget(OwningPawn, HitPawn, InSpecHandle);
			}
			else
			{
				if (UCharacterFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
				{
					ApplyHandleToFoundTarget(OwningPawn, HitPawn, InSpecHandle);
				}
			}
		}
	}
}

void UCharacterGameplayAbility::ApplyHandleToFoundTarget(const APawn* InstigatorPawn, APawn* TargetPawn,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	FActiveGameplayEffectHandle ActiveEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetPawn, InSpecHandle);
	if (ActiveEffectHandle.WasSuccessfullyApplied())
	{
		FGameplayEventData Data;
		Data.Instigator = InstigatorPawn;
		Data.Target = TargetPawn;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetPawn,
			CryptRaiderGameplayTags::Shared_Event_HitReact, Data);
	}
}
