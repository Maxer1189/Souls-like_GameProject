// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GE_ExecCalc/GE_ExecCalc_DamageTaken.h"

#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "CryptRaider/DebugMacros.h"

struct FCharacterDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower); // MACRO that builds the Property
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefencePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);

	FCharacterDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, AttackPower, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, DefencePower, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, DamageTaken, Target, false);
	}
};

static const FCharacterDamageCapture& GetCharacterDamageCapture()
{
	static FCharacterDamageCapture CharacterDamageCapture;
	return CharacterDamageCapture;
}

UGE_ExecCalc_DamageTaken::UGE_ExecCalc_DamageTaken()
{
	// The slow way of doing a capture //
	
	// Searches for a field name in a specified scope, IE in CharacterAttributeSet it searches for AttackPower while verifying that it exists using GET_MEMBER_NAME_CHECKED
	
	// FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
	// 	UCharacterAttributeSet::StaticClass(),
	// 	GET_MEMBER_NAME_CHECKED(UCharacterAttributeSet, AttackPower));

	// Defines Gameplay attributes for gameplay effects. InSnapshot is when do we want to apply this capture.
	// when true we capture right when the gameplay spec is created and false means that we want to capture it when gameplay spec is applied
	
	// FGameplayEffectAttributeCaptureDefinition AttackCaptureDefinition(
	// 	AttackPowerProperty,
	// 	EGameplayEffectAttributeCaptureSource::Source,
	// 	false);

	RelevantAttributesToCapture.Add(GetCharacterDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetCharacterDamageCapture().DefencePowerDef);
	RelevantAttributesToCapture.Add(GetCharacterDamageCapture().DamageTakenDef);

	
}

// Full implementation of Damage calculation for all characters 
void UGE_ExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectSpec EffectSpec = ExecutionParams.GetOwningSpec(); // This is being set in the hero gameplay ability and its params can be used here
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetCharacterDamageCapture().AttackPowerDef, EvaluateParams, SourceAttackPower);
	Debug::Print(TEXT("SourceAttackPower"), SourceAttackPower);

	float BaseDamage = 0.f;
	float SpecialDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;
	
	for (const TPair<FGameplayTag, float> TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		// If this if check fires it means that we got hold of the base damage of the weapon
		if (TagMagnitude.Key.MatchesTagExact(CryptRaiderGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			// Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}
		// if this if check fires it means we got the hold of our current light attack combo count
		if (TagMagnitude.Key.MatchesTagExact(CryptRaiderGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount = TagMagnitude.Value;
			// Debug::Print(TEXT("UsedLightAttackComboCount"), UsedLightAttackComboCount);
		}

		if (TagMagnitude.Key.MatchesTagExact(CryptRaiderGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitude.Value;
			// Debug::Print(TEXT("UsedHeavyAttackComboCount"), UsedHeavyAttackComboCount);
		}

		if (TagMagnitude.Key.MatchesTagExact(CryptRaiderGameplayTags::Shared_SetByCaller_SpecialDamage))
		{
			SpecialDamage = TagMagnitude.Value;
			Debug::Print(TEXT("SpecialDamage"), SpecialDamage);
		}
	}

	float TargetDefencePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetCharacterDamageCapture().DefencePowerDef, EvaluateParams, TargetDefencePower);
	// Debug::Print(TEXT("TargetDefencePower"), TargetDefencePower);

	// Calculation of damage increment based on combo count

	if (UsedLightAttackComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05f + 1.f;
		BaseDamage *= DamageIncreasePercentLight;
		// Debug::Print(TEXT("ScaledBaseDamageLight"), BaseDamage);
	}

	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = (UsedHeavyAttackComboCount -1) * 0.15f + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
		// Debug::Print(TEXT("ScaledBasedDamageHeavy"), BaseDamage);
	}
	float DamageDone;
	if (BaseDamage > 0.f)
		DamageDone = BaseDamage * SourceAttackPower / TargetDefencePower;	
	else
		DamageDone = SpecialDamage * SourceAttackPower / TargetDefencePower;	
	
	Debug::Print(TEXT("FinalDamageDone"), DamageDone);
	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(GetCharacterDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override, DamageDone));
	}
}
