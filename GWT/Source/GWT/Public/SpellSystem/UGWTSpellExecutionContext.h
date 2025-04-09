// GWTSpellExecutionContext.h
// Manages the state during spell execution

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "GWTSpellExecutionContext.generated.h"

// Forward declarations
class UGWTSpellNode;
class AGWTCharacter;

/**
 * Context object for spell execution
 * Maintains state and variables during spell casting
 * Acts as a communication channel between spell nodes
 */
UCLASS()
class GWT_API UGWTSpellExecutionContext : public UObject
{
    GENERATED_BODY()
    
public:
    UGWTSpellExecutionContext();
    
    // Execution participants
    UPROPERTY()
    AActor* Caster;
    
    UPROPERTY()
    AActor* Target;
    
    // Hit information
    UPROPERTY()
    FHitResult HitResult;
    
    // Variable storage
    UPROPERTY()
    TMap<FName, FGWTVariableValue> Variables;
    
    // Flow control
    UPROPERTY()
    TArray<UGWTSpellNode*> ExecutionStack;
    
    // Methods
    UFUNCTION(BlueprintCallable, Category="Variables")
    void SetVariable(FName Name, const FGWTVariableValue& Value);
    
    UFUNCTION(BlueprintCallable, Category="Variables")
    FGWTVariableValue GetVariable(FName Name);
    
    UFUNCTION(BlueprintCallable, Category="Execution")
    void PushToStack(UGWTSpellNode* Node);
    
    UFUNCTION(BlueprintCallable, Category="Execution")
    UGWTSpellNode* PopFromStack();
    
    UFUNCTION(BlueprintCallable, Category="Execution")
    void ApplyEffect(AGWTCharacter* Target, EGWTEffectType EffectType, float Value, float Duration);
    
    // Utility methods
    UFUNCTION(BlueprintCallable, Category="Utility")
    bool HasVariable(FName Name) const;
    
    UFUNCTION(BlueprintCallable, Category="Utility")
    void ClearVariables();
    
    UFUNCTION(BlueprintCallable, Category="Utility")
    void LogContextState();
};