// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_CryptRaiderStartupData.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "Data/ItemDataStruct.h"

void UDataAsset_CryptRaiderStartupData::GiveToAbilitySystemComponent(
	UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InCharacterASCToGive, ApplyLevel);

	for (const FCryptRaiderAbilitySet& CryptRaiderAbilitySet : CryptRaiderStartUpAbilitySets)
	{
		if (!CryptRaiderAbilitySet.IsValid()) continue;
	
		FGameplayAbilitySpec AbilitySpec(CryptRaiderAbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = InCharacterASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(CryptRaiderAbilitySet.InputTag);
		
		InCharacterASCToGive->GiveAbility(AbilitySpec);
	}
}
