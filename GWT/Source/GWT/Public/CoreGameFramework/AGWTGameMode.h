// GWTGameMode.h
// Controls game flow and wave progression

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GWTGameMode.generated.h"

// Forward declarations
class UGWTObjective;
class AGWTLevelGenerator;
class UGWTEducationalTracker;
class UGWTSpell;

/**
 * Game mode class for Grand Wizard Tournament
 * Handles wave progression, objectives, and game flow
 */
UCLASS()
class GWT_API AGWTGameMode : public AGameMode
{
    GENERATED_BODY()
    
public:
    AGWTGameMode();
    
    // Wave management
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Flow")
    int32 CurrentWave = 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Flow")
    int32 MaxWaves = 10;
    
    // Objective tracking
    UPROPERTY()
    TArray<UGWTObjective*> CurrentObjectives;
    
    // Level generator reference
    UPROPERTY()
    AGWTLevelGenerator* LevelGenerator;
    
    // Educational tracking
    UPROPERTY()
    UGWTEducationalTracker* EducationalTracker;
    
    // Game flow methods
    virtual void StartPlay() override;
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    void CompleteWave();
    
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    void SetupWaveObjectives();
    
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    void GrantWaveRewards();
    
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    void EndGame(bool bWasSuccessful);
    
    // Educational methods
    UFUNCTION(BlueprintCallable, Category="Educational")
    void TrackSpellUsage(UGWTSpell* Spell, bool bWasSuccessful);
    
    // Helper methods
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    int32 GetWaveDifficulty() const;
    
    UFUNCTION(BlueprintCallable, Category="Game Flow")
    bool AreAllObjectivesComplete() const;
    
protected:
    // Spawns the level generator if needed
    void InitLevelGenerator();
    
    // Creates the educational tracker if needed
    void InitEducationalTracker();
    
    // Creates default objectives for a wave
    void CreateDefaultObjectives(int32 WaveNumber);
};