// AGWTEffectNode.h
// Action-producing effect node for spells

#pragma once

#include "CoreMinimal.h"
#include "UGWTSpellNode.h"
#include "UGWTEffectNode.generated.h"

/**
 * Effect node that produces specific spell actions
 * Handles different effects like damage, healing, knockback, etc.
 */
UCLASS(BlueprintType)
class GWT_API UGWTEffectNode : public UGWTSpellNode
{
    GENERATED_BODY()

public:
    UGWTEffectNode();

    // Effect properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
    EGWTEffectType EffectType = EGWTEffectType::Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
    float EffectValue = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
    float EffectDuration = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effect")
    EGWTElementType ElementType = EGWTElementType::None;

    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;

protected:
    // Effect implementation methods
    void ApplyDamageEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplyHealEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplyStatusEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplyTeleportEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplyKnockbackEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplyShieldEffect(AActor* Target, UGWTSpellExecutionContext* Context);
    void ApplySummonEffect(UGWTSpellExecutionContext* Context);
};