// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();
		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenerateStamina(float DeltaTime);
	void RegenerateHealth(float HealthToRegen);
	void ReceiveDamage(float DamageAmount);
	void UseStamina(float StaminaAmount);
	void AddSouls(int32 SoulAmountToAdd);
	void AddGold(int32 GoldAmountToAdd);
	bool IsAlive();

	FORCEINLINE float GetHealthBarProgressPercent() const {return HealthProgress / MaxHealth;}
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }
	FORCEINLINE float GetCurrentMaxHealth() const {return MaxHealth; }
	FORCEINLINE float GetHealth() const {return Health;}
	FORCEINLINE float GetHealthProgress() const {return HealthProgress;}

	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetStaminaProgress() const {return StaminaProgress;}
	FORCEINLINE float GetStaminaProgressPercent() const {return StaminaProgress / MaxStamina;}

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetAttackCost() const { return AttackCost; }
	FORCEINLINE void SetAttackCost(float NewCost) { AttackCost = NewCost; }
	FORCEINLINE int32 GetSoulsAmount() const { return Souls; }
	FORCEINLINE int32 GetGoldAmount() const { return Gold; }


	UPROPERTY(EditDefaultsOnly, Category = "Actor Buffs", meta = (ClampMin="0.0", ClampMax="1.0", UIMin = "0", UIMax = "1"))
	float HealPercent = 0.2f;

protected:

	virtual void BeginPlay() override;

private:

	// Visual Health Value
	float HealthProgress;

	float StaminaProgress;

	// Current health value
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float AttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 0.8;

	UPROPERTY(EditAnywhere, Category = "Actor Possesions")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Possesions")
	int32 Gold;
};
