// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTypes/CharacterStructTypes.h"
#include "AbilitySystem/Abilities/HeroGameplayAbility.h"

bool FCryptRaiderAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
