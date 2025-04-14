// UGWTWand.cpp
// Implementation of the wand equipment type

#include "UGWTWand.h"
#include "AGWTCharacter.h"

UGWTWand::UGWTWand()
{
    // Set default equipment properties
    ItemName = FText::FromString("Wand");
    ItemDescription = FText::FromString("A basic wand for casting spells.");
    EquipmentSlot = EGWTEquipmentSlot::Wand;

    // Set wand-specific defaults
    WandElement = EGWTElementType::None;
    ElementalBonusPercent = 15.0f;
    ManaEfficiencyPercent = 0.0f;
    CriticalHitChance = 5.0f;
    CriticalHitMultiplier = 1.5f;

    // Default stat bonuses
    SpellPowerBonus = 0.05f;  // 5% spell power bonus

    UE_LOG(LogTemp, Verbose, TEXT("Wand created: %s"), *ItemName.ToString());
}

bool UGWTWand::Equip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Equip(Character))
    {
        return false;
    }

    // Additional wand-specific equip code could go here

    return true;
}

bool UGWTWand::Unequip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Unequip(Character))
    {
        return false;
    }

    // Additional wand-specific unequip code could go here

    return true;
}

FString UGWTWand::GetItemDescription() const
{
    // Get base equipment description
    FString Description = Super::GetItemDescription();

    // Add wand-specific description
    Description += TEXT("\n\nWand Properties:\n");

    // Add elemental affinity
    if (WandElement != EGWTElementType::None)
    {
        Description += FString::Printf(TEXT("%s Affinity: +%.1f%% damage\n"),
            *UEnum::GetValueAsString(WandElement), ElementalBonusPercent);
    }

    // Add mana efficiency
    if (ManaEfficiencyPercent != 0.0f)
    {
        Description += FString::Printf(TEXT("Mana Efficiency: %.1f%%\n"), ManaEfficiencyPercent);
    }

    // Add critical hit info
    Description += FString::Printf(TEXT("Critical Hit: %.1f%% chance, %.1fx damage"),
        CriticalHitChance, CriticalHitMultiplier);

    return Description;
}

UGWTItem* UGWTWand::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTWand* NewWand = NewObject<UGWTWand>(GetOuter(), GetClass());

    // Copy base item properties
    NewWand->ItemName = ItemName;
    NewWand->ItemDescription = ItemDescription;
    NewWand->ItemIcon = ItemIcon;
    NewWand->Rarity = Rarity;
    NewWand->GoldValue = GoldValue;
    NewWand->RequiredLevel = RequiredLevel;

    // Copy equipment properties
    NewWand->EquipmentSlot = EquipmentSlot;
    NewWand->EquipmentMesh = EquipmentMesh;
    NewWand->MaxHealthBonus = MaxHealthBonus;
    NewWand->MaxManaBonus = MaxManaBonus;
    NewWand->ManaRegenBonus = ManaRegenBonus;
    NewWand->SpellPowerBonus = SpellPowerBonus;
    NewWand->CastingSpeedBonus = CastingSpeedBonus;
    NewWand->MovementSpeedBonus = MovementSpeedBonus;

    // Copy wand-specific properties
    NewWand->WandElement = WandElement;
    NewWand->ElementalBonusPercent = ElementalBonusPercent;
    NewWand->ManaEfficiencyPercent = ManaEfficiencyPercent;
    NewWand->CriticalHitChance = CriticalHitChance;
    NewWand->CriticalHitMultiplier = CriticalHitMultiplier;

    UE_LOG(LogTemp, Verbose, TEXT("Created copy of wand: %s"), *ItemName.ToString());

    return NewWand;
}

float UGWTWand::GetDamageMultiplierForElement(EGWTElementType SpellElement) const
{
    // Calculate damage multiplier based on elemental affinity
    if (WandElement == EGWTElementType::None || SpellElement == EGWTElementType::None)
    {
        // No elemental bonus for non-elemental spells or wands
        return 1.0f;
    }

    // Bonus for matching elements
    if (WandElement == SpellElement)
    {
        return 1.0f + (ElementalBonusPercent / 100.0f);
    }

    // In a more complex implementation, we could handle penalties or bonuses
    // for opposing elements

    return 1.0f; // No bonus for non-matching elements
}

float UGWTWand::GetManaCostMultiplier() const
{
    // Calculate mana cost reduction
    if (ManaEfficiencyPercent <= 0.0f)
    {
        return 1.0f; // No reduction
    }

    // Convert percentage to multiplier (e.g., 10% efficiency = 0.9x cost)
    return 1.0f - (ManaEfficiencyPercent / 100.0f);
}

bool UGWTWand::ShouldCrit() const
{
    // Determine if a spell should critically hit
    float Random = FMath::FRand() * 100.0f;
    return Random <= CriticalHitChance;
}