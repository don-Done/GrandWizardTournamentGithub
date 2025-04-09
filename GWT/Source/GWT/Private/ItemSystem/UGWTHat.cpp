// GWTHat.cpp
// Implementation of the hat equipment type

#include "GWTHat.h"
#include "GWTCharacter.h"

UGWTHat::UGWTHat()
{
    // Set default equipment properties
    ItemName = FText::FromString("Wizard Hat");
    ItemDescription = FText::FromString("A hat enhancing magical capabilities.");
    EquipmentSlot = EGWTEquipmentSlot::Hat;
    
    // Set hat-specific defaults
    SpellRangeBonus = 0.0f;
    SpellDurationBonus = 0.0f;
    bReduceCastingInterruption = false;
    ManaDiscountChance = 0.0f;
    
    // Default stat bonuses
    MaxManaBonus = 20.0f;      // +20 Max Mana
    ManaRegenBonus = 1.0f;     // +1 Mana Regen per second
    
    UE_LOG(LogTemp, Verbose, TEXT("Hat created: %s"), *ItemName.ToString());
}

bool UGWTHat::Equip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Equip(Character))
    {
        return false;
    }
    
    // Additional hat-specific equip code could go here
    // For example, unlocking spell components if this hat provides them
    
    // If the character has a Grimoire, we could unlock components
    // This assumes the character has a reference to its Grimoire
    /* Example code:
    if (Character->Grimoire)
    {
        for (EGWTSpellComponentType Component : UnlockedComponents)
        {
            // Get the class for this component type and unlock it
            TSubclassOf<UGWTSpellNode> NodeClass = GetClassForComponentType(Component);
            if (NodeClass)
            {
                Character->Grimoire->UnlockNodeType(NodeClass);
            }
        }
    }
    */
    
    UE_LOG(LogTemp, Display, TEXT("Character %s equipped hat: %s"), 
           *Character->GetName(), *ItemName.ToString());
    
    return true;
}

bool UGWTHat::Unequip(AGWTCharacter* Character)
{
    // Call base implementation
    if (!Super::Unequip(Character))
    {
        return false;
    }
    
    // Additional hat-specific unequip code could go here
    // For example, removing unlocked spell components if this hat provided them
    
    UE_LOG(LogTemp, Display, TEXT("Character %s unequipped hat: %s"), 
           *Character->GetName(), *ItemName.ToString());
    
    return true;
}

FString UGWTHat::GetItemDescription() const
{
    // Get base equipment description
    FString Description = Super::GetItemDescription();
    
    // Add hat-specific description
    Description += TEXT("\n\nHat Properties:\n");
    
    // Add spell range bonus
    if (SpellRangeBonus != 0.0f)
    {
        Description += FString::Printf(TEXT("Spell Range: +%.1f%%\n"), SpellRangeBonus * 100.0f);
    }
    
    // Add spell duration bonus
    if (SpellDurationBonus != 0.0f)
    {
        Description += FString::Printf(TEXT("Spell Duration: +%.1f%%\n"), SpellDurationBonus * 100.0f);
    }
    
    // Add casting interruption reduction
    if (bReduceCastingInterruption)
    {
        Description += TEXT("Reduces chance of casting interruption\n");
    }
    
    // Add mana discount chance
    if (ManaDiscountChance > 0.0f)
    {
        Description += FString::Printf(TEXT("%.1f%% chance to cast spells at no mana cost\n"), ManaDiscountChance);
    }
    
    // Add unlocked components
    if (UnlockedComponents.Num() > 0)
    {
        Description += TEXT("\nUnlocks spell components:\n");
        for (EGWTSpellComponentType Component : UnlockedComponents)
        {
            Description += FString::Printf(TEXT("- %s\n"), *UEnum::GetValueAsString(Component));
        }
    }
    
    return Description;
}

UGWTItem* UGWTHat::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTHat* NewHat = NewObject<UGWTHat>(GetOuter(), GetClass());
    
    // Copy base item properties
    NewHat->ItemName = ItemName;
    NewHat->ItemDescription = ItemDescription;
    NewHat->ItemIcon = ItemIcon;
    NewHat->Rarity = Rarity;
    NewHat->GoldValue = GoldValue;
    NewHat->RequiredLevel = RequiredLevel;
    
    // Copy equipment properties
    NewHat->EquipmentSlot = EquipmentSlot;
    NewHat->EquipmentMesh = EquipmentMesh;
    NewHat->MaxHealthBonus = MaxHealthBonus;
    NewHat->MaxManaBonus = MaxManaBonus;
    NewHat->ManaRegenBonus = ManaRegenBonus;
    NewHat->SpellPowerBonus = SpellPowerBonus;
    NewHat->CastingSpeedBonus = CastingSpeedBonus;
    NewHat->MovementSpeedBonus = MovementSpeedBonus;
    
    // Copy hat-specific properties
    NewHat->SpellRangeBonus = SpellRangeBonus;
    NewHat->SpellDurationBonus = SpellDurationBonus;
    NewHat->bReduceCastingInterruption = bReduceCastingInterruption;
    NewHat->ManaDiscountChance = ManaDiscountChance;
    NewHat->UnlockedComponents = UnlockedComponents;
    
    UE_LOG(LogTemp, Verbose, TEXT("Created copy of hat: %s"), *ItemName.ToString());
    
    return NewHat;
}

float UGWTHat::GetRangeMultiplier() const
{
    // Calculate range multiplier
    return 1.0f + SpellRangeBonus;
}

float UGWTHat::GetDurationMultiplier() const
{
    // Calculate duration multiplier
    return 1.0f + SpellDurationBonus;
}

bool UGWTHat::ShouldDiscountMana() const
{
    // Determine if a spell should cost no mana
    if (ManaDiscountChance <= 0.0f)
    {
        return false;
    }
    
    float Random = FMath::FRand() * 100.0f;
    return Random <= ManaDiscountChance;
}