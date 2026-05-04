// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/PawnUIComponent.h"


UPawnUIComponent::UPawnUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPawnUIComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UPawnUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

