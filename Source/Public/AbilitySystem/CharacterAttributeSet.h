// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CharacterAttributeSet.generated.h"

// A helper macros that simplifies the getters, setters and initers setup for all attributes instead of declaring and defining ourselfes
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class IPawnUIInterface;

UCLASS()
class CRYPTRAIDER_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UCharacterAttributeSet();

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, CurrentHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, CurrentStamina);

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStamina);
	
	UPROPERTY(BlueprintReadOnly, Category = "Magic")
	FGameplayAttributeData CurrentMagic;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, CurrentMagic);

	UPROPERTY(BlueprintReadOnly, Category = "Magic")
	FGameplayAttributeData MaxMagic;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxMagic);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, AttackPower);
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefencePower;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, DefencePower);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, DamageTaken);

private:
	// Used here in order to cast to the avatar actor once if the weak object is null
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;
};
