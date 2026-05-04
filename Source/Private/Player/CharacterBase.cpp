// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CharacterBase.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true; // Disables TICK  entirely
	GetMesh()->SetReceivesDecals(false);
	
	CharacterAbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterBase::UseItem(UItemBase* ItemToUse, int32 AmountToUse)
{
	
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return CharacterAbilitySystemComponent;
}

UPawnCombatComponent* ACharacterBase::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* ACharacterBase::GetPawnUIComponent() const
{
	return nullptr;
}

void ACharacterBase::PlayHitReactMontage(const float& Angle)
{
	
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (CharacterAbilitySystemComponent)
	{
		CharacterAbilitySystemComponent->InitAbilityActorInfo(this, this);
		ensureMsgf(!CharacterStartupData.IsNull(), TEXT("No assigned startup data found on %s!!"), *GetName());
	}
}


float ACharacterBase::CalculateDirectionalHit(const FVector& ImpactPoint) const // Calculates the direction the hit came from the player using dot product and cross product
{
	const FVector Forward = GetActorForwardVector();
	const FVector LoweredImpactPoint = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (LoweredImpactPoint - GetActorLocation()).GetSafeNormal(); // Calculates the Vector magnitude between enemies location and its forward vector and then normalizes it for the dot product calculation. ** Vector Normalization: Dividing the Vector itself by its magnitude that will result in 1

	// Forward (dotProduct) ToHit = |Forward| |ToHit| * cos(theta);
	// Forward = 1, ToHit = 1, so Forward (dotProduct) ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse of cosine (arc cosine of theta)
	float Angle = FMath::Acos(CosTheta); // in radians
	
	Angle = FMath::RadiansToDegrees(Angle);

	// if cross product points down, theta is negative, meaning hit from the left
	FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if(CrossProduct.Z < 0)
	{
		Angle *= -1.f;
	}
	return Angle;
}

FVector ACharacterBase::GetTranslationWarpTarget(const float AnimationDuration, const AActor* CombatTarget) const
{
	if(CombatTarget == nullptr) return FVector();
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector DeltaDistance = (CombatTargetLocation - Location).GetSafeNormal(); // Distance between the combat target and the current actor
	DeltaDistance*=WarpTargetDistance;

	const auto InterpLocation = FMath::VInterpTo(Location, CombatTargetLocation, UGameplayStatics::GetWorldDeltaSeconds(this), AnimationDuration);
	return InterpLocation;
}

FRotator ACharacterBase::GetRotationWarpTarget(const AActor* CombatTarget)
{
	if(CombatTarget) return CombatTarget->GetActorLocation().Rotation();

	return FRotator();
}

