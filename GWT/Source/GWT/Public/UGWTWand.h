// UGWTWand.h
// Wand equipment type

#pragma once

#include "CoreMinimal.h"
#include "UGWTEquipment.h"
#include "GWTTypes.h"
#include "UGWTWand.generated.h"

/**
 * Wand class for Grand Wizard Tournament
 * Primary casting tool for wizards
 * Affects spell damage, mana efficiency, and elemental properties
 */
UCLASS(BlueprintType)
class GWT_API UGWTWand : public UGWTEquipment
{
    GENERATED_BODY()

public:
    UGWTWand();

    // Wand properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wand")
    EGWTElementType WandElement = EGWTElementType::None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wand")
    float ElementalBonusPercent = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wand")
    float ManaEfficiencyPercent = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wand")
    float CriticalHitChance = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wand")
    float CriticalHitMultiplier = 1.5f;

    // Override equipment methods
    virtual bool Equip(AGWTCharacter* Character) override;
    virtual bool Unequip(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;

    // Wand-specific methods
    UFUNCTION(BlueprintCallable, Category = "Wand")
    float GetDamageMultiplierForElement(EGWTElementType SpellElement) const;

    UFUNCTION(BlueprintCallable, Category = "Wand")
    float GetManaCostMultiplier() const;

    UFUNCTION(BlueprintCallable, Category = "Wand")
    bool ShouldCrit() const;
};