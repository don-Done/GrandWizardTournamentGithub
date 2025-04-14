// UGWTEffectNode.cpp
// Implementation of the effect node

#include "UGWTEffectNode.h"
#include "UGWTSpellExecutionContext.h"
#include "AGWTCharacter.h"
#include "Kismet/GameplayStatics.h"

UGWTEffectNode::UGWTEffectNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Effect");
    NodeDescription = FText::FromString("Produces specific spell effects like damage, healing, or teleportation.");
    NodeCategory = FText::FromString("Effects");
    NodeColor = FLinearColor(0.2f, 0.2f, 1.0f, 1.0f); // Blue for effect nodes

    // Set default effect properties
    EffectType = EGWTEffectType::Damage;
    EffectValue = 10.0f;
    EffectDuration = 0.0f;
    ElementType = EGWTElementType::None;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Effect Node created with type: %d"), (int32)EffectType);
}

void UGWTEffectNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context || !Context->Caster)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Effect node: Invalid context or caster"));
        return;
    }

    // Get the target from the context or use the caster's current target
    AActor* Target = Context->Target;
    if (!Target)
    {
        // If no target in context, try to use hit result
        if (Context->HitResult.GetActor())
        {
            Target = Context->HitResult.GetActor();
        }
    }

    // Apply effect based on type
    switch (EffectType)
    {
    case EGWTEffectType::Damage:
        if (Target)
        {
            ApplyDamageEffect(Target, Context);
        }
        break;

    case EGWTEffectType::Heal:
        if (Target)
        {
            ApplyHealEffect(Target, Context);
        }
        break;

    case EGWTEffectType::ApplyStatus:
        if (Target)
        {
            ApplyStatusEffect(Target, Context);
        }
        break;

    case EGWTEffectType::Teleport:
        if (Target)
        {
            ApplyTeleportEffect(Target, Context);
        }
        break;

    case EGWTEffectType::Knockback:
        if (Target)
        {
            ApplyKnockbackEffect(Target, Context);
        }
        break;

    case EGWTEffectType::Shield:
        if (Target)
        {
            ApplyShieldEffect(Target, Context);
        }
        break;

    case EGWTEffectType::Summon:
        ApplySummonEffect(Context);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown effect type in Effect node"));
        break;
    }

    // Execute connected nodes
    Super::Execute(Context);
}

EGWTSpellComponentType UGWTEffectNode::GetNodeType() const
{
    return EGWTSpellComponentType::Effect;
}

FString UGWTEffectNode::GetNodeTypeAsString() const
{
    return TEXT("Effect");
}

void UGWTEffectNode::ApplyDamageEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Apply damage to target
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        TargetCharacter->TakeDamage(EffectValue, ElementType, Context->Caster);

        UE_LOG(LogTemp, Verbose, TEXT("Applied damage effect to %s: %.1f damage"),
            *Target->GetName(), EffectValue);
    }
}

void UGWTEffectNode::ApplyHealEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Apply healing to target
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        TargetCharacter->Heal(EffectValue);

        UE_LOG(LogTemp, Verbose, TEXT("Applied heal effect to %s: %.1f healing"),
            *Target->GetName(), EffectValue);
    }
}

void UGWTEffectNode::ApplyStatusEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Apply status effect to target
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        // Create a status effect based on element type
        FGWTStatusEffect StatusEffect;
        StatusEffect.Duration = EffectDuration;
        StatusEffect.Strength = EffectValue;
        StatusEffect.Causer = Context->Caster;
        StatusEffect.TimeRemaining = EffectDuration;

        // Select status effect type based on element
        switch (ElementType)
        {
        case EGWTElementType::Fire:
            StatusEffect.EffectType = EGWTStatusEffectType::Burning;
            break;

        case EGWTElementType::Ice:
            StatusEffect.EffectType = EGWTStatusEffectType::Frozen;
            break;

        case EGWTElementType::Lightning:
            StatusEffect.EffectType = EGWTStatusEffectType::Electrified;
            break;

        default:
            // Default to burning for other elements
            StatusEffect.EffectType = EGWTStatusEffectType::Burning;
            break;
        }

        TargetCharacter->ApplyStatusEffect(StatusEffect);

        UE_LOG(LogTemp, Verbose, TEXT("Applied status effect to %s: Type %d, Duration %.1f, Strength %.1f"),
            *Target->GetName(), (int32)StatusEffect.EffectType, EffectDuration, EffectValue);
    }
}

void UGWTEffectNode::ApplyTeleportEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Teleport target to hit location
    if (Target && Context->HitResult.IsValidBlockingHit())
    {
        FVector TeleportLocation = Context->HitResult.Location;

        // Adjust location to be above the surface
        TeleportLocation.Z += 100.0f; // Raise by 100 units to prevent embedding in ground

        // Teleport the actor
        Target->SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);

        UE_LOG(LogTemp, Verbose, TEXT("Applied teleport effect to %s: New location (%f, %f, %f)"),
            *Target->GetName(), TeleportLocation.X, TeleportLocation.Y, TeleportLocation.Z);
    }
}

void UGWTEffectNode::ApplyKnockbackEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Apply knockback to target
    if (Target && Context->Caster)
    {
        // Calculate direction from caster to target
        FVector Direction = Target->GetActorLocation() - Context->Caster->GetActorLocation();
        Direction.Normalize();

        // Apply force based on effect value
        FVector KnockbackForce = Direction * EffectValue * 1000.0f; // Scale up for appropriate force

        // Apply force to character
        UPrimitiveComponent* TargetPrimitive = Cast<UPrimitiveComponent>(Target->GetRootComponent());
        if (TargetPrimitive && TargetPrimitive->IsSimulatingPhysics())
        {
            TargetPrimitive->AddImpulse(KnockbackForce, NAME_None, true);

            UE_LOG(LogTemp, Verbose, TEXT("Applied physics knockback to %s with force (%f, %f, %f)"),
                *Target->GetName(), KnockbackForce.X, KnockbackForce.Y, KnockbackForce.Z);
        }
        else
        {
            // For non-physics actors, we can manually move them
            // This would typically be implemented with character movement component
            UE_LOG(LogTemp, Verbose, TEXT("Applied movement knockback to %s with force (%f, %f, %f)"),
                *Target->GetName(), KnockbackForce.X, KnockbackForce.Y, KnockbackForce.Z);
        }
    }
}

void UGWTEffectNode::ApplyShieldEffect(AActor* Target, UGWTSpellExecutionContext* Context)
{
    // Apply shield effect to target
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        // Create shield status effect
        FGWTStatusEffect ShieldEffect;
        ShieldEffect.EffectType = EGWTStatusEffectType::Shielded;
        ShieldEffect.Duration = EffectDuration;
        ShieldEffect.Strength = EffectValue; // Shield absorbs this amount of damage
        ShieldEffect.Causer = Context->Caster;
        ShieldEffect.TimeRemaining = EffectDuration;

        TargetCharacter->ApplyStatusEffect(ShieldEffect);

        UE_LOG(LogTemp, Verbose, TEXT("Applied shield effect to %s: Absorbs %.1f damage for %.1f seconds"),
            *Target->GetName(), EffectValue, EffectDuration);
    }
}

void UGWTEffectNode::ApplySummonEffect(UGWTSpellExecutionContext* Context)
{
    // Summon effect would spawn a companion/minion
    // This is a placeholder implementation

    // Get spawn location from hit result or in front of caster
    FVector SpawnLocation;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    if (Context->HitResult.IsValidBlockingHit())
    {
        SpawnLocation = Context->HitResult.Location;
        SpawnLocation.Z += 100.0f; // Raise above ground
    }
    else if (Context->Caster)
    {
        // Spawn in front of caster
        SpawnLocation = Context->Caster->GetActorLocation() +
            (Context->Caster->GetActorForwardVector() * 200.0f);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot apply summon effect: Invalid spawn location"));
        return;
    }

    // In a full implementation, we would spawn an actual minion actor here
    UE_LOG(LogTemp, Verbose, TEXT("Applied summon effect at location (%f, %f, %f)"),
        SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
}