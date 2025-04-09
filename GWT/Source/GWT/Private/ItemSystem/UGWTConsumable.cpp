// GWTConsumable.cpp
// Implementation of the consumable item type

#include "GWTConsumable.h"
#include "GWTCharacter.h"
#include "GWTSpell.h"

UGWTConsumable::UGWTConsumable()
{
    // Set base item properties
    ItemName = FText::FromString("Potion");
    ItemDescription = FText::FromString("A consumable item with magical effects.");
    bIsStackable = true;
    MaxStackSize = 10;
    
    // Set consumable defaults
    ConsumableType = EGWTConsumableType::Potion;
    EffectValue = 50.0f;
    EffectDuration = 0.0f;
    bHasInstantEffect = true;
    SpellToUse = nullptr;
    
    UE_LOG(LogTemp, Verbose, TEXT("Consumable created: %s"), *ItemName.ToString());
}

bool UGWTConsumable::Use(AGWTCharacter* Character)
{
    // Check if character is valid
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use consumable: Invalid character"));
        return false;
    }
    
    // Check if character meets requirements
    if (!CanUse(Character))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character cannot use consumable: %s"), *ItemName.ToString());
        return false;
    }
    
    // Apply the consumable's effect
    ApplyEffect(Character);
    
    // Reduce stack count
    RemoveFromStack(1);
    
    UE_LOG(LogTemp, Display, TEXT("Character %s used consumable: %s"), 
           *Character->GetName(), *ItemName.ToString());
    
    return true;
}

FString UGWTConsumable::GetItemDescription() const
{
    // Get base description
    FString Description = Super::GetItemDescription();
    
    // Add consumable-specific details
    Description += TEXT("\n\n");
    Description += GetEffectDescription();
    
    return Description;
}

UGWTItem* UGWTConsumable::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTConsumable* NewConsumable = NewObject<UGWTConsumable>(GetOuter(), GetClass());
    
    // Copy base properties
    NewConsumable->ItemName = ItemName;
    NewConsumable->ItemDescription = ItemDescription;
    NewConsumable->ItemIcon = ItemIcon;
    NewConsumable->Rarity = Rarity;
    NewConsumable->GoldValue = GoldValue;
    NewConsumable->bIsStackable = bIsStackable;
    NewConsumable->MaxStackSize = MaxStackSize;
    NewConsumable->StackCount = 1; // New item has just 1 in stack
    NewConsumable->RequiredLevel = RequiredLevel;
    
    // Copy consumable properties
    NewConsumable->ConsumableType = ConsumableType;
    NewConsumable->EffectValue = EffectValue;
    NewConsumable->EffectDuration = EffectDuration;
    NewConsumable->bHasInstantEffect = bHasInstantEffect;
    NewConsumable->SpellToUse = SpellToUse;
    
    UE_LOG(LogTemp, Verbose, TEXT("Created copy of consumable: %s"), *ItemName.ToString());
    
    return NewConsumable;
}

bool UGWTConsumable::CanStack(const UGWTItem* OtherItem) const
{
    // Basic stacking check from parent
    if (!Super::CanStack(OtherItem))
    {
        return false;
    }
    
    // Additional consumable-specific checks
    const UGWTConsumable* OtherConsumable = Cast<UGWTConsumable>(OtherItem);
    if (!OtherConsumable)
    {
        return false;
    }
    
    // Make sure they're the same type of consumable with same effect values
    if (OtherConsumable->ConsumableType != ConsumableType ||
        OtherConsumable->EffectValue != EffectValue ||
        OtherConsumable->EffectDuration != EffectDuration)
    {
        return false;
    }
    
    // Scrolls must have the same spell
    if (ConsumableType == EGWTConsumableType::Scroll &&
        OtherConsumable->SpellToUse != SpellToUse)
    {
        return false;
    }
    
    return true;
}

void UGWTConsumable::ApplyEffect(AGWTCharacter* Character)
{
    // Apply effects based on consumable type
    switch (ConsumableType)
    {
        case EGWTConsumableType::Potion:
            ApplyPotionEffect(Character);
            break;
            
        case EGWTConsumableType::Scroll:
            ApplyScrollEffect(Character);
            break;
            
        case EGWTConsumableType::Food:
            ApplyFoodEffect(Character);
            break;
            
        case EGWTConsumableType::Gem:
            ApplyGemEffect(Character);
            break;
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown consumable type: %d"), (int32)ConsumableType);
            break;
    }
}

FString UGWTConsumable::GetEffectDescription() const
{
    // Format effect description based on type
    FString Description;
    
    switch (ConsumableType)
    {
        case EGWTConsumableType::Potion:
            // For health and mana potions
            if (ItemName.ToString().Contains(TEXT("Health")))
            {
                Description = FString::Printf(TEXT("Restores %.0f health"), EffectValue);
            }
            else if (ItemName.ToString().Contains(TEXT("Mana")))
            {
                Description = FString::Printf(TEXT("Restores %.0f mana"), EffectValue);
            }
            else
            {
                Description = FString::Printf(TEXT("Effect: %.0f"), EffectValue);
            }
            
            // Add duration for over-time effects
            if (EffectDuration > 0.0f)
            {
                Description += FString::Printf(TEXT(" over %.1f seconds"), EffectDuration);
            }
            else
            {
                Description += TEXT(" instantly");
            }
            break;
            
        case EGWTConsumableType::Scroll:
            Description = TEXT("Single-use spell scroll");
            if (SpellToUse)
            {
                UGWTSpell* SpellDefaultObject = SpellToUse.GetDefaultObject();
                if (SpellDefaultObject)
                {
                    Description += FString::Printf(TEXT(": %s"), *SpellDefaultObject->SpellName.ToString());
                }
            }
            break;
            
        case EGWTConsumableType::Food:
            Description = FString::Printf(TEXT("Restores %.0f health and %.0f mana over %.1f seconds"),
                                        EffectValue, EffectValue * 0.5f, EffectDuration);
            break;
            
        case EGWTConsumableType::Gem:
            Description = FString::Printf(TEXT("Permanent stat upgrade: +%.0f"), EffectValue);
            break;
            
        default:
            Description = TEXT("Unknown effect");
            break;
    }
    
    return Description;
}

void UGWTConsumable::ApplyPotionEffect(AGWTCharacter* Character)
{
    // Apply potion effects
    if (!Character)
    {
        return;
    }
    
    // Determine potion type based on name (simple approach)
    FString ItemNameStr = ItemName.ToString();
    
    if (ItemNameStr.Contains(TEXT("Health")))
    {
        // Health potion
        if (bHasInstantEffect)
        {
            // Instant healing
            Character->Heal(EffectValue);
            UE_LOG(LogTemp, Display, TEXT("Applied instant healing: %.1f"), EffectValue);
        }
        else
        {
            // Healing over time using status effect
            FGWTStatusEffect HealingEffect;
            HealingEffect.EffectType = EGWTStatusEffectType::HealthRegen;
            HealingEffect.Duration = EffectDuration;
            HealingEffect.Strength = EffectValue / EffectDuration; // HPS
            HealingEffect.TimeRemaining = EffectDuration;
            
            Character->ApplyStatusEffect(HealingEffect);
            UE_LOG(LogTemp, Display, TEXT("Applied healing over time: %.1f over %.1f seconds"), 
                   EffectValue, EffectDuration);
        }
    }
    else if (ItemNameStr.Contains(TEXT("Mana")))
    {
        // Mana potion
        if (bHasInstantEffect)
        {
            // Instant mana restore
            Character->RegenerateMana(EffectValue);
            UE_LOG(LogTemp, Display, TEXT("Applied instant mana restoration: %.1f"), EffectValue);
        }
        else
        {
            // Mana regen over time using status effect
            FGWTStatusEffect ManaEffect;
            ManaEffect.EffectType = EGWTStatusEffectType::ManaRegen;
            ManaEffect.Duration = EffectDuration;
            ManaEffect.Strength = EffectValue / EffectDuration; // MPS
            ManaEffect.TimeRemaining = EffectDuration;
            
            Character->ApplyStatusEffect(ManaEffect);
            UE_LOG(LogTemp, Display, TEXT("Applied mana regeneration: %.1f over %.1f seconds"), 
                   EffectValue, EffectDuration);
        }
    }
    else if (ItemNameStr.Contains(TEXT("Shield")))
    {
        // Shield potion
        FGWTStatusEffect ShieldEffect;
        ShieldEffect.EffectType = EGWTStatusEffectType::Shielded;
        ShieldEffect.Duration = EffectDuration;
        ShieldEffect.Strength = EffectValue;
        ShieldEffect.TimeRemaining = EffectDuration;
        
        Character->ApplyStatusEffect(ShieldEffect);
        UE_LOG(LogTemp, Display, TEXT("Applied shield effect: %.1f for %.1f seconds"), 
               EffectValue, EffectDuration);
    }
    // Additional potion types could be added here
}

void UGWTConsumable::ApplyScrollEffect(AGWTCharacter* Character)
{
    // Cast the spell from the scroll
    if (!Character || !SpellToUse)
    {
        return;
    }
    
    // Create spell instance
    UGWTSpell* Spell = NewObject<UGWTSpell>(Character, SpellToUse);
    if (Spell)
    {
        // Cast the spell with no mana cost
        Spell->Cast(Character);
        UE_LOG(LogTemp, Display, TEXT("Cast scroll spell: %s"), *Spell->SpellName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create spell from scroll"));
    }
}

void UGWTConsumable::ApplyFoodEffect(AGWTCharacter* Character)
{
    // Apply food effects (health and mana over time)
    if (!Character)
    {
        return;
    }
    
    // Health regen effect
    FGWTStatusEffect HealthEffect;
    HealthEffect.EffectType = EGWTStatusEffectType::HealthRegen;
    HealthEffect.Duration = EffectDuration;
    HealthEffect.Strength = EffectValue / EffectDuration; // HPS
    HealthEffect.TimeRemaining = EffectDuration;
    
    Character->ApplyStatusEffect(HealthEffect);
    
    // Mana regen effect
    FGWTStatusEffect ManaEffect;
    ManaEffect.EffectType = EGWTStatusEffectType::ManaRegen;
    ManaEffect.Duration = EffectDuration;
    ManaEffect.Strength = (EffectValue * 0.5f) / EffectDuration; // MPS (half the health value)
    ManaEffect.TimeRemaining = EffectDuration;
    
    Character->ApplyStatusEffect(ManaEffect);
    
    UE_LOG(LogTemp, Display, TEXT("Applied food effect: %.1f health and %.1f mana over %.1f seconds"), 
           EffectValue, EffectValue * 0.5f, EffectDuration);
}

void UGWTConsumable::ApplyGemEffect(AGWTCharacter* Character)
{
    // Apply permanent stat upgrade
    if (!Character)
    {
        return;
    }
    
    // Determine gem type based on name (simple approach)
    FString ItemNameStr = ItemName.ToString();
    
    if (ItemNameStr.Contains(TEXT("Health")))
    {
        // Max health increase
        Character->MaxHealth += EffectValue;
        Character->CurrentHealth += EffectValue; // Also increase current health
        UE_LOG(LogTemp, Display, TEXT("Applied permanent health increase: +%.1f"), EffectValue);
    }
    else if (ItemNameStr.Contains(TEXT("Mana")))
    {
        // Max mana increase
        Character->MaxMana += EffectValue;
        Character->CurrentMana += EffectValue; // Also increase current mana
        UE_LOG(LogTemp, Display, TEXT("Applied permanent mana increase: +%.1f"), EffectValue);
    }
    else if (ItemNameStr.Contains(TEXT("Regen")))
    {
        // Mana regen increase
        Character->ManaRegenRate += EffectValue;
        UE_LOG(LogTemp, Display, TEXT("Applied permanent mana regen increase: +%.1f"), EffectValue);
    }
    // Additional gem types could be added here
}