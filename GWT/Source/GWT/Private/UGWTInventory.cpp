// UGWTInventory.cpp
// Implementation of the inventory management system

#include "UGWTInventory.h"
#include "UGWTTItem.h"
#include "UGWTEquipment.h"
#include "AGWTCharacter.h"

UGWTInventory::UGWTInventory()
{
    // Initialize inventory properties
    MaxInventorySlots = 30;
    Gold = 0;
    OwningCharacter = nullptr;

    // Initialize the equipped items map with empty slots
    EquippedItems.Add(EGWTEquipmentSlot::Wand, nullptr);
    EquippedItems.Add(EGWTEquipmentSlot::Hat, nullptr);
    EquippedItems.Add(EGWTEquipmentSlot::Robe, nullptr);

    UE_LOG(LogTemp, Verbose, TEXT("Inventory created with %d slots"), MaxInventorySlots);
}

bool UGWTInventory::AddItem(UGWTItem* Item, bool bAutoStack)
{
    // Check if item is valid
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add item: Invalid item"));
        return false;
    }

    // Check if inventory has space
    if (!HasFreeSlot() && !bAutoStack)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add item: Inventory full"));
        return false;
    }

    // Try to stack item if auto-stacking is enabled
    if (bAutoStack && Item->bIsStackable && TryStackItem(Item))
    {
        UE_LOG(LogTemp, Verbose, TEXT("Item stacked: %s"), *Item->ItemName.ToString());
        return true;
    }

    // If we can't stack or don't want to, check for inventory space
    if (!HasFreeSlot())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add item: Inventory full"));
        return false;
    }

    // Add item to inventory
    Items.Add(Item);

    // Sort inventory
    SortInventory();

    UE_LOG(LogTemp, Display, TEXT("Added item to inventory: %s"), *Item->ItemName.ToString());

    return true;
}

bool UGWTInventory::RemoveItem(UGWTItem* Item, int32 Count)
{
    // Check if item is valid
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot remove item: Invalid item"));
        return false;
    }

    // Check if item is in inventory
    if (!Items.Contains(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot remove item: Item not in inventory"));
        return false;
    }

    // Handle stacks
    if (Item->bIsStackable && Item->StackCount > Count)
    {
        // Remove from stack
        Item->RemoveFromStack(Count);
        UE_LOG(LogTemp, Verbose, TEXT("Removed %d from stack of %s. Remaining: %d"),
            Count, *Item->ItemName.ToString(), Item->StackCount);
        return true;
    }

    // Remove entire item
    Items.Remove(Item);

    UE_LOG(LogTemp, Display, TEXT("Removed item from inventory: %s"), *Item->ItemName.ToString());

    return true;
}

bool UGWTInventory::UseItem(UGWTItem* Item)
{
    // Check if item is valid
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use item: Invalid item"));
        return false;
    }

    // Check if item is in inventory
    if (!Items.Contains(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use item: Item not in inventory"));
        return false;
    }

    // Use the item
    bool bSuccess = Item->Use(OwningCharacter);

    // If item was used and is now at zero stack count, remove it
    if (bSuccess && Item->bIsStackable && Item->StackCount <= 0)
    {
        Items.Remove(Item);
        UE_LOG(LogTemp, Verbose, TEXT("Item used up and removed from inventory: %s"),
            *Item->ItemName.ToString());
    }

    return bSuccess;
}

int32 UGWTInventory::GetItemCount(TSubclassOf<UGWTItem> ItemClass) const
{
    // Count total number of items of the specified class
    int32 Count = 0;

    for (UGWTItem* Item : Items)
    {
        if (Item && Item->IsA(ItemClass))
        {
            // Add stack count for stackable items
            if (Item->bIsStackable)
            {
                Count += Item->StackCount;
            }
            else
            {
                Count++;
            }
        }
    }

    return Count;
}

UGWTItem* UGWTInventory::FindItem(TSubclassOf<UGWTItem> ItemClass) const
{
    // Find the first item of the specified class
    for (UGWTItem* Item : Items)
    {
        if (Item && Item->IsA(ItemClass))
        {
            return Item;
        }
    }

    return nullptr;
}

bool UGWTInventory::EquipItem(UGWTEquipment* Equipment)
{
    // Check if equipment is valid
    if (!Equipment)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Invalid equipment"));
        return false;
    }

    // Check if equipment is in inventory
    if (!Items.Contains(Equipment))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Equipment not in inventory"));
        return false;
    }

    // Check slot
    if (Equipment->EquipmentSlot == EGWTEquipmentSlot::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Invalid equipment slot"));
        return false;
    }

    // Unequip any existing item in that slot
    UGWTEquipment* CurrentEquipment = GetEquippedItem(Equipment->EquipmentSlot);
    if (CurrentEquipment)
    {
        UnequipItem(Equipment->EquipmentSlot);
    }

    // Update equipped items map
    EquippedItems[Equipment->EquipmentSlot] = Equipment;

    // Equip the item on the character
    if (OwningCharacter)
    {
        Equipment->Equip(OwningCharacter);
    }

    UE_LOG(LogTemp, Display, TEXT("Equipped item: %s"), *Equipment->ItemName.ToString());

    return true;
}

bool UGWTInventory::UnequipItem(EGWTEquipmentSlot Slot)
{
    // Get current equipment in slot
    UGWTEquipment* Equipment = GetEquippedItem(Slot);
    if (!Equipment)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot unequip: No equipment in slot %d"), (int32)Slot);
        return false;
    }

    // Unequip from character
    if (OwningCharacter)
    {
        Equipment->Unequip(OwningCharacter);
    }

    // Clear slot
    EquippedItems[Slot] = nullptr;

    UE_LOG(LogTemp, Display, TEXT("Unequipped item: %s"), *Equipment->ItemName.ToString());

    return true;
}

UGWTEquipment* UGWTInventory::GetEquippedItem(EGWTEquipmentSlot Slot) const
{
    // Get equipment in specified slot
    if (EquippedItems.Contains(Slot))
    {
        return EquippedItems[Slot];
    }

    return nullptr;
}

TArray<UGWTEquipment*> UGWTInventory::GetAllEquippedItems() const
{
    // Get all equipped items
    TArray<UGWTEquipment*> EquippedArray;

    for (const auto& Pair : EquippedItems)
    {
        if (Pair.Value)
        {
            EquippedArray.Add(Pair.Value);
        }
    }

    return EquippedArray;
}

void UGWTInventory::AddGold(int32 Amount)
{
    // Add gold
    if (Amount <= 0)
    {
        return;
    }

    Gold += Amount;

    UE_LOG(LogTemp, Verbose, TEXT("Added %d gold. New total: %d"), Amount, Gold);
}

bool UGWTInventory::RemoveGold(int32 Amount)
{
    // Remove gold if enough
    if (Amount <= 0)
    {
        return true;
    }

    if (Gold < Amount)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold. Have %d, need %d"), Gold, Amount);
        return false;
    }

    Gold -= Amount;

    UE_LOG(LogTemp, Verbose, TEXT("Removed %d gold. New total: %d"), Amount, Gold);

    return true;
}

bool UGWTInventory::HasEnoughGold(int32 Amount) const
{
    return Gold >= Amount;
}

int32 UGWTInventory::GetInventorySize() const
{
    return Items.Num();
}

bool UGWTInventory::HasFreeSlot() const
{
    return GetInventorySize() < MaxInventorySlots;
}

int32 UGWTInventory::GetFreeSlotCount() const
{
    return FMath::Max(0, MaxInventorySlots - GetInventorySize());
}

void UGWTInventory::SaveToString(FString& OutString) const
{
    // Serialize inventory to string
    // In a real implementation, this would use binary serialization or JSON
    // For this example, we'll create a simple string representation

    // Start with header
    OutString = TEXT("INVENTORY");

    // Add gold
    OutString += FString::Printf(TEXT(":%d"), Gold);

    // Add item count
    OutString += FString::Printf(TEXT(":%d"), Items.Num());

    // Add each item
    for (UGWTItem* Item : Items)
    {
        if (Item)
        {
            FString ItemData;
            Item->SaveToString(ItemData);
            OutString += FString::Printf(TEXT(":%s"), *ItemData);
        }
    }

    // Add equipped items
    for (const auto& Pair : EquippedItems)
    {
        if (Pair.Value)
        {
            OutString += FString::Printf(TEXT(":%d:%s"), (int32)Pair.Key, *Pair.Value->GetName());
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("Saved inventory to string, %d items, %d gold"),
        Items.Num(), Gold);
}

UGWTInventory* UGWTInventory::LoadFromString(const FString& InventoryData)
{
    // Deserialize inventory from string
    // This is a simplified implementation

    TArray<FString> Parts;
    InventoryData.ParseIntoArray(Parts, TEXT(":"));

    if (Parts.Num() < 1 || Parts[0] != TEXT("INVENTORY"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid inventory data format"));
        return nullptr;
    }

    // Create new inventory
    UGWTInventory* NewInventory = NewObject<UGWTInventory>();

    // In a real implementation, you would parse the gold and items
    // For this example, we'll just use the default initialization

    UE_LOG(LogTemp, Display, TEXT("Loaded inventory from string"));

    return NewInventory;
}

void UGWTInventory::SetOwningCharacter(AGWTCharacter* Character)
{
    OwningCharacter = Character;

    UE_LOG(LogTemp, Verbose, TEXT("Set inventory owner: %s"),
        Character ? *Character->GetName() : TEXT("None"));
}

bool UGWTInventory::TryStackItem(UGWTItem* NewItem)
{
    // Try to stack with existing items
    if (!NewItem || !NewItem->bIsStackable)
    {
        return false;
    }

    // Find items of the same type
    for (UGWTItem* Item : Items)
    {
        if (Item && Item->CanStack(NewItem))
        {
            // Calculate how many can be stacked
            int32 InitialStack = NewItem->StackCount;
            int32 RemainingStack = Item->AddToStack(InitialStack);

            // If we stacked all items, return success
            if (RemainingStack <= 0)
            {
                UE_LOG(LogTemp, Verbose, TEXT("Fully stacked %d items of %s"),
                    InitialStack, *NewItem->ItemName.ToString());
                return true;
            }

            // Update the new item's stack count
            NewItem->StackCount = RemainingStack;

            UE_LOG(LogTemp, Verbose, TEXT("Partially stacked %d items of %s, %d remaining"),
                InitialStack - RemainingStack, *NewItem->ItemName.ToString(), RemainingStack);
        }
    }

    // If we get here, we couldn't stack all items
    // If we partially stacked (stack count changed), return true
    return NewItem->StackCount < NewItem->MaxStackSize;
}

void UGWTInventory::SortInventory()
{
    // Sort inventory based on item type and name
    // This is a simple implementation that groups items by type

    // Group by item type
    Items.Sort([](const UGWTItem& A, const UGWTItem& B)
        {
            // First sort by class
            if (A.GetClass() != B.GetClass())
            {
                return A.GetClass()->GetName() < B.GetClass()->GetName();
            }

            // Then by name
            return A.ItemName.ToString() < B.ItemName.ToString();
        });

    UE_LOG(LogTemp, Verbose, TEXT("Sorted inventory"));
}