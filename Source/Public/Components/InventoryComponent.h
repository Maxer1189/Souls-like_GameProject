// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemBase.h"
#include "InventoryComponent.generated.h"

struct FGameplayTag;

// Declares a custom log message for the inventory
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddItemMessage);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, FUsedInventoryCategorySlots UsedSlots);
// A Delegate is A function that acts as a "Broadcaster" which will be seen only by a "Subscriber". it sends a broadcast and only the specified subscriber will run it.
// for example we need to update the UI and redraw it when an item is added. The UI will call a delegate to do that.

class UItemBase;
class AWeaponBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8 
{
	EIR_NoItemAdded UMETA(DisplayName = "No Item Added"),
	EIR_PartialAmountItemAdded UMETA(DisplayName = "Partial Item Added"),
	EIR_AllItemAdded UMETA(DisplayName = "All of Item Added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_USTRUCT_BODY()

	FItemAddResult() : ActualAmountAdded(0), OperationResult(EItemAddResult::EIR_NoItemAdded), ResultMessage(FText::GetEmpty()) {};

	// Actual amount of item that was added to the inventory
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	// Enum representing the end state of an item add operation
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	// Informational message that can be passed with the result
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	// Helper functions that will help locating problems

	static FItemAddResult AddedNone(const FText& ErrorText) // static class allows to call the function directly without creating an object in a memory that will call the function later
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult  = EItemAddResult::EIR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	}
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult PartialAddResult;
		PartialAddResult.ActualAmountAdded = PartialAmountAdded;
		PartialAddResult.OperationResult = EItemAddResult::EIR_PartialAmountItemAdded;
		PartialAddResult.ResultMessage = ErrorText;
		return PartialAddResult;
	}
	
	static FItemAddResult AddedAll(const int32 AmountAdded,  const FText& Message)
	{
		FItemAddResult AddAllResult;
		AddAllResult.ActualAmountAdded = AmountAdded;
		AddAllResult.OperationResult = EItemAddResult::EIR_AllItemAdded;
		AddAllResult.ResultMessage = Message;
		return AddAllResult;
	}

};


USTRUCT(BlueprintType)
struct FUsedInventoryCategorySlots
{
	GENERATED_USTRUCT_BODY()
	
	void SetUsedConsumableSlots(const int32 NewUsedConsumableSlots) {UsedConsumableSlots = NewUsedConsumableSlots;}
	void SetUsedCraftableSlots(const int32 NewUsedCraftableSlots) {UsedCraftableSlots = NewUsedCraftableSlots;}
	void SetUsedWeaponSlots(const int32 NewUsedWeaponSlots) {UsedWeaponSlots = NewUsedWeaponSlots;}
	void SetUsedKeyItemSlots(const int32 NewUsedKeyItemSlots) {UsedKeyItemSlots = NewUsedKeyItemSlots;}
	int32 GetUsedConsumableSlots() const {return UsedConsumableSlots;}
	int32 GetUsedCraftableSlots() const {return UsedCraftableSlots;}
	int32 GetUsedWeaponSlots() const {return UsedWeaponSlots;}
	int32 GetUsedKeyItemSlots() const {return UsedKeyItemSlots;}
	
protected:
	UPROPERTY()
	int32 UsedConsumableSlots{0};

	UPROPERTY()
	int32 UsedCraftableSlots{0};

	UPROPERTY()
	int32 UsedWeaponSlots{0};

	UPROPERTY()
	int32 UsedKeyItemSlots{0};
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	//=================================================================================================
	// PROPERTIES AND VARIABLES
	//=================================================================================================
	FOnInventoryUpdated OnInventoryUpdated;
	FOnAddItemMessage OnAddItemMessage;

	//=================================================================================================
	//  FUNCTIONS
	//=================================================================================================

	UInventoryComponent();
	
	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemIn) const; // will take in the pointer of the item and compare this item in memory to items in the array, if its there or not

	UFUNCTION(category = "Inventory")
	UItemBase* FindMatchingItemByGameplayTag(EItemCategory SearchCategory ,FGameplayTag TagToFind); 
	
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* ItemIn) const; // uses the overloaded operator in UItemBase to search items ID
	
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;

	// UFUNCTION(Category = "Inventory")
	// int32 DistributeItemStack(UItemBase* ItemIn, int32 RequestedAddAmount, int32 AmountToDistribute);
	
	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItemBase* InputItem); // this will check if adding new item is possible or not

	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddPotions(UItemBase* PotionIn);
	
	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove); // will completely delete entry from an array, quantity will be set to 0
	
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove); // will remove desired amount but will keep the rest of the item, 
	
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit, int32& UsedCategorySlots);
	
	bool IsItemWeapon(UItemBase* ItemIn) const;
	static int32 DetermineItemCategoryCapacity(const UItemBase* ItemIn, const FUsedInventoryCategorySlots& UsedCategoriesSlots);
	// getters ------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FORCEINLINE FUsedInventoryCategorySlots GetUsedInventorySlots() const {return AllCategoriesUsedSlots;}
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const {return InventorySlotsCapacity;} // returns amount of slots available in the inventory

	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetAmountOfHeldPotions() const {return AmountOfHeldPotions;}

	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const {return InventoryContents;} // returns all contents of the inventory in a array of objects
	
	UFUNCTION(Category = "Inventory")
	FGameplayTag GetItemType(UItemBase* ItemIn) const;
	
	// setters ------------------------------------
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotCapacity(const int32 NewSlotsCapacity) {InventorySlotsCapacity = NewSlotsCapacity; OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);}

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetHealingPotionsCapacity(const int32 NewPotionsCapacity) {InventoryHealingPotionsCapacity = NewPotionsCapacity;}

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetAmountOfHeldPotions(const int32 NewAmount) {AmountOfHeldPotions = NewAmount;}


protected:
	
	//=================================================================================================
	// PROPERTIES AND VARIABLES
	//=================================================================================================

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	FUsedInventoryCategorySlots AllCategoriesUsedSlots;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 InventorySlotsCapacity = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 InventoryHealingPotionsCapacity = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 AmountOfHeldPotions = 0;

	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents; // Templated object pointer, a better way to refer to an array of objects of UItemBase. This is the actual array of objects that we will work with.
	
	

	//=================================================================================================
	//  FUNCTIONS
	//=================================================================================================

	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn, int32& UsedCategorySlots);
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount, int32& UsedCategorySlots);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount); // calculates how much item is needed to complete full stack
	

	// will put a new entry to the array
	void AddNewItem(UItemBase* Item, const int32 AmountToAdd, const int32 UsedCategorySlots);
	void AssignCategorySlotsCapacity(const UItemBase* Item, const int32& NewSlotsCapacity);

};
