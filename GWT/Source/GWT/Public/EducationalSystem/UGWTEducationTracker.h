// GWTEducationalTracker.h
// Tracks programming concept understanding

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "GWTEducationalTracker.generated.h"

// Forward declarations
class UGWTSpell;

/**
 * Educational tracker for Grand Wizard Tournament
 * Analyzes spells and player actions to track educational progress
 * Records metrics on programming concept understanding
 */
UCLASS(BlueprintType)
class GWT_API UGWTEducationalTracker : public UObject
{
    GENERATED_BODY()
    
public:
    UGWTEducationalTracker();
    
    // Concept tracking
    UPROPERTY(BlueprintReadOnly, Category="Educational")
    TMap<EGWTProgrammingConcept, float> ConceptMastery;
    
    // Track history of spell usage
    UPROPERTY(BlueprintReadOnly, Category="Educational")
    TArray<FGWTSpellUsageData> SpellUsageHistory;
    
    // Methods
    UFUNCTION(BlueprintCallable, Category="Analysis")
    void AnalyzeSpellUsage(UGWTSpell* Spell, bool bWasSuccessful);
    
    UFUNCTION(BlueprintCallable, Category="Analysis")
    void AnalyzeSpellStructure(UGWTSpell* Spell);
    
    UFUNCTION(BlueprintCallable, Category="Analysis")
    float GetConceptMastery(EGWTProgrammingConcept Concept) const;
    
    UFUNCTION(BlueprintCallable, Category="Analysis")
    void UpdateConceptMastery(EGWTProgrammingConcept Concept, float DeltaMastery);
    
    UFUNCTION(BlueprintCallable, Category="Analysis")
    TArray<EGWTProgrammingConcept> GetMasteredConcepts(float MasteryThreshold = 0.7f) const;
    
    UFUNCTION(BlueprintCallable, Category="Analysis")
    TArray<EGWTProgrammingConcept> GetStruggleConcepts(float MasteryThreshold = 0.3f) const;
    
    UFUNCTION(BlueprintCallable, Category="Analytics")
    FString GenerateProgressReport() const;
    
    UFUNCTION(BlueprintCallable, Category="Analytics")
    void SaveAnalytics(const FString& FileName);
    
    UFUNCTION(BlueprintCallable, Category="Analytics")
    void LoadAnalytics(const FString& FileName);
    
protected:
    // Detailed analysis helpers
    float AnalyzeSequentialLogic(UGWTSpell* Spell) const;
    float AnalyzeConditionalLogic(UGWTSpell* Spell) const;
    float AnalyzeVariableUsage(UGWTSpell* Spell) const;
    float AnalyzeLoopUsage(UGWTSpell* Spell) const;
    float AnalyzeFunctionAbstraction(UGWTSpell* Spell) const;
    
    // Track iteration attempts
    int32 GetSpellIterationCount(UGWTSpell* Spell) const;
};