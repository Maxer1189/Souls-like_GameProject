// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"


class UInventoryComponent;
class ACryptRaiderWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRYPTRAIDER_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:

	// UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	// ACryptRaiderWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
	
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	ACryptRaiderWeapon* GetHeroCarriedLightWeapon(UInventoryComponent* HeroInventory) const;

	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	ACryptRaiderWeapon* GetHeroCarriedHeavyWeapon(UInventoryComponent* HeroInventory) const;
	
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	const ACryptRaiderWeapon* GetHeroCurrentEquippedWeapon(EWeaponWeightClass WeaponWeightClass) const;

	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	float GetHeroEquippedWeaponDamageAtLevel(EWeaponWeightClass WeaponWeightClass, float InLevel) const;

	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
};
