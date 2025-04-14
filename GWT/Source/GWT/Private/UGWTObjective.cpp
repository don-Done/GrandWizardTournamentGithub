// UGWTObjective.cpp
// Implementation of the objective system

#include "UGWTObjective.h"
#include "AGWTGameMode.h"
#include "AGWTGameState.h"

UGWTObjective::UGWTObjective()
{
    // Initialize properties
    ObjectiveTitle = FText::FromString("Default Objective");
    ObjectiveDescription = FText::FromString("Complete this objective to progress.");
    bIsPrimary = true;
    RewardScore = 100;
    CurrentProgress = 0.0f;
    RequiredProgress = 1.0f;
    OwningGameMode = nullptr;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("UGWTObjective created"));
}

void UGWTObjective::InitializeObjective(AGWTGameMode* GameMode, int32 WaveNumber)
{
    // Store the game mode reference
    OwningGameMode = GameMode;

    // Reset progress
    CurrentProgress = 0.0f;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Objective initialized: %s"), *ObjectiveTitle.ToString());
}

void UGWTObjective::UpdateProgress(float ProgressDelta)
{
    // Add to current progress
    float PreviousProgress = CurrentProgress;
    CurrentProgress = FMath::Min(RequiredProgress, CurrentProgress + ProgressDelta);

    // Log progress update
    UE_LOG(LogTemp, Verbose, TEXT("Objective '%s' progress: %.1f / %.1f"),
        *ObjectiveTitle.ToString(), CurrentProgress, RequiredProgress);

    // Check if objective was just completed
    if (PreviousProgress < RequiredProgress && CurrentProgress >= RequiredProgress)
    {
        OnCompleted();
    }
}

bool UGWTObjective::IsCompleted() const
{
    // Check if progress meets or exceeds the requirement
    return CurrentProgress >= RequiredProgress;
}

void UGWTObjective::OnCompleted()
{
    // Add score for completing the objective
    if (OwningGameMode)
    {
        AGWTGameState* GWTGameState = OwningGameMode->GetGameState<AGWTGameState>();
        if (GWTGameState)
        {
            GWTGameState->AddScore(RewardScore);
        }
    }

    // Log completion
    UE_LOG(LogTemp, Display, TEXT("Objective completed: %s"), *ObjectiveTitle.ToString());

    // Check if all objectives are complete
    if (OwningGameMode && bIsPrimary && OwningGameMode->AreAllObjectivesComplete())
    {
        OwningGameMode->CompleteWave();
    }
}

float UGWTObjective::GetProgressPercentage() const
{
    // Calculate progress as a percentage
    return (RequiredProgress > 0.0f) ? (CurrentProgress / RequiredProgress) * 100.0f : 0.0f;
}

FString UGWTObjective::GetProgressText() const
{
    // Format progress as a string
    return FString::Printf(TEXT("%.0f / %.0f"), CurrentProgress, RequiredProgress);
}