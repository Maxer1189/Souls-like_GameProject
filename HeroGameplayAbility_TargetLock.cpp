// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "CharacterFunctionLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "EnhancedInputSubSystems.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/CryptRaiderCharacter.h"
#include "Player/CryptRaiderPlayerController.h"
#include "UserInterface/HUD/CharacterWidgetBase.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	if (CurrentLockedActor)
		InitTargetLockMappingContext();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	Cleanup();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		UCharacterFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, CryptRaiderGameplayTags::Shared_Status_Dead))
	{
		TryLockOnTarget();	
	}
	if (UCharacterFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), CryptRaiderGameplayTags::Shared_Status_Dead) || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();
	

	bool IsCurrentlyDodging = UCharacterFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), CryptRaiderGameplayTags::Player_Status_Dodging);
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetHeroCharacterFromActorInfo()->GetActorLocation(), CurrentLockedActor->GetActorLocation());
	LookAtRot-= FRotator(TargetLockCameraOffsetDistance, 0, 0);
	
	const FRotator CurrentRot = GetControllerFromActorInfo()->GetControlRotation();
	const FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, TargetLockRotationInterpSpeed);
	GetControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f)); // For the rotation of the camera

	if (!IsCurrentlyDodging)
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f)); // For the rotation of the player
	
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(FGameplayTag InSwitchDirection)
{
	GetAvailableTargetsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock;
	
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);
	if (InSwitchDirection.MatchesTagExact(CryptRaiderGameplayTags::Player_Event_SwitchTarget_Left))
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	else
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);

	if (NewTargetToLock)
		CurrentLockedActor = NewTargetToLock;
	
	
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableTargetsToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);
	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
		InitTargetLockMovement();
	}
	else
		CancelTargetLockAbility();
	
}

void UHeroGameplayAbility_TargetLock::GetAvailableTargetsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult> BoxHitResults;
	TArray<AActor*> ActorsToIgnore;

	if (CurrentLockedActor && UCharacterFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, CryptRaiderGameplayTags::Shared_Status_Dead))
		ActorsToIgnore.Add(CurrentLockedActor);
	
	ACryptRaiderCharacter* HeroCharacter = GetHeroCharacterFromActorInfo();
	UKismetSystemLibrary::BoxTraceMultiForObjects(HeroCharacter,
		HeroCharacter->GetActorLocation(),
		HeroCharacter->GetActorLocation() + HeroCharacter->GetActorForwardVector() * BoxTraceDistance,
		BoxTraceSize/ 2.f,
		HeroCharacter->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		ActorsToIgnore,
		bShowPersistentShapes ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxHitResults,
		true);

	for (FHitResult& TraceHit : BoxHitResults)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != HeroCharacter)
			{
				AvailableActorsToLock.AddUnique(HitActor);
				// Debug::Print(HitActor->GetActorNameOrLabel());
			}
		}
	}
	if (ActorsToIgnore.Num() > 0)
		ActorsToIgnore.Empty();
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
		FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
			OutActorsOnRight.AddUnique(AvailableActor);
		
		else
			OutActorsOnLeft.AddUnique(AvailableActor);
		
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Missing Target Lock Widget Class"));
		DrawnTargetLockWidget = CreateWidget<UCharacterWidgetBase>(GetControllerFromActorInfo(), TargetLockWidgetClass);
		
		check(DrawnTargetLockWidget);
		DrawnTargetLockWidget->AddToViewport();
	}
	
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}
	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true);

	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget([this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
			);
	}

	ScreenPosition -= TargetLockWidgetSize / 2.f;
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition,false);	
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::Cleanup()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
		DrawnTargetLockWidget->RemoveFromParent();

	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetControllerFromActorInfo()->GetLocalPlayer());
	check(SubSystem);

	SubSystem->AddMappingContext(TargetLockMappingContext, 3);
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetControllerFromActorInfo()) return;
	
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetControllerFromActorInfo()->GetLocalPlayer());
	check(SubSystem);

	SubSystem->RemoveMappingContext(TargetLockMappingContext);
}
