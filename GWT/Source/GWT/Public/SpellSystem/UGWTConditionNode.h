// GWTConditionNode.h
// Logic control node for spell branching

#pragma once

#include "CoreMinimal.h"
#include "SpellSystem/UGWTSpellNode.h"
#include "UGWTConditionNode.generated.h"

/**
 * Condition node that enables branching logic in spells
 * Evaluates a condition and directs execution down either the true or false path
 * This introduces the fundamental programming concept of conditional statements
 */
UCLASS(BlueprintType)
class GWT_API UGWTConditionNode : public UGWTSpellNode
{
    GENERATED_BODY()
    
public:
    UGWTConditionNode();
    
    // Condition properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Condition")
    EGWTConditionType ConditionType;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Condition")
    float ComparisonValue = 0.0f;
    
    // These pointers track the connected nodes for true/false paths
    // These are redundant with OutputNodes but help clarify which node is which
    UPROPERTY()
    UGWTSpellNode* TruePathNode;
    
    UPROPERTY()
    UGWTSpellNode* FalsePathNode;
    
    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;
    
    // Override connection methods to track true/false paths
    virtual void AddOutputConnection(UGWTSpellNode* Node) override;
    virtual void RemoveOutputConnection(UGWTSpellNode* Node) override;
    
    // Set which output is the true/false path
    UFUNCTION(BlueprintCallable, Category="Connections")
    void SetTruePath(UGWTSpellNode* Node);
    
    UFUNCTION(BlueprintCallable, Category="Connections")
    void SetFalsePath(UGWTSpellNode* Node);
    
    // Condition evaluation
    UFUNCTION(BlueprintCallable, Category="Condition")
    virtual bool EvaluateCondition(UGWTSpellExecutionContext* Context);
    
protected:
    // Helper methods for different condition types
    bool EvaluateHealthCheck(UGWTSpellExecutionContext* Context);
    bool EvaluateManaCheck(UGWTSpellExecutionContext* Context);
    bool EvaluateDistanceCheck(UGWTSpellExecutionContext* Context);
    bool EvaluateElementalCheck(UGWTSpellExecutionContext* Context);
    bool EvaluateStatusEffectCheck(UGWTSpellExecutionContext* Context);
    bool EvaluateRandomChance(UGWTSpellExecutionContext* Context);
};