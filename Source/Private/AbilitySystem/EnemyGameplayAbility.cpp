// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EnemyGameplayAbility.h"

#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Enemies/Enemy.h"

AEnemy* UEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedEnemyCharacter.IsValid())
	{
		CachedEnemyCharacter = Cast<AEnemy>(CurrentActorInfo->AvatarActor);
	}

	return CachedEnemyCharacter.IsValid() ? CachedEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);
	FGameplayEffectContextHandle ContextHandle = GetCharacterAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetCharacterAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(CryptRaiderGameplayTags::Shared_SetByCaller_BaseDamage, InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));
	EffectSpecHandle.Data->SetSetByCallerMagnitude(CryptRaiderGameplayTags::Shared_SetByCaller_SpecialDamage, InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));
	
	return EffectSpecHandle;
}
