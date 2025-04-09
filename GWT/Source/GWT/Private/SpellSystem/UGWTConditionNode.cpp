// GWTConditionNode.cpp
// Implementation of the condition node

#include "SpellSystem/UGWTConditionNode.h"
#include "SpellSystem/UGWTSpellExecutionContext.h"
#include "CharacterSystemS/AGWTCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGWTConditionNode::UGWTConditionNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Condition");
    NodeDescription = FText::FromString("Evaluates a condition and directs execution to either the true or false path.");
    NodeCategory = FText::FromString("Logic");
    NodeColor = FLinearColor(0.8f, 0.8f, 0.2f, 1.0f); // Yellow for condition nodes
    
    // Set default condition properties
    ConditionType = EGWTConditionType::HealthCheck;
    ComparisonValue = 50.0f; // Default to 50% health check
    
    // Initialize path nodes
    TruePathNode = nullptr;
    FalsePathNode = nullptr;
    
    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Condition Node created with type: %d"), (int32)ConditionType);
}

void UGWTConditionNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Condition node: Invalid context"));
        return;
    }
    
    // Evaluate the condition and follow appropriate path
    bool bConditionResult = EvaluateCondition(Context);
    
    // Log the condition result
    UE_LOG(LogTemp, Verbose, TEXT("Condition evaluated to: %s"), bConditionResult ? TEXT("True") : TEXT("False"));
    
    // Execute the appropriate path
    if (bConditionResult)
    {
        // Execute true path
        if (TruePathNode)
        {
            TruePathNode->Execute(Context);
        }
        else
        {
            UE_LOG(LogTemp, Verbose, TEXT("No true path node connected"));
        }
    }
    else
    {
        // Execute false path
        if (FalsePathNode)
        {
            FalsePathNode->Execute(Context);
        }
        else
        {
            UE_LOG(LogTemp, Verbose, TEXT("No false path node connected"));
        }
    }
}

EGWTSpellComponentType UGWTConditionNode::GetNodeType() const
{
    return EGWTSpellComponentType::Condition;
}

FString UGWTConditionNode::GetNodeTypeAsString() const
{
    return TEXT("Condition");
}

void UGWTConditionNode::AddOutputConnection(UGWTSpellNode* Node)
{
    // Add the node to outputs first
    Super::AddOutputConnection(Node);
    
    // Set as true path if it's the first connection
    if (!TruePathNode && Node)
    {
        SetTruePath(Node);
    }
    // Set as false path if true is already set and false isn't
    else if (TruePathNode && !FalsePathNode && Node && Node != TruePathNode)
    {
        SetFalsePath(Node);
    }
}

void UGWTConditionNode::RemoveOutputConnection(UGWTSpellNode* Node)
{
    // Remove from outputs first
    Super::RemoveOutputConnection(Node);
    
    // Clear true/false path references if needed
    if (TruePathNode == Node)
    {
        TruePathNode = nullptr;
        UE_LOG(LogTemp, Verbose, TEXT("Removed true path connection"));
    }
    
    if (FalsePathNode == Node)
    {
        FalsePathNode = nullptr;
        UE_LOG(LogTemp, Verbose, TEXT("Removed false path connection"));
    }
}

void UGWTConditionNode::SetTruePath(UGWTSpellNode* Node)
{
    // Make sure the node is a valid output
    if (Node && !OutputNodes.Contains(Node))
    {
        AddOutputConnection(Node);
    }
    
    // Set as true path
    TruePathNode = Node;
    
    UE_LOG(LogTemp, Verbose, TEXT("Set true path to node: %s"), 
           Node ? *Node->NodeTitle.ToString() : TEXT("None"));
}

void UGWTConditionNode::SetFalsePath(UGWTSpellNode* Node)
{
    // Make sure the node is a valid output
    if (Node && !OutputNodes.Contains(Node))
    {
        AddOutputConnection(Node);
    }
    
    // Set as false path
    FalsePathNode = Node;
    
    UE_LOG(LogTemp, Verbose, TEXT("Set false path to node: %s"), 
           Node ? *Node->NodeTitle.ToString() : TEXT("None"));
}

bool UGWTConditionNode::EvaluateCondition(UGWTSpellExecutionContext* Context)
{
    // Evaluate based on condition type
    switch (ConditionType)
    {
        case EGWTConditionType::HealthCheck:
            return EvaluateHealthCheck(Context);
            
        case EGWTConditionType::ManaCheck:
            return EvaluateManaCheck(Context);
            
        case EGWTConditionType::DistanceCheck:
            return EvaluateDistanceCheck(Context);
            
        case EGWTConditionType::ElementalCheck:
            return EvaluateElementalCheck(Context);
            
        case EGWTConditionType::StatusEffectCheck:
            return EvaluateStatusEffectCheck(Context);
            
        case EGWTConditionType::RandomChance:
            return EvaluateRandomChance(Context);
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown condition type in Condition node"));
            return false;
    }
}

bool UGWTConditionNode::EvaluateHealthCheck(UGWTSpellExecutionContext* Context)
{
    // Check health percentage of target or caster
    AActor* TargetActor = Context->Target ? Context->Target : Context->Caster;
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(TargetActor);
    
    if (TargetCharacter)
    {
        // Calculate health percentage
        float HealthPercentage = (TargetCharacter->CurrentHealth / TargetCharacter->MaxHealth) * 100.0f;
        
        // Compare to condition value
        bool bResult = HealthPercentage <= ComparisonValue;
        
        UE_LOG(LogTemp, Verbose, TEXT("Health check: %.1f%% <= %.1f%% = %s"), 
               HealthPercentage, ComparisonValue, bResult ? TEXT("True") : TEXT("False"));
        
        return bResult;
    }
    
    return false;
}

bool UGWTConditionNode::EvaluateManaCheck(UGWTSpellExecutionContext* Context)
{
    // Check mana percentage of target or caster
    AActor* TargetActor = Context->Target ? Context->Target : Context->Caster;
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(TargetActor);
    
    if (TargetCharacter)
    {
        // Calculate mana percentage
        float ManaPercentage = (TargetCharacter->CurrentMana / TargetCharacter->MaxMana) * 100.0f;
        
        // Compare to condition value
        bool bResult = ManaPercentage >= ComparisonValue;
        
        UE_LOG(LogTemp, Verbose, TEXT("Mana check: %.1f%% >= %.1f%% = %s"), 
               ManaPercentage, ComparisonValue, bResult ? TEXT("True") : TEXT("False"));
        
        return bResult;
    }
    
    return false;
}

bool UGWTConditionNode::EvaluateDistanceCheck(UGWTSpellExecutionContext* Context)
{
    // Check distance between caster and target
    if (Context->Caster && Context->Target)
    {
        // Calculate distance
        float Distance = FVector::Dist(Context->Caster->GetActorLocation(), Context->Target->GetActorLocation());
        
        // Compare to condition value (in units/cm)
        bool bResult = Distance <= ComparisonValue;
        
        UE_LOG(LogTemp, Verbose, TEXT("Distance check: %.1f units <= %.1f units = %s"), 
               Distance, ComparisonValue, bResult ? TEXT("True") : TEXT("False"));
        
        return bResult;
    }
    
    return false;
}

bool UGWTConditionNode::EvaluateElementalCheck(UGWTSpellExecutionContext* Context)
{
    // Check if target is vulnerable to specific element
    // This would typically be implemented with a more complex system
    // For this example, we'll just check if they have a related status effect
    
    AActor* TargetActor = Context->Target ? Context->Target : Context->Caster;
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(TargetActor);
    
    if (TargetCharacter)
    {
        // Check for elemental status effects
        bool bHasEffect = false;
        
        for (const FGWTStatusEffect& Effect : TargetCharacter->ActiveEffects)
        {
            // Check for relevant status effects based on element
            if ((Effect.EffectType == EGWTStatusEffectType::Burning && ComparisonValue == (int)EGWTElementType::Fire) ||
                (Effect.EffectType == EGWTStatusEffectType::Frozen && ComparisonValue == (int)EGWTElementType::Ice) ||
                (Effect.EffectType == EGWTStatusEffectType::Electrified && ComparisonValue == (int)EGWTElementType::Lightning))
            {
                bHasEffect = true;
                break;
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Elemental check for element %d: %s"), 
               (int)ComparisonValue, bHasEffect ? TEXT("True") : TEXT("False"));
        
        return bHasEffect;
    }
    
    return false;
}

bool UGWTConditionNode::EvaluateStatusEffectCheck(UGWTSpellExecutionContext* Context)
{
    // Check if target has a specific status effect
    AActor* TargetActor = Context->Target ? Context->Target : Context->Caster;
    AGWTCharacter* TargetCharacter = Cast<AGWTCharacter>(TargetActor);
    
    if (TargetCharacter)
    {
        // Check for specified status effect
        bool bHasEffect = false;
        
        for (const FGWTStatusEffect& Effect : TargetCharacter->ActiveEffects)
        {
            if ((int)Effect.EffectType == (int)ComparisonValue)
            {
                bHasEffect = true;
                break;
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Status effect check for effect %d: %s"), 
               (int)ComparisonValue, bHasEffect ? TEXT("True") : TEXT("False"));
        
        return bHasEffect;
    }
    
    return false;
}

bool UGWTConditionNode::EvaluateRandomChance(UGWTSpellExecutionContext* Context)
{
    // Random chance based on comparison value (0-100%)
    float RandomValue = FMath::RandRange(0.0f, 100.0f);
    bool bResult = RandomValue <= ComparisonValue;
    
    UE_LOG(LogTemp, Verbose, TEXT("Random chance: %.1f%% <= %.1f%% = %s"), 
           RandomValue, ComparisonValue, bResult ? TEXT("True") : TEXT("False"));
    
    return bResult;
}