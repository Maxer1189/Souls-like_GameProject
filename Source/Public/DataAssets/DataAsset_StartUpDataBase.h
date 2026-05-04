// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UCharacterGameplayAbility;
/**
 *  ASC - Ability system component
 */
UCLASS()
class CRYPTRAIDER_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
public:

	virtual void GiveToAbilitySystemComponent(UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel = 1); // The character that the ability will be granted and the difficulty level of the level
	
protected:

	void GrantAbilities(TArray<TSubclassOf<UCharacterGameplayAbility>>& AbilitiesToGive, UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel = 1);
	
	UPROPERTY(EditDefaultsOnly, Category = "StartupData")
	TArray<TSubclassOf<UCharacterGameplayAbility>> ActivateGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartupData")
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;
	
	UPROPERTY(EditDefaultsOnly, Category = "StartupData")
	TArray<TSubclassOf<UCharacterGameplayAbility>> ReactiveAbilities; // Abilities such as hit react and death, both shared by enemies and player
};
