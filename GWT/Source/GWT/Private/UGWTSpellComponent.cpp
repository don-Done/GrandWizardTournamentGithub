// UGWTSpellComponent.cpp
// Implementation of the spell component item

#include "UGWTSpellComponent.h"
#include "AGWTCharacter.h"
#include "UGWTSpellNode.h"
#include "UGWTGrimoire.h"
#include "AGWTPlayerCharacter.h"

UGWTSpellComponent::UGWTSpellComponent()
{
    // Set base item properties
    ItemName = FText::FromString("Spell Component");
    ItemDescription = FText::FromString("Unlocks a new spell component when used.");
    bIsStackable = false;
    MaxStackSize = 1;

    // Set spell component defaults
    NodeType = nullptr;
    ComponentType = EGWTSpellComponentType::Magic;
    ElementType = EGWTElementType::None;
    bIsPermanentUnlock = true;
    UsageCount = 0; // Unlimited for permanent

    UE_LOG(LogTemp, Verbose, TEXT("Spell Component created: %s"), *ItemName.ToString());
}

bool UGWTSpellComponent::Use(AGWTCharacter* Character)
{
    // Check if character is valid
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot use spell component: Invalid character"));
        return false;
    }

    // Check if character meets requirements
    if (!CanUse(Character))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character cannot use spell component: %s"), *ItemName.ToString());
        return false;
    }

    // Get the player's grimoire
    UGWTGrimoire* Grimoire = nullptr;
    AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(Character);
    if (PlayerChar)
    {
        Grimoire = PlayerChar->Grimoire;
    }

    if (!Grimoire)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character doesn't have a grimoire"));
        return false;
    }

    // Unlock the node
    bool bSuccess = UnlockNodeInGrimoire(Grimoire);

    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("Character %s used spell component: %s"),
            *Character->GetName(), *ItemName.ToString());

        // Consume item if not permanent or if all uses are expended
        if (!bIsPermanentUnlock)
        {
            if (UsageCount > 0)
            {
                UsageCount--;
                UE_LOG(LogTemp, Verbose, TEXT("Spell component has %d uses remaining"), UsageCount);

                if (UsageCount <= 0)
                {
                    RemoveFromStack(1);
                }
            }
            else
            {
                RemoveFromStack(1);
            }
        }

        return true;
    }

    return false;
}

FString UGWTSpellComponent::GetItemDescription() const
{
    // Get base description
    FString Description = Super::GetItemDescription();

    // Add spell component-specific details
    Description += TEXT("\n\n");
    Description += GetComponentDescription();

    // Add usage info
    if (bIsPermanentUnlock)
    {
        Description += TEXT("\nPermanently unlocks when used");
    }
    else if (UsageCount > 0)
    {
        Description += FString::Printf(TEXT("\nTemporarily unlocks for %d uses"), UsageCount);
    }
    else
    {
        Description += TEXT("\nSingle use");
    }

    return Description;
}

UGWTItem* UGWTSpellComponent::CreateCopy() const
{
    // Create a new instance with the same properties
    UGWTSpellComponent* NewComponent = NewObject<UGWTSpellComponent>(GetOuter(), GetClass());

    // Copy base properties
    NewComponent->ItemName = ItemName;
    NewComponent->ItemDescription = ItemDescription;
    NewComponent->ItemIcon = ItemIcon;
    NewComponent->Rarity = Rarity;
    NewComponent->GoldValue = GoldValue;
    NewComponent->RequiredLevel = RequiredLevel;

    // Copy spell component properties
    NewComponent->NodeType = NodeType;
    NewComponent->ComponentType = ComponentType;
    NewComponent->ElementType = ElementType;
    NewComponent->AdditionalNodeTypes = AdditionalNodeTypes;
    NewComponent->bIsPermanentUnlock = bIsPermanentUnlock;
    NewComponent->UsageCount = UsageCount;

    UE_LOG(LogTemp, Verbose, TEXT("Created copy of spell component: %s"), *ItemName.ToString());

    return NewComponent;
}

bool UGWTSpellComponent::UnlockNodeInGrimoire(UGWTGrimoire* Grimoire)
{
    // Unlock the node in the grimoire
    if (!Grimoire)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot unlock node: Invalid grimoire"));
        return false;
    }

    // Make sure we have a valid node type
    if (!NodeType)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot unlock node: Invalid node type"));
        return false;
    }

    // Check if already unlocked
    if (Grimoire->IsNodeTypeUnlocked(NodeType))
    {
        UE_LOG(LogTemp, Warning, TEXT("Node type already unlocked: %s"), *NodeType->GetName());
        return false;
    }

    // Unlock the node
    Grimoire->UnlockNodeType(NodeType);

    // Unlock additional nodes if any
    for (TSubclassOf<UGWTSpellNode> AdditionalType : AdditionalNodeTypes)
    {
        if (AdditionalType && !Grimoire->IsNodeTypeUnlocked(AdditionalType))
        {
            Grimoire->UnlockNodeType(AdditionalType);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Unlocked spell component node type: %s"), *NodeType->GetName());

    return true;
}

FString UGWTSpellComponent::GetComponentDescription() const
{
    // Format the component description
    FString Description = TEXT("Unlocks the following spell component:\n");

    // Add component type
    Description += FString::Printf(TEXT("Type: %s\n"), *UEnum::GetValueAsString(ComponentType));

    // Add element if applicable
    if (ElementType != EGWTElementType::None)
    {
        Description += FString::Printf(TEXT("Element: %s\n"), *UEnum::GetValueAsString(ElementType));
    }

    // Add node description if available
    if (NodeType)
    {
        UGWTSpellNode* DefaultNode = NodeType.GetDefaultObject();
        if (DefaultNode)
        {
            Description += FString::Printf(TEXT("Node: %s\n"), *DefaultNode->NodeTitle.ToString());
            Description += FString::Printf(TEXT("%s"), *DefaultNode->NodeDescription.ToString());
        }
    }

    // Add additional nodes if any
    if (AdditionalNodeTypes.Num() > 0)
    {
        Description += TEXT("\n\nAlso unlocks:");
        for (TSubclassOf<UGWTSpellNode> AdditionalType : AdditionalNodeTypes)
        {
            if (AdditionalType)
            {
                UGWTSpellNode* DefaultNode = AdditionalType.GetDefaultObject();
                if (DefaultNode)
                {
                    Description += FString::Printf(TEXT("\n- %s"), *DefaultNode->NodeTitle.ToString());
                }
            }
        }
    }

    return Description;
}

FLinearColor UGWTSpellComponent::GetComponentColor() const
{
    // Return color based on component type
    switch (ComponentType)
    {
    case EGWTSpellComponentType::Magic:
        return FLinearColor(1.0f, 0.2f, 0.2f); // Red

    case EGWTSpellComponentType::Trigger:
        return FLinearColor(0.2f, 0.7f, 0.2f); // Green

    case EGWTSpellComponentType::Effect:
        return FLinearColor(0.2f, 0.2f, 1.0f); // Blue

    case EGWTSpellComponentType::Condition:
        return FLinearColor(0.8f, 0.8f, 0.2f); // Yellow

    case EGWTSpellComponentType::Variable:
        return FLinearColor(0.7f, 0.4f, 0.7f); // Purple

    case EGWTSpellComponentType::Flow:
        return FLinearColor(0.2f, 0.5f, 0.7f); // Light blue

    default:
        return FLinearColor(0.5f, 0.5f, 0.5f); // Gray
    }
}