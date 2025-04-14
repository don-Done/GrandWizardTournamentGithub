// UGWTVariableNode.cpp
// Implementation of the variable node

#include "UGWTVariableNode.h"
#include "UGWTSpellExecutionContext.h"

UGWTVariableNode::UGWTVariableNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Variable");
    NodeDescription = FText::FromString("Stores, retrieves, or modifies data in the spell.");
    NodeCategory = FText::FromString("Data");
    NodeColor = FLinearColor(0.7f, 0.4f, 0.7f, 1.0f); // Purple for variable nodes

    // Set default variable properties
    VariableName = FName("MyVar");
    VariableType = EGWTVariableType::Float;
    DefaultValue.Type = EGWTVariableType::Float;
    DefaultValue.FloatValue = 0.0f;
    Operation = EVariableOperation::Read;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Variable Node created with name: %s"), *VariableName.ToString());
}

void UGWTVariableNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Variable node: Invalid context"));
        return;
    }

    // Handle based on operation type
    switch (Operation)
    {
    case EVariableOperation::Read:
        ReadVariable(Context);
        break;

    case EVariableOperation::Write:
        WriteVariable(Context);
        break;

    case EVariableOperation::Add:
    case EVariableOperation::Subtract:
    case EVariableOperation::Multiply:
    case EVariableOperation::Divide:
        ModifyVariable(Context);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown operation type in Variable node"));
        break;
    }

    // Execute connected nodes
    Super::Execute(Context);
}

EGWTSpellComponentType UGWTVariableNode::GetNodeType() const
{
    return EGWTSpellComponentType::Variable;
}

FString UGWTVariableNode::GetNodeTypeAsString() const
{
    return TEXT("Variable");
}

void UGWTVariableNode::ReadVariable(UGWTSpellExecutionContext* Context)
{
    // Check if variable exists in context
    if (Context->HasVariable(VariableName))
    {
        // Variable exists, use it
        FGWTVariableValue Value = Context->GetVariable(VariableName);

        // Log the variable value
        switch (Value.Type)
        {
        case EGWTVariableType::Float:
            UE_LOG(LogTemp, Verbose, TEXT("Read variable %s: %f"), *VariableName.ToString(), Value.FloatValue);
            break;

        case EGWTVariableType::Int:
            UE_LOG(LogTemp, Verbose, TEXT("Read variable %s: %d"), *VariableName.ToString(), Value.IntValue);
            break;

        case EGWTVariableType::Bool:
            UE_LOG(LogTemp, Verbose, TEXT("Read variable %s: %s"), *VariableName.ToString(),
                Value.BoolValue ? TEXT("True") : TEXT("False"));
            break;

        case EGWTVariableType::Vector:
            UE_LOG(LogTemp, Verbose, TEXT("Read variable %s: (%f, %f, %f)"), *VariableName.ToString(),
                Value.VectorValue.X, Value.VectorValue.Y, Value.VectorValue.Z);
            break;

        case EGWTVariableType::Target:
            UE_LOG(LogTemp, Verbose, TEXT("Read variable %s: %s"), *VariableName.ToString(),
                Value.TargetValue ? *Value.TargetValue->GetName() : TEXT("None"));
            break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown variable type"));
            break;
        }
    }
    else
    {
        // Variable doesn't exist, use default
        Context->SetVariable(VariableName, DefaultValue);

        UE_LOG(LogTemp, Verbose, TEXT("Variable %s not found, using default value"), *VariableName.ToString());
    }
}

void UGWTVariableNode::WriteVariable(UGWTSpellExecutionContext* Context)
{
    // Get the value to write
    FGWTVariableValue ValueToWrite = GetOperationValue(Context);

    // Write to the context
    Context->SetVariable(VariableName, ValueToWrite);

    // Log the variable write
    switch (ValueToWrite.Type)
    {
    case EGWTVariableType::Float:
        UE_LOG(LogTemp, Verbose, TEXT("Write variable %s = %f"), *VariableName.ToString(), ValueToWrite.FloatValue);
        break;

    case EGWTVariableType::Int:
        UE_LOG(LogTemp, Verbose, TEXT("Write variable %s = %d"), *VariableName.ToString(), ValueToWrite.IntValue);
        break;

    case EGWTVariableType::Bool:
        UE_LOG(LogTemp, Verbose, TEXT("Write variable %s = %s"), *VariableName.ToString(),
            ValueToWrite.BoolValue ? TEXT("True") : TEXT("False"));
        break;

    case EGWTVariableType::Vector:
        UE_LOG(LogTemp, Verbose, TEXT("Write variable %s = (%f, %f, %f)"), *VariableName.ToString(),
            ValueToWrite.VectorValue.X, ValueToWrite.VectorValue.Y, ValueToWrite.VectorValue.Z);
        break;

    case EGWTVariableType::Target:
        UE_LOG(LogTemp, Verbose, TEXT("Write variable %s = %s"), *VariableName.ToString(),
            ValueToWrite.TargetValue ? *ValueToWrite.TargetValue->GetName() : TEXT("None"));
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown variable type"));
        break;
    }
}

void UGWTVariableNode::ModifyVariable(UGWTSpellExecutionContext* Context)
{
    // Get the current value (or default if it doesn't exist)
    FGWTVariableValue CurrentValue;
    if (Context->HasVariable(VariableName))
    {
        CurrentValue = Context->GetVariable(VariableName);
    }
    else
    {
        CurrentValue = DefaultValue;
    }

    // Get the operation value
    FGWTVariableValue OperationValue = GetOperationValue(Context);

    // Apply modification based on operation type
    FGWTVariableValue ResultValue = CurrentValue; // Start with current value

    // Only numeric types can be modified
    if (CurrentValue.Type == EGWTVariableType::Float && OperationValue.Type == EGWTVariableType::Float)
    {
        // Apply float operation
        switch (Operation)
        {
        case EVariableOperation::Add:
            ResultValue.FloatValue = CurrentValue.FloatValue + OperationValue.FloatValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %f + %f = %f"),
                *VariableName.ToString(), CurrentValue.FloatValue, OperationValue.FloatValue, ResultValue.FloatValue);
            break;

        case EVariableOperation::Subtract:
            ResultValue.FloatValue = CurrentValue.FloatValue - OperationValue.FloatValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %f - %f = %f"),
                *VariableName.ToString(), CurrentValue.FloatValue, OperationValue.FloatValue, ResultValue.FloatValue);
            break;

        case EVariableOperation::Multiply:
            ResultValue.FloatValue = CurrentValue.FloatValue * OperationValue.FloatValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %f * %f = %f"),
                *VariableName.ToString(), CurrentValue.FloatValue, OperationValue.FloatValue, ResultValue.FloatValue);
            break;

        case EVariableOperation::Divide:
            if (OperationValue.FloatValue != 0.0f)
            {
                ResultValue.FloatValue = CurrentValue.FloatValue / OperationValue.FloatValue;
                UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %f / %f = %f"),
                    *VariableName.ToString(), CurrentValue.FloatValue, OperationValue.FloatValue, ResultValue.FloatValue);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Division by zero prevented"));
            }
            break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unhandled operation type"));
            break;
        }
    }
    else if (CurrentValue.Type == EGWTVariableType::Int && OperationValue.Type == EGWTVariableType::Int)
    {
        // Apply int operation
        switch (Operation)
        {
        case EVariableOperation::Add:
            ResultValue.IntValue = CurrentValue.IntValue + OperationValue.IntValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %d + %d = %d"),
                *VariableName.ToString(), CurrentValue.IntValue, OperationValue.IntValue, ResultValue.IntValue);
            break;

        case EVariableOperation::Subtract:
            ResultValue.IntValue = CurrentValue.IntValue - OperationValue.IntValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %d - %d = %d"),
                *VariableName.ToString(), CurrentValue.IntValue, OperationValue.IntValue, ResultValue.IntValue);
            break;

        case EVariableOperation::Multiply:
            ResultValue.IntValue = CurrentValue.IntValue * OperationValue.IntValue;
            UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %d * %d = %d"),
                *VariableName.ToString(), CurrentValue.IntValue, OperationValue.IntValue, ResultValue.IntValue);
            break;

        case EVariableOperation::Divide:
            if (OperationValue.IntValue != 0)
            {
                ResultValue.IntValue = CurrentValue.IntValue / OperationValue.IntValue;
                UE_LOG(LogTemp, Verbose, TEXT("Modified variable %s: %d / %d = %d"),
                    *VariableName.ToString(), CurrentValue.IntValue, OperationValue.IntValue, ResultValue.IntValue);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Division by zero prevented"));
            }
            break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unhandled operation type"));
            break;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot modify variable of type %d"), (int)CurrentValue.Type);
    }

    // Write the result back to the context
    Context->SetVariable(VariableName, ResultValue);
}

FGWTVariableValue UGWTVariableNode::GetOperationValue(UGWTSpellExecutionContext* Context)
{
    // For this implementation, we'll just use the default value
    // In a full implementation, this might come from input connections or other sources

    // Create a context-specific value
    FGWTVariableValue Value = DefaultValue;

    // If the variable type is Target, we might want to use the current target
    if (Value.Type == EGWTVariableType::Target && Value.TargetValue == nullptr && Context->Target)
    {
        Value.TargetValue = Context->Target;
    }

    return Value;
}