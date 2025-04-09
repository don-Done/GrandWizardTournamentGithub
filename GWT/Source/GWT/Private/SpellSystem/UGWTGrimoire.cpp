// GWTGrimoire.cpp
// Implementation of the player's grimoire

#include "GWTGrimoire.h"
#include "GWTSpell.h"
#include "GWTSpellNode.h"
#include "GWTMagicNode.h"
#include "GWTTriggerNode.h"
#include "GWTEffectNode.h"
#include "GWTConditionNode.h"
#include "GWTVariableNode.h"
#include "GWTFlowNode.h"

UGWTGrimoire::UGWTGrimoire()
{
    // Initialize with some basic spell components
    // These are the components the player starts with
    UnlockNodeType(UGWTMagicNode::StaticClass());
    UnlockNodeType(UGWTTriggerNode::StaticClass());
    
    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("Grimoire created with %d unlocked node types"), UnlockedNodeTypes.Num());
}

void UGWTGrimoire::AddSpell(UGWTSpell* Spell)
{
    // Add spell to collection if valid and not already present
    if (Spell && !Spells.Contains(Spell))
    {
        Spells.Add(Spell);
        
        UE_LOG(LogTemp, Display, TEXT("Added spell to grimoire: %s"), *Spell->SpellName.ToString());
    }
}

void UGWTGrimoire::RemoveSpell(UGWTSpell* Spell)
{
    // Remove spell from collection
    if (Spell && Spells.Contains(Spell))
    {
        Spells.Remove(Spell);
        
        UE_LOG(LogTemp, Display, TEXT("Removed spell from grimoire: %s"), *Spell->SpellName.ToString());
    }
}

UGWTSpell* UGWTGrimoire::GetSpell(int32 Index)
{
    // Return spell at index if valid
    if (Spells.IsValidIndex(Index))
    {
        return Spells[Index];
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Invalid spell index: %d"), Index);
    return nullptr;
}

void UGWTGrimoire::UnlockNodeType(TSubclassOf<UGWTSpellNode> NodeType)
{
    // Add node type to unlocked list if not already present
    if (NodeType && !UnlockedNodeTypes.Contains(NodeType))
    {
        UnlockedNodeTypes.Add(NodeType);
        
        // Get the default object to log the node type name
        UGWTSpellNode* DefaultNode = NodeType.GetDefaultObject();
        if (DefaultNode)
        {
            UE_LOG(LogTemp, Display, TEXT("Unlocked node type: %s"), *DefaultNode->NodeTitle.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("Unlocked node type"));
        }
    }
}

bool UGWTGrimoire::IsNodeTypeUnlocked(TSubclassOf<UGWTSpellNode> NodeType)
{
    // Check if node type is in the unlocked list
    return UnlockedNodeTypes.Contains(NodeType);
}

void UGWTGrimoire::SaveToString(FString& OutString)
{
    // Serialize grimoire to string
    // In a real implementation, this would use binary serialization or JSON
    // For this example, we'll create a simple string representation
    
    // Start with header
    OutString = TEXT("GRIMOIRE");
    
    // Add unlocked node types count
    OutString += FString::Printf(TEXT(":%d"), UnlockedNodeTypes.Num());
    
    // Add each unlocked node type
    for (TSubclassOf<UGWTSpellNode> NodeType : UnlockedNodeTypes)
    {
        UGWTSpellNode* DefaultNode = NodeType.GetDefaultObject();
        if (DefaultNode)
        {
            OutString += FString::Printf(TEXT(":%s"), *DefaultNode->GetNodeTypeAsString());
        }
    }
    
    // Add spell count
    OutString += FString::Printf(TEXT(":%d"), Spells.Num());
    
    // Add each spell (in a real game, this would be more comprehensive)
    for (UGWTSpell* Spell : Spells)
    {
        if (Spell)
        {
            FString SpellData;
            Spell->SaveToString(SpellData);
            OutString += FString::Printf(TEXT(":%s"), *SpellData);
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Saved grimoire to string, %d spells, %d node types"), 
           Spells.Num(), UnlockedNodeTypes.Num());
}

UGWTGrimoire* UGWTGrimoire::LoadFromString(const FString& GrimoireData)
{
    // Deserialize grimoire from string
    // This is a simplified implementation
    
    TArray<FString> Parts;
    GrimoireData.ParseIntoArray(Parts, TEXT(":"));
    
    if (Parts.Num() < 1 || Parts[0] != TEXT("GRIMOIRE"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grimoire data format"));
        return nullptr;
    }
    
    // Create new grimoire
    UGWTGrimoire* NewGrimoire = NewObject<UGWTGrimoire>();
    
    // In a real implementation, you would parse the node types and spells
    // For this example, we'll just use the default initialization
    
    UE_LOG(LogTemp, Display, TEXT("Loaded grimoire from string"));
    
    return NewGrimoire;
}

int32 UGWTGrimoire::GetSpellCount() const
{
    return Spells.Num();
}

UGWTSpell* UGWTGrimoire::CreateNewSpell(const FString& SpellName)
{
    // Create a new blank spell
    UGWTSpell* NewSpell = NewObject<UGWTSpell>();
    NewSpell->SpellName = FText::FromString(SpellName);
    NewSpell->SpellDescription = FText::FromString("A newly created spell.");
    
    // Add it to the grimoire
    AddSpell(NewSpell);
    
    UE_LOG(LogTemp, Display, TEXT("Created new spell: %s"), *SpellName);
    
    return NewSpell;
}

TArray<TSubclassOf<UGWTSpellNode>> UGWTGrimoire::GetUnlockedNodeTypes() const
{
    return UnlockedNodeTypes;
}

TArray<FText> UGWTGrimoire::GetUnlockedNodeCategories() const
{
    // Get unique categories from unlocked node types
    TArray<FText> Categories;
    
    for (TSubclassOf<UGWTSpellNode> NodeType : UnlockedNodeTypes)
    {
        UGWTSpellNode* DefaultNode = NodeType.GetDefaultObject();
        if (DefaultNode)
        {
            // Add category if not already in the list
            bool bFound = false;
            for (const FText& Category : Categories)
            {
                if (Category.EqualTo(DefaultNode->NodeCategory))
                {
                    bFound = true;
                    break;
                }
            }
            
            if (!bFound)
            {
                Categories.Add(DefaultNode->NodeCategory);
                UE_LOG(LogTemp, Verbose, TEXT("Found node category: %s"), *DefaultNode->NodeCategory.ToString());
            }
        }
    }
    
    return Categories;
}

TArray<TSubclassOf<UGWTSpellNode>> UGWTGrimoire::GetNodeTypesInCategory(const FText& Category) const
{
    // Get all node types in a specific category
    TArray<TSubclassOf<UGWTSpellNode>> NodeTypes;
    
    for (TSubclassOf<UGWTSpellNode> NodeType : UnlockedNodeTypes)
    {
        UGWTSpellNode* DefaultNode = NodeType.GetDefaultObject();
        if (DefaultNode && DefaultNode->NodeCategory.EqualTo(Category))
        {
            NodeTypes.Add(NodeType);
            UE_LOG(LogTemp, Verbose, TEXT("Node in category %s: %s"), 
                   *Category.ToString(), *DefaultNode->NodeTitle.ToString());
        }
    }
    
    return NodeTypes;
}