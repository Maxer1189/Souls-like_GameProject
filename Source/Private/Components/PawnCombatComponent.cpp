// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"

#include "Components/BoxComponent.h"
#include "Components/InventoryComponent.h"
#include "CryptRaider/DebugMacros.h"
#include "World/Items/Weapons/WeaponBase.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWeaponBase* InWeaponToRegister,
                                                   EWeaponWeightClass WeaponWeightClass, bool bRegisterWeaponEquipped)
{
	checkf(!CharacterCarriedWeaponsMap.Contains(InWeaponTagToRegister), TEXT("A tag named %s has already been added as Carried Weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	CharacterCarriedWeaponsMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
	if (bRegisterWeaponEquipped)
	{
		if (WeaponWeightClass == EWeaponWeightClass::Light)
			CurrentEquippedLightWeaponTag = InWeaponTagToRegister;
		
		else if (WeaponWeightClass == EWeaponWeightClass::Heavy)
			CurrentEquippedHeavyWeaponTag = InWeaponTagToRegister;
	}
	
}

void UPawnCombatComponent::RemoveRegisteredWeapon(FGameplayTag InWeaponTagToRemove)
{
	CharacterCarriedWeaponsMap.Remove(InWeaponTagToRemove);
	
	if (CurrentEquippedLightWeaponTag.MatchesTagExact(InWeaponTagToRemove))
		CurrentEquippedLightWeaponTag = FGameplayTag::EmptyTag;
	
	else if (CurrentEquippedHeavyWeaponTag.MatchesTagExact(InWeaponTagToRemove))
		CurrentEquippedHeavyWeaponTag = FGameplayTag::EmptyTag;
	
}

AWeaponBase* UPawnCombatComponent::GetCharacterCarriedLightWeaponFromInventory(UInventoryComponent* HeroInventory) const
{
	if (const UItemBase* FoundWeapon = HeroInventory->FindMatchingItemByGameplayTag(EItemCategory::Weapon, CurrentEquippedLightWeaponTag))
	{
		return GetCharacterCarriedWeaponByTag(FoundWeapon->ItemType);
	}
	UE_LOG(LogTemp, Error, TEXT("Weapon Item Base not Found"))
	return nullptr;
}

AWeaponBase* UPawnCombatComponent::GetCharacterCarriedHeavyWeaponFromInventory(UInventoryComponent* HeroInventory) const
{
	if (const UItemBase* FoundWeapon = HeroInventory->FindMatchingItemByGameplayTag(EItemCategory::Weapon, CurrentEquippedHeavyWeaponTag))
	{
		return GetCharacterCarriedWeaponByTag(FoundWeapon->ItemType);
	}
	UE_LOG(LogTemp, Error, TEXT("Weapon Item Base not Found"))
	return nullptr;
}

AWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponsMap.Contains(InWeaponTagToGet))
	{
		if (AWeaponBase* const* FoundWeapon = CharacterCarriedWeaponsMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}
	return nullptr;
}

const AWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon(EWeaponWeightClass WeaponWeightClass) const
{
	switch(WeaponWeightClass)
	{
	case EWeaponWeightClass::Light:
		if (CurrentEquippedLightWeaponTag.IsValid())
			return GetCharacterCarriedWeaponByTag(CurrentEquippedLightWeaponTag);

	case EWeaponWeightClass::Heavy:
		if (CurrentEquippedHeavyWeaponTag.IsValid())
			return GetCharacterCarriedWeaponByTag(CurrentEquippedHeavyWeaponTag);
		
	case EWeaponWeightClass::Small:
		check(TEXT("Small Weapons not implemented!"))
		
	case EWeaponWeightClass::None:
		check(TEXT("None Weapon Type detected!"))
	}

	return nullptr;
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EWeaponWeightClass WeaponWeightClass, EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		ToggleEquippedWeaponCollisionBox(bShouldEnable, WeaponWeightClass);
	}
	else
	{
		ToggleBodyCollisionBox(bShouldEnable, ToggleDamageType);
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	
}

void UPawnCombatComponent::ToggleEquippedWeaponCollisionBox(bool bShouldEnable,  EWeaponWeightClass WeaponWeightClass)
{
	const AWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon(WeaponWeightClass);

	check(WeaponToToggle)
	if (bShouldEnable)
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
	}
	else
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlappedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBox(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	
}
