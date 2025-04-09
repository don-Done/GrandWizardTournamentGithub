// GWTFlowNode.h
// Loop and branch flow control node for spells

#pragma once

#include "CoreMinimal.h"
#include "GWTSpellNode.h"
#include "GWTFlowNode.generated.h"

/**
 * Flow node that controls the execution flow of spells
 * Implements programming concepts like loops, delays, and branching
 * Allows for repeated execution and timed effects
 */
UCLASS(BlueprintType)
class GWT_API UGWTFlowNode : public UGWTSpellNode
{
    GENERATED_BODY()
    
public:
    UGWTFlowNode();
    
    // Flow properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Flow")
    EGWTFlowType FlowType = EGWTFlowType::Repeat;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Flow")
    int32 IterationCount = 3;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Flow")
    float TimeLimit = 5.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Flow")
    FName ConditionVariableName;
    
    // Special node connections - body is the node(s) that get executed in the loop
    UPROPERTY()
    UGWTSpellNode* BodyNode;
    
    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;
    
    // Override connection methods to track body node
    virtual void AddOutputConnection(UGWTSpellNode* Node) override;
    virtual void RemoveOutputConnection(UGWTSpellNode* Node) override;
    
    // Set which output is the body
    UFUNCTION(BlueprintCallable, Category="Connections")
    void SetBodyNode(UGWTSpellNode* Node);
    
protected:
    // Flow implementation methods
    void ExecuteRepeat(UGWTSpellExecutionContext* Context);
    void ExecuteWhile(UGWTSpellExecutionContext* Context);
    void ExecuteForEach(UGWTSpellExecutionContext* Context);
    void ExecuteDelay(UGWTSpellExecutionContext* Context);
    
    // Helper to check a while condition
    bool EvaluateWhileCondition(UGWTSpellExecutionContext* Context);
};