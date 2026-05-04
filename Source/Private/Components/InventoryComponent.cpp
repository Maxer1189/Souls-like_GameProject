// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindMatchingItemByGameplayTag(EItemCategory SearchCategory, FGameplayTag TagToFind)
{
	for (UItemBase* CurrentItem : InventoryContents)
	{
		if (CurrentItem->ItemCategory != SearchCategory) continue;
		if (CurrentItem->ItemType.MatchesTagExact(TagToFind)) return CurrentItem;
	}
	return nullptr;
}

bool UInventoryComponent::IsItemWeapon(UItemBase* ItemIn) const
{
	if(ItemIn->ItemCategory == EItemCategory::Weapon)
	{
		return true;
	}
	return false;
}

FGameplayTag UInventoryComponent::GetItemType(UItemBase* ItemIn) const
{
	return ItemIn->ItemType;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if(const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn)) // we get the pointer to the array, then get the element type and stores in a pointer called result which then is cheked if its ID equals to the ItemIn.
		{
			return *Result; // dereferences the pointer. so that we return a pointer and not a double pointer 
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{ // Predicate is a condition. 
  // Lambda is a temporary function that is made by using []() syntax. []- is a capture clause. it allows the labmda to see outside variables.
  // FindByPredicate loops through our TArray, for each element it places it into a lambda and names the element InventoryItem, then for each element our lambda checks and returns the item that is indeed a partial stack. If all succeeds returns dereferenced Result
  	if(const TArray<TObjectPtr<UItemBase>>::ElementType* Result = 
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem) 
		{
			return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
		}))
	{
		return *Result;
	}

	return nullptr;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(AddAmountToMakeFullStack, InitialRequestAmount);
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	int32 UsedCategorySlots = DetermineItemCategoryCapacity(ItemToRemove, AllCategoriesUsedSlots);
	InventoryContents.RemoveSingle(ItemToRemove);
	AssignCategorySlotsCapacity(ItemToRemove, UsedCategorySlots-=1);
	
	OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots); // Whichever class is subscribed to this delegate, will execute a function bound to this delegate.
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	int32 UsedCategorySlots = DetermineItemCategoryCapacity(ItemIn, AllCategoriesUsedSlots);
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	if(ItemIn->Quantity <= 0)
	{
		InventoryContents.RemoveSingle(ItemIn);
		AssignCategorySlotsCapacity(ItemIn, UsedCategorySlots-=1);
	}
	OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);

	return ActualAmountToRemove;
}

int32 UInventoryComponent::DetermineItemCategoryCapacity(const UItemBase* ItemIn, const FUsedInventoryCategorySlots& UsedCategoriesSlots)
{
	switch (ItemIn->ItemCategory)
	{
	case EItemCategory::Consumable:
		return UsedCategoriesSlots.GetUsedConsumableSlots();
	case EItemCategory::Craftable:
		return UsedCategoriesSlots.GetUsedConsumableSlots();
	case EItemCategory::Weapon:
		return UsedCategoriesSlots.GetUsedWeaponSlots();
	case EItemCategory::KeyItem:
		return UsedCategoriesSlots.GetUsedKeyItemSlots();
	default:
		return 0;
	}
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit, int32& UsedCategorySlots)
{
	if(!(UsedCategorySlots + 1 > GetSlotsCapacity())) // if one more thing will not overflow our inventory, continue
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit, UsedCategorySlots+=1);
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* ItemIn, int32& UsedCategorySlots)
{
	// adding one more item will overflow slot capacity
	 if (UsedCategorySlots + 1 > GetSlotsCapacity() && !ItemIn->IsKeyItem())
	 {
	 	OnAddItemMessage.Broadcast();
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("No space left for {0}"), ItemIn->TextData.Name));
	 }

	AddNewItem(ItemIn, 1, UsedCategorySlots+=1);
	OnAddItemMessage.Broadcast();
	return FItemAddResult::AddedAll(1, FText::Format(
		FText::FromString("Picked up {0}."), ItemIn->TextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount, int32& UsedCategorySlots)
{
	UE_LOG(LogInventory, Display, TEXT("UsedCategorySlots: %d"), UsedCategorySlots);
	if (RequestedAddAmount <= 0)
	{
		return 0;
	}
	int32 AmountToDistribute = RequestedAddAmount;

	// check if the input item already exists in the inventory and is not a full stack
	UItemBase* ExistingItem = FindNextPartialStack(ItemIn);


	// distribute item stack over existing stacks
	while(ExistingItem)
	{
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItem, AmountToDistribute);
		ExistingItem->SetQuantity(ExistingItem->Quantity + AmountToMakeFullStack);
		
		ItemIn->SetQuantity(AmountToDistribute - AmountToMakeFullStack);
		AmountToDistribute-=AmountToMakeFullStack;
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);
			return RequestedAddAmount;
		}
		// check if there's still a valid partial stack of the item
		
		ExistingItem = FindNextPartialStack(ItemIn);
	}
	
	// no more partial stacks found, check if new stack can be added
	
	if(UsedCategorySlots + 1 <= GetSlotsCapacity())
	{
		// attempt to add as many of the remaining item quantity that can fit inventory weight capacity
		if (AmountToDistribute > ItemIn->NumericData.MaxStackSize)
		{
			// adjust the input item and add a new stack with as many as can be held
			AmountToDistribute -= ItemIn->NumericData.MaxStackSize;
			ItemIn->SetQuantity(AmountToDistribute);
			// create a copy since only a partial stack is being added
			
			AddNewItem(ItemIn->CreateItemCopy(), RequestedAddAmount - AmountToDistribute, UsedCategorySlots+=1);
			return RequestedAddAmount - AmountToDistribute;
		}
		
		// otherwise, the full remainder of the stack can be added
		AddNewItem(ItemIn, AmountToDistribute, UsedCategorySlots+=1);
		return RequestedAddAmount;
	}

	OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);
	return RequestedAddAmount - AmountToDistribute;
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if(GetOwner())
	{
		int32 UsedCategorySlots = DetermineItemCategoryCapacity(InputItem, AllCategoriesUsedSlots);
		const int32 InitialRequestedAddAmount = InputItem->Quantity;
		
		// handle non-stackable items
		if (!InputItem->NumericData.IsStackable)
		{
			return HandleNonStackableItems(InputItem, UsedCategorySlots);
		}
		// handle stackable items
		int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount, UsedCategorySlots);
		UE_LOG(LogInventory, Warning, TEXT("StackableAmountAdded: %d"), StackableAmountAdded);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			// return all item added result
			OnAddItemMessage.Broadcast();
			UE_LOG(LogInventory, Warning, TEXT("Added All"));
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("{0} - {1}"), InputItem->TextData.Name, InitialRequestedAddAmount));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			// return partial item added result
			OnAddItemMessage.Broadcast();
			UE_LOG(LogInventory, Warning, TEXT("Added Partial"));
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
				FText::FromString("{0} - {1}"), InputItem->TextData.Name, StackableAmountAdded));
		}

		if (StackableAmountAdded <=0)
		{
			// return no item added result
			OnAddItemMessage.Broadcast();
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("No Room Left For - {0} "), InputItem->TextData.Name));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::FromString("Internal error, TryAddItem fallthrough error. GetOwner() was not found somehow"));
} 

FItemAddResult UInventoryComponent::HandleAddPotions(UItemBase* PotionIn)
{
	if(GetOwner())
	{
		int32 RequestedAddPotionAmount = AmountOfHeldPotions + PotionIn->Quantity;
		if(AmountOfHeldPotions == InventoryHealingPotionsCapacity)
		{
			OnAddItemMessage.Broadcast();
			return FItemAddResult::AddedNone(FText::FromString("Potion Inventory Is Full"));
		}
		else if(RequestedAddPotionAmount <= InventoryHealingPotionsCapacity)
		{
			AmountOfHeldPotions+=PotionIn->Quantity;
			OnAddItemMessage.Broadcast();
			return FItemAddResult::AddedAll(PotionIn->Quantity, FText::Format(
			FText::FromString("{0} - {1}"), PotionIn->TextData.Name, PotionIn->Quantity));
		}
		else if(RequestedAddPotionAmount > InventoryHealingPotionsCapacity)
		{
			const int32 PartialAmount = InventoryHealingPotionsCapacity - AmountOfHeldPotions;
			AmountOfHeldPotions+=PartialAmount;
			PotionIn->SetQuantity(PotionIn->Quantity - PartialAmount);
			OnAddItemMessage.Broadcast();
			UE_LOG(LogInventory, Warning, TEXT("Added Partial"));
			return FItemAddResult::AddedPartial(PartialAmount, FText::Format(
				FText::FromString("{0} - {1}"), PotionIn->TextData.Name, PartialAmount));
		}
	}
	OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);
	return FItemAddResult::AddedNone(FText::FromString("Internal error, HandleAddPotions fallthrough error. GetOwner() was not found somehow"));
}

void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd, const int32 UsedCategorySlots)
{
	UItemBase* NewItem;
	if (Item->bIsCopy || Item->bIsPickup)
	{
		// the logic of creating a new copy is used because in the memory when we create a new item we will point to it twice unless its destroyed or handled otherwise.
		// Example: if we split an item in the inventory, we need to make sure its a new item pointing to a new memory location

		// if the item is already a copy, or is a world pickup, treat it normally. it will be destroyed
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// used when splitting items
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	AssignCategorySlotsCapacity(Item, UsedCategorySlots);
	OnInventoryUpdated.Broadcast(AllCategoriesUsedSlots);
}

void UInventoryComponent::AssignCategorySlotsCapacity(const UItemBase* Item, const int32& NewSlotsCapacity)
{
	UE_LOG(LogInventory, Warning, TEXT("%d"), NewSlotsCapacity)
	switch (Item->ItemCategory)
	{
	case EItemCategory::Consumable:
		AllCategoriesUsedSlots.SetUsedConsumableSlots(NewSlotsCapacity);
		break;
	case EItemCategory::Craftable:
		AllCategoriesUsedSlots.SetUsedCraftableSlots(NewSlotsCapacity);
		break;
	case EItemCategory::Weapon:
		AllCategoriesUsedSlots.SetUsedWeaponSlots(NewSlotsCapacity);
		break;
	case EItemCategory::KeyItem:
		AllCategoriesUsedSlots.SetUsedKeyItemSlots(NewSlotsCapacity);
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Inventory item type not recognized"));
	}
}
