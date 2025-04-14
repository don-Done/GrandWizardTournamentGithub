// UGWTTutorialManager.h
// Manages tutorial sequences

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTTutorialManager.generated.h"

// Forward declarations
class UGWTSpellEditorWidget;

/**
 * Tutorial manager for Grand Wizard Tournament
 * Guides players through learning the spell crafting system
 * Presents step-by-step tutorials on programming concepts
 */
UCLASS(BlueprintType)
class GWT_API UGWTTutorialManager : public UObject
{
    GENERATED_BODY()

public:
    UGWTTutorialManager();

    // Tutorial data
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial")
    TArray<FGWTTutorialSequence> TutorialSequences;

    // Current tutorial state
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
    int32 CurrentSequenceIndex = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
    int32 CurrentStepIndex = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
    bool bTutorialActive = false;

    // Widget references
    UPROPERTY(BlueprintReadWrite, Category = "References")
    UGWTSpellEditorWidget* SpellEditorWidget;

    // Methods
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void StartTutorial(int32 SequenceIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void EndTutorial();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void NextStep();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void PreviousStep();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void SkipToStep(int32 StepIndex);

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    FGWTTutorialStep GetCurrentStep() const;

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    FString GetCurrentInstructions() const;

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    bool IsCurrentStepComplete() const;

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    bool IsTutorialComplete() const;

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void LoadTutorialData(const FString& FileName);

    UFUNCTION(BlueprintCallable, Category = "Setup")
    void InitializeDefaultTutorials();

protected:
    // Tutorial management
    void ExecuteStepAction(const FGWTTutorialStep& Step);
    bool CheckStepCompletion(const FGWTTutorialStep& Step);

    // Helper to set up the editor for a specific tutorial step
    void SetupEditorForStep(const FGWTTutorialStep& Step);

    // Clear any temporary state from previous steps
    void CleanupPreviousStep();
};