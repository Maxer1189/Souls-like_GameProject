// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "CharacterTypes/CharacterStructTypes.h"
#include "Interfaces/Interaction_Interface.h"
#include "GameplayAbilitySpec.h"
#include "CryptRaiderCharacter.generated.h"

class ACryptRaiderWeapon;
class UHeroUIComponent;
class UDataAsset_InputConfig;
class ACryptRaiderHUD;
class UInventoryComponent;
class USpringArmComponent;
class UInputComponent;
class UCameraComponent;
class UHeroCombatComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable; // the thing that has been found already

	UPROPERTY()
	float LastInteractionCheckTime;
};


UCLASS(config=Game)
class ACryptRaiderCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ACryptRaiderCharacter();
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;
	
	
	FORCEINLINE UCameraComponent* GetThirdPersonCameraComponent() const { return ViewCameraComponent; }
	FORCEINLINE UInventoryComponent* GetHeroInventory() const { return PlayerInventory; }
	FORCEINLINE bool IsInteracting() const { return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Interaction); } // will return true if interaction timer is active
	FORCEINLINE ACryptRaiderWeapon* GetCurrentlyAssignedLightWeapon() const {return AssignedLightWeapon;}
	FORCEINLINE ACryptRaiderWeapon* GetCurrentlyAssignedHeavyWeapon() const {return AssignedHeavyWeapon;}
	
	FORCEINLINE void SetShouldUpdateStamina(const bool bShouldRecover) {bShouldUpdateStamina = bShouldRecover;}
	FORCEINLINE ACryptRaiderHUD* GetPlayerHUD() const {return HUD;}
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const {return HeroCombatComponent;}
	FORCEINLINE TMap<FGameplayTag, FGameplayAbilitySpecHandle> GetGrantedAbilitiesSpecHandlesMap() {return GrantedAbilitiesSpecHandlesMap;}
	
	void UpdateInteractionWidget() const;
	void ShowInventoryInfoMessage(const FText& MessageToDisplay) const;
	void DropItem(UItemBase* ItemToDrop, const int32 AmountToDrop);
	void AssignSelectedWeapon(UItemBase* ItemToAssign);
	void RemoveSelectedWeapon(UItemBase* ItemToRemove);
	void SetupPlayerInputMapping();
	virtual void UseItem(UItemBase* ItemToUse, const int32 AmountToUse) override;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Actions")
	TObjectPtr<UAnimMontage> HealMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Actions")
	TObjectPtr<UAnimMontage> PickupMontage;

protected:
	
	UPROPERTY()
	ACryptRaiderHUD* HUD;

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction") 
	TScriptInterface<IInteraction_Interface> TargetInteractable; // this helps to cast a line trace to an object and check if that object inherits the Interaction interface, this is the object we are checking
	
	UPROPERTY(EditDefaultsOnly, Category = "Character | Abilities")
	TArray<FCryptRaiderAbilitySet> AvailableAbilities;
	
	UPROPERTY(VisibleAnyWhere, Category = "Weapon Settings")
	TObjectPtr<ACryptRaiderWeapon> AssignedHeavyWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Settings")
	TObjectPtr<ACryptRaiderWeapon> AssignedLightWeapon;
	
	float InteractionFrequency;
	float InteractionCheckDistance; // distance of the line trace
	bool bIsCurrentlyOverlapping = false;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;
	
	
	UFUNCTION()
	void PerformInteractionCheckBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void PerformInteractionCheckEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface

	// ~ Begin IPawnCombatInterface Interface
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	// ~ End IPawnCombatInterface Interface

	// ~ Begin IPawnUIInterface Interface
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UHeroUIComponent* GetHeroUIComponent() const override;
	// ~ End IPawnUIInterface Interface

	
	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void EndInteract();
	void Interact(); // this is player interaction, the active, while the interact of the interface is the one receiving the action
	
	
	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	
	void LookUpAtRate(float Rate);

private:

#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameplayComponents", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHeroCombatComponent> HeroCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameplayComponents", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHeroUIComponent> HeroUIComponent;
		
	UPROPERTY(VisibleAnywhere, Category = "GameplayComponents", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> PlayerInventory;

#pragma endregion Components

#pragma region Inputs

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleMenu();
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void SwitchTargetTriggered(const FInputActionValue& Value);
	void SwitchTargetCompleted(const FInputActionValue& Value);

	FVector2D SwitchDirection = FVector2D::ZeroVector;
	
	void AbilityInput_Pressed(FGameplayTag InInputTag);
	void AbilityInput_Released(FGameplayTag InInputTag);
	void ToggleHeavyWeaponPressedState();
	void ToggleLightWeaponPressedState();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void BeginInteract();

#pragma endregion Inputs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	UPROPERTY()
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> GrantedAbilitiesSpecHandlesMap;
	
	FTimerHandle StaminaRecoveryHandle;
	bool bShouldUpdateStamina = true;
	void EquipWeapon(const bool bIsLightWeapon, const bool bShouldEquip, const bool bShouldSwap);
	void ClearAndInvalidateWeapon(ACryptRaiderWeapon* WeaponToClear);

};

