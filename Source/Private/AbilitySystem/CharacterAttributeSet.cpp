// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CharacterAttributeSet.h"

#include "CharacterFunctionLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "Components/UI/HeroUIComponent.h"
#include "Components/UI/PawnUIComponent.h"
#include "CryptRaider/DebugMacros.h"
#include "Interfaces/PawnUIInterface.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentStamina(1.f);
	InitMaxStamina(1.f);
	InitCurrentMagic(1.f);
	InitMaxMagic(1.f);
	InitAttackPower(1.f);
	InitDefencePower(1.f);
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s, didnt implement IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetName());

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent, TEXT("Couldn't extract a PawnUIComponent from %s"), *Data.Target.GetAvatarActor()->GetName());
	
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	if (Data.EvaluatedData.Attribute == GetCurrentMagicAttribute())
	{
		const float NewCurrentMagic = FMath::Clamp(GetCurrentMagic(), 0.f, GetMaxMagic());
		SetCurrentMagic(NewCurrentMagic);

		if (GetCurrentMagic() == GetMaxMagic())
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Magic_Full);
		}
		else if (GetCurrentMagic() == 0.f)
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Magic_None);
		}
		else
		{
			UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Magic_Full);
			UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Magic_None);
		}
		
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentMagicChanged.Broadcast(GetCurrentMagic() / GetMaxMagic());
		}
	}
	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		const float NewCurrentStamina = FMath::Clamp(GetCurrentStamina(), 0.f, GetMaxStamina());
		SetCurrentStamina(NewCurrentStamina);
		
		if (GetCurrentStamina() == GetMaxStamina())
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_Full);
			UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_NotFull);
		}
		else if (GetCurrentStamina() <= 0.f)
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_None);
		}
		else
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_NotFull);
			UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_Full);
			UCharacterFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Player_Status_Stamina_None);
		}
		
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentStaminaChanged.Broadcast(GetCurrentStamina() / GetMaxStamina());
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		
		// FString DebugMsg = FString::Printf(TEXT("Old health: %f, Damage done: %f, New current health: %f"),
		// 	OldHealth, DamageDone, NewCurrentHealth);
		// Debug::Print(DebugMsg, FColor::Green);
		
		if (GetCurrentHealth() <= 0.f)
		{
			UCharacterFunctionLibrary::AddGameplayTagToActorIfNone(	Data.Target.GetAvatarActor(), CryptRaiderGameplayTags::Shared_Status_Dead);
			if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
				HeroUIComponent->OnAbilityCancelCoolDown.Broadcast();
		}
	}
}
