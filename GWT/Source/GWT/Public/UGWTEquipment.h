// UGWTEquipment.h
// Base class for all equipable items

#pragma once

#include "CoreMinimal.h"
#include "UGWTItem.h"
#include "GWTTypes.h"
#include "UGWTEquipment.generated.h"

/**
 * Equipment base class for Grand Wizard Tournament
 * Represents items that can be equipped by the character
 * Provides equipment properties and equip/unequip functionality
 */
UCLASS(Abstract, BlueprintType)
class GWT_API UGWTEquipment : public UGWTItem
{
    GENERATED_BODY()

public:
    UGWTEquipment();

    // Equipment properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    EGWTEquipmentSlot EquipmentSlot = EGWTEquipmentSlot::None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    UStaticMesh* EquipmentMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    bool bIsEquipped = false;

    // Stats bonuses
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MaxHealthBonus = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MaxManaBonus = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float ManaRegenBonus = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float SpellPowerBonus = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float CastingSpeedBonus = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MovementSpeedBonus = 0.0f;

    // Equipment methods
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual bool Equip(AGWTCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual bool Unequip(AGWTCharacter* Character);

    // Override base item methods
    virtual bool Use(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;

    // Equipment-specific description
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual FString GetStatsDescription() const;

    // Apply and remove stat bonuses
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual void ApplyStatBonuses(AGWTCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    virtual void RemoveStatBonuses(AGWTCharacter* Character);
};