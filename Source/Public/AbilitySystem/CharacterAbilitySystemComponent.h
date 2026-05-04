// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CharacterTypes/CharacterStructTypes.h"
#include "CharacterAbilitySystemComponent.generated.h"

class ACryptRaiderCharacter;
class ACryptRaiderWeapon;
/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(FGameplayTag InInputTag);
	void OnAbilityInputReleased(FGameplayTag InInputTag);

	UFUNCTION(BlueprintCallable, Category = "Character|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FCryptRaiderAbilitySet>& InDefaultWeaponAbilities,const TArray<FCharacterHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle);
	
	// UPARAM(ref) tells unreal that this parameter is not an out param. just a reference
	UFUNCTION(BlueprintCallable, Category = "Character|Ability")
	void RemoveGrantedHeroWeaponAbilities(ACryptRaiderWeapon* HeroWeapon);

	UFUNCTION(BlueprintCallable, Category = "Character|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
	
	TMap<FGameplayTag, FGameplayAbilitySpecHandle>& GrantAvailableAbilityToHero(const TArray<FCryptRaiderAbilitySet>& InDefaultCharacterAbilities, FGameplayTag AbilityTagToActivate,  TMap<FGameplayTag, FGameplayAbilitySpecHandle>& GrantedAbilitiesSpecHandlesMap, int32 ApplyLevel = 1);
	TMap<FGameplayTag, FGameplayAbilitySpecHandle>& RemoveAvailableAbilityFromHero(FGameplayTag AbilityTagToRemove, TMap<FGameplayTag, FGameplayAbilitySpecHandle>& GrantedAbilitiesSpecHandlesMap);

	
};
