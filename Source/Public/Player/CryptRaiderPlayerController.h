// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CryptRaiderPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API ACryptRaiderPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	ACryptRaiderPlayerController();

	//~ Begin IGenericTeamAgentInterface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface
	
private:
	FGenericTeamId HeroTeamID;
}; 
