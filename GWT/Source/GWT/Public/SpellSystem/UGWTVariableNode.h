// GWTVariableNode.h
// Data storage node for spells

#pragma once

#include "CoreMinimal.h"
#include "GWTSpellNode.h"
#include "GWTTypes.h"
#include "GWTVariableNode.generated.h"

/**
 * Variable node that stores and manipulates data in spells
 * This introduces the fundamental programming concept of variables
 * Can be used to store values, retrieve values, or perform operations
 */
UCLASS(BlueprintType)
class GWT_API UGWTVariableNode : public UGWTSpellNode
{
    GENERATED_BODY()
    
public:
    UGWTVariableNode();
    
    // Variable properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Variable")
    FName VariableName;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Variable")
    EGWTVariableType VariableType = EGWTVariableType::Float;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Variable")
    FGWTVariableValue DefaultValue;
    
    // The operation to perform (Read, Write, Modify)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Variable")
    enum class EVariableOperation : uint8
    {
        Read,   // Get value
        Write,  // Set value
        Add,    // Add to value
        Subtract, // Subtract from value
        Multiply, // Multiply value
        Divide  // Divide value
    } Operation = EVariableOperation::Read;
    
    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;
    
protected:
    // Operation handling methods
    void ReadVariable(UGWTSpellExecutionContext* Context);
    void WriteVariable(UGWTSpellExecutionContext* Context);
    void ModifyVariable(UGWTSpellExecutionContext* Context);
    
    // Helper for getting operation value from context
    FGWTVariableValue GetOperationValue(UGWTSpellExecutionContext* Context);
};