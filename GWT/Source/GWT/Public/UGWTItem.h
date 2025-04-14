// UGWTItem.h
// Base class for all items in the game

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTItem.generated.h"

// Forward declarations
class AGWTCharacter;

/**
 * Base item class for Grand Wizard Tournament
 * All collectable and usable items in the game derive from this class
 * Provides basic item properties and functionality
 */
UCLASS(Abstract, BlueprintType)
class GWT_API UGWTItem : public UObject
{
    GENERATED_BODY()

public:
    UGWTItem();

    // Item identity
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* ItemIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    EGWTItemRarity Rarity = EGWTItemRarity::Common;

    // Value
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    int32 GoldValue = 10;

    // Stack information
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    bool bIsStackable = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 1;

    UPROPERTY(BlueprintReadWrite, Category = "Item")
    int32 StackCount = 1;

    // Level restrictions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    int32 RequiredLevel = 1;

    // Item methods
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual bool Use(AGWTCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual FString GetItemDescription() const;

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual UGWTItem* CreateCopy() const;

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual bool CanStack(const UGWTItem* OtherItem) const;

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual int32 AddToStack(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual int32 RemoveFromStack(int32 Count);

    // Returns true if item can be used by the character
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual bool CanUse(const AGWTCharacter* Character) const;

    // Item color based on rarity
    UFUNCTION(BlueprintCallable, Category = "Item")
    FLinearColor GetRarityColor() const;

    // Serialization
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void SaveToString(FString& OutString) const;

    UFUNCTION(BlueprintCallable, Category = "Item")
    static UGWTItem* LoadFromString(const FString& ItemData);
};