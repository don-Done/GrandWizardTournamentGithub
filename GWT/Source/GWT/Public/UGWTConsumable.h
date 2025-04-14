// UGWTConsumable.h
// Consumable item type with temporary effects

#pragma once

#include "CoreMinimal.h"
#include "UGWTItem.h"
#include "GWTTypes.h"
#include "UGWTConsumable.generated.h"

/**
 * Consumable item class for Grand Wizard Tournament
 * Represents items that can be consumed for temporary effects
 * Includes potions, scrolls, and other single-use items
 */
UCLASS(BlueprintType)
class GWT_API UGWTConsumable : public UGWTItem
{
    GENERATED_BODY()

public:
    UGWTConsumable();

    // Consumable properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    EGWTConsumableType ConsumableType = EGWTConsumableType::Potion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    float EffectValue = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    float EffectDuration = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    bool bHasInstantEffect = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    TSubclassOf<UGWTSpell> SpellToUse;

    // Override base item methods
    virtual bool Use(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;
    virtual bool CanStack(const UGWTItem* OtherItem) const override;

    // Consumable-specific methods
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    virtual void ApplyEffect(AGWTCharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Consumable")
    virtual FString GetEffectDescription() const;

protected:
    // Helper methods for specific consumable types
    void ApplyPotionEffect(AGWTCharacter* Character);
    void ApplyScrollEffect(AGWTCharacter* Character);
    void ApplyFoodEffect(AGWTCharacter* Character);
    void ApplyGemEffect(AGWTCharacter* Character);
};