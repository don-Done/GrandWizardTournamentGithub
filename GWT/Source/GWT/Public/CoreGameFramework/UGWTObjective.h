// GWTObjective.h
// Defines gameplay objectives for each wave

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTObjective.generated.h"

// Forward declarations
class AGWTGameMode;

/**
 * Represents a gameplay objective that players must complete
 * Can be primary (required) or secondary (optional) objectives
 */
UCLASS(BlueprintType, Blueprintable)
class GWT_API UGWTObjective : public UObject
{
    GENERATED_BODY()
    
public:
    UGWTObjective();
    
    // Objective details
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Objective")
    FText ObjectiveTitle;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Objective")
    FText ObjectiveDescription;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Objective")
    bool bIsPrimary = true;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Objective")
    int32 RewardScore = 100;
    
    // Progress tracking
    UPROPERTY(BlueprintReadOnly, Category="Progress")
    float CurrentProgress = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Progress")
    float RequiredProgress = 1.0f;
    
    // Game mode reference
    UPROPERTY()
    AGWTGameMode* OwningGameMode;
    
    // Methods
    UFUNCTION(BlueprintCallable, Category="Objective")
    virtual void InitializeObjective(AGWTGameMode* GameMode, int32 WaveNumber);
    
    UFUNCTION(BlueprintCallable, Category="Objective")
    virtual void UpdateProgress(float ProgressDelta);
    
    UFUNCTION(BlueprintCallable, Category="Objective")
    virtual bool IsCompleted() const;
    
    UFUNCTION(BlueprintCallable, Category="Objective")
    virtual void OnCompleted();
    
    UFUNCTION(BlueprintCallable, Category="Progress")
    float GetProgressPercentage() const;
    
    UFUNCTION(BlueprintCallable, Category="Display")
    FString GetProgressText() const;
};