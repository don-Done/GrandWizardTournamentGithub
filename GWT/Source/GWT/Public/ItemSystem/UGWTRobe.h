// GWTRobe.h
// Robe equipment type

#pragma once

#include "CoreMinimal.h"
#include "GWTEquipment.h"
#include "GWTTypes.h"
#include "GWTRobe.generated.h"

/**
 * Robe class for Grand Wizard Tournament
 * Body armor that provides defensive properties and spell enhancements
 * Primarily affects health, defense, and spell cooldowns
 */
UCLASS(BlueprintType)
class GWT_API UGWTRobe : public UGWTEquipment
{
    GENERATED_BODY()
    
public:
    UGWTRobe();
    
    // Robe properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Robe")
    float DamageReductionPercent = 10.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Robe")
    TMap<EGWTElementType, float> ElementalResistances;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Robe")
    float SpellCooldownReduction = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Robe")
    float ManaCostReduction = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Robe")
    float StatusEffectDuration = 1.0f;
    
    // Override equipment methods
    virtual bool Equip(AGWTCharacter* Character) override;
    virtual bool Unequip(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;
    
    // Robe-specific methods
    UFUNCTION(BlueprintCallable, Category="Robe")
    float GetDamageReduction() const;
    
    UFUNCTION(BlueprintCallable, Category="Robe")
    float GetResistanceForElement(EGWTElementType Element) const;
    
    UFUNCTION(BlueprintCallable, Category="Robe")
    float GetCooldownMultiplier() const;
    
    UFUNCTION(BlueprintCallable, Category="Robe")
    float GetManaCostMultiplier() const;
    
    UFUNCTION(BlueprintCallable, Category="Robe")
    float GetStatusEffectDurationMultiplier() const;
};