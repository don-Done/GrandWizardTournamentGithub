// UGWTMagicNode.cpp
// Implementation of the magic node

#include "UGWTMagicNode.h"
#include "UGWTSpellExecutionContext.h"
#include "AGWTCharacter.h"
#include "Kismet/GameplayStatics.h"

UGWTMagicNode::UGWTMagicNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Magic");
    NodeDescription = FText::FromString("Base spell effect that applies elemental damage.");
    NodeCategory = FText::FromString("Magic");
    NodeColor = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f); // Red for magic nodes

    // Set default magic properties
    BaseDamage = 10.0f;
    Range = 10.0f;
    CastTime = 1.0f;
    ManaCost = 5.0f;
    ElementType = EGWTElementType::Fire;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Magic Node created with element: %d"), (int32)ElementType);
}

void UGWTMagicNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context || !Context->Caster)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Magic node: Invalid context or caster"));
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

    if (!Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("Magic node execution failed: No target"));
        return;
    }

    // Check if target is in range
    float DistanceSquared = FVector::DistSquared(Context->Caster->GetActorLocation(), Target->GetActorLocation());
    float RangeSquared = Range * Range;

    if (DistanceSquared > RangeSquared)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target is out of range for magic effect"));
        return;
    }

    // Apply different effects based on element type
    switch (ElementType)
    {
    case EGWTElementType::Fire:
        ApplyFireEffect(Target, BaseDamage, Context);
        break;

    case EGWTElementType::Ice:
        ApplyIceEffect(Target, BaseDamage, Context);
        break;

    case EGWTElementType::Lightning:
        ApplyLightningEffect(Target, BaseDamage, Context);
        break;

    default:
        // Generic damage for other element types
        ApplyElementalEffect(Target, BaseDamage, Context);
        break;
    }

    UE_LOG(LogTemp, Verbose, TEXT("Magic node executed with %s element, %f damage"),
        *UEnum::GetValueAsString(ElementType), BaseDamage);

    // Execute connected nodes
    Super::Execute(Context);
}

EGWTSpellComponentType UGWTMagicNode::GetNodeType() const
{
    return EGWTSpellComponentType::Magic;
}

FString UGWTMagicNode::GetNodeTypeAsString() const
{
    return TEXT("Magic");
}

void UGWTMagicNode::ApplyFireEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context)
{
    // Apply fire damage
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        // Apply direct damage
        TargetCharacter->TakeDamage(Damage, EGWTElementType::Fire, Context->Caster);

        // Apply burning status effect
        FGWTStatusEffect BurningEffect;
        BurningEffect.EffectType = EGWTStatusEffectType::Burning;
        BurningEffect.Duration = 5.0f;
        BurningEffect.Strength = Damage * 0.2f; // DoT is 20% of initial damage per second
        BurningEffect.Causer = Context->Caster;
        BurningEffect.TimeRemaining = BurningEffect.Duration;

        TargetCharacter->ApplyStatusEffect(BurningEffect);

        UE_LOG(LogTemp, Verbose, TEXT("Applied fire effect to %s, Damage: %f, Burning: %f damage for %f seconds"),
            *Target->GetName(), Damage, BurningEffect.Strength, BurningEffect.Duration);
    }
}

void UGWTMagicNode::ApplyIceEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context)
{
    // Apply ice damage
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        // Apply direct damage
        TargetCharacter->TakeDamage(Damage, EGWTElementType::Ice, Context->Caster);

        // Apply frozen status effect
        FGWTStatusEffect FrozenEffect;
        FrozenEffect.EffectType = EGWTStatusEffectType::Frozen;
        FrozenEffect.Duration = 3.0f;
        FrozenEffect.Strength = 1.0f; // Slow effect (would be applied in character movement)
        FrozenEffect.Causer = Context->Caster;
        FrozenEffect.TimeRemaining = FrozenEffect.Duration;

        TargetCharacter->ApplyStatusEffect(FrozenEffect);

        UE_LOG(LogTemp, Verbose, TEXT("Applied ice effect to %s, Damage: %f, Frozen for %f seconds"),
            *Target->GetName(), Damage, FrozenEffect.Duration);
    }
}

void UGWTMagicNode::ApplyLightningEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context)
{
    // Apply lightning damage
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        // Apply direct damage
        TargetCharacter->TakeDamage(Damage, EGWTElementType::Lightning, Context->Caster);

        // Apply electrified status effect
        FGWTStatusEffect ElectrifiedEffect;
        ElectrifiedEffect.EffectType = EGWTStatusEffectType::Electrified;
        ElectrifiedEffect.Duration = 2.0f;
        ElectrifiedEffect.Strength = Damage * 0.1f; // DoT is 10% of initial damage per second
        ElectrifiedEffect.Causer = Context->Caster;
        ElectrifiedEffect.TimeRemaining = ElectrifiedEffect.Duration;

        TargetCharacter->ApplyStatusEffect(ElectrifiedEffect);

        // Chain lightning to nearby enemies
        TArray<AActor*> NearbyActors;
        UGameplayStatics::GetAllActorsOfClass(Target->GetWorld(), AGWTCharacter::StaticClass(), NearbyActors);

        int32 ChainCount = 0;
        for (AActor* NearbyActor : NearbyActors)
        {
            if (NearbyActor != Target && NearbyActor != Context->Caster)
            {
                float ChainDistance = FVector::Dist(Target->GetActorLocation(), NearbyActor->GetActorLocation());
                if (ChainDistance < 300.0f) // Chain effect range
                {
                    AGWTCharacter* ChainTarget = Cast<AGWTCharacter>(NearbyActor);
                    if (ChainTarget)
                    {
                        // Apply reduced damage to chain targets
                        ChainTarget->TakeDamage(Damage * 0.5f, EGWTElementType::Lightning, Context->Caster);
                        ChainCount++;
                    }
                }
            }
        }

        UE_LOG(LogTemp, Verbose, TEXT("Applied lightning effect to %s, Damage: %f, Electrified: %f damage for %f seconds, Chained to %d enemies"),
            *Target->GetName(), Damage, ElectrifiedEffect.Strength, ElectrifiedEffect.Duration, ChainCount);
    }
}

void UGWTMagicNode::ApplyElementalEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context)
{
    // Apply generic elemental damage
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(Target);
    if (TargetCharacter)
    {
        TargetCharacter->TakeDamage(Damage, ElementType, Context->Caster);

        UE_LOG(LogTemp, Verbose, TEXT("Applied %s damage to %s: %f damage"),
            *UEnum::GetValueAsString(ElementType), *Target->GetName(), Damage);
    }
}