// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CryptRaiderPlayerController.h"

ACryptRaiderPlayerController::ACryptRaiderPlayerController()
{
	HeroTeamID = FGenericTeamId(0);
}

FGenericTeamId ACryptRaiderPlayerController::GetGenericTeamId() const
{
	return HeroTeamID;
}
