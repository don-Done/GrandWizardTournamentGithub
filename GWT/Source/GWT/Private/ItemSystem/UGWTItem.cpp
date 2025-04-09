// GWTItem.cpp
// Implementation of the base item class

#include "GWTItem.h"
#include "GWTCharacter.h"

UGWTItem::UGWTItem()
{
    // Set default properties
    ItemName = FText::FromString("Item");
    ItemDescription = FText::FromString("A useful item.");
    ItemIcon = nullptr;
    Rarity = EGWTItemRarity::Common;
    GoldValue = 10;
    bIsStackable = false;
    MaxStackSize = 1;
    StackCount = 1;
    RequiredLevel = 1;
    
    UE_LOG(LogTemp, Verbose, TEXT("Item created: %s"), *ItemName.ToString());
}

bool UGWTItem::Use(AGWTCharacter* Character)
{
    // Base implementation just logs usage
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use item: Invalid character"));
        return false;
    }
    
    // Check if character meets requirements
    if (!CanUse(Character))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character cannot use item: %s"), *ItemName.ToString());
        return false;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Character %s used item: %s"), 
           *Character->GetName(), *ItemName.ToString());
    
    // Consume one item if used successfully
    RemoveFromStack(1);
    
    return true;
}

FString UGWTItem::GetItemDescription() const
{
    // Format the item description with relevant stats
    FString Description = ItemDescription.ToString();
    
    // Add rarity
    Description += FString::Printf(TEXT("\nRarity: %s"), *UEnum::GetValueAsString(Rarity));
    
    // Add value
    Description += FString::Printf(TEXT("\nValue: %d gold"), GoldValue);
    
    // Add level requirement if above 1
    if (RequiredLevel > 1)
    {
        Description += FString::Printf(TEXT("\nRequired Level: %d"), RequiredLevel);
    }
    
    return Description;
}

UGWTItem* UGWTItem::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTItem* NewItem = NewObject<UGWTItem>(GetOuter(), GetClass());
    
    // Copy properties
    NewItem->ItemName = ItemName;
    NewItem->ItemDescription = ItemDescription;
    NewItem->ItemIcon = ItemIcon;
    NewItem->Rarity = Rarity;
    NewItem->GoldValue = GoldValue;
    NewItem->bIsStackable = bIsStackable;
    NewItem->MaxStackSize = MaxStackSize;
    NewItem->StackCount = 1; // New item has just 1 in stack
    NewItem->RequiredLevel = RequiredLevel;
    
    UE_LOG(LogTemp, Verbose, TEXT("Created copy of item: %s"), *ItemName.ToString());
    
    return NewItem;
}

bool UGWTItem::CanStack(const UGWTItem* OtherItem) const
{
    // Check if items can stack together
    if (!bIsStackable)
    {
        return false;
    }
    
    if (!OtherItem || OtherItem->GetClass() != GetClass())
    {
        return false;
    }
    
    // Check if this stack is full
    if (StackCount >= MaxStackSize)
    {
        return false;
    }
    
    return true;
}

int32 UGWTItem::AddToStack(int32 Count)
{
    // Add items to this stack, returns overflow
    if (!bIsStackable || Count <= 0)
    {
        return Count;
    }
    
    // Calculate how many can be added
    int32 CanAdd = FMath::Min(Count, MaxStackSize - StackCount);
    
    // Add to stack
    StackCount += CanAdd;
    
    // Return overflow
    return Count - CanAdd;
}

int32 UGWTItem::RemoveFromStack(int32 Count)
{
    // Remove items from this stack, returns how many were removed
    if (Count <= 0)
    {
        return 0;
    }
    
    // Calculate how many can be removed
    int32 CanRemove = FMath::Min(Count, StackCount);
    
    // Remove from stack
    StackCount -= CanRemove;
    
    UE_LOG(LogTemp, Verbose, TEXT("Removed %d from stack of %s. Remaining: %d"), 
           CanRemove, *ItemName.ToString(), StackCount);
    
    return CanRemove;
}

bool UGWTItem::CanUse(const AGWTCharacter* Character) const
{
    // Check if character meets requirements to use this item
    if (!Character)
    {
        return false;
    }
    
    // Check level requirement
    // Assuming the character has a level property - in a full implementation, 
    // this would check Character->Progression->GetCurrentLevel()
    int32 CharacterLevel = 1;
    if (Character->Progression)
    {
        // This assumes there's a GetCurrentLevel method in the progression class
        // CharacterLevel = Character->Progression->GetCurrentLevel();
        CharacterLevel = 10; // Default to a high level for testing
    }
    
    if (CharacterLevel < RequiredLevel)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Character level %d is too low for item %s (requires %d)"), 
               CharacterLevel, *ItemName.ToString(), RequiredLevel);
        return false;
    }
    
    return true;
}

FLinearColor UGWTItem::GetRarityColor() const
{
    // Get color based on item rarity
    switch (Rarity)
    {
        case EGWTItemRarity::Common:
            return FLinearColor(0.7f, 0.7f, 0.7f); // Gray
            
        case EGWTItemRarity::Uncommon:
            return FLinearColor(0.0f, 0.7f, 0.0f); // Green
            
        case EGWTItemRarity::Rare:
            return FLinearColor(0.0f, 0.4f, 0.8f); // Blue
            
        case EGWTItemRarity::Epic:
            return FLinearColor(0.5f, 0.0f, 0.8f); // Purple
            
        case EGWTItemRarity::Legendary:
            return FLinearColor(1.0f, 0.5f, 0.0f); // Orange
            
        default:
            return FLinearColor(1.0f, 1.0f, 1.0f); // White
    }
}

void UGWTItem::SaveToString(FString& OutString) const
{
    // Serialize item to string
    // In a real implementation, this would use binary serialization or JSON
    // For this example, we'll create a simple string representation
    
    OutString = FString::Printf(TEXT("ITEM:%s:%s:%d:%d:%d"),
                               *ItemName.ToString(),
                               *UEnum::GetValueAsString(Rarity),
                               GoldValue,
                               RequiredLevel,
                               StackCount);
    
    UE_LOG(LogTemp, Verbose, TEXT("Saved item to string: %s"), *OutString);
}

UGWTItem* UGWTItem::LoadFromString(const FString& ItemData)
{
    // Deserialize item from string
    // This is a simplified implementation
    
    TArray<FString> Parts;
    ItemData.ParseIntoArray(Parts, TEXT(":"));
    
    if (Parts.Num() < 2 || Parts[0] != TEXT("ITEM"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid item data format"));
        return nullptr;
    }
    
    // In a full implementation, you would create the specific item type and set its properties
    
    UE_LOG(LogTemp, Display, TEXT("Loaded item from string"));
    
    return nullptr; // Placeholder
}