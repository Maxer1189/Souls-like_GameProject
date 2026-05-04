// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "CharacterStructTypes.generated.h"

class UHeroGameplayAbility;
class UInputMappingContext;
class UCryptRaiderLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FCryptRaiderAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UHeroGameplayAbility> AbilityToGrant;

	bool IsValid() const;
	
};

USTRUCT(BlueprintType)
struct FCharacterHeroSpecialAbilitySet : public FCryptRaiderAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> SoftAbilityIconMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Player.Cooldown"))
	FGameplayTag AbilityCooldownTag;
};

USTRUCT(BlueprintType)
struct FWeaponNamesData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EquipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnequipSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EquipMontageBegin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnequipMontageBegin;
};

USTRUCT(BlueprintType)
struct FHeroWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCryptRaiderLinkedAnimLayer> WeaponAnimLayerToLink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> WeaponInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FCryptRaiderAbilitySet> Abilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FCharacterHeroSpecialAbilitySet> SpecialWeaponAbilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponNamesData WeaponNamesData;
};

class CRYPTRAIDER_API FCharacterStructTypes
{
	
};
