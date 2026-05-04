// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/HeroUIComponent.h"

#include "CharacterFunctionLibrary.h"
#include "CryptRaiderGameplayTags.h"


UHeroUIComponent::UHeroUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHeroUIComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UHeroUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

