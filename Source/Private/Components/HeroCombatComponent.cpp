// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/HeroCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "Player/CryptRaiderCharacter.h"
#include "World/Items/Weapons/CryptRaiderWeapon.h"


ACryptRaiderWeapon* UHeroCombatComponent::GetHeroCarriedLightWeapon(UInventoryComponent* HeroInventory) const
{
	if (ACryptRaiderWeapon* HeroWeapon =  Cast<ACryptRaiderWeapon>(GetCharacterCarriedLightWeaponFromInventory(HeroInventory)))
	{
		return HeroWeapon;
	}
	return nullptr;
}

ACryptRaiderWeapon* UHeroCombatComponent::GetHeroCarriedHeavyWeapon(UInventoryComponent* HeroInventory) const
{
	if (ACryptRaiderWeapon* HeroWeapon =  Cast<ACryptRaiderWeapon>(GetCharacterCarriedHeavyWeaponFromInventory(HeroInventory)))
	{
		return HeroWeapon;
	}
	
	return nullptr;
}

const ACryptRaiderWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon(EWeaponWeightClass WeaponWeightClass) const
{
	return Cast<ACryptRaiderWeapon>(GetCharacterCurrentEquippedWeapon(WeaponWeightClass));
}

float UHeroCombatComponent::GetHeroEquippedWeaponDamageAtLevel(EWeaponWeightClass WeaponWeightClass, float InLevel) const
{
	return GetHeroCurrentEquippedWeapon(WeaponWeightClass)->GetWeaponTypeData().WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) return;

	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(),
		CryptRaiderGameplayTags::Shared_Event_MeleeHit, Data);
	
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	
}
