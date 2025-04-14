// UGWTTriggerNode.h
// Event-based trigger node for spells

#pragma once

#include "CoreMinimal.h"
#include "UGWTSpellNode.h"
#include "UGWTTriggerNode.generated.h"

/**
 * Trigger node that activates spells based on events
 * Can respond to various event types like OnCast, OnHit, etc.
 */
UCLASS(BlueprintType)
class GWT_API UGWTTriggerNode : public UGWTSpellNode
{
    GENERATED_BODY()

public:
    UGWTTriggerNode();

    // Trigger properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trigger")
    EGWTTriggerType TriggerType = EGWTTriggerType::OnCast;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trigger")
    float TriggerValue = 0.0f;

    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;

    // Trigger-specific methods
    UFUNCTION(BlueprintCallable, Category = "Trigger")
    virtual bool ShouldTrigger(UGWTSpellExecutionContext* Context);

protected:
    // Event handlers for different trigger types
    void HandleOnCast(UGWTSpellExecutionContext* Context);
    void HandleOnHit(UGWTSpellExecutionContext* Context);
    void HandleOnEnemyEnter(UGWTSpellExecutionContext* Context);
    void HandleOnHealthBelow(UGWTSpellExecutionContext* Context);
    void HandleOnManaAbove(UGWTSpellExecutionContext* Context);
    void HandleOnTimerExpired(UGWTSpellExecutionContext* Context);
};