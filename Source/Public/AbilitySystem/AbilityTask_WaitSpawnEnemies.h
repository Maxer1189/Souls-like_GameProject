// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

// BlueprintInternalUseOnly hides the node from a regular blueprint but for example a gameplay ability blueprint it will still be visible because they use a special Blueprint class with custom K2 nodes and custom schema rules.

class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate,const TArray<AEnemy*>&, SpawnedEnemies);
/**
 * In Ability Tasks, any UPROPERTY(BlueprintAssignable) inside the task class becomes an output execution pin on the Blueprint node that calls your factory function (WaitSpawnEnemies).

 */
UCLASS()
class CRYPTRAIDER_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()
public:
	// Hidepin hides the pin from the blueprint in the editor, DefaultToSelf uses the default value provided so that the blueprint in the editor won't necessarily require connection, BlueprintInternalUseOnly hides the function from normal use and not shown in action menu. Used behind the scenes
	UFUNCTION(BlueprintCallable, Category = "Character|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", NumToSpawn = "1", RandomSpawnRadius = "200"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<AEnemy> SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius);

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface

private:
	FGameplayTag CachedEventTag;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	TSoftClassPtr<AEnemy> CachedSoftEnemyClassToSpawn;
	FDelegateHandle DelegateHandle;

	void OnGameplayEventReceived(const FGameplayEventData* InPayload);
	void OnEnemyClassLoaded();
};
