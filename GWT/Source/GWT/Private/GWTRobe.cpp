// UGWTRobe.cpp
// Implementation of the robe equipment type

#include "UGWTRobe.h"
#include "AGWTCharacter.h"

UGWTRobe::UGWTRobe()
{
    // Set default equipment properties
    ItemName = FText::FromString("Wizard Robe");
    ItemDescription = FText::FromString("A robe providing magical protection.");
    EquipmentSlot = EGWTEquipmentSlot::Robe;

    // Set robe-specific defaults
    DamageReductionPercent = 10.0f;
    SpellCooldownReduction = 0.0f;
    ManaCostReduction = 0.0f;
    StatusEffectDuration = 1.0f;

    // Initialize elemental resistances
    ElementalResistances.Add(EGWTElementType::Fire, 0.0f);
    ElementalResistances.Add(EGWTElementType::Ice, 0.0f);
    ElementalResistances.Add(EGWTElementType::Lightning, 0.0f);
    ElementalResistances.Add(EGWTElementType::Earth, 0.0f);
    ElementalResistances.Add(EGWTElementType::Wind, 0.0f);
    ElementalResistances.Add(EGWTElementType::Light, 0.0f);
    ElementalResistances.Add(EGWTElementType::Void, 0.0f);

    // Default stat bonuses
    MaxHealthBonus = 30.0f;    // +30 Max Health

    UE_LOG(LogTemp, Verbose, TEXT("Robe created: %s"), *ItemName.ToString());
}

bool UGWTRobe::Equip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Equip(Character))
    {
        return false;
    }

    // Additional robe-specific equip code could go here
    // For example, adding resistances to the character

    UE_LOG(LogTemp, Display, TEXT("Character %s equipped robe: %s"),
        *Character->GetName(), *ItemName.ToString());

    return true;
}

bool UGWTRobe::Unequip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Unequip(Character))
    {
        return false;
    }

    // Additional robe-specific unequip code could go here

    UE_LOG(LogTemp, Display, TEXT("Character %s unequipped robe: %s"),
        *Character->GetName(), *ItemName.ToString());

    return true;
}

FString UGWTRobe::GetItemDescription() const
{
    // Get base equipment description
    FString Description = Super::GetItemDescription();

    // Add robe-specific description
    Description += TEXT("\n\nRobe Properties:\n");

    // Add damage reduction
    Description += FString::Printf(TEXT("Damage Reduction: %.1f%%\n"), DamageReductionPercent);

    // Add elemental resistances
    bool bHasResistances = false;
    for (const auto& Pair : ElementalResistances)
    {
        if (Pair.Value > 0.0f)
        {
            if (!bHasResistances)
            {
                Description += TEXT("\nElemental Resistances:\n");
                bHasResistances = true;
            }

            Description += FString::Printf(TEXT("%s: %.1f%%\n"),
                *UEnum::GetValueAsString(Pair.Key), Pair.Value);
        }
    }

    // Add cooldown reduction
    if (SpellCooldownReduction > 0.0f)
    {
        Description += FString::Printf(TEXT("Spell Cooldown Reduction: %.1f%%\n"),
            SpellCooldownReduction * 100.0f);
    }

    // Add mana cost reduction
    if (ManaCostReduction > 0.0f)
    {
        Description += FString::Printf(TEXT("Mana Cost Reduction: %.1f%%\n"),
            ManaCostReduction * 100.0f);
    }

    // Add status effect modifier
    if (StatusEffectDuration != 1.0f)
    {
        if (StatusEffectDuration > 1.0f)
        {
            Description += FString::Printf(TEXT("Status Effect Duration: +%.1f%%\n"),
                (StatusEffectDuration - 1.0f) * 100.0f);
        }
        else
        {
            Description += FString::Printf(TEXT("Status Effect Duration: %.1f%%\n"),
                StatusEffectDuration * 100.0f);
        }
    }

    return Description;
}

UGWTItem* UGWTRobe::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTRobe* NewRobe = NewObject<UGWTRobe>(GetOuter(), GetClass());

    // Copy base item properties
    NewRobe->ItemName = ItemName;
    NewRobe->ItemDescription = ItemDescription;
    NewRobe->ItemIcon = ItemIcon;
    NewRobe->Rarity = Rarity;
    NewRobe->GoldValue = GoldValue;
    NewRobe->RequiredLevel = RequiredLevel;

    // Copy equipment properties
    NewRobe->EquipmentSlot = EquipmentSlot;
    NewRobe->EquipmentMesh = EquipmentMesh;
    NewRobe->MaxHealthBonus = MaxHealthBonus;
    NewRobe->MaxManaBonus = MaxManaBonus;
    NewRobe->ManaRegenBonus = ManaRegenBonus;
    NewRobe->SpellPowerBonus = SpellPowerBonus;
    NewRobe->CastingSpeedBonus = CastingSpeedBonus;
    NewRobe->MovementSpeedBonus = MovementSpeedBonus;

    // Copy robe-specific properties
    NewRobe->DamageReductionPercent = DamageReductionPercent;
    NewRobe->ElementalResistances = ElementalResistances;
    NewRobe->SpellCooldownReduction = SpellCooldownReduction;
    NewRobe->ManaCostReduction = ManaCostReduction;
    NewRobe->StatusEffectDuration = StatusEffectDuration;

    UE_LOG(LogTemp, Verbose, TEXT("Created copy of robe: %s"), *ItemName.ToString());

    return NewRobe;
}

float UGWTRobe::GetDamageReduction() const
{
    // Convert percentage to multiplier (e.g., 10% reduction = 0.9x damage)
    return 1.0f - (DamageReductionPercent / 100.0f);
}

float UGWTRobe::GetResistanceForElement(EGWTElementType Element) const
{
    // Get resistance value for a specific element
    if (ElementalResistances.Contains(Element))
    {
        float Resistance = ElementalResistances.FindRef(Element);
        // Convert percentage to multiplier (e.g., 25% resistance = 0.75x damage)
        return 1.0f - (Resistance / 100.0f);
    }

    return 1.0f; // No resistance
}

float UGWTRobe::GetCooldownMultiplier() const
{
    // Convert reduction to multiplier (e.g., 20% reduction = 0.8x cooldown)
    return 1.0f - SpellCooldownReduction;
}

float UGWTRobe::GetManaCostMultiplier() const
{
    // Convert reduction to multiplier (e.g., 15% reduction = 0.85x mana cost)
    return 1.0f - ManaCostReduction;
}

float UGWTRobe::GetStatusEffectDurationMultiplier() const
{
    // Direct multiplier for status effect duration
    return StatusEffectDuration;
}