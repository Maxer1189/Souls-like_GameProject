// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterGameplayAbility.h"
#include "HeroGameplayAbility.generated.h"

class UInventoryComponent;
class UHeroCombatComponent;
class ACryptRaiderPlayerController;
class ACryptRaiderCharacter;
/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UHeroGameplayAbility : public UCharacterGameplayAbility
{
	GENERATED_BODY()

public:

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	ACryptRaiderCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	ACryptRaiderPlayerController* GetControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	UInventoryComponent* GetInventoryComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 UsedAttackComboCount);
	
	UFUNCTION(BlueprintCallable, Category = "CharacterAbility")
	bool GetAbilityRemainingCoolDownByTag(FGameplayTag InCoolDownTag, float& TotalCoolDownTime, float& RemainingCoolDownTime);

private:
	// Only holds the reference of the object, doesnt keep it alive. This way we only keep a reference and dont increase the reference counter
	TWeakObjectPtr<ACryptRaiderCharacter> CachedHeroCharacter;
	TWeakObjectPtr<ACryptRaiderPlayerController> CachedController;
};
