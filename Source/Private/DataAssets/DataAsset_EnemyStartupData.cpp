// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_EnemyStartupData.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/EnemyGameplayAbility.h"


void UDataAsset_EnemyStartupData::GiveToAbilitySystemComponent(UCharacterAbilitySystemComponent* InCharacterASCToGive,
                                                               int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InCharacterASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf<UEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InCharacterASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			
			InCharacterASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
