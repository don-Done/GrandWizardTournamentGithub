// AGWTGameState.h
// Tracks global game state information

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AGWTGameState.generated.h"

/**
 * Game state class for Grand Wizard Tournament
 * Tracks score, time, and other global game information
 */
UCLASS()
class GWT_API AGWTGameState : public AGameState
{
    GENERATED_BODY()

public:
    AGWTGameState();

    // Global score tracking
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scoring")
    int32 TotalScore = 0;

    // Time tracking
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Time")
    float WaveStartTime = 0.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Time")
    float WaveTimeLimit = 300.0f; // 5 minutes default

    // Enemy tracking
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Enemies")
    int32 RemainingEnemies = 0;

    // Game over state
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    bool bGameOver = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    bool bWasSuccessful = false;

    // Methods
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Scoring")
    void AddScore(int32 ScoreToAdd);

    UFUNCTION(BlueprintCallable, Category = "Time")
    float GetRemainingWaveTime() const;

    UFUNCTION(BlueprintCallable, Category = "Time")
    float GetElapsedWaveTime() const;

    UFUNCTION(BlueprintCallable, Category = "Enemies")
    void EnemyKilled();

    UFUNCTION(BlueprintCallable, Category = "Enemies")
    void EnemySpawned();
};
