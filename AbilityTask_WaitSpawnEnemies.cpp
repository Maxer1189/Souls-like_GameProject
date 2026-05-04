// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"
#include "Engine/AssetManager.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/Enemy.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
                                                                               FGameplayTag EventTag, TSoftClassPtr<AEnemy> SoftEnemyClassToSpawn, int32 NumToSpawn, const FVector& SpawnOrigin,
                                                                               float RandomSpawnRadius)
{
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;

	return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	// Finds and retrieves the multicast delegate that is associated to the gameplay tag and makes sure that it exists. Once we got the delegate we can bind a function to it
	// Must be stored in a local reference. If not itll create a new copy that is not bound by the callback function.
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	// Super must be called at the end
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	Super::OnDestroy(bInOwnerFinished);
}

/*
- Starts loading the asset(s)
- When finished, invokes the delegate
- Your callback runs, and you then fetch the loaded asset from your soft pointer
 */

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded));
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AEnemy*>());
		}
		EndTask();
	}
}

// K2 functions are blueprint callable, meaning the can be used in BP
// Navigation system allows us to spawn enemies within radius while also making sure they're not overlapping each other

// Fills array of spawned enemies and broadcasts them. If failed broadcasts empty Tarray
void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();
	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AEnemy*>());
		}
		EndTask();
		return;
	}
	TArray<AEnemy*> SpawnedEnemies;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	const FVector InstigatorLocation = AbilitySystemComponent->GetAvatarActor()->GetActorLocation();
	UCapsuleComponent* InstigatorCapsule = AbilitySystemComponent->GetAvatarActor()->FindComponentByClass<UCapsuleComponent>();
	float InstigatorCapsuleRadius = InstigatorCapsule->GetScaledCapsuleRadius();
	
	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(World, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);
		RandomLocation += FVector(0.f, 0.f, 150.f); // Offset the spawn location Z to make sure its not in the ground
		const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
		float Dist = FVector::Distance(InstigatorLocation, RandomLocation);

		if (Dist < InstigatorCapsuleRadius*2)
		{
			const int32 RandomXOffset = FMath::RandRange(-(InstigatorCapsuleRadius*3), -(InstigatorCapsuleRadius*2));
			const int32 RandomYOffset = FMath::RandRange(InstigatorCapsuleRadius*2, InstigatorCapsuleRadius*3);
			RandomLocation += FVector(RandomXOffset, RandomYOffset, 0.f);
		}
		
		if (AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParams))
			SpawnedEnemies.Add(SpawnedEnemy);
	}
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<AEnemy*>());
		}
	}

	EndTask();
	
}
