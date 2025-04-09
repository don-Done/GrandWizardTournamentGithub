// GWTEquipment.cpp
// Implementation of the equipment base class

#include "GWTEquipment.h"
#include "GWTCharacter.h"

UGWTEquipment::UGWTEquipment()
{
    // Equipment defaults
    bIsStackable = false;
    MaxStackSize = 1;
    EquipmentSlot = EGWTEquipmentSlot::None;
    bIsEquipped = false;
    
    // Default stats bonuses
    MaxHealthBonus = 0.0f;
    MaxManaBonus = 0.0f;
    ManaRegenBonus = 0.0f;
    SpellPowerBonus = 0.0f;
    CastingSpeedBonus = 0.0f;
    MovementSpeedBonus = 0.0f;
    
    UE_LOG(LogTemp, Verbose, TEXT("Equipment created: %s"), *ItemName.ToString());
}

bool UGWTEquipment::Equip(AGWTCharacter* Character)
{
    // Equip this item on the character
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip: Invalid character"));
        return false;
    }
    
    // Check if character meets requirements
    if (!CanUse(Character))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character cannot equip: %s"), *ItemName.ToString());
        return false;
    }
    
    // Equip based on slot
    bool bEquipSuccess = false;
    switch (EquipmentSlot)
    {
        case EGWTEquipmentSlot::Wand:
            Character->EquipWand(Cast<UGWTWand>(this));
            bEquipSuccess = true;
            break;
            
        case EGWTEquipmentSlot::Hat:
            Character->EquipHat(Cast<UGWTHat>(this));
            bEquipSuccess = true;
            break;
            
        case EGWTEquipmentSlot::Robe:
            Character->EquipRobe(Cast<UGWTRobe>(this));
            bEquipSuccess = true;
            break;
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown equipment slot: %d"), (int32)EquipmentSlot);
            return false;
    }
    
    if (bEquipSuccess)
    {
        // Apply stat bonuses
        ApplyStatBonuses(Character);
        
        // Mark as equipped
        bIsEquipped = true;
        
        UE_LOG(LogTemp, Display, TEXT("Character %s equipped %s"), 
               *Character->GetName(), *ItemName.ToString());
        
        return true;
    }
    
    return false;
}

bool UGWTEquipment::Unequip(AGWTCharacter* Character)
{
    // Unequip this item from the character
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot unequip: Invalid character"));
        return false;
    }
    
    // Make sure it's equipped
    if (!bIsEquipped)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item is not equipped: %s"), *ItemName.ToString());
        return false;
    }
    
    // Unequip based on slot
    Character->UnequipItem(EquipmentSlot);
    
    // Remove stat bonuses
    RemoveStatBonuses(Character);
    
    // Mark as unequipped
    bIsEquipped = false;
    
    UE_LOG(LogTemp, Display, TEXT("Character %s unequipped %s"), 
           *Character->GetName(), *ItemName.ToString());
    
    return true;
}

bool UGWTEquipment::Use(AGWTCharacter* Character)
{
    // Using equipment attempts to equip it
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use equipment: Invalid character"));
        return false;
    }
    
    // Equipment doesn't get consumed like other items
    return Equip(Character);
}

FString UGWTEquipment::GetItemDescription() const
{
    // Get base description
    FString Description = Super::GetItemDescription();
    
    // Add equipment-specific details
    Description += FString::Printf(TEXT("\nSlot: %s"), *UEnum::GetValueAsString(EquipmentSlot));
    
    // Add stats
    Description += TEXT("\n\n");
    Description += GetStatsDescription();
    
    return Description;
}

UGWTItem* UGWTEquipment::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTEquipment* NewEquipment = NewObject<UGWTEquipment>(GetOuter(), GetClass());
    
    // Copy base properties
    NewEquipment->ItemName = ItemName;
    NewEquipment->ItemDescription = ItemDescription;
    NewEquipment->ItemIcon = ItemIcon;
    NewEquipment->Rarity = Rarity;
    NewEquipment->GoldValue = GoldValue;
    NewEquipment->RequiredLevel = RequiredLevel;
    
    // Copy equipment properties
    NewEquipment->EquipmentSlot = EquipmentSlot;
    NewEquipment->EquipmentMesh = EquipmentMesh;
    
    // Copy stat bonuses
    NewEquipment->MaxHealthBonus = MaxHealthBonus;
    NewEquipment->MaxManaBonus = MaxManaBonus;
    NewEquipment->ManaRegenBonus = ManaRegenBonus;
    NewEquipment->SpellPowerBonus = SpellPowerBonus;
    NewEquipment->CastingSpeedBonus = CastingSpeedBonus;
    NewEquipment->MovementSpeedBonus = MovementSpeedBonus;
    
    UE_LOG(LogTemp, Verbose, TEXT("Created copy of equipment: %s"), *ItemName.ToString());
    
    return NewEquipment;
}

FString UGWTEquipment::GetStatsDescription() const
{
    // Format the stats into a string
    FString StatsDesc;
    
    // Only include stats that have values
    if (MaxHealthBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f Max Health\n"), MaxHealthBonus);
    }
    
    if (MaxManaBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f Max Mana\n"), MaxManaBonus);
    }
    
    if (ManaRegenBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f Mana Regeneration\n"), ManaRegenBonus);
    }
    
    if (SpellPowerBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f%% Spell Power\n"), SpellPowerBonus * 100.0f);
    }
    
    if (CastingSpeedBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f%% Casting Speed\n"), CastingSpeedBonus * 100.0f);
    }
    
    if (MovementSpeedBonus != 0.0f)
    {
        StatsDesc += FString::Printf(TEXT("+%.1f%% Movement Speed\n"), MovementSpeedBonus * 100.0f);
    }
    
    // If no stats, say so
    if (StatsDesc.IsEmpty())
    {
        StatsDesc = TEXT("No stat bonuses");
    }
    
    return StatsDesc;
}

void UGWTEquipment::ApplyStatBonuses(AGWTCharacter* Character)
{
    // Apply stat bonuses to the character
    if (!Character)
    {
        return;
    }
    
    // Apply health bonus
    if (MaxHealthBonus != 0.0f)
    {
        Character->MaxHealth += MaxHealthBonus;
        // Optionally increase current health proportionally
        float HealthRatio = Character->CurrentHealth / (Character->MaxHealth - MaxHealthBonus);
        Character->CurrentHealth = HealthRatio * Character->MaxHealth;
    }
    
    // Apply mana bonus
    if (MaxManaBonus != 0.0f)
    {
        Character->MaxMana += MaxManaBonus;
        // Optionally increase current mana proportionally
        float ManaRatio = Character->CurrentMana / (Character->MaxMana - MaxManaBonus);
        Character->CurrentMana = ManaRatio * Character->MaxMana;
    }
    
    // Apply mana regen bonus
    if (ManaRegenBonus != 0.0f)
    {
        Character->ManaRegenRate += ManaRegenBonus;
    }
    
    // Apply movement speed bonus
    if (MovementSpeedBonus != 0.0f)
    {
        Character->MovementSpeed *= (1.0f + MovementSpeedBonus);
        
        // Update character movement component
        UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
        if (MoveComp)
        {
            MoveComp->MaxWalkSpeed = Character->MovementSpeed;
        }
    }
    
    // Spell power and casting speed would be handled in the character's spellcasting code
    
    UE_LOG(LogTemp, Verbose, TEXT("Applied stat bonuses from %s to %s"), 
           *ItemName.ToString(), *Character->GetName());
}

void UGWTEquipment::RemoveStatBonuses(AGWTCharacter* Character)
{
    // Remove stat bonuses from the character
    if (!Character)
    {
        return;
    }
    
    // Remove health bonus
    if (MaxHealthBonus != 0.0f)
    {
        float HealthRatio = Character->CurrentHealth / Character->MaxHealth;
        Character->MaxHealth -= MaxHealthBonus;
        // Adjust current health proportionally
        Character->CurrentHealth = HealthRatio * Character->MaxHealth;
    }
    
    // Remove mana bonus
    if (MaxManaBonus != 0.0f)
    {
        float ManaRatio = Character->CurrentMana / Character->MaxMana;
        Character->MaxMana -= MaxManaBonus;
        // Adjust current mana proportionally
        Character->CurrentMana = ManaRatio * Character->MaxMana;
    }
    
    // Remove mana regen bonus
    if (ManaRegenBonus != 0.0f)
    {
        Character->ManaRegenRate -= ManaRegenBonus;
    }
    
    // Remove movement speed bonus
    if (MovementSpeedBonus != 0.0f)
    {
        Character->MovementSpeed /= (1.0f + MovementSpeedBonus);
        
        // Update character movement component
        UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
        if (MoveComp)
        {
            MoveComp->MaxWalkSpeed = Character->MovementSpeed;
        }
    }
    
    // Spell power and casting speed would be handled in the character's spellcasting code
    
    UE_LOG(LogTemp, Verbose, TEXT("Removed stat bonuses from %s to %s"), 
           *ItemName.ToString(), *Character->GetName());
}