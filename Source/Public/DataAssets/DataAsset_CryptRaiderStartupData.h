// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset_StartUpDataBase.h"
#include "CharacterTypes/CharacterStructTypes.h"
#include "DataAsset_CryptRaiderStartupData.generated.h"

UCLASS()
class CRYPTRAIDER_API UDataAsset_CryptRaiderStartupData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UCharacterAbilitySystemComponent* InCharacterASCToGive, int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartupData", meta = (TitleProperty = "InputTag"))
	TArray<FCryptRaiderAbilitySet> CryptRaiderStartUpAbilitySets;

	
};
