// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Player/CharacterStates.h"
#include "CharacterGameplayAbility.generated.h"

/**
 * 
 */

class UCharacterAbilitySystemComponent;
class UPawnCombatComponent;

UENUM(BlueprintType)
enum class ECharacterAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class CRYPTRAIDER_API UCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override; // Called when an ability is given to the ability system component
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~ End UGameplayAbility Interface

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);
	
	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "CharacterAbility")
	UCharacterAbilitySystemComponent* GetCharacterAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterAbility", meta = (DisplayName = "Apply Effect Spec Handle To Target", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, ECharacterSuccessType& OutSuccessType);
	
	UFUNCTION(BlueprintCallable, Category = "CharacterAbility")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults, bool FriendlyFireEnabled);
	
	UPROPERTY(EditDefaultsOnly, Category = "CharacterAbility")
	ECharacterAbilityActivationPolicy ActivationPolicy = ECharacterAbilityActivationPolicy::OnTriggered;
	
private:
	void ApplyHandleToFoundTarget(const APawn* InstigatorPawn, APawn* TargetPawn, const FGameplayEffectSpecHandle& InSpecHandle);
};
