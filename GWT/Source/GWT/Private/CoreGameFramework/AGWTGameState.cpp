// GWTGameState.cpp
// Implementation of the game state

#include "AGWTGameState.h"
#include "Net/UnrealNetwork.h"

AGWTGameState::AGWTGameState()
{
    // Initialize properties
    TotalScore = 0;
    WaveStartTime = 0.0f;
    WaveTimeLimit = 300.0f; // 5 minutes default
    RemainingEnemies = 0;
    bGameOver = false;
    bWasSuccessful = false;
    
    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("GWTGameState initialized"));
}

void AGWTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // Register replicated properties
    DOREPLIFETIME(AGWTGameState, TotalScore);
    DOREPLIFETIME(AGWTGameState, WaveStartTime);
    DOREPLIFETIME(AGWTGameState, WaveTimeLimit);
    DOREPLIFETIME(AGWTGameState, RemainingEnemies);
    DOREPLIFETIME(AGWTGameState, bGameOver);
    DOREPLIFETIME(AGWTGameState, bWasSuccessful);
}

void AGWTGameState::AddScore(int32 ScoreToAdd)
{
    // Add to the total score
    TotalScore += ScoreToAdd;
    
    // Log score change
    UE_LOG(LogTemp, Verbose, TEXT("Score increased by %d. New total: %d"), ScoreToAdd, TotalScore);
}

float AGWTGameState::GetRemainingWaveTime() const
{
    // Calculate time remaining for current wave
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - WaveStartTime;
    float RemainingTime = FMath::Max(0.0f, WaveTimeLimit - ElapsedTime);
    
    return RemainingTime;
}

float AGWTGameState::GetElapsedWaveTime() const
{
    // Calculate time elapsed in current wave
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - WaveStartTime;
    
    return ElapsedTime;
}

void AGWTGameState::EnemyKilled()
{
    // Decrement enemy count
    RemainingEnemies = FMath::Max(0, RemainingEnemies - 1);
    
    // Log enemy killed
    UE_LOG(LogTemp, Verbose, TEXT("Enemy killed. Remaining enemies: %d"), RemainingEnemies);
}

void AGWTGameState::EnemySpawned()
{
    // Increment enemy count
    RemainingEnemies++;
    
    // Log enemy spawned
    UE_LOG(LogTemp, Verbose, TEXT("Enemy spawned. Total enemies: %d"), RemainingEnemies);
}