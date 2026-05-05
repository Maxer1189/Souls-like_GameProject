// World and Player
#include "Player/CryptRaiderCharacter.h"
#include "Enemies/Enemy.h"
#include "UserInterface/CryptRaiderHUD.h"
#include "World/Items/Weapons/WeaponBase.h"
#include "World/Items/Pickup.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "AbilitySystem/CharacterAttributeSet.h"

// Input
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"

// Game Framework
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Kismet
#include "Kismet/KismetSystemLibrary.h"

// Camera
#include "Camera/CameraComponent.h"

// Debug
#include "AbilitySystemBlueprintLibrary.h"
#include "CryptRaiderGameplayTags.h"
#include "AbilitySystem/CharacterAbilitySystemComponent.h"
#include "AnimInstances/Player/CryptRaiderLinkedAnimLayer.h"
#include "Components/CryptRaiderInputComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "DataAssets/StartupData/DataAsset_StartUpDataBase.h"
#include "Items/ItemBase.h"
#include "World/Items/Weapons/CryptRaiderWeapon.h"


ACryptRaiderCharacter::ACryptRaiderCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SocketOffset = FVector(0.f, 55.f, 65.f);
	SpringArm->TargetArmLength = 200.f;
	SpringArm->bUsePawnControlRotation = true;

	// Create a CameraComponent	
	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ViewCameraComponent->bUsePawnControlRotation = false;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	
	// Players default inventory Initializer
	InteractionFrequency = 0.1f;
	InteractionCheckDistance = 225.0f;
	
}

void ACryptRaiderCharacter::BeginPlay()
{ 
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACryptRaiderCharacter::PerformInteractionCheckBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ACryptRaiderCharacter::PerformInteractionCheckEndOverlap);
	HUD = Cast<ACryptRaiderHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	
	SetupPlayerInputMapping();
	
	Tags.Add(FName("PlayerCharacter"));
}

void ACryptRaiderCharacter::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionFrequency)
	{
	 	PerformInteractionCheck();
	}
}

void ACryptRaiderCharacter::SetupPlayerInputMapping()
{
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			for (const UInputMappingContext* MappingContext : InputConfigDataAsset->PlayerIMCs)
			{
				SubSystem->AddMappingContext(MappingContext, 0);
			}
		}
	}
}

void ACryptRaiderCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	if(UCryptRaiderInputComponent* EnhancedInputComponent = CastChecked<UCryptRaiderInputComponent>(PlayerInputComponent))
	{

		/*
		   Bind movement events
		   We have 2 versions of the rotation bindings to handle different kinds of devices differently
		   "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
		   "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
		*/
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_Move, ETriggerEvent::Triggered,this, &ThisClass::Move);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_ToggleMenu, ETriggerEvent::Triggered, this, &ThisClass::ToggleMenu);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_Equip_Weapon_Light, ETriggerEvent::Triggered, this, &ThisClass::ToggleLightWeaponPressedState);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_Equip_Weapon_Heavy, ETriggerEvent::Triggered, this, &ThisClass::ToggleHeavyWeaponPressedState);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::SwitchTargetTriggered);
		EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, CryptRaiderGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::SwitchTargetCompleted);
		
		EnhancedInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ACryptRaiderCharacter::AbilityInput_Pressed, &ACryptRaiderCharacter::AbilityInput_Released);
	}
}

void ACryptRaiderCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (CharacterAbilitySystemComponent && CharacterAttributeSet)
	{
		if (!CharacterStartupData.IsNull())
		{
			if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartupData.LoadSynchronous())
			{
				LoadedData->GiveToAbilitySystemComponent(CharacterAbilitySystemComponent);
			}
		}
	}
}

UPawnCombatComponent* ACryptRaiderCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* ACryptRaiderCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* ACryptRaiderCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void ACryptRaiderCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionalVector = Value.Get<FVector2D>();
	if (GetController())
	{
		const FRotator ControllerRotator = GetControlRotation();
		const FRotator YawControl(0.f, ControllerRotator.Yaw, 0.f);

		FVector ForwardControllerDirection = FRotationMatrix(YawControl).GetUnitAxis(EAxis::X);
		FVector RightControllerDirection = FRotationMatrix(YawControl).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardControllerDirection, DirectionalVector.Y);
		AddMovementInput(RightControllerDirection, DirectionalVector.X);
	}
}

void ACryptRaiderCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if(GetController())
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACryptRaiderCharacter::SwitchTargetTriggered(const FInputActionValue& Value) // actually binds the switch input 
{
	SwitchDirection = Value.Get<FVector2D>();
}

void ACryptRaiderCharacter::SwitchTargetCompleted(const FInputActionValue& Value) // Notifies the ability of the performed switch
{
	FGameplayEventData EventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.f? CryptRaiderGameplayTags::Player_Event_SwitchTarget_Right : CryptRaiderGameplayTags::Player_Event_SwitchTarget_Left,
		EventData
		);
	
}

void ACryptRaiderCharacter::AbilityInput_Pressed(FGameplayTag InInputTag)
{
	CharacterAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void ACryptRaiderCharacter::AbilityInput_Released(FGameplayTag InInputTag)
{
	CharacterAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}

void ACryptRaiderCharacter::ToggleHeavyWeaponPressedState()
{
	if (HeroCombatComponent)
	{
		if (HeroCombatComponent->bLightWeaponAttachedSuccessfully && AssignedHeavyWeapon)
			EquipWeapon(false, true, true);
		else if (HeroCombatComponent->bHeavyWeaponAttachedSuccessfully)
			EquipWeapon(false, false, false);
		else
			EquipWeapon(false, true, false);	
	}
}

void ACryptRaiderCharacter::ToggleLightWeaponPressedState()
{
	if (HeroCombatComponent)
	{
		if (HeroCombatComponent->bHeavyWeaponAttachedSuccessfully && AssignedLightWeapon)
			EquipWeapon(true, true, true);
		else if (HeroCombatComponent->bLightWeaponAttachedSuccessfully)
			EquipWeapon(true, false, false);
		else
			EquipWeapon(true, true, false);	
	}
}

void ACryptRaiderCharacter::EquipWeapon(const bool bIsLightWeapon, const bool bShouldEquip, const bool bShouldSwap)
{
	FGameplayTag EventTag;
	// Light Weapon Action Checks
	if (bIsLightWeapon && bShouldEquip)
	{
		if (bShouldSwap)
			EventTag =  CryptRaiderGameplayTags::Player_Event_SwapWeapon;
		else
			EventTag = CryptRaiderGameplayTags::Player_Event_EquipLightWeapon;	
	}
	else if (bIsLightWeapon && !bShouldEquip)
		EventTag = CryptRaiderGameplayTags::Player_Event_UnequipLightWeapon;
	// Heavy Weapon Action Checks
	else if (!bIsLightWeapon && bShouldEquip)
	{
		if (bShouldSwap)
			EventTag = CryptRaiderGameplayTags::Player_Event_SwapWeapon;
		else
			EventTag = CryptRaiderGameplayTags::Player_Event_EquipHeavyWeapon;
	}
	else if (!bIsLightWeapon && !bShouldEquip)
		EventTag = CryptRaiderGameplayTags::Player_Event_UnequipHeavyWeapon;

	FGameplayEventData EventData;
	EventData.Instigator = this;
	EventData.Target = this;
	EventData.EventTag = EventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void ACryptRaiderCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACryptRaiderCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

																														  /////////////////////////////////
 																														 /// CORE INTERACTION SYSTEM  ////
																														/////////////////////////////////

void ACryptRaiderCharacter::PerformInteractionCheckBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsCurrentlyOverlapping = true;
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	if(SweepResult.GetActor()->GetClass()->ImplementsInterface(UInteraction_Interface::StaticClass())) // Returns true if that hit Actor implements the interaction interface c++ class
	{
		// const float Distance = (TraceStart - HitResult.ImpactPoint).Size(); stores the length of the vector between the actor and the object that being traced to

		if (SweepResult.GetActor() != InteractionData.CurrentInteractable) // Checks if the actor is not looking at the same interactable
		{
			FoundInteractable(SweepResult.GetActor()); // Adds the Object 
			return;
		}

		if(SweepResult.GetActor() == InteractionData.CurrentInteractable)
		{
			return;
		}
	}

	NoInteractableFound();
}

void ACryptRaiderCharacter::PerformInteractionCheckEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsCurrentlyOverlapping = false;
	NoInteractableFound();
}

void ACryptRaiderCharacter::PerformInteractionCheck()
{
	if(!bIsCurrentlyOverlapping)
	{
		InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
		
		FVector TraceStart{GetPawnViewLocation()}; //  bracket initialization occurs when a variable is created and assigns an initial value, while assignment happens after the variable is already created and assigns a new value to it, will throw an error if something else gets in instead of converting it 
		FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

		float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector()); // Dot Product measures how closely the vectors align. If the vectors angle is greater than 90 degrees the dot product will be negative

		if(LookDirection > 0)
		{
			FCollisionQueryParams PlayerCollisionParams;
			PlayerCollisionParams.AddIgnoredActor(this);
			FHitResult HitResult;

			if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, PlayerCollisionParams)) // Returns true if a hit has been registered
			{
				if(HitResult.GetActor()->GetClass()->ImplementsInterface(UInteraction_Interface::StaticClass())) // Returns true if that hit Actor implements the interaction interface
				{
					// const float Distance = (TraceStart - HitResult.ImpactPoint).Size(); stores the length of the vector between the actor and the object that being traced to

					if (HitResult.GetActor() != InteractionData.CurrentInteractable) // Checks if the actor is not looking at the same interactable
					{
						FoundInteractable(HitResult.GetActor()); // Adds the Object 
						return;
					}

					if(HitResult.GetActor() == InteractionData.CurrentInteractable)
					{
						return;
					}
				}
			}
		}

		NoInteractableFound();
	}
}

void ACryptRaiderCharacter::FoundInteractable(AActor* NewInteractable) 
{
	// TODO: Has a bug, gets filled with garbage data when an enemy is attacking the player, player can see the pickup message and still crashes sometimes
	
	if (IsInteracting()) // backup if, if somehow a new object is discovered in the middle of interacting, then it'll end it
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;
	
	if (TargetInteractable->InteractableData.bHasBeenOperated)
	{
		NoInteractableFound();
		return;
	}
	
	TargetInteractable->BeginFocus();
	if(HUD) HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

}

void ACryptRaiderCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable) // Checks If there's an object from previous use
	{
		if (IsValid(TargetInteractable.GetObject())) // Checks if that object exists and ends the focus. This check happens because sometimes the object is destroyed and EndFocus called on nullptr
		{
			TargetInteractable->EndFocus();
		}
	}

	if(HUD) HUD->HideInteractionWidget();

	InteractionData.CurrentInteractable = nullptr;
	TargetInteractable = nullptr;
}

void ACryptRaiderCharacter::BeginInteract()
{
	// verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			// If there's a valid delay above the error tolerance then we set the timer, the timer runs then interacts
			if(FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDelayDuration, 0.1f)) // error tolerance is maximum allowed difference to zero
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(
					TimerHandle_Interaction,
					this, 
					&ACryptRaiderCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDelayDuration,
					false);
			}
		}
	}
}

void ACryptRaiderCharacter::EndInteract()
{

	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject()) && HUD)
	{
		TargetInteractable->EndInteract();
		HUD->HideInteractionWidget();
	}
	
}

void ACryptRaiderCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject())) 
	{
		TargetInteractable->Interact(this);
	}

	EndInteract();
}

void ACryptRaiderCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData); // Updates the widget on the character
	}
}

void ACryptRaiderCharacter::ShowInventoryInfoMessage(const FText& MessageToDisplay) const
{
	if (HUD) HUD->ShowInfoMessage(MessageToDisplay);
}

void ACryptRaiderCharacter::ToggleMenu()
{
	if(HUD) HUD->ToggleMenu();
}

void ACryptRaiderCharacter::DropItem(UItemBase* ItemToDrop, const int32 AmountToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop)) // Checks if the inventory has that item
	{
		if (!PlayerInventory->IsItemWeapon(ItemToDrop))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Will handle any problems while spawning such spawn inside a wall, will re-adjust it

			FHitResult TraceHitResult;
			TArray<TObjectPtr<AActor>> ActorsToIgnore;
			ActorsToIgnore.Add(this);
			
			const FVector SpawnLocation{GetActorLocation() + GetActorForwardVector() * 130.0f}; // location of the spawn, players location + offset of 100 units
			const FVector SpawnGroundEndPoint(0.f,0.f,-500.f);
			
			bool bFoundGroundHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, SpawnLocation * SpawnGroundEndPoint, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, TraceHitResult, true);
			if (bFoundGroundHit)
			{
				const FTransform SpawnTransform(GetActorRotation(), TraceHitResult.ImpactPoint); // Transform has all the parameters of location rotation and scale
				const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, AmountToDrop);
				APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
				Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Cannot Drop Weapons!"));
		
	}
	else
	{
		UE_LOG(LogInventory, Error, TEXT("Item to drop was somehow null!"));
		check(false);
	}

}

void ACryptRaiderCharacter::ClearAndInvalidateWeapon(ACryptRaiderWeapon* WeaponToClear)
{
	FHeroWeaponData& WeaponData = WeaponToClear->GetWeaponTypeData();
	
	HeroCombatComponent->RemoveRegisteredWeapon(WeaponToClear->GetItemData()->ItemType);
	GetMesh()->UnlinkAnimClassLayers(WeaponData.WeaponAnimLayerToLink);
	
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		SubSystem->RemoveMappingContext(WeaponData.WeaponInputMappingContext);
		GetCharacterAbilitySystemComponent()->RemoveGrantedHeroWeaponAbilities(WeaponToClear);
	}
}


void ACryptRaiderCharacter::AssignSelectedWeapon(UItemBase* ItemToAssign)
{
	if (HUD)
	{
		if (!IsValid(WeaponClass))
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon Class is null! Will not spawn weapon"));
			return;
		}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const FTransform SpawnTransform(GetActorRotation(), GetActorLocation());
		
		switch(ItemToAssign->WeaponData.WeaponWeightClass)
		{
		case EWeaponWeightClass::Light:
			if (AssignedLightWeapon)
			{
				ClearAndInvalidateWeapon(AssignedLightWeapon);
				GetHeroCombatComponent()->bLightWeaponAttachedSuccessfully = false;
				AssignedLightWeapon->Destroy();
				AssignedLightWeapon = nullptr;
			}
			
			AssignedLightWeapon = GetWorld()->SpawnActor<ACryptRaiderWeapon>(WeaponClass, SpawnTransform, SpawnParams);
			AssignedLightWeapon->ArmPlayerWithWeapon(this, ItemToAssign);
			HeroCombatComponent->RegisterSpawnedWeapon(AssignedLightWeapon->GetItemData()->ItemType, AssignedLightWeapon, EWeaponWeightClass::Light, true);
			
			if (!GrantedAbilitiesSpecHandlesMap.Contains((CryptRaiderGameplayTags::InputTag_Equip_Weapon_Light)) && !GrantedAbilitiesSpecHandlesMap.Contains((CryptRaiderGameplayTags::InputTag_Equip_Weapon_Heavy)))
				GrantedAbilitiesSpecHandlesMap = CharacterAbilitySystemComponent->GrantAvailableAbilityToHero(AvailableAbilities, CryptRaiderGameplayTags::InputTag_Equip_Weapon_Light, GrantedAbilitiesSpecHandlesMap);;
			break;
			
		case EWeaponWeightClass::Heavy:
			if (AssignedHeavyWeapon)
			{
				ClearAndInvalidateWeapon(AssignedHeavyWeapon);
				GetHeroCombatComponent()->bHeavyWeaponAttachedSuccessfully = false;
				AssignedHeavyWeapon->Destroy();
				AssignedHeavyWeapon = nullptr;
			}
			
			AssignedHeavyWeapon = GetWorld()->SpawnActor<ACryptRaiderWeapon>(WeaponClass, SpawnTransform, SpawnParams);
			AssignedHeavyWeapon->ArmPlayerWithWeapon(this, ItemToAssign);
			HeroCombatComponent->RegisterSpawnedWeapon(AssignedHeavyWeapon->GetItemData()->ItemType, AssignedHeavyWeapon, EWeaponWeightClass::Heavy, true);

			if (!GrantedAbilitiesSpecHandlesMap.Contains((CryptRaiderGameplayTags::InputTag_Equip_Weapon_Light)) && !GrantedAbilitiesSpecHandlesMap.Contains((CryptRaiderGameplayTags::InputTag_Equip_Weapon_Heavy)))
				GrantedAbilitiesSpecHandlesMap = CharacterAbilitySystemComponent->GrantAvailableAbilityToHero(AvailableAbilities, CryptRaiderGameplayTags::InputTag_Equip_Weapon_Heavy, GrantedAbilitiesSpecHandlesMap);
			break;
			
		case EWeaponWeightClass::Small:
			return;
		case EWeaponWeightClass::None:
			return;
		}
		
		HUD->UpdateWeaponSlot(ItemToAssign, ItemToAssign->WeaponData.WeaponWeightClass);
	}
}

void ACryptRaiderCharacter::RemoveSelectedWeapon(UItemBase* ItemToRemove)
{
	switch (ItemToRemove->WeaponData.WeaponWeightClass)
	{
	case EWeaponWeightClass::Light:
		
		HeroCombatComponent->RemoveRegisteredWeapon(AssignedLightWeapon->GetItemData()->ItemType);
		if (!AssignedHeavyWeapon)
			GrantedAbilitiesSpecHandlesMap = CharacterAbilitySystemComponent->RemoveAvailableAbilityFromHero(CryptRaiderGameplayTags::InputTag_Equip_Weapon_Light, GrantedAbilitiesSpecHandlesMap);
		
		AssignedLightWeapon->Destroy();
		AssignedLightWeapon = nullptr;
		break;
		
	case EWeaponWeightClass::Heavy:
		
		HeroCombatComponent->RemoveRegisteredWeapon(AssignedHeavyWeapon->GetItemData()->ItemType);
		if (!AssignedLightWeapon)
			GrantedAbilitiesSpecHandlesMap = CharacterAbilitySystemComponent->RemoveAvailableAbilityFromHero(CryptRaiderGameplayTags::InputTag_Equip_Weapon_Heavy, GrantedAbilitiesSpecHandlesMap);
		
		AssignedHeavyWeapon->Destroy();
		AssignedHeavyWeapon = nullptr;
		break;
		
	case EWeaponWeightClass::Small:
		return;
	case EWeaponWeightClass::None:
		return;
	}
	HUD->UpdateWeaponSlot(nullptr, ItemToRemove->WeaponData.WeaponWeightClass);
}

void ACryptRaiderCharacter::UseItem(UItemBase* ItemToUse, const int32 AmountToUse)
{
	if (PlayerInventory->FindMatchingItem(ItemToUse))
	{
		if (ItemToUse->IsConsumable())
		{
			
		}
	}
	else
	{
		UE_LOG(LogInventory, Error, TEXT("Item to use was somehow null!"));
		check(false);
	}
}

