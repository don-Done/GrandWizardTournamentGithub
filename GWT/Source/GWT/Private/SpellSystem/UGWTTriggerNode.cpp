// GWTTriggerNode.cpp
// Implementation of the trigger node

#include "GWTTriggerNode.h"
#include "GWTSpellExecutionContext.h"
#include "GWTCharacter.h"

UGWTTriggerNode::UGWTTriggerNode()
{
    // Set node identity
    NodeTitle = FText::FromString("Trigger");
    NodeDescription = FText::FromString("Executes connected nodes when a specific event occurs.");
    NodeCategory = FText::FromString("Events");
    NodeColor = FLinearColor(0.2f, 0.7f, 0.2f, 1.0f); // Green for trigger nodes
    
    // Set default trigger properties
    TriggerType = EGWTTriggerType::OnCast;
    TriggerValue = 0.0f;
    
    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Trigger Node created with type: %d"), (int32)TriggerType);
}

void UGWTTriggerNode::Execute(UGWTSpellExecutionContext* Context)
{
    // Check if context is valid
    if (!Context)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot execute Trigger node: Invalid context"));
        return;
    }
    
    // Handle based on trigger type
    switch (TriggerType)
    {
        case EGWTTriggerType::OnCast:
            HandleOnCast(Context);
            break;
            
        case EGWTTriggerType::OnHit:
            HandleOnHit(Context);
            break;
            
        case EGWTTriggerType::OnEnemyEnter:
            HandleOnEnemyEnter(Context);
            break;
            
        case EGWTTriggerType::OnHealthBelow:
            HandleOnHealthBelow(Context);
            break;
            
        case EGWTTriggerType::OnManaAbove:
            HandleOnManaAbove(Context);
            break;
            
        case EGWTTriggerType::OnTimerExpired:
            HandleOnTimerExpired(Context);
            break;
            
        default:
            // Unknown trigger type, log warning
            UE_LOG(LogTemp, Warning, TEXT("Unknown trigger type in Trigger node"));
            break;
    }
}

EGWTSpellComponentType UGWTTriggerNode::GetNodeType() const
{
    return EGWTSpellComponentType::Trigger;
}

FString UGWTTriggerNode::GetNodeTypeAsString() const
{
    return TEXT("Trigger");
}

bool UGWTTriggerNode::ShouldTrigger(UGWTSpellExecutionContext* Context)
{
    // Default implementation always returns true
    // Derived classes might override this with specific conditions
    return true;
}

void UGWTTriggerNode::HandleOnCast(UGWTSpellExecutionContext* Context)
{
    // OnCast trigger always executes on initial spell cast
    // Simply execute all output nodes
    for (UGWTSpellNode* OutputNode : OutputNodes)
    {
        if (OutputNode)
        {
            OutputNode->Execute(Context);
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("OnCast trigger executed"));
}

void UGWTTriggerNode::HandleOnHit(UGWTSpellExecutionContext* Context)
{
    // OnHit requires a valid hit result
    if (Context->HitResult.GetActor())
    {
        // Execute all output nodes
        for (UGWTSpellNode* OutputNode : OutputNodes)
        {
            if (OutputNode)
            {
                OutputNode->Execute(Context);
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("OnHit trigger executed, hit actor: %s"), 
               *Context->HitResult.GetActor()->GetName());
    }
}

void UGWTTriggerNode::HandleOnEnemyEnter(UGWTSpellExecutionContext* Context)
{
    // This would typically be implemented with overlap events
    // For this example, we'll simulate it based on the hit result
    
    if (Context->HitResult.GetActor())
    {
        // Check if hit actor is an enemy (simplified implementation)
        AGWTCharacter* HitCharacter = Cast<AGWTCharacter>(Context->HitResult.GetActor());
        if (HitCharacter && HitCharacter != Context->Caster)
        {
            // Execute all output nodes
            for (UGWTSpellNode* OutputNode : OutputNodes)
            {
                if (OutputNode)
                {
                    OutputNode->Execute(Context);
                }
            }
            
            UE_LOG(LogTemp, Verbose, TEXT("OnEnemyEnter trigger executed for enemy: %s"), 
                   *HitCharacter->GetName());
        }
    }
}

void UGWTTriggerNode::HandleOnHealthBelow(UGWTSpellExecutionContext* Context)
{
    // Check caster's health
    AGWTCharacter* CasterCharacter = Cast<AGWTCharacter>(Context->Caster);
    if (CasterCharacter)
    {
        float HealthPercentage = (CasterCharacter->CurrentHealth / CasterCharacter->MaxHealth) * 100.0f;
        
        // If health percentage is below trigger value
        if (HealthPercentage <= TriggerValue)
        {
            // Execute all output nodes
            for (UGWTSpellNode* OutputNode : OutputNodes)
            {
                if (OutputNode)
                {
                    OutputNode->Execute(Context);
                }
            }
            
            UE_LOG(LogTemp, Verbose, TEXT("OnHealthBelow trigger executed, health: %.1f%%, threshold: %.1f%%"), 
                   HealthPercentage, TriggerValue);
        }
    }
}

void UGWTTriggerNode::HandleOnManaAbove(UGWTSpellExecutionContext* Context)
{
    // Check caster's mana
    AGWTCharacter* CasterCharacter = Cast<AGWTCharacter>(Context->Caster);
    if (CasterCharacter)
    {
        float ManaPercentage = (CasterCharacter->CurrentMana / CasterCharacter->MaxMana) * 100.0f;
        
        // If mana percentage is above trigger value
        if (ManaPercentage >= TriggerValue)
        {
            // Execute all output nodes
            for (UGWTSpellNode* OutputNode : OutputNodes)
            {
                if (OutputNode)
                {
                    OutputNode->Execute(Context);
                }
            }
            
            UE_LOG(LogTemp, Verbose, TEXT("OnManaAbove trigger executed, mana: %.1f%%, threshold: %.1f%%"), 
                   ManaPercentage, TriggerValue);
        }
    }
}

void UGWTTriggerNode::HandleOnTimerExpired(UGWTSpellExecutionContext* Context)
{
    // Timer-based triggers would typically be implemented with a timer system
    // For this example, we'll just log that it would execute after the specified time
    
    UE_LOG(LogTemp, Display, TEXT("Timer trigger would execute after %.1f seconds"), TriggerValue);
    
    // In a real implementation, you'd set up a timer
    // For now, we'll execute immediately for demonstration
    for (UGWTSpellNode* OutputNode : OutputNodes)
    {
        if (OutputNode)
        {
            OutputNode->Execute(Context);
        }
    }
}