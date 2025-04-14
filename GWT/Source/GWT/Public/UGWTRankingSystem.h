// UGWTRankingSystem.h
// Handles competitive rankings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTRankingSystem.generated.h"

/**
 * Ranking system for Grand Wizard Tournament
 * Handles player ranks, matchmaking, and leaderboards
 * Tracks performance across competitive matches
 */
UCLASS(BlueprintType)
class GWT_API UGWTRankingSystem : public UObject
{
    GENERATED_BODY()

public:
    UGWTRankingSystem();

    // Ranking data
    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    EGWTRank CurrentRank = EGWTRank::Bronze;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    int32 RankPoints = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    int32 PointsToNextRank = 100;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    int32 TotalMatches = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    int32 Wins = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    int32 Losses = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    TArray<FGWTMatchResult> MatchHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Ranking")
    TArray<FGWTLeaderboardEntry> Leaderboard;

    // Methods
    UFUNCTION(BlueprintCallable, Category = "Ranking")
    void AddMatchResult(bool bWasVictory, int32 Score, int32 WaveReached, TArray<FGuid> SpellsUsed);

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    void UpdateRank();

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    float GetWinRate() const;

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    int32 GetAverageScore() const;

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    int32 GetPointsNeededForRank(EGWTRank TargetRank) const;

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    void UpdateLeaderboard(const TArray<FGWTLeaderboardEntry>& NewLeaderboard);

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    TArray<FGWTLeaderboardEntry> GetTopPlayers(int32 Count) const;

    UFUNCTION(BlueprintCallable, Category = "Ranking")
    int32 GetPlayerRanking(const FString& PlayerName) const;

    UFUNCTION(BlueprintCallable, Category = "Matchmaking")
    TArray<FGWTMatchmakingEntry> FindSuitableMatches(int32 MaxEntries = 5) const;

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void SaveRankingData(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool LoadRankingData(const FString& SlotName);

protected:
    // Calculate points gained/lost for a match
    int32 CalculatePointsForMatch(bool bWasVictory, int32 Score, int32 WaveReached) const;

    // Get match difficulty factor
    float GetMatchDifficultyFactor(int32 WaveReached) const;

    // Rank thresholds
    TMap<EGWTRank, int32> RankThresholds;

    // Initialize rank thresholds
    void InitializeRankThresholds();
};