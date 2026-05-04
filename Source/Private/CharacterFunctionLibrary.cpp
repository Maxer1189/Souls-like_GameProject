// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "CharacterTypes/CharacterCountDownAction.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "CryptRaider/DebugMacros.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

UCharacterAbilitySystemComponent* UCharacterFunctionLibrary::NativeGetCharacterASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UCharacterAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

bool UCharacterFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag InTagToCheck)
{
	UCharacterAbilitySystemComponent* ASC = NativeGetCharacterASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(InTagToCheck);
}

void UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag InTagToAdd)
{
	UCharacterAbilitySystemComponent* ASC = NativeGetCharacterASCFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(InTagToAdd))
	{
		ASC->AddLooseGameplayTag(InTagToAdd); // Adds the tag. Won't add the same tag if already exists
	}
}

void UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag InTagToRemove)
{
	UCharacterAbilitySystemComponent* ASC = NativeGetCharacterASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(InTagToRemove))
	{
		ASC->RemoveLooseGameplayTag(InTagToRemove);
	}
}

void UCharacterFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag InTagToCheck,
	ECharacterConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, InTagToCheck) ? ECharacterConfirmType::Yes : ECharacterConfirmType::No;
}

UPawnCombatComponent* UCharacterFunctionLibrary::BP_GetPawnCombatComponentFromActorInfo(AActor* InActor,
	ECharacterValidType& OutValidType)
{
	OutValidType = NativeGetPawnCombatComponentFromActorInfo(InActor) ? ECharacterValidType::Valid : ECharacterValidType::Invalid;
	return NativeGetPawnCombatComponentFromActorInfo(InActor);
}

bool UCharacterFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

FGameplayTag UCharacterFunctionLibrary::ComputeHitReactDirectionAttack(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	float DotResult = FVector::DotProduct(VictimToAttackerNormalized, VictimForward);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);
	if (CrossResult.Z < 0.f)
		OutAngleDifference *= -1.f;

	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
		return CryptRaiderGameplayTags::Shared_Status_HitReact_Front;
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
		return CryptRaiderGameplayTags::Shared_Status_HitReact_Left;
	else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f )
		return CryptRaiderGameplayTags::Shared_Status_HitReact_Back;
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
		return CryptRaiderGameplayTags::Shared_Status_HitReact_Right;
	
	return CryptRaiderGameplayTags::Shared_Status_HitReact_Front;
}

bool UCharacterFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());
	// const FString DebugString = FString::Printf(TEXT("Dot Result: %f %s"), DotResult, DotResult<0.f? TEXT("Valid Block") : TEXT("No Block"));
	// Debug::Print(DebugString, DotResult<-0.1f? FColor::Green : FColor::Red);
	return DotResult < -0.1f;
}

bool UCharacterFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UCharacterAbilitySystemComponent* SourceASC = NativeGetCharacterASCFromActor(InInstigator);
	UCharacterAbilitySystemComponent* TargetASC = NativeGetCharacterASCFromActor(InTargetActor);
	
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UCharacterFunctionLibrary::CountDown(UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, ECharacterCountdownActionInput CountDownInput,
	UPARAM(DisplayName = "Output") ECharacterCountdownActionOutput& CountDownOutput, FLatentActionInfo LatentActionInfo)
{
	UWorld* World = nullptr;
	if (GEngine)
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	
	if (!World)
		return;
	
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FCharacterCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FCharacterCountDownAction>(LatentActionInfo.CallbackTarget, LatentActionInfo.UUID);
	if (CountDownInput == ECharacterCountdownActionInput::Start)
	{
		if (!FoundAction)
		{
			// Constructs a new latent action using the created action class.
			LatentActionManager.AddNewAction(
				LatentActionInfo.CallbackTarget,
				LatentActionInfo.UUID,
				new FCharacterCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentActionInfo)); // the new creation won't cause a memory leak because it will be managed by the LatentActionManager
		}
	}
	if (CountDownInput == ECharacterCountdownActionInput::Cancel)
	{
		if (FoundAction)
			FoundAction->CancelAction();
	}
}

UPawnCombatComponent* UCharacterFunctionLibrary::NativeGetPawnCombatComponentFromActorInfo(AActor* InActor)
{
	check(InActor);
	if (const IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}
