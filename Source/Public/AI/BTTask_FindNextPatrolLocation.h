// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextPatrolLocation.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UBTTask_FindNextPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_FindNextPatrolLocation();

	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Patrol Target")
	FBlackboardKeySelector InNextPatrolTargetActor;

	UPROPERTY(EditAnywhere, Category = "Patrol Target")
	FBlackboardKeySelector InNextWaypointIndex;
	
	UPROPERTY(EditAnywhere, Category = "Patrol Target")
	bool bShouldRandomizePath;
	
private:
	
	bool HasFoundNextPatrolTargetLocation(AEnemy* EnemyPawn, UBehaviorTreeComponent& OwnerComp) const;
};
