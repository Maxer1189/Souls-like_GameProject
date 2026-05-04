// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterGameplayAbility.h"
#include "EnemyGameplayAbility.generated.h"

class UEnemyCombatComponent;
class AEnemy;
/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UEnemyGameplayAbility : public UCharacterGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Character Ability")
	AEnemy* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Character Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Character Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);

private:

	TWeakObjectPtr<AEnemy> CachedEnemyCharacter;
};
