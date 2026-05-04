// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartupData.generated.h"

class UEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UDataAsset_EnemyStartupData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:

	virtual void GiveToAbilitySystemComponent(UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartupData")
	TArray<TSubclassOf<UEnemyGameplayAbility>> EnemyCombatAbilities;
};
