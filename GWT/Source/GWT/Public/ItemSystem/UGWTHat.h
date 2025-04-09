// GWTHat.h
// Hat equipment type

#pragma once

#include "CoreMinimal.h"
#include "GWTEquipment.h"
#include "GWTTypes.h"
#include "GWTHat.generated.h"

/**
 * Hat class for Grand Wizard Tournament
 * Headwear that primarily affects mana and spell range
 * Provides additional mana and improved mana regeneration
 */
UCLASS(BlueprintType)
class GWT_API UGWTHat : public UGWTEquipment
{
    GENERATED_BODY()
    
public:
    UGWTHat();
    
    // Hat properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hat")
    float SpellRangeBonus = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hat")
    float SpellDurationBonus = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hat")
    bool bReduceCastingInterruption = false;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hat")
    float ManaDiscountChance = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hat")
    TArray<EGWTSpellComponentType> UnlockedComponents;
    
    // Override equipment methods
    virtual bool Equip(AGWTCharacter* Character) override;
    virtual bool Unequip(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;
    
    // Hat-specific methods
    UFUNCTION(BlueprintCallable, Category="Hat")
    float GetRangeMultiplier() const;
    
    UFUNCTION(BlueprintCallable, Category="Hat")
    float GetDurationMultiplier() const;
    
    UFUNCTION(BlueprintCallable, Category="Hat")
    bool ShouldDiscountMana() const;
};