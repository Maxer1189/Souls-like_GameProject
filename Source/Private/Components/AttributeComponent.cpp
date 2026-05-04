// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AttributeComponent.h"
#include "Kismet/KismetMathLibrary.h"


UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthProgress = Health;
	StaminaProgress = Stamina;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(HealthProgress != Health)
		HealthProgress = UKismetMathLibrary::FInterpTo_Constant(HealthProgress, Health, DeltaTime, 30.f);
		// HealthProgress = FMath::Lerp(HealthProgress, Health, 0.03f);
	if(StaminaProgress != Stamina)
		StaminaProgress =  UKismetMathLibrary::FInterpTo_Constant(StaminaProgress, Stamina, DeltaTime, 30.f);
}

void UAttributeComponent::AddSouls(int32 SoulsAmountToAdd)
{
	Souls += SoulsAmountToAdd;
}

void UAttributeComponent::AddGold(int32 GoldAmountToAdd)
{
	Gold += GoldAmountToAdd;
}

void UAttributeComponent::ReceiveDamage(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaAmount)
{
	Stamina = FMath::Clamp(Stamina - StaminaAmount, 0.f, MaxStamina);
}

void UAttributeComponent::RegenerateStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::RegenerateHealth(float HealthToRegen)
{
	Health = FMath::Clamp(Health + HealthToRegen, 0.f, MaxHealth);
}

bool UAttributeComponent::IsAlive()
{
	if(Health > 0)
		return true;
	else return false; 
}

