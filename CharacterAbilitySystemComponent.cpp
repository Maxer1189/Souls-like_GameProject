// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CharacterAbilitySystemComponent.h"

#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"
#include "Player/CryptRaiderCharacter.h"
#include "World/Items/Weapons/CryptRaiderWeapon.h"

void UCharacterAbilitySystemComponent::OnAbilityInputPressed(FGameplayTag InInputTag)
{
	if (!InInputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		if (InInputTag.MatchesTag(CryptRaiderGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
			TryActivateAbility(AbilitySpec.Handle);
	}
}

void UCharacterAbilitySystemComponent::OnAbilityInputReleased(FGameplayTag InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(CryptRaiderGameplayTags::InputTag_MustBeHeld)) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
	
}

void UCharacterAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FCryptRaiderAbilitySet>& InDefaultWeaponAbilities,
	const TArray<FCharacterHeroSpecialAbilitySet>& InSpecialWeaponAbilities,int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle)
{
	if (InDefaultWeaponAbilities.IsEmpty()) return;

	for (const FCryptRaiderAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandle.AddUnique(GiveAbility(AbilitySpec));
	}

	for (const FCharacterHeroSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandle.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UCharacterAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(ACryptRaiderWeapon* HeroWeapon)
{

	TArray<FGameplayAbilitySpecHandle>&  GrantedAbilitySpecHandles = HeroWeapon->GetGrantedAbilitySpecHandles();

	for (const auto& SpecHandle : GrantedAbilitySpecHandles)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	GrantedAbilitySpecHandles.Empty();
	
}

bool UCharacterAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}

TMap<FGameplayTag, FGameplayAbilitySpecHandle>& UCharacterAbilitySystemComponent::GrantAvailableAbilityToHero(
	const TArray<FCryptRaiderAbilitySet>& InDefaultCharacterAbilities, FGameplayTag AbilityTagToActivate,
	 TMap<FGameplayTag, FGameplayAbilitySpecHandle>& GrantedAbilitiesSpecHandlesMap, int32 ApplyLevel)
{
	if (InDefaultCharacterAbilities.IsEmpty()) return GrantedAbilitiesSpecHandlesMap;

	for (const FCryptRaiderAbilitySet& AbilitySet : InDefaultCharacterAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		if (AbilitySet.InputTag.MatchesTagExact(AbilityTagToActivate))
		{
			FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
			AbilitySpec.SourceObject = GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
			
			GrantedAbilitiesSpecHandlesMap.Emplace(AbilityTagToActivate, GiveAbility(AbilitySpec));
			return GrantedAbilitiesSpecHandlesMap;
		}
	}
	return GrantedAbilitiesSpecHandlesMap;
}

TMap<FGameplayTag, FGameplayAbilitySpecHandle>& UCharacterAbilitySystemComponent::RemoveAvailableAbilityFromHero(FGameplayTag AbilityTagToRemove,
	 TMap<FGameplayTag, FGameplayAbilitySpecHandle>& GrantedAbilitiesSpecHandlesMap)
{
	FGameplayAbilitySpecHandle* FoundHandle = GrantedAbilitiesSpecHandlesMap.Find(AbilityTagToRemove);
	if (FoundHandle->IsValid())
	{
		ClearAbility(*FoundHandle);
		if (GrantedAbilitiesSpecHandlesMap.Contains(AbilityTagToRemove))
		{
			GrantedAbilitiesSpecHandlesMap.Remove(AbilityTagToRemove);
			return GrantedAbilitiesSpecHandlesMap;
		}
	}

	return GrantedAbilitiesSpecHandlesMap;
}
