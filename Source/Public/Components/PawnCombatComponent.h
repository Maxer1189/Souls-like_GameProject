// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"


enum class EWeaponWeightClass : uint8;
class UInventoryComponent;
class AWeaponBase;

UENUM()
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRYPTRAIDER_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWeaponBase* InWeaponToRegister,  EWeaponWeightClass WeaponWeightClass, bool bRegisterWeaponEquipped = false);

	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	void RemoveRegisteredWeapon(FGameplayTag InWeaponTagToRemove);

	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	AWeaponBase* GetCharacterCarriedLightWeaponFromInventory(UInventoryComponent* HeroInventory) const;

	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	AWeaponBase* GetCharacterCarriedHeavyWeaponFromInventory(UInventoryComponent* HeroInventory) const;
	
	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	AWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	const AWeaponBase* GetCharacterCurrentEquippedWeapon(EWeaponWeightClass WeaponWeightClass) const;
	
	UFUNCTION(BlueprintCallable, Category="Character | Combat")
	void ToggleWeaponCollision(bool bShouldEnable, EWeaponWeightClass WeaponWeightClass, EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);

	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
	
	UPROPERTY(BlueprintReadWrite, Category="Character | Combat")
	FGameplayTag CurrentEquippedLightWeaponTag;

	UPROPERTY(BlueprintReadWrite, Category="Character | Combat")
	FGameplayTag CurrentEquippedHeavyWeaponTag;
	
	UPROPERTY(BlueprintReadWrite, Category="Character | Combat")
	bool bLightWeaponAttachedSuccessfully;

	UPROPERTY(BlueprintReadWrite, Category="Character | Combat")
	bool bHeavyWeaponAttachedSuccessfully;

protected:
	virtual void ToggleEquippedWeaponCollisionBox(bool bShouldEnable, EWeaponWeightClass WeaponWeightClass);
	virtual void ToggleBodyCollisionBox(bool bShouldEnable, EToggleDamageType ToggleDamageType);
	
	UPROPERTY()
	TArray<AActor*> OverlappedActors;
	
private:

	// Allows to store multiple weapons that can be easily retrieved
	UPROPERTY()
	TMap<FGameplayTag, AWeaponBase*> CharacterCarriedWeaponsMap;

	
};
