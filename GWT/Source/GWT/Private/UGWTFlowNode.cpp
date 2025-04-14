// UGWTFlowNode.cpp
// Implementation of the flow control node

#include "UGWTFlowNode.h"
#include "UGWTSpellExecutionContext.h"

UGWTFlowNode::UGWTFlowNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Flow Control");
    NodeDescription = FText::FromString("Controls the flow of execution with loops, delays, and other control structures.");
    NodeCategory = FText::FromString("Flow");
    NodeColor = FLinearColor(0.2f, 0.5f, 0.7f, 1.0f); // Blue for flow nodes

    // Set default flow properties
    FlowType = EGWTFlowType::Repeat;
    IterationCount = 3;
    TimeLimit = 5.0f;
    ConditionVariableName = FName("Condition");

    // Initialize body node
    BodyNode = nullptr;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Flow Node created with type: %d"), (int32)FlowType);
}

void UGWTFlowNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Flow node: Invalid context"));
        return;
    }

    // Execute based on flow type
    switch (FlowType)
    {
    case EGWTFlowType::Repeat:
        ExecuteRepeat(Context);
        break;

    case EGWTFlowType::While:
        ExecuteWhile(Context);
        break;

    case EGWTFlowType::ForEach:
        ExecuteForEach(Context);
        break;

    case EGWTFlowType::Delay:
        ExecuteDelay(Context);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown flow type in Flow node"));
        break;
    }

    // Execute remaining connected nodes (after loop completes)
    for (UGWTSpellNode* OutputNode : OutputNodes)
    {
        if (OutputNode && OutputNode != BodyNode)
        {
            OutputNode->Execute(Context);
        }
    }
}

EGWTSpellComponentType UGWTFlowNode::GetNodeType() const
{
    return EGWTSpellComponentType::Flow;
}

FString UGWTFlowNode::GetNodeTypeAsString() const
{
    return TEXT("Flow");
}

void UGWTFlowNode::AddOutputConnection(UGWTSpellNode* Node)
{
    // Add the node to outputs first
    Super::AddOutputConnection(Node);

    // Set as body node if not already set
    if (!BodyNode && Node)
    {
        SetBodyNode(Node);
    }
}

void UGWTFlowNode::RemoveOutputConnection(UGWTSpellNode* Node)
{
    // Remove from outputs first
    Super::RemoveOutputConnection(Node);

    // Clear body node reference if needed
    if (BodyNode == Node)
    {
        BodyNode = nullptr;
        UE_LOG(LogTemp, Verbose, TEXT("Removed body node connection"));
    }
}

void UGWTFlowNode::SetBodyNode(UGWTSpellNode* Node)
{
    // Make sure the node is a valid output
    if (Node && !OutputNodes.Contains(Node))
    {
        AddOutputConnection(Node);
    }

    // Set as body node
    BodyNode = Node;

    UE_LOG(LogTemp, Verbose, TEXT("Set body node to: %s"),
        Node ? *Node->NodeTitle.ToString() : TEXT("None"));
}

void UGWTFlowNode::ExecuteRepeat(UGWTSpellExecutionContext* Context)
{
    // Execute body node multiple times
    if (BodyNode)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Starting repeat loop for %d iterations"), IterationCount);

        // In a real game, we might want to spread these executions over time
        // For this example, we'll execute them all at once
        for (int32 i = 0; i < IterationCount; i++)
        {
            UE_LOG(LogTemp, Verbose, TEXT("Repeat iteration %d/%d"), i + 1, IterationCount);

            // Execute the body node
            BodyNode->Execute(Context);
        }

        UE_LOG(LogTemp, Verbose, TEXT("Repeat loop completed"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Repeat loop has no body node"));
    }
}

void UGWTFlowNode::ExecuteWhile(UGWTSpellExecutionContext* Context)
{
    // Execute body node as long as condition is true
    if (BodyNode)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Starting while loop"));

        // In a real game, we would spread these over time and have a max iteration count
        // For this example, we'll use a simple counter to prevent infinite loops
        int32 MaxIterations = 100; // Safety limit
        int32 Iterations = 0;

        while (EvaluateWhileCondition(Context) && Iterations < MaxIterations)
        {
            UE_LOG(LogTemp, Verbose, TEXT("While loop iteration %d"), Iterations + 1);

            // Execute the body node
            BodyNode->Execute(Context);

            // Increment counter
            Iterations++;
        }

        if (Iterations >= MaxIterations)
        {
            UE_LOG(LogTemp, Warning, TEXT("While loop reached iteration limit"));
        }
        else
        {
            UE_LOG(LogTemp, Verbose, TEXT("While loop completed after %d iterations"), Iterations);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("While loop has no body node"));
    }
}

void UGWTFlowNode::ExecuteForEach(UGWTSpellExecutionContext* Context)
{
    // Execute body node for each item in a collection
    // For this example, we'll just simulate it with a fixed iteration count

    if (BodyNode)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Starting forEach loop (simulated)"));

        // Simulate forEach with a simple loop
        for (int32 i = 0; i < IterationCount; i++)
        {
            UE_LOG(LogTemp, Verbose, TEXT("ForEach iteration %d/%d"), i + 1, IterationCount);

            // In a real implementation, we would extract items from a collection
            // and add them to the context

            // Create an index variable to simulate iteration
            FGWTVariableValue IndexValue;
            IndexValue.Type = EGWTVariableType::Int;
            IndexValue.IntValue = i;
            Context->SetVariable(FName("Index"), IndexValue);

            // Execute the body node
            BodyNode->Execute(Context);
        }

        UE_LOG(LogTemp, Verbose, TEXT("ForEach loop completed"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ForEach loop has no body node"));
    }
}

void UGWTFlowNode::ExecuteDelay(UGWTSpellExecutionContext* Context)
{
    // Execute body node after a delay
    // For this example, we'll just log that it would execute after the delay

    if (BodyNode)
    {
        UE_LOG(LogTemp, Display, TEXT("Delay node would execute body after %.1f seconds"), TimeLimit);

        // In a real game, we would use a timer here
        // For this example, we'll execute immediately
        BodyNode->Execute(Context);

        UE_LOG(LogTemp, Verbose, TEXT("Delay completed (simulated)"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Delay has no body node"));
    }
}

bool UGWTFlowNode::EvaluateWhileCondition(UGWTSpellExecutionContext* Context)
{
    // Check if the named condition variable is true
    if (Context->HasVariable(ConditionVariableName))
    {
        FGWTVariableValue ConditionValue = Context->GetVariable(ConditionVariableName);

        // Check based on variable type
        switch (ConditionValue.Type)
        {
        case EGWTVariableType::Bool:
            return ConditionValue.BoolValue;

        case EGWTVariableType::Int:
            return ConditionValue.IntValue != 0;

        case EGWTVariableType::Float:
            return ConditionValue.FloatValue != 0.0f;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unsupported variable type for while condition"));
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Condition variable %s not found"), *ConditionVariableName.ToString());
        return false;
    }
}