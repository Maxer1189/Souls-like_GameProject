// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility.h"

#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Player/CryptRaiderCharacter.h"
#include "Player/CryptRaiderPlayerController.h"

bool UHeroGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
}

ACryptRaiderCharacter* UHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedHeroCharacter.IsValid())
	{
		CachedHeroCharacter = Cast<ACryptRaiderCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedHeroCharacter.IsValid() ? CachedHeroCharacter.Get() : nullptr;
}

ACryptRaiderPlayerController* UHeroGameplayAbility::GetControllerFromActorInfo()
{
	if (!CachedController.IsValid())
	{
		CachedController = Cast<ACryptRaiderPlayerController>(CurrentActorInfo->PlayerController);
	}

	return CachedController.IsValid() ? CachedController.Get() : nullptr;
}

UHeroCombatComponent* UHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UInventoryComponent* UHeroGameplayAbility::GetInventoryComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroInventory();
}

// Stores all the necessary data about the damage to the effectSpecHandle and returns it 
FGameplayEffectSpecHandle UHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass,
	float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 UsedAttackComboCount)
{
	check(EffectClass);
	// Context Handle contains a bunxh of useful info if well need it later such as the instigator, the causer.
	FGameplayEffectContextHandle ContextHandle = GetCharacterAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetCharacterAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(CryptRaiderGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, UsedAttackComboCount);
	}

	return EffectSpecHandle;
}

bool UHeroGameplayAbility::GetAbilityRemainingCoolDownByTag(FGameplayTag InCoolDownTag, float& TotalCoolDownTime,
	float& RemainingCoolDownTime)
{
	check(InCoolDownTag.IsValid());
	TotalCoolDownTime = 0.f;
	RemainingCoolDownTime = 0.f;
	
	FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCoolDownTag.GetSingleTagContainer());
	TArray<TPair<float, float>> TimeRemainingAndDuration = GetCharacterAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(EffectQuery);
	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCoolDownTime = TimeRemainingAndDuration[0].Key;
		TotalCoolDownTime = TimeRemainingAndDuration[0].Value;
	}
	
	return RemainingCoolDownTime > 0.f;
}

