// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRYPTRAIDER_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UEnemyCombatComponent();

	virtual void OnHitTargetActor(AActor* HitActor) override;

protected:
	virtual void ToggleBodyCollisionBox(bool bShouldEnable, EToggleDamageType ToggleDamageType) override;	
};
