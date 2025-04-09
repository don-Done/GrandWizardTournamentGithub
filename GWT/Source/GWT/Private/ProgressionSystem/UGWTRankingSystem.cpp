// GWTRankingSystem.cpp
// Implementation of the ranking system

#include "GWTRankingSystem.h"
#include "GWTGameMode.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

UGWTRankingSystem::UGWTRankingSystem()
{
    // Initialize ranking data
    CurrentRank = EGWTRank::Bronze;
    RankPoints = 0;
    PointsToNextRank = 100;
    TotalMatches = 0;
    Wins = 0;
    Losses = 0;
    
    // Initialize rank thresholds
    InitializeRankThresholds();
    
    UE_LOG(LogTemp, Display, TEXT("Ranking System initialized"));
}

void UGWTRankingSystem::AddMatchResult(bool bWasVictory, int32 Score, int32 WaveReached, TArray<FGuid> SpellsUsed)
{
    // Create match result entry
    FGWTMatchResult Result;
    Result.WasVictory = bWasVictory;
    Result.Score = Score;
    Result.WaveReached = WaveReached;
    Result.TimeStamp = FDateTime::Now();
    Result.SpellsUsed = SpellsUsed;
    
    // Calculate points gained/lost
    int32 PointDelta = CalculatePointsForMatch(bWasVictory, Score, WaveReached);
    Result.PointsEarned = PointDelta;
    
    // Add to match history
    MatchHistory.Add(Result);
    
    // Update totals
    TotalMatches++;
    if (bWasVictory)
    {
        Wins++;
    }
    else
    {
        Losses++;
    }
    
    // Update rank points
    RankPoints += PointDelta;
    
    // Ensure rank points don't go below 0
    RankPoints = FMath::Max(0, RankPoints);
    
    // Check for rank changes
    UpdateRank();
    
    UE_LOG(LogTemp, Display, TEXT("Added match result: Victory=%s, Score=%d, Wave=%d, Points=%d"),
           bWasVictory ? TEXT("Yes") : TEXT("No"), Score, WaveReached, PointDelta);
}

void UGWTRankingSystem::UpdateRank()
{
    // Check if points qualify for a new rank
    EGWTRank NewRank = CurrentRank;
    
    // Check each rank in ascending order
    for (auto It = RankThresholds.CreateConstIterator(); It; ++It)
    {
        if (RankPoints >= It.Value())
        {
            NewRank = It.Key();
        }
        else
        {
            // Stop when we find a rank we don't qualify for
            break;
        }
    }
    
    // If rank has changed
    if (NewRank != CurrentRank)
    {
        EGWTRank OldRank = CurrentRank;
        CurrentRank = NewRank;
        
        UE_LOG(LogTemp, Display, TEXT("Rank changed: %d -> %d"), (int32)OldRank, (int32)NewRank);
    }
    
    // Calculate points to next rank
    int32 NextRankPoints = 0;
    
    // Find the next higher rank
    bool bFoundCurrentRank = false;
    for (auto It = RankThresholds.CreateConstIterator(); It; ++It)
    {
        if (bFoundCurrentRank)
        {
            // This is the next rank after current
            NextRankPoints = It.Value();
            break;
        }
        
        if (It.Key() == CurrentRank)
        {
            bFoundCurrentRank = true;
        }
    }
    
    if (NextRankPoints > 0)
    {
        PointsToNextRank = NextRankPoints - RankPoints;
    }
    else
    {
        // Already at highest rank
        PointsToNextRank = 0;
    }
}

float UGWTRankingSystem::GetWinRate() const
{
    // Calculate win rate
    if (TotalMatches > 0)
    {
        return (float)Wins / (float)TotalMatches;
    }
    
    return 0.0f;
}

int32 UGWTRankingSystem::GetAverageScore() const
{
    // Calculate average score across all matches
    if (MatchHistory.Num() == 0)
    {
        return 0;
    }
    
    int32 TotalScore = 0;
    for (const FGWTMatchResult& Result : MatchHistory)
    {
        TotalScore += Result.Score;
    }
    
    return TotalScore / MatchHistory.Num();
}

int32 UGWTRankingSystem::GetPointsNeededForRank(EGWTRank TargetRank) const
{
    // Get points needed to reach a specific rank
    if (RankThresholds.Contains(TargetRank))
    {
        int32 RequiredPoints = RankThresholds[TargetRank];
        
        if (RankPoints >= RequiredPoints)
        {
            // Already at or above this rank
            return 0;
        }
        
        return RequiredPoints - RankPoints;
    }
    
    return -1; // Invalid rank
}

void UGWTRankingSystem::UpdateLeaderboard(const TArray<FGWTLeaderboardEntry>& NewLeaderboard)
{
    // Update the leaderboard with new data
    Leaderboard = NewLeaderboard;
    
    UE_LOG(LogTemp, Display, TEXT("Updated leaderboard with %d entries"), Leaderboard.Num());
}

TArray<FGWTLeaderboardEntry> UGWTRankingSystem::GetTopPlayers(int32 Count) const
{
    // Get the top N players from the leaderboard
    TArray<FGWTLeaderboardEntry> TopPlayers;
    
    int32 NumToGet = FMath::Min(Count, Leaderboard.Num());
    
    for (int32 i = 0; i < NumToGet; i++)
    {
        TopPlayers.Add(Leaderboard[i]);
    }
    
    return TopPlayers;
}

int32 UGWTRankingSystem::GetPlayerRanking(const FString& PlayerName) const
{
    // Find a player's position in the leaderboard
    for (int32 i = 0; i < Leaderboard.Num(); i++)
    {
        if (Leaderboard[i].PlayerName == PlayerName)
        {
            return i + 1; // 1-based ranking
        }
    }
    
    return -1; // Player not found
}

TArray<FGWTMatchmakingEntry> UGWTRankingSystem::FindSuitableMatches(int32 MaxEntries) const
{
    // Find suitable opponents based on rank
    TArray<FGWTMatchmakingEntry> Matches;
    
    // In a real implementation, this would query an online service
    // For this example, we'll create some dummy entries
    
    for (int32 i = 0; i < MaxEntries; i++)
    {
        FGWTMatchmakingEntry Entry;
        Entry.PlayerName = FString::Printf(TEXT("Player_%d"), i + 1);
        
        // Calculate a rank close to the player's rank
        int32 RankDelta = FMath::RandRange(-1, 1);
        int32 NewRank = FMath::Clamp((int32)CurrentRank + RankDelta, 0, (int32)EGWTRank::Grandmaster);
        Entry.Rank = (EGWTRank)NewRank;
        
        Entry.RankPoints = RankPoints + FMath::RandRange(-50, 50);
        Entry.WinRate = 0.5f + FMath::FRandRange(-0.2f, 0.2f);
        
        Matches.Add(Entry);
    }
    
    return Matches;
}

void UGWTRankingSystem::SaveRankingData(const FString& SlotName)
{
    // Save ranking data to a save file
    // This would be implemented using UGameplayStatics::SaveGameToSlot
    
    UE_LOG(LogTemp, Display, TEXT("Saved ranking data to slot: %s"), *SlotName);
}

bool UGWTRankingSystem::LoadRankingData(const FString& SlotName)
{
    // Load ranking data from a save file
    // This would be implemented using UGameplayStatics::LoadGameFromSlot
    
    UE_LOG(LogTemp, Display, TEXT("Loaded ranking data from slot: %s"), *SlotName);
    
    return true;
}

int32 UGWTRankingSystem::CalculatePointsForMatch(bool bWasVictory, int32 Score, int32 WaveReached) const
{
    // Calculate points gained or lost for a match
    // Base points for win/loss
    int32 BasePoints = bWasVictory ? 20 : -10;
    
    // Adjust based on wave reached (difficulty)
    float DifficultyFactor = GetMatchDifficultyFactor(WaveReached);
    
    // Adjust based on score (performance)
    float ScoreFactor = 1.0f + (Score / 10000.0f);
    
    // Calculate final points
    int32 FinalPoints = FMath::RoundToInt(BasePoints * DifficultyFactor * ScoreFactor);
    
    // Limit maximum points lost for beginners
    if (!bWasVictory && CurrentRank == EGWTRank::Bronze)
    {
        FinalPoints = FMath::Max(FinalPoints, -5); // Lose at most 5 points if in Bronze
    }
    
    return FinalPoints;
}

float UGWTRankingSystem::GetMatchDifficultyFactor(int32 WaveReached) const
{
    // Calculate a difficulty factor based on the wave reached
    // Higher waves = more difficult = more points
    
    // Base factor
    float Factor = 1.0f;
    
    // Adjust based on wave
    if (WaveReached <= 3)
    {
        Factor = 0.8f; // Early waves are easier
    }
    else if (WaveReached <= 7)
    {
        Factor = 1.0f; // Mid waves are normal
    }
    else if (WaveReached <= 10)
    {
        Factor = 1.2f; // Later waves are harder
    }
    else
    {
        Factor = 1.5f; // Very late waves are much harder
    }
    
    return Factor;
}

void UGWTRankingSystem::InitializeRankThresholds()
{
    // Set point thresholds for each rank
    RankThresholds.Add(EGWTRank::Bronze, 0);
    RankThresholds.Add(EGWTRank::Silver, 100);
    RankThresholds.Add(EGWTRank::Gold, 300);
    RankThresholds.Add(EGWTRank::Platinum, 600);
    RankThresholds.Add(EGWTRank::Diamond, 1000);
    RankThresholds.Add(EGWTRank::Master, 1500);
    RankThresholds.Add(EGWTRank::Grandmaster, 2000);
}