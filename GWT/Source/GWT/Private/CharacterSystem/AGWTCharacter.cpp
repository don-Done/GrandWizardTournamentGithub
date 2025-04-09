// GWTCharacter.cpp
// Implementation of the base character class

#include "GWTCharacter.h"
#include "GWTWand.h"
#include "GWTHat.h"
#include "GWTRobe.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGWTCharacter::AGWTCharacter()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // Set default stats
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    MaxMana = 100.0f;
    CurrentMana = MaxMana;
    ManaRegenRate = 5.0f;
    MovementSpeed = 600.0f;
    
    // Create equipment components
    WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
    WandMesh->SetupAttachment(GetMesh(), "hand_r"); // Attach to right hand socket
    WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    HatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HatMesh"));
    HatMesh->SetupAttachment(GetMesh(), "head"); // Attach to head socket
    HatMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    RobeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RobeMesh"));
    RobeMesh->SetupAttachment(GetMesh());
    RobeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Set movement speed
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->MaxWalkSpeed = MovementSpeed;
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Character created"));
}

void AGWTCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Start mana regeneration timer
    GetWorld()->GetTimerManager().SetTimer(
        ManaRegenTimerHandle,
        this,
        &AGWTCharacter::ManaRegenTick,
        1.0f, // Every 1 second
        true // Looping
    );
    
    UE_LOG(LogTemp, Verbose, TEXT("Character BeginPlay"));
}

void AGWTCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Process status effects
    ProcessStatusEffects(DeltaTime);
}

void AGWTCharacter::TakeDamage(float Damage, EGWTElementType DamageType, AActor* DamageCauser)
{
    // Check for shield effect
    bool bHasShield = false;
    float ShieldStrength = 0.0f;
    
    for (const FGWTStatusEffect& Effect : ActiveEffects)
    {
        if (Effect.EffectType == EGWTStatusEffectType::Shielded)
        {
            bHasShield = true;
            ShieldStrength = Effect.Strength;
            break;
        }
    }
    
    // Apply shield damage reduction if active
    if (bHasShield)
    {
        float AbsorbedDamage = FMath::Min(ShieldStrength, Damage);
        Damage -= AbsorbedDamage;
        
        UE_LOG(LogTemp, Verbose, TEXT("Shield absorbed %.1f damage"), AbsorbedDamage);
        
        // Remove shield if completely absorbed
        if (AbsorbedDamage >= ShieldStrength)
        {
            RemoveStatusEffect(EGWTStatusEffectType::Shielded);
        }
    }
    
    // Apply the damage
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - Damage);
    
    UE_LOG(LogTemp, Display, TEXT("%s took %.1f damage of type %s. Health: %.1f/%.1f"), 
           *GetName(), Damage, *UEnum::GetValueAsString(DamageType), CurrentHealth, MaxHealth);
    
    // Check if damage should apply a status effect
    if (Damage > 0.0f)
    {
        // Chance for elemental damage to apply status effect
        float StatusChance = 0.3f; // 30% chance
        
        if (FMath::FRand() < StatusChance)
        {
            FGWTStatusEffect NewEffect;
            NewEffect.Duration = 5.0f;
            NewEffect.TimeRemaining = NewEffect.Duration;
            NewEffect.Strength = Damage * 0.2f; // 20% of damage per second
            NewEffect.Causer = DamageCauser;
            
            // Select effect type based on damage type
            switch (DamageType)
            {
                case EGWTElementType::Fire:
                    NewEffect.EffectType = EGWTStatusEffectType::Burning;
                    ApplyStatusEffect(NewEffect);
                    break;
                    
                case EGWTElementType::Ice:
                    NewEffect.EffectType = EGWTStatusEffectType::Frozen;
                    ApplyStatusEffect(NewEffect);
                    break;
                    
                case EGWTElementType::Lightning:
                    NewEffect.EffectType = EGWTStatusEffectType::Electrified;
                    ApplyStatusEffect(NewEffect);
                    break;
                    
                default:
                    // No status effect for other damage types
                    break;
            }
        }
    }
    
    // Check for death
    if (CurrentHealth <= 0.0f)
    {
        OnDeath();
    }
}

void AGWTCharacter::Heal(float Amount)
{
    // Add health, clamped to max health
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
    
    float ActualHealAmount = CurrentHealth - PreviousHealth;
    
    UE_LOG(LogTemp, Display, TEXT("%s healed for %.1f. Health: %.1f/%.1f"), 
           *GetName(), ActualHealAmount, CurrentHealth, MaxHealth);
}

void AGWTCharacter::ConsumeMana(float Amount)
{
    // Check if we have enough mana
    if (CurrentMana < Amount)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s tried to consume %.1f mana but only has %.1f"), 
               *GetName(), Amount, CurrentMana);
        return;
    }
    
    // Reduce mana
    CurrentMana -= Amount;
    
    UE_LOG(LogTemp, Verbose, TEXT("%s consumed %.1f mana. Remaining: %.1f/%.1f"), 
           *GetName(), Amount, CurrentMana, MaxMana);
}

void AGWTCharacter::RegenerateMana(float Amount)
{
    // Add mana, clamped to max mana
    float PreviousMana = CurrentMana;
    CurrentMana = FMath::Min(MaxMana, CurrentMana + Amount);
    
    float ActualRegenAmount = CurrentMana - PreviousMana;
    
    if (ActualRegenAmount > 0.0f)
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s regenerated %.1f mana. Mana: %.1f/%.1f"), 
               *GetName(), ActualRegenAmount, CurrentMana, MaxMana);
    }
}

void AGWTCharacter::ApplyStatusEffect(FGWTStatusEffect Effect)
{
    // Check if this effect type already exists
    bool bEffectExists = false;
    
    for (int32 i = 0; i < ActiveEffects.Num(); i++)
    {
        if (ActiveEffects[i].EffectType == Effect.EffectType)
        {
            // Effect exists, replace if new one is stronger or has longer duration
            if (Effect.Strength > ActiveEffects[i].Strength || Effect.Duration > ActiveEffects[i].TimeRemaining)
            {
                ActiveEffects[i] = Effect;
                UE_LOG(LogTemp, Display, TEXT("%s: Status effect %s refreshed with strength %.1f and duration %.1f"), 
                       *GetName(), *UEnum::GetValueAsString(Effect.EffectType), Effect.Strength, Effect.Duration);
            }
            
            bEffectExists = true;
            break;
        }
    }
    
    // If effect doesn't exist, add it
    if (!bEffectExists)
    {
        ActiveEffects.Add(Effect);
        UE_LOG(LogTemp, Display, TEXT("%s: Status effect %s applied with strength %.1f and duration %.1f"), 
               *GetName(), *UEnum::GetValueAsString(Effect.EffectType), Effect.Strength, Effect.Duration);
        
        // Handle immediate effects of status
        switch (Effect.EffectType)
        {
            case EGWTStatusEffectType::Frozen:
                // Slow movement
                UCharacterMovementComponent* MoveComp = GetCharacterMovement();
                if (MoveComp)
                {
                    MoveComp->MaxWalkSpeed = MovementSpeed * 0.5f;
                }
                break;
        }
    }
}

void AGWTCharacter::RemoveStatusEffect(EGWTStatusEffectType EffectType)
{
    // Find and remove the effect
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; i--)
    {
        if (ActiveEffects[i].EffectType == EffectType)
        {
            UE_LOG(LogTemp, Display, TEXT("%s: Status effect %s removed"), 
                   *GetName(), *UEnum::GetValueAsString(EffectType));
            
            // Handle removal effects
            switch (EffectType)
            {
                case EGWTStatusEffectType::Frozen:
                    // Restore movement speed
                    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
                    if (MoveComp)
                    {
                        MoveComp->MaxWalkSpeed = MovementSpeed;
                    }
                    break;
            }
            
            ActiveEffects.RemoveAt(i);
        }
    }
}

void AGWTCharacter::ProcessStatusEffects(float DeltaTime)
{
    // Apply damage over time effects
    ApplyStatusEffectDamage(DeltaTime);
    
    // Update effect durations
    UpdateStatusEffectDurations(DeltaTime);
}

void AGWTCharacter::OnDeath()
{
    // Implement death behavior
    UE_LOG(LogTemp, Display, TEXT("%s has died"), *GetName());
    
    // Clear status effects
    ActiveEffects.Empty();
    
    // Stop timers
    GetWorld()->GetTimerManager().ClearTimer(ManaRegenTimerHandle);
    
    // Set state for death
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Disable ticking
    SetActorTickEnabled(false);
}

void AGWTCharacter::EquipWand(UGWTWand* Wand)
{
    if (!Wand)
        return;
    
    // Set the wand mesh
    if (Wand->EquipmentMesh)
    {
        WandMesh->SetStaticMesh(Wand->EquipmentMesh);
    }
    
    UE_LOG(LogTemp, Display, TEXT("%s equipped wand: %s"), 
           *GetName(), *Wand->ItemName.ToString());
}

void AGWTCharacter::EquipHat(UGWTHat* Hat)
{
    if (!Hat)
        return;
    
    // Set the hat mesh
    if (Hat->EquipmentMesh)
    {
        HatMesh->SetStaticMesh(Hat->EquipmentMesh);
    }
    
    // Apply hat bonuses
    MaxMana += Hat->MaxManaBonus;
    
    UE_LOG(LogTemp, Display, TEXT("%s equipped hat: %s"), 
           *GetName(), *Hat->ItemName.ToString());
}

void AGWTCharacter::EquipRobe(UGWTRobe* Robe)
{
    if (!Robe)
        return;
    
    // Set the robe mesh
    // In a real implementation, this would set the skeletal mesh
    
    UE_LOG(LogTemp, Display, TEXT("%s equipped robe: %s"), 
           *GetName(), *Robe->ItemName.ToString());
}

float AGWTCharacter::GetHealthPercent() const
{
    return (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
}

float AGWTCharacter::GetManaPercent() const
{
    return (MaxMana > 0.0f) ? (CurrentMana / MaxMana) : 0.0f;
}

bool AGWTCharacter::HasStatusEffect(EGWTStatusEffectType EffectType) const
{
    // Check if the effect exists
    for (const FGWTStatusEffect& Effect : ActiveEffects)
    {
        if (Effect.EffectType == EffectType)
        {
            return true;
        }
    }
    
    return false;
}

void AGWTCharacter::ApplyStatusEffectDamage(float DeltaTime)
{
    // Track total damage from status effects
    float TotalDamage = 0.0f;
    
    // Apply damage for each damage-over-time effect
    for (const FGWTStatusEffect& Effect : ActiveEffects)
    {
        switch (Effect.EffectType)
        {
            case EGWTStatusEffectType::Burning:
                TotalDamage += Effect.Strength * DeltaTime;
                break;
                
            case EGWTStatusEffectType::Electrified:
                TotalDamage += Effect.Strength * DeltaTime;
                break;
                
            case EGWTStatusEffectType::Poisoned:
                TotalDamage += Effect.Strength * DeltaTime;
                break;
                
            case EGWTStatusEffectType::HealthRegen:
                // Health regeneration (negative damage)
                TotalDamage -= Effect.Strength * DeltaTime;
                break;
                
            default:
                // Other effects don't cause damage
                break;
        }
    }
    
    // Apply the total damage
    if (TotalDamage != 0.0f)
    {
        if (TotalDamage > 0.0f)
        {
            // Apply damage
            CurrentHealth = FMath::Max(0.0f, CurrentHealth - TotalDamage);
            
            if (TotalDamage > 0.1f)
            {
                UE_LOG(LogTemp, Verbose, TEXT("%s took %.1f status effect damage. Health: %.1f/%.1f"), 
                       *GetName(), TotalDamage, CurrentHealth, MaxHealth);
            }
            
            // Check for death
            if (CurrentHealth <= 0.0f)
            {
                OnDeath();
            }
        }
        else
        {
            // Apply healing
            CurrentHealth = FMath::Min(MaxHealth, CurrentHealth - TotalDamage);
            
            if (TotalDamage < -0.1f)
            {
                UE_LOG(LogTemp, Verbose, TEXT("%s healed for %.1f from status effects. Health: %.1f/%.1f"), 
                       *GetName(), -TotalDamage, CurrentHealth, MaxHealth);
            }
        }
    }
}

void AGWTCharacter::UpdateStatusEffectDurations(float DeltaTime)
{
    // Update duration and remove expired effects
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; i--)
    {
        ActiveEffects[i].TimeRemaining -= DeltaTime;
        
        // Remove if expired
        if (ActiveEffects[i].TimeRemaining <= 0.0f)
        {
            UE_LOG(LogTemp, Verbose, TEXT("%s: Status effect %s expired"), 
                   *GetName(), *UEnum::GetValueAsString(ActiveEffects[i].EffectType));
            
            // Handle removal effects
            switch (ActiveEffects[i].EffectType)
            {
                case EGWTStatusEffectType::Frozen:
                    // Restore movement speed
                    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
                    if (MoveComp)
                    {
                        MoveComp->MaxWalkSpeed = MovementSpeed;
                    }
                    break;
            }
            
            ActiveEffects.RemoveAt(i);
        }
    }
}

void AGWTCharacter::ManaRegenTick()
{
    // Calculate mana regeneration amount
    float RegenAmount = ManaRegenRate;
    
    // Check for mana regen status effect
    for (const FGWTStatusEffect& Effect : ActiveEffects)
    {
        if (Effect.EffectType == EGWTStatusEffectType::ManaRegen)
        {
            RegenAmount += Effect.Strength;
        }
    }
    
    // Apply mana regeneration
    RegenerateMana(RegenAmount);
}