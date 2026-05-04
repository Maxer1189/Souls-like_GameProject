// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/Abilities/CharacterGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UCharacterAbilitySystemComponent* InCharacterASCToGive,
                                                              int32 ApplyLevel)
{
	check(InCharacterASCToGive);
	GrantAbilities(ActivateGivenAbilities, InCharacterASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InCharacterASCToGive, ApplyLevel);

	if (!StartupGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupGameplayEffects)
		{
			if (!EffectClass) continue;

			UGameplayEffect* EffectClassDefaultObject = EffectClass->GetDefaultObject<UGameplayEffect>(); // Casts to specified template and attempts to retrieve the object of that class
			InCharacterASCToGive->ApplyGameplayEffectToSelf(EffectClassDefaultObject, ApplyLevel, InCharacterASCToGive->MakeEffectContext());
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(TArray<TSubclassOf<UCharacterGameplayAbility>>& AbilitiesToGive,
                                                UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel)
{
	if (AbilitiesToGive.IsEmpty()) return;

	for (const TSubclassOf<UCharacterGameplayAbility>&  Ability : AbilitiesToGive)
	{
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InCharacterASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InCharacterASCToGive->GiveAbility(AbilitySpec);
	}
}
