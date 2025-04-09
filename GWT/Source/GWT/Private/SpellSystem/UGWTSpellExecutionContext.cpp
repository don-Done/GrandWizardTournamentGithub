// GWTSpellExecutionContext.cpp
// Implementation of the spell execution context

#include "GWTSpellExecutionContext.h"
#include "GWTSpellNode.h"
#include "GWTCharacter.h"

UGWTSpellExecutionContext::UGWTSpellExecutionContext()
{
    // Initialize properties
    Caster = nullptr;
    Target = nullptr;
    
    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Spell execution context created"));
}

void UGWTSpellExecutionContext::SetVariable(FName Name, const FGWTVariableValue& Value)
{
    // Store variable in the map
    Variables.Add(Name, Value);
    
    // Log variable details based on type
    switch (Value.Type)
    {
        case EGWTVariableType::Float:
            UE_LOG(LogTemp, Verbose, TEXT("Set variable '%s' = %f"), *Name.ToString(), Value.FloatValue);
            break;
            
        case EGWTVariableType::Int:
            UE_LOG(LogTemp, Verbose, TEXT("Set variable '%s' = %d"), *Name.ToString(), Value.IntValue);
            break;
            
        case EGWTVariableType::Bool:
            UE_LOG(LogTemp, Verbose, TEXT("Set variable '%s' = %s"), *Name.ToString(), 
                   Value.BoolValue ? TEXT("true") : TEXT("false"));
            break;
            
        case EGWTVariableType::Vector:
            UE_LOG(LogTemp, Verbose, TEXT("Set variable '%s' = (%f, %f, %f)"), *Name.ToString(), 
                   Value.VectorValue.X, Value.VectorValue.Y, Value.VectorValue.Z);
            break;
            
        case EGWTVariableType::Target:
            UE_LOG(LogTemp, Verbose, TEXT("Set variable '%s' = %s"), *Name.ToString(), 
                   Value.TargetValue ? *Value.TargetValue->GetName() : TEXT("nullptr"));
            break;
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown variable type for '%s'"), *Name.ToString());
            break;
    }
}

FGWTVariableValue UGWTSpellExecutionContext::GetVariable(FName Name)
{
    // Try to find the variable
    if (Variables.Contains(Name))
    {
        return Variables[Name];
    }
    
    // Return default value if not found
    UE_LOG(LogTemp, Warning, TEXT("Variable '%s' not found in spell context"), *Name.ToString());
    return FGWTVariableValue();
}

void UGWTSpellExecutionContext::PushToStack(UGWTSpellNode* Node)
{
    // Add node to execution stack
    if (Node)
    {
        ExecutionStack.Push(Node);
        UE_LOG(LogTemp, Verbose, TEXT("Pushed node to stack: %s (stack size: %d)"), 
               *Node->NodeTitle.ToString(), ExecutionStack.Num());
    }
}

UGWTSpellNode* UGWTSpellExecutionContext::PopFromStack()
{
    // Remove and return top node from stack
    if (ExecutionStack.Num() > 0)
    {
        UGWTSpellNode* Node = ExecutionStack.Pop();
        UE_LOG(LogTemp, Verbose, TEXT("Popped node from stack: %s (stack size: %d)"), 
               *Node->NodeTitle.ToString(), ExecutionStack.Num());
        return Node;
    }
    
    return nullptr;
}

void UGWTSpellExecutionContext::ApplyEffect(AGWTCharacter* Target, EGWTEffectType EffectType, float Value, float Duration)
{
    // Apply effect to target character
    if (!Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot apply effect: Invalid target"));
        return;
    }
    
    // Handle different effect types
    switch (EffectType)
    {
        case EGWTEffectType::Damage:
            // Apply damage
            Target->TakeDamage(Value, EGWTElementType::None, Caster);
            UE_LOG(LogTemp, Verbose, TEXT("Applied damage effect to %s: %.1f damage"), 
                   *Target->GetName(), Value);
            break;
            
        case EGWTEffectType::Heal:
            // Apply healing
            Target->Heal(Value);
            UE_LOG(LogTemp, Verbose, TEXT("Applied healing effect to %s: %.1f healing"), 
                   *Target->GetName(), Value);
            break;
            
        case EGWTEffectType::ApplyStatus:
            // Apply status effect
            {
                FGWTStatusEffect StatusEffect;
                StatusEffect.Duration = Duration;
                StatusEffect.Strength = Value;
                StatusEffect.Causer = Caster;
                StatusEffect.TimeRemaining = Duration;
                
                // For this example, we'll use Burning as a placeholder
                StatusEffect.EffectType = EGWTStatusEffectType::Burning;
                
                Target->ApplyStatusEffect(StatusEffect);
                UE_LOG(LogTemp, Verbose, TEXT("Applied status effect to %s: Type %d, Duration %.1f, Strength %.1f"), 
                       *Target->GetName(), (int)StatusEffect.EffectType, Duration, Value);
            }
            break;
            
        // Implementation for other effect types would go here
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unsupported effect type: %d"), (int)EffectType);
            break;
    }
}

bool UGWTSpellExecutionContext::HasVariable(FName Name) const
{
    return Variables.Contains(Name);
}

void UGWTSpellExecutionContext::ClearVariables()
{
    Variables.Empty();
    UE_LOG(LogTemp, Verbose, TEXT("Cleared all variables from context"));
}

void UGWTSpellExecutionContext::LogContextState()
{
    // Log current state for debugging
    UE_LOG(LogTemp, Display, TEXT("=== Spell Execution Context ==="));
    UE_LOG(LogTemp, Display, TEXT("Caster: %s"), Caster ? *Caster->GetName() : TEXT("None"));
    UE_LOG(LogTemp, Display, TEXT("Target: %s"), Target ? *Target->GetName() : TEXT("None"));
    UE_LOG(LogTemp, Display, TEXT("Variables: %d"), Variables.Num());
    UE_LOG(LogTemp, Display, TEXT("Stack Size: %d"), ExecutionStack.Num());
    
    // Log variables
    for (const TPair<FName, FGWTVariableValue>& Pair : Variables)
    {
        FString ValueString;
        switch (Pair.Value.Type)
        {
            case EGWTVariableType::Float:
                ValueString = FString::Printf(TEXT("%.2f"), Pair.Value.FloatValue);
                break;
                
            case EGWTVariableType::Int:
                ValueString = FString::Printf(TEXT("%d"), Pair.Value.IntValue);
                break;
                
            case EGWTVariableType::Bool:
                ValueString = Pair.Value.BoolValue ? TEXT("true") : TEXT("false");
                break;
                
            case EGWTVariableType::Vector:
                ValueString = Pair.Value.VectorValue.ToString();
                break;
                
            case EGWTVariableType::Target:
                ValueString = Pair.Value.TargetValue ? *Pair.Value.TargetValue->GetName() : TEXT("None");
                break;
                
            default:
                ValueString = TEXT("Unknown type");
                break;
        }
        
        UE_LOG(LogTemp, Display, TEXT("  %s = %s"), *Pair.Key.ToString(), *ValueString);
    }
}