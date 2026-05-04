// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdatedDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>,SoftObjectMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate, FGameplayTag, AbilityInputTag, float, TotalCooldownTime, float, RemainingCooldownTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelCoolDownIfActorDeadDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRYPTRAIDER_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UHeroUIComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentMagicChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentStaminaChanged;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityIconSlotUpdatedDelegate OnAbilityIconSlotUpdated;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBegin;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnCancelCoolDownIfActorDeadDelegate OnAbilityCancelCoolDown;
	
protected:
	virtual void BeginPlay() override;

	
};
