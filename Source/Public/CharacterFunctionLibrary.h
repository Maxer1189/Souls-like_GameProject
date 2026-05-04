// Fill out your copyright notice in the Description page of Project Settings.

/* Latent Action Function much like Delay node, a asynchronous task that BP starts and runs in the background 
* while pausing the BP execution unlike timers that don't.
* In order to create a latent action, you must construct a BP node like it is done below and create a separate native class for it
*/

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Player/CharacterStates.h"
#include "CharacterFunctionLibrary.generated.h"

class UPawnCombatComponent;
class UCharacterAbilitySystemComponent;

UCLASS()
class CRYPTRAIDER_API UCharacterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static UCharacterAbilitySystemComponent* NativeGetCharacterASCFromActor(AActor* InActor);
	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag InTagToCheck);
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActorInfo(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "Character|Function Library")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag InTagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Character|Function Library")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag InTagToRemove);

	UFUNCTION(BlueprintCallable, Category = "Character|Function Library", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag InTagToCheck, ECharacterConfirmType& OutConfirmType);

	UFUNCTION(BlueprintCallable, Category = "Character|Function Library", meta = (DisplayName = "Get Pawn Combat Component From Actor Info", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActorInfo(AActor* InActor, ECharacterValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Character|Function Library")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category = "Character|Function Library")
	static FGameplayTag ComputeHitReactDirectionAttack(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "Character|Function Library")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	UFUNCTION(BlueprintCallable, Category = "Character|Function Library")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);
	
	// A Latent Action Function for counting down
	UFUNCTION(BlueprintCallable, Category = "Character|Function Library", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentActionInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput", TotalTime = "1.0", UpdateInterval = "1.0"))
	static void CountDown(UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime,
		ECharacterCountdownActionInput CountDownInput, UPARAM(DisplayName = "Output") ECharacterCountdownActionOutput& CountDownOutput,
		FLatentActionInfo LatentActionInfo);
};
