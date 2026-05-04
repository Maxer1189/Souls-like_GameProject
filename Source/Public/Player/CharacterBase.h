// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/CharacterStates.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "CharacterBase.generated.h"

class UMotionWarpingComponent;
class UDataAsset_StartUpDataBase;
class UCharacterAttributeSet;
class UCharacterAbilitySystemComponent;
class UItemBase;
class AWeaponBase;
class UAnimMontage;


UCLASS()
class CRYPTRAIDER_API ACharacterBase : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	
	ACharacterBase();
	
	FORCEINLINE UCharacterAbilitySystemComponent* GetCharacterAbilitySystemComponent() const {return CharacterAbilitySystemComponent;}
	FORCEINLINE UCharacterAttributeSet* GetCharacterAttributeSet() const {return CharacterAttributeSet;}
	
	// ~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ End IAbilitySystemInterface Interface

	// ~ Begin IPawnCombatInterface Interface
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	// ~ End IPawnCombatInterface Interface

	// ~ Begin IPawnUIInterface Interface
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	// ~ End IPawnUIInterface Interface

	// virtual void PlayMontage(UAnimMontage* MontageToPlay, const FName& SectionName);
	virtual void UseItem(UItemBase* ItemToUse, int32 AmountToUse);

protected:
	
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	float CalculateDirectionalHit(const FVector& ImpactPoint) const;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	FVector GetTranslationWarpTarget(const float AnimationDuration, const AActor* CombatTarget) const;
	FRotator GetRotationWarpTarget(const AActor* CombatTarget);
	
	
	UFUNCTION()
	virtual void PlayHitReactMontage(const float& Angle);
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	TSubclassOf<AWeaponBase> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Settings")
	UStaticMeshComponent* WeaponMesh;
	
	UPROPERTY(EditAnywhere, Category = "Montages | Combat")
	float WarpTargetDistance = 75.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttributeSystem")
	TObjectPtr<UCharacterAbilitySystemComponent> CharacterAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttributeSystem")
	TObjectPtr<UCharacterAttributeSet> CharacterAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarpingComponent")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartupData;
	
};
