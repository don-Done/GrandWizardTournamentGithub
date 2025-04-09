// GWTSpellNode.cpp
// Implementation of the base spell node

#include "GWTSpellNode.h"
#include "GWTSpellExecutionContext.h"

UGWTSpellNode::UGWTSpellNode()
{
    // Generate a unique ID for this node
    NodeID = FGuid::NewGuid();
    
    // Set default properties
    NodeTitle = FText::FromString("Base Node");
    NodeDescription = FText::FromString("Base node for all spell nodes.");
    NodeCategory = FText::FromString("Base");
    NodeColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f); // Gray
    NodePosition = FVector2D::ZeroVector;
    
    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Spell Node created: %s"), *NodeTitle.ToString());
}

void UGWTSpellNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Base implementation just executes all output nodes
    if (Context)
    {
        for (UGWTSpellNode* OutputNode : OutputNodes)
        {
            if (OutputNode)
            {
                OutputNode->Execute(Context);
            }
        }
    }
}

bool UGWTSpellNode::ValidateConnections()
{
    // Base validation just ensures connections are valid
    // Derived classes may have more specific requirements
    return true;
}

bool UGWTSpellNode::CanConnectInput(UGWTSpellNode* Node)
{
    // Default implementation allows any node as input
    // Derived classes may have restrictions
    return Node != nullptr && Node != this;
}

bool UGWTSpellNode::CanConnectOutput(UGWTSpellNode* Node)
{
    // Default implementation allows any node as output
    // Derived classes may have restrictions
    return Node != nullptr && Node != this;
}

void UGWTSpellNode::AddInputConnection(UGWTSpellNode* Node)
{
    // Add to inputs if valid and not already connected
    if (Node && Node != this && !InputNodes.Contains(Node))
    {
        InputNodes.Add(Node);
        Node->AddOutputConnection(this);
        
        // Log connection
        UE_LOG(LogTemp, Verbose, TEXT("Node connection added: %s -> %s"),
               *Node->NodeTitle.ToString(), *NodeTitle.ToString());
    }
}

void UGWTSpellNode::AddOutputConnection(UGWTSpellNode* Node)
{
    // Add to outputs if valid and not already connected
    if (Node && Node != this && !OutputNodes.Contains(Node))
    {
        OutputNodes.Add(Node);
        
        // Log connection (don't add reciprocal connection to avoid infinite recursion)
        UE_LOG(LogTemp, Verbose, TEXT("Node output connection added: %s -> %s"),
               *NodeTitle.ToString(), *Node->NodeTitle.ToString());
    }
}

void UGWTSpellNode::RemoveInputConnection(UGWTSpellNode* Node)
{
    // Remove from inputs if exists
    if (Node && InputNodes.Contains(Node))
    {
        InputNodes.Remove(Node);
        Node->RemoveOutputConnection(this);
        
        // Log disconnection
        UE_LOG(LogTemp, Verbose, TEXT("Node connection removed: %s -> %s"),
               *Node->NodeTitle.ToString(), *NodeTitle.ToString());
    }
}

void UGWTSpellNode::RemoveOutputConnection(UGWTSpellNode* Node)
{
    // Remove from outputs if exists
    if (Node && OutputNodes.Contains(Node))
    {
        OutputNodes.Remove(Node);
        
        // Log disconnection (don't remove reciprocal connection to avoid infinite recursion)
        UE_LOG(LogTemp, Verbose, TEXT("Node output connection removed: %s -> %s"),
               *NodeTitle.ToString(), *Node->NodeTitle.ToString());
    }
}

void UGWTSpellNode::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    
    // Serialize node properties
    Ar << NodeID;
    // Additional serialization would go here
}

FString UGWTSpellNode::GetNodeTypeAsString() const
{
    // Default implementation
    return TEXT("Base");
}

EGWTSpellComponentType UGWTSpellNode::GetNodeType() const
{
    // Base nodes don't have a specific type
    // Derived classes should override this
    return EGWTSpellComponentType::Magic;
}