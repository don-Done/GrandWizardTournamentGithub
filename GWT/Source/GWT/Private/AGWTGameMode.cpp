// AGWTGameMode.cpp
// Implementation of the game mode

#include "AGWTGameMode.h"
#include "GWTTypes.h"
#include "AGWTLevelGenerator.h"
#include "UGWTObjective.h"
#include "UGWTEducationalTracker.h"
#include "UGWTSpell.h"
#include "AGWTGameState.h"
#include "AGWTPlayerController.h"
#include "AGWTPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AGWTGameMode::AGWTGameMode()
{
    // Set default classes
    DefaultPawnClass = AGWTPlayerCharacter::StaticClass();
    GameStateClass = AGWTGameState::StaticClass();
    PlayerControllerClass = AGWTPlayerController::StaticClass();

    // Game settings
    CurrentWave = 1;
    MaxWaves = 10;

    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("GWTGameMode initialized"));
}

void AGWTGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Initialize systems
    InitLevelGenerator();
    InitEducationalTracker();

    UE_LOG(LogTemp, Display, TEXT("Game initialized with map: %s"), *MapName);
}

void AGWTGameMode::StartPlay()
{
    Super::StartPlay();

    // Generate first level
    if (LevelGenerator)
    {
        LevelGenerator->GenerateLevel(CurrentWave);
    }

    // Set up initial objectives
    SetupWaveObjectives();

    // Reset game state
    AGWTGameState* GWTGameState = GetGameState<AGWTGameState>();
    if (GWTGameState)
    {
        GWTGameState->TotalScore = 0;
        GWTGameState->WaveStartTime = GetWorld()->GetTimeSeconds();
    }

    UE_LOG(LogTemp, Display, TEXT("Grand Wizard Tournament - Game Started"));
}

void AGWTGameMode::CompleteWave()
{
    // Grant rewards for completing the wave
    GrantWaveRewards();

    // Move to next wave
    CurrentWave++;

    UE_LOG(LogTemp, Display, TEXT("Wave %d completed!"), CurrentWave - 1);

    // Check if game is complete
    if (CurrentWave > MaxWaves)
    {
        // Player has won the game
        EndGame(true);
        return;
    }

    // Generate new level for next wave
    if (LevelGenerator)
    {
        LevelGenerator->GenerateLevel(CurrentWave);
    }

    // Setup new objectives
    SetupWaveObjectives();

    // Update game state
    AGWTGameState* GWTGameState = GetGameState<AGWTGameState>();
    if (GWTGameState)
    {
        GWTGameState->WaveStartTime = GetWorld()->GetTimeSeconds();
    }

    UE_LOG(LogTemp, Display, TEXT("Starting wave %d"), CurrentWave);
}

void AGWTGameMode::SetupWaveObjectives()
{
    // Clear any existing objectives
    CurrentObjectives.Empty();

    // Create objectives based on current wave
    CreateDefaultObjectives(CurrentWave);

    // Initialize each objective
    for (UGWTObjective* Objective : CurrentObjectives)
    {
        if (Objective)
        {
            Objective->InitializeObjective(this, CurrentWave);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Set up %d objectives for wave %d"), CurrentObjectives.Num(), CurrentWave);
}

void AGWTGameMode::GrantWaveRewards()
{
    // Calculate rewards based on objectives and wave number
    int32 BaseGold = 50 * CurrentWave;
    int32 BaseXP = 100 * CurrentWave;

    // Additional rewards for completing optional objectives
    int32 BonusGold = 0;
    int32 BonusXP = 0;

    for (UGWTObjective* Objective : CurrentObjectives)
    {
        if (Objective && Objective->IsCompleted())
        {
            BonusGold += Objective->RewardScore / 2;
            BonusXP += Objective->RewardScore;
        }
    }

    // Apply rewards to all players
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AGWTPlayerController* PC = Cast<AGWTPlayerController>(*It);
        if (PC)
        {
            // Add gold to inventory
            if (PC->PlayerInventory)
            {
                PC->PlayerInventory->AddGold(BaseGold + BonusGold);
            }

            // Add XP to progression
            if (PC->PlayerProgression)
            {
                PC->PlayerProgression->AddXP(BaseXP + BonusXP);
            }
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Wave rewards granted: %d gold, %d XP"), BaseGold + BonusGold, BaseXP + BonusXP);
}

void AGWTGameMode::EndGame(bool bWasSuccessful)
{
    // Update game state
    AGWTGameState* GWTGameState = GetGameState<AGWTGameState>();
    if (GWTGameState)
    {
        GWTGameState->bGameOver = true;
        GWTGameState->bWasSuccessful = bWasSuccessful;
    }

    // Notify all player controllers
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AGWTPlayerController* PC = Cast<AGWTPlayerController>(*It);
        if (PC)
        {
            PC->OnGameEnded(bWasSuccessful);
        }
    }

    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Display, TEXT("Game completed successfully! All %d waves cleared."), MaxWaves);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Game over at wave %d."), CurrentWave);
    }
}

void AGWTGameMode::TrackSpellUsage(UGWTSpell* Spell, bool bWasSuccessful)
{
    // Use educational tracker to analyze spell
    if (EducationalTracker && Spell)
    {
        EducationalTracker->AnalyzeSpellUsage(Spell, bWasSuccessful);

        UE_LOG(LogTemp, Verbose, TEXT("Tracked usage of spell: %s, Success: %s"),
            *Spell->SpellName.ToString(), bWasSuccessful ? TEXT("Yes") : TEXT("No"));
    }
}

int32 AGWTGameMode::GetWaveDifficulty() const
{
    // Base difficulty increases with each wave
    int32 Difficulty = FMath::Max(1, CurrentWave);

    // Scale difficulty by player count (for multiplayer)
    int32 PlayerCount = UGameplayStatics::GetNumPlayerControllers(GetWorld());
    if (PlayerCount > 1)
    {
        Difficulty = FMath::RoundToInt(Difficulty * (1.0f + 0.5f * (PlayerCount - 1)));
    }

    return Difficulty;
}

bool AGWTGameMode::AreAllObjectivesComplete() const
{
    // Check if all primary objectives are complete
    bool bAllComplete = true;

    for (UGWTObjective* Objective : CurrentObjectives)
    {
        if (Objective && Objective->bIsPrimary && !Objective->IsCompleted())
        {
            bAllComplete = false;
            break;
        }
    }

    return bAllComplete;
}

void AGWTGameMode::InitLevelGenerator()
{
    // Create a new level generator if none exists
    if (!LevelGenerator)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        LevelGenerator = GetWorld()->SpawnActor<AGWTLevelGenerator>(AGWTLevelGenerator::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams);

        UE_LOG(LogTemp, Display, TEXT("Level Generator initialized"));
    }
}

void AGWTGameMode::InitEducationalTracker()
{
    // Create educational tracker if none exists
    if (!EducationalTracker)
    {
        EducationalTracker = NewObject<UGWTEducationalTracker>(this);
        EducationalTracker->AddToRoot(); // Prevent garbage collection

        UE_LOG(LogTemp, Display, TEXT("Educational Tracker initialized"));
    }
}

void AGWTGameMode::CreateDefaultObjectives(int32 WaveNumber)
{
    // Create a few standard objectives based on wave number

    // Objective 1: Defeat enemies
    UGWTObjective* DefeatObjective = NewObject<UGWTObjective>(this);
    DefeatObjective->ObjectiveTitle = FText::Format(NSLOCTEXT("GWT", "DefeatEnemies", "Defeat {0} Enemies"), FText::AsNumber(5 * WaveNumber));
    DefeatObjective->ObjectiveDescription = FText::Format(NSLOCTEXT("GWT", "DefeatEnemiesDesc", "Defeat {0} enemies to advance to the next wave."), FText::AsNumber(5 * WaveNumber));
    DefeatObjective->bIsPrimary = true;
    DefeatObjective->RewardScore = 100 * WaveNumber;
    DefeatObjective->RequiredProgress = 5.0f * WaveNumber;
    CurrentObjectives.Add(DefeatObjective);

    // Objective 2: Find treasure (optional)
    UGWTObjective* TreasureObjective = NewObject<UGWTObjective>(this);
    TreasureObjective->ObjectiveTitle = NSLOCTEXT("GWT", "FindTreasure", "Find Hidden Treasure");
    TreasureObjective->ObjectiveDescription = NSLOCTEXT("GWT", "FindTreasureDesc", "Locate and collect the hidden treasure in the labyrinth.");
    TreasureObjective->bIsPrimary = false;
    TreasureObjective->RewardScore = 150 * WaveNumber;
    TreasureObjective->RequiredProgress = 1.0f;
    CurrentObjectives.Add(TreasureObjective);

    // Advanced waves get additional objectives
    if (WaveNumber >= 5)
    {
        // Objective 3: Complete within time limit
        UGWTObjective* TimeObjective = NewObject<UGWTObjective>(this);
        TimeObjective->ObjectiveTitle = FText::Format(NSLOCTEXT("GWT", "CompleteInTime", "Complete in {0} Minutes"), FText::AsNumber(5));
        TimeObjective->ObjectiveDescription = NSLOCTEXT("GWT", "CompleteInTimeDesc", "Complete all primary objectives before the time runs out.");
        TimeObjective->bIsPrimary = false;
        TimeObjective->RewardScore = 200 * WaveNumber;
        TimeObjective->RequiredProgress = 1.0f;
        CurrentObjectives.Add(TimeObjective);
    }
}