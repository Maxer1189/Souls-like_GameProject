// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindNextPatrolLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Enemy.h"

UBTTask_FindNextPatrolLocation::UBTTask_FindNextPatrolLocation()
{
	NodeName = TEXT("Native Find Next Patrol Location");
	bNotifyTick = false;
	bShouldRandomizePath = false;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	InNextPatrolTargetActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InNextPatrolTargetActor), AActor::StaticClass());
	InNextWaypointIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InNextWaypointIndex));
}

void UBTTask_FindNextPatrolLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InNextPatrolTargetActor.ResolveSelectedKey(*BBAsset);
		InNextWaypointIndex.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTTask_FindNextPatrolLocation::GetStaticDescription() const
{
	const FString KeyDescription = InNextPatrolTargetActor.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Finds the location of %s"), *KeyDescription);
}

EBTNodeResult::Type UBTTask_FindNextPatrolLocation::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	APawn* OwningPawn = Comp.GetAIOwner()->GetPawn();
	AEnemy* CurrentEnemyPawn = Cast<AEnemy>(OwningPawn);
	
	if (HasFoundNextPatrolTargetLocation(CurrentEnemyPawn, Comp))
		return EBTNodeResult::Succeeded;
	
	return EBTNodeResult::Failed;
}

bool UBTTask_FindNextPatrolLocation::HasFoundNextPatrolTargetLocation(AEnemy* EnemyPawn, UBehaviorTreeComponent& OwnerComp) const
{
	if (EnemyPawn && EnemyPawn->PatrolTargets.Num() > 0)
	{
		int32 NextWaypointIndex = OwnerComp.GetBlackboardComponent()->GetValueAsInt(InNextWaypointIndex.SelectedKeyName);
		if (bShouldRandomizePath)
		{
			int32 RandomWaypointIndex = FMath::RandRange(0, EnemyPawn->PatrolTargets.Num());
			if (NextWaypointIndex != RandomWaypointIndex)
			{
				NextWaypointIndex = RandomWaypointIndex;
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("PatrolTarget"), EnemyPawn->PatrolTargets[NextWaypointIndex]);
				OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("NextWaypointIndex"), NextWaypointIndex);
			}
			return true;
		}
		
		if (NextWaypointIndex > EnemyPawn->PatrolTargets.Num() -1)
			NextWaypointIndex = 0;
		
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("PatrolTarget"), EnemyPawn->PatrolTargets[NextWaypointIndex]);
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(FName("NextWaypointIndex"), NextWaypointIndex+=1);
		return true;
	}

	return false;
}
