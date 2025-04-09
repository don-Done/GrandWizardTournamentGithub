// GWTInventory.h
// Manages player's items and equipment

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "GWTInventory.generated.h"

// Forward declarations
class UGWTItem;
class UGWTEquipment;
class AGWTCharacter;

/**
 * Inventory class for Grand Wizard Tournament
 * Manages the player's collection of items and equipment
 * Handles item storage, equipment tracking, and gold
 */
UCLASS(BlueprintType)
class GWT_API UGWTInventory : public UObject
{
    GENERATED_BODY()
    
public:
    UGWTInventory();
    
    // Inventory properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
    int32 MaxInventorySlots = 30;
    
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TArray<UGWTItem*> Items;
    
    UPROPERTY(BlueprintReadOnly, Category="Inventory")
    TMap<EGWTEquipmentSlot, UGWTEquipment*> EquippedItems;
    
    // Currency
    UPROPERTY(BlueprintReadOnly, Category="Currency")
    int32 Gold = 0;
    
    // Character reference
    UPROPERTY()
    AGWTCharacter* OwningCharacter;
    
    // Inventory methods
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool AddItem(UGWTItem* Item, bool bAutoStack = true);
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RemoveItem(UGWTItem* Item, int32 Count = 1);
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool UseItem(UGWTItem* Item);
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    int32 GetItemCount(TSubclassOf<UGWTItem> ItemClass) const;
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    UGWTItem* FindItem(TSubclassOf<UGWTItem> ItemClass) const;
    
    // Equipment methods
    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool EquipItem(UGWTEquipment* Equipment);
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool UnequipItem(EGWTEquipmentSlot Slot);
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    UGWTEquipment* GetEquippedItem(EGWTEquipmentSlot Slot) const;
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    TArray<UGWTEquipment*> GetAllEquippedItems() const;
    
    // Currency methods
    UFUNCTION(BlueprintCallable, Category="Currency")
    void AddGold(int32 Amount);
    
    UFUNCTION(BlueprintCallable, Category="Currency")
    bool RemoveGold(int32 Amount);
    
    UFUNCTION(BlueprintCallable, Category="Currency")
    bool HasEnoughGold(int32 Amount) const;
    
    // Inventory state
    UFUNCTION(BlueprintCallable, Category="Inventory")
    int32 GetInventorySize() const;
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool HasFreeSlot() const;
    
    UFUNCTION(BlueprintCallable, Category="Inventory")
    int32 GetFreeSlotCount() const;
    
    // Serialization
    UFUNCTION(BlueprintCallable, Category="Save")
    void SaveToString(FString& OutString) const;
    
    UFUNCTION(BlueprintCallable, Category="Save")
    static UGWTInventory* LoadFromString(const FString& InventoryData);
    
    // Set owning character
    UFUNCTION(BlueprintCallable, Category="Inventory")
    void SetOwningCharacter(AGWTCharacter* Character);
    
protected:
    // Helper methods
    bool TryStackItem(UGWTItem* NewItem);
    void SortInventory();
};