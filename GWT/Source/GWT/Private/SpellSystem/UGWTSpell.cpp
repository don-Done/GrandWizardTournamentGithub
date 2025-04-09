// GWTSpell.cpp
// Implementation of the spell class

#include "GWTSpell.h"
#include "GWTSpellNode.h"
#include "GWTMagicNode.h"
#include "GWTSpellExecutionContext.h"
#include "GWTCharacter.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UGWTSpell::UGWTSpell()
{
    // Set default properties
    SpellName = FText::FromString("New Spell");
    SpellDescription = FText::FromString("A blank spell with no effects.");
    SpellIcon = nullptr;
    TotalManaCost = 0.0f;
    BaseDamage = 0.0f;
    
    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Created new spell: %s"), *SpellName.ToString());
}

void UGWTSpell::Cast(AActor* Caster, AActor* Target)
{
    // Check if spell is valid and caster exists
    if (!ValidateSpell() || !Caster)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cast spell: Invalid spell or caster"));
        return;
    }
    
    // Create execution context
    UGWTSpellExecutionContext* Context = NewObject<UGWTSpellExecutionContext>(this);
    Context->Caster = Caster;
    Context->Target = Target;
    
    // Check mana cost
    AGWTCharacter* CasterCharacter = Cast<AGWTCharacter>(Caster);
    if (CasterCharacter)
    {
        float ManaCost = CalculateManaCost();
        if (CasterCharacter->CurrentMana < ManaCost)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot cast spell: Not enough mana (%.1f/%.1f)"), 
                  CasterCharacter->CurrentMana, ManaCost);
            return;
        }
        
        // Consume mana
        CasterCharacter->ConsumeMana(ManaCost);
        
        UE_LOG(LogTemp, Verbose, TEXT("Consumed %.1f mana to cast spell"), ManaCost);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Casting spell: %s"), *SpellName.ToString());
    
    // Execute all root nodes
    for (UGWTSpellNode* RootNode : RootNodes)
    {
        if (RootNode)
        {
            UE_LOG(LogTemp, Verbose, TEXT("Executing root node: %s"), *RootNode->NodeTitle.ToString());
            RootNode->Execute(Context);
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Spell cast complete: %s"), *SpellName.ToString());
}

void UGWTSpell::AddNode(UGWTSpellNode* Node)
{
    // Add a node to the spell
    if (Node && !AllNodes.Contains(Node))
    {
        AllNodes.Add(Node);
        
        // Check if this is a root node (no inputs)
        if (Node->InputNodes.Num() == 0)
        {
            RootNodes.AddUnique(Node);
            UE_LOG(LogTemp, Verbose, TEXT("Added root node: %s"), *Node->NodeTitle.ToString());
        }
        
        // Update spell stats
        CalculateManaCost();
        CalculateBaseDamage();
        
        UE_LOG(LogTemp, Verbose, TEXT("Added node to spell: %s"), *Node->NodeTitle.ToString());
    }
}

void UGWTSpell::RemoveNode(UGWTSpellNode* Node)
{
    // Remove a node from the spell
    if (Node && AllNodes.Contains(Node))
    {
        // Disconnect all connections
        TArray<UGWTSpellNode*> InputNodesCopy = Node->InputNodes;
        TArray<UGWTSpellNode*> OutputNodesCopy = Node->OutputNodes;
        
        // Remove this node from all input nodes' outputs
        for (UGWTSpellNode* InputNode : InputNodesCopy)
        {
            if (InputNode)
            {
                InputNode->RemoveOutputConnection(Node);
            }
        }
        
        // Remove this node from all output nodes' inputs
        for (UGWTSpellNode* OutputNode : OutputNodesCopy)
        {
            if (OutputNode)
            {
                OutputNode->RemoveInputConnection(Node);
            }
        }
        
        // Remove from collections
        AllNodes.Remove(Node);
        RootNodes.Remove(Node);
        
        // Update spell stats
        CalculateManaCost();
        CalculateBaseDamage();
        
        UE_LOG(LogTemp, Verbose, TEXT("Removed node from spell: %s"), *Node->NodeTitle.ToString());
    }
}

bool UGWTSpell::ValidateSpell()
{
    // Check if the spell has any nodes
    if (AllNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spell validation failed: No nodes"));
        return false;
    }
    
    // Check if it has any root nodes
    if (RootNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spell validation failed: No root nodes"));
        return false;
    }
    
    // Validate each node's connections
    bool bAllNodesValid = true;
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node && !Node->ValidateConnections())
        {
            UE_LOG(LogTemp, Warning, TEXT("Node validation failed: %s"), *Node->NodeTitle.ToString());
            bAllNodesValid = false;
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Spell validation result: %s"), bAllNodesValid ? TEXT("Valid") : TEXT("Invalid"));
    
    return bAllNodesValid;
}

float UGWTSpell::CalculateManaCost()
{
    // Calculate total mana cost of the spell
    float ManaCost = 0.0f;
    
    // Sum up costs from all magic nodes
    for (UGWTSpellNode* Node : AllNodes)
    {
        UGWTMagicNode* MagicNode = Cast<UGWTMagicNode>(Node);
        if (MagicNode)
        {
            ManaCost += MagicNode->ManaCost;
        }
    }
    
    // Apply complexity scaling (more nodes = higher cost)
    float ComplexityFactor = 1.0f + (FMath::Max(0, AllNodes.Num() - 3) * 0.1f);
    ManaCost *= ComplexityFactor;
    
    // Update stored value
    TotalManaCost = ManaCost;
    
    UE_LOG(LogTemp, Verbose, TEXT("Calculated mana cost: %.1f (complexity factor: %.2f)"), 
           ManaCost, ComplexityFactor);
    
    return ManaCost;
}

float UGWTSpell::CalculateBaseDamage()
{
    // Calculate total base damage of the spell
    float Damage = 0.0f;
    
    // Sum up damage from all magic nodes
    for (UGWTSpellNode* Node : AllNodes)
    {
        UGWTMagicNode* MagicNode = Cast<UGWTMagicNode>(Node);
        if (MagicNode)
        {
            Damage += MagicNode->BaseDamage;
        }
    }
    
    // Update stored value
    BaseDamage = Damage;
    
    UE_LOG(LogTemp, Verbose, TEXT("Calculated base damage: %.1f"), Damage);
    
    return Damage;
}

void UGWTSpell::SaveToString(FString& OutString)
{
    // Serialize spell to string
    // In a real implementation, this would use binary serialization or JSON
    // For this example, we'll create a simple string representation
    
    OutString = FString::Printf(TEXT("SPELL:%s:%s"),
                               *SpellName.ToString(),
                               *SpellDescription.ToString());
    
    // Add node count
    OutString += FString::Printf(TEXT(":%d"), AllNodes.Num());
    
    // For each node, add its type and properties
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node)
        {
            OutString += FString::Printf(TEXT(":%s:%s"),
                                       *Node->GetNodeTypeAsString(),
                                       *Node->NodeTitle.ToString());
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Saved spell to string: %s"), *OutString);
}

UGWTSpell* UGWTSpell::LoadFromString(const FString& SpellData)
{
    // Deserialize spell from string
    // This is a simplified implementation
    
    TArray<FString> Parts;
    SpellData.ParseIntoArray(Parts, TEXT(":"));
    
    if (Parts.Num() < 3 || Parts[0] != TEXT("SPELL"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid spell data format"));
        return nullptr;
    }
    
    // Create new spell
    UGWTSpell* NewSpell = NewObject<UGWTSpell>();
    NewSpell->SpellName = FText::FromString(Parts[1]);
    NewSpell->SpellDescription = FText::FromString(Parts[2]);
    
    // In a real implementation, you would also recreate all the nodes
    // and their connections
    
    UE_LOG(LogTemp, Display, TEXT("Loaded spell from string: %s"), *NewSpell->SpellName.ToString());
    
    return NewSpell;
}

int32 UGWTSpell::CountNodes() const
{
    return AllNodes.Num();
}

int32 UGWTSpell::CountConnections() const
{
    int32 ConnectionCount = 0;
    
    // Count all node connections
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node)
        {
            ConnectionCount += Node->OutputNodes.Num();
        }
    }
    
    return ConnectionCount;
}

TArray<UGWTSpellNode*> UGWTSpell::GetNodesOfType(EGWTSpellComponentType NodeType) const
{
    TArray<UGWTSpellNode*> NodesOfType;
    
    // Filter nodes by type
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node && Node->GetNodeType() == NodeType)
        {
            NodesOfType.Add(Node);
        }
    }
    
    return NodesOfType;
}

UGWTSpellNode* UGWTSpell::FindNodeByID(const FGuid& NodeID) const
{
    // Find a node by its unique ID
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node && Node->NodeID == NodeID)
        {
            return Node;
        }
    }
    
    return nullptr;
}

void UGWTSpell::UpdateNodeConnections()
{
    // Update root nodes collection
    RootNodes.Empty();
    
    for (UGWTSpellNode* Node : AllNodes)
    {
        if (Node && Node->InputNodes.Num() == 0)
        {
            RootNodes.Add(Node);
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Updated node connections, found %d root nodes"), RootNodes.Num());
}