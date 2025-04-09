// GWTEnemySpawner.cpp
// Implementation of the enemy spawner

#include "GWTEnemySpawner.h"
#include "GWTRoom.h"
#include "GWTEnemyCharacter.h"
#include "GWTGameMode.h"
#include "GWTGameState.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"

UGWTEnemySpawner::UGWTEnemySpawner()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 5.0f; // Tick every 5 seconds
    
    // Set default spawning properties
    MinSpawnDistance = 500.0f;
    MaxEnemiesPerRoom = 5;
    MaxConcurrentEnemies = 20;
    
    UE_LOG(LogTemp, Display, TEXT("Enemy Spawner created"));
}

void UGWTEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize enemy classes if needed
    InitializeEnemyClasses();
    
    UE_LOG(LogTemp, Display, TEXT("Enemy Spawner initialized with %d enemy types"), 
           EnemyClasses.Num());
}

void UGWTEnemySpawner::TickComponent(float DeltaTime, ELevelTick TickType, 
                                    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Clean up any dead enemies
    CleanupDeadEnemies();
    
    // Log active enemy count
    UE_LOG(LogTemp, Verbose, TEXT("Active enemies: %d/%d"), 
           ActiveEnemies.Num(), MaxConcurrentEnemies);
}

void UGWTEnemySpawner::SpawnEnemiesForWave(int32 WaveNumber, AGWTRoom* Room)
{
    // Make sure room is valid
    if (!Room)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn enemies: Invalid room"));
        return;
    }
    
    // Only spawn in combat or boss rooms
    if (Room->RoomType != EGWTRoomType::Combat && Room->RoomType != EGWTRoomType::Boss)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Room is not a combat or boss room, skipping enemy spawn"));
        return;
    }
    
    // Check if we're at the enemy cap
    if (GetActiveEnemyCount() >= MaxConcurrentEnemies)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn more enemies: At max capacity (%d)"), 
               MaxConcurrentEnemies);
        return;
    }
    
    // Calculate how many enemies to spawn
    int32 EnemyCount = CalculateEnemyCountForRoom(Room, WaveNumber);
    
    // Get the wave configuration
    FGWTEnemyWaveInfo WaveConfig = GetWaveConfiguration(WaveNumber);
    
    UE_LOG(LogTemp, Display, TEXT("Spawning %d enemies in room for wave %d"), 
           EnemyCount, WaveNumber);
    
    // Spawn the enemies
    for (int32 i = 0; i < EnemyCount; i++)
    {
        // Check if we're at the enemy cap
        if (GetActiveEnemyCount() >= MaxConcurrentEnemies)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit max enemy cap during spawn, stopping"));
            break;
        }
        
        // Select an enemy type for this wave
        TSubclassOf<AGWTEnemyCharacter> EnemyClass = SelectEnemyTypeForWave(WaveNumber);
        
        if (EnemyClass)
        {
            // Get a spawn point
            FVector SpawnLocation = GetRandomSpawnPointInRoom(Room);
            
            // Spawn the enemy
            AGWTEnemyCharacter* Enemy = SpawnEnemy(EnemyClass, SpawnLocation, WaveNumber);
            
            if (Enemy)
            {
                UE_LOG(LogTemp, Verbose, TEXT("Spawned enemy %d/%d in room"), i + 1, EnemyCount);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy %d/%d"), i + 1, EnemyCount);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No valid enemy class for wave %d"), WaveNumber);
        }
    }
}

AGWTEnemyCharacter* UGWTEnemySpawner::SpawnEnemy(TSubclassOf<AGWTEnemyCharacter> EnemyClass, 
                                               FVector Location, int32 WaveNumber)
{
    // Make sure we have a valid class
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spawn enemy: Invalid class"));
        return nullptr;
    }
    
    // Spawn the enemy
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    AGWTEnemyCharacter* Enemy = GetWorld()->SpawnActor<AGWTEnemyCharacter>(
        EnemyClass,
        Location,
        FRotator(0.0f, FMath::FRand() * 360.0f, 0.0f),
        SpawnParams
    );
    
    if (Enemy)
    {
        // Set difficulty based on wave
        Enemy->SetDifficultyLevel(WaveNumber);
        
        // Register the enemy
        RegisterEnemy(Enemy);
        
        // Update game state
        AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
        if (GameState && GameState->IsA(AGWTGameState::StaticClass()))
        {
            AGWTGameState* GWTGameState = Cast<AGWTGameState>(GameState);
            if (GWTGameState)
            {
                GWTGameState->EnemySpawned();
            }
        }
        
        UE_LOG(LogTemp, Display, TEXT("Spawned enemy of type %s at location %s"), 
               *Enemy->GetClass()->GetName(), *Location.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy at location %s"), 
               *Location.ToString());
    }
    
    return Enemy;
}

TSubclassOf<AGWTEnemyCharacter> UGWTEnemySpawner::SelectEnemyTypeForWave(int32 WaveNumber)
{
    // Get enemy types allowed for this wave
    TArray<EGWTEnemyType> AllowedTypes = GetEnemyTypesForWave(WaveNumber);
    
    // Select a random type
    EGWTEnemyType SelectedType = GetRandomEnemyType(AllowedTypes);
    
    // Get the class for this type
    if (EnemyClasses.Contains(SelectedType))
    {
        return EnemyClasses[SelectedType];
    }
    
    // Fallback to first available type
    for (const auto& Pair : EnemyClasses)
    {
        return Pair.Value;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("No enemy classes available"));
    return nullptr;
}

int32 UGWTEnemySpawner::CalculateEnemyCountForRoom(AGWTRoom* Room, int32 WaveNumber)
{
    // Base count depends on room type
    int32 BaseCount = 3; // Default for combat rooms
    
    if (Room->RoomType == EGWTRoomType::Boss)
    {
        BaseCount = 1; // Boss rooms have fewer, stronger enemies
    }
    
    // Scale by wave number
    float WaveScale = 1.0f + (WaveNumber * 0.1f); // +10% per wave
    
    // Apply wave difficulty from game mode
    float DifficultyMultiplier = 1.0f;
    AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
    if (GameMode && GameMode->IsA(AGWTGameMode::StaticClass()))
    {
        AGWTGameMode* GWTGameMode = Cast<AGWTGameMode>(GameMode);
        if (GWTGameMode)
        {
            DifficultyMultiplier = GWTGameMode->GetWaveDifficulty() / 10.0f;
        }
    }
    
    // Calculate final count
    int32 FinalCount = FMath::RoundToInt(BaseCount * WaveScale * DifficultyMultiplier);
    
    // Clamp to max enemies per room
    FinalCount = FMath::Min(FinalCount, (int32)MaxEnemiesPerRoom);
    
    // Clamp to max concurrent enemies
    FinalCount = FMath::Min(FinalCount, MaxConcurrentEnemies - GetActiveEnemyCount());
    
    return FMath::Max(1, FinalCount); // Always spawn at least 1 enemy
}

TArray<EGWTEnemyType> UGWTEnemySpawner::GetEnemyTypesForWave(int32 WaveNumber) const
{
    // Define which enemy types can appear in which waves
    TArray<EGWTEnemyType> AllowedTypes;
    
    // Early waves (1-3): Basic enemies
    if (WaveNumber <= 3)
    {
        AllowedTypes.Add(EGWTEnemyType::Goblin);
        AllowedTypes.Add(EGWTEnemyType::Rat);
        AllowedTypes.Add(EGWTEnemyType::Slime);
    }
    // Mid waves (4-7): Basic + intermediate enemies
    else if (WaveNumber <= 7)
    {
        AllowedTypes.Add(EGWTEnemyType::Goblin);
        AllowedTypes.Add(EGWTEnemyType::Rat);
        AllowedTypes.Add(EGWTEnemyType::Slime);
        AllowedTypes.Add(EGWTEnemyType::Skeleton);
        AllowedTypes.Add(EGWTEnemyType::DarkWizard);
    }
    // Late waves (8+): All enemy types including elite versions
    else
    {
        AllowedTypes.Add(EGWTEnemyType::Skeleton);
        AllowedTypes.Add(EGWTEnemyType::DarkWizard);
        AllowedTypes.Add(EGWTEnemyType::Mimic);
        AllowedTypes.Add(EGWTEnemyType::EliteGoblin);
        AllowedTypes.Add(EGWTEnemyType::EliteSkeleton);
        AllowedTypes.Add(EGWTEnemyType::EliteWizard);
    }
    
    // If we're on the final wave, add the boss
    if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld()))
    {
        if (GameMode->IsA(AGWTGameMode::StaticClass()))
        {
            AGWTGameMode* GWTGameMode = Cast<AGWTGameMode>(GameMode);
            if (GWTGameMode && WaveNumber == GWTGameMode->MaxWaves)
            {
                AllowedTypes.Add(EGWTEnemyType::Boss);
            }
        }
    }
    
    // Filter to only enemy types we have classes for
    TArray<EGWTEnemyType> ValidTypes;
    for (EGWTEnemyType Type : AllowedTypes)
    {
        if (EnemyClasses.Contains(Type))
        {
            ValidTypes.Add(Type);
        }
    }
    
    // If no valid types, add all available types
    if (ValidTypes.Num() == 0)
    {
        for (const auto& Pair : EnemyClasses)
        {
            ValidTypes.Add(Pair.Key);
        }
    }
    
    return ValidTypes;
}

EGWTEnemyType UGWTEnemySpawner::GetRandomEnemyType(const TArray<EGWTEnemyType>& EnemyTypes) const
{
    // Return a random enemy type from the array
    if (EnemyTypes.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, EnemyTypes.Num() - 1);
        return EnemyTypes[RandomIndex];
    }
    
    // Fallback to Goblin
    return EGWTEnemyType::Goblin;
}

FVector UGWTEnemySpawner::GetRandomSpawnPointInRoom(AGWTRoom* Room) const
{
    // Get room bounds
    FBox RoomBounds = Room->GetComponentsBoundingBox();
    
    // Shrink bounds to avoid spawning too close to walls
    FVector BoundsExtent = RoomBounds.GetExtent();
    BoundsExtent *= 0.8f; // 80% of room size
    
    // Get room center
    FVector RoomCenter = RoomBounds.GetCenter();
    
    // Generate random point within bounds
    FVector RandomPoint = RoomCenter + FVector(
        FMath::FRandRange(-BoundsExtent.X, BoundsExtent.X),
        FMath::FRandRange(-BoundsExtent.Y, BoundsExtent.Y),
        0.0f // Keep z at center height
    );
    
    // Make sure point is on navigation mesh
    FNavLocation NavLocation;
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    
    if (NavSystem && NavSystem->ProjectPointToNavigation(RandomPoint, NavLocation))
    {
        return NavLocation.Location;
    }
    
    // Fallback to original point if nav projection fails
    return RandomPoint;
}

void UGWTEnemySpawner::RegisterEnemy(AGWTEnemyCharacter* Enemy)
{
    // Add enemy to active list if valid
    if (Enemy && !ActiveEnemies.Contains(Enemy))
    {
        ActiveEnemies.Add(Enemy);
        
        // Subscribe to enemy's death event
        Enemy->OnDestroyed.AddDynamic(this, &UGWTEnemySpawner::OnEnemyDestroyed);
        
        UE_LOG(LogTemp, Verbose, TEXT("Registered enemy %s, total active: %d"), 
               *Enemy->GetName(), ActiveEnemies.Num());
    }
}

void UGWTEnemySpawner::UnregisterEnemy(AGWTEnemyCharacter* Enemy)
{
    // Remove enemy from active list
    if (Enemy && ActiveEnemies.Contains(Enemy))
    {
        ActiveEnemies.Remove(Enemy);
        
        UE_LOG(LogTemp, Verbose, TEXT("Unregistered enemy %s, total active: %d"), 
               *Enemy->GetName(), ActiveEnemies.Num());
    }
}

int32 UGWTEnemySpawner::GetActiveEnemyCount() const
{
    return ActiveEnemies.Num();
}

void UGWTEnemySpawner::CleanupDeadEnemies()
{
    // Remove any null or invalid enemies from the active list
    for (int32 i = ActiveEnemies.Num() - 1; i >= 0; i--)
    {
        if (!ActiveEnemies[i] || ActiveEnemies[i]->IsPendingKill())
        {
            ActiveEnemies.RemoveAt(i);
        }
    }
}

FGWTEnemyWaveInfo UGWTEnemySpawner::GetWaveConfiguration(int32 WaveNumber) const
{
    // Try to find a specific configuration for this wave
    for (const FGWTEnemyWaveInfo& WaveInfo : WaveConfigurations)
    {
        if (WaveInfo.WaveNumber == WaveNumber)
        {
            return WaveInfo;
        }
    }
    
    // Create a default configuration if not found
    FGWTEnemyWaveInfo DefaultConfig;
    DefaultConfig.WaveNumber = WaveNumber;
    DefaultConfig.SpawnFrequency = 5.0f;
    DefaultConfig.DifficultyMultiplier = 1.0f + (WaveNumber * 0.1f);
    
    // For enemy types, use what's available
    TArray<EGWTEnemyType> AvailableTypes = GetEnemyTypesForWave(WaveNumber);
    for (EGWTEnemyType Type : AvailableTypes)
    {
        if (EnemyClasses.Contains(Type))
        {
            DefaultConfig.EnemyTypes.Add(EnemyClasses[Type]);
        }
    }
    
    return DefaultConfig;
}

void UGWTEnemySpawner::InitializeEnemyClasses()
{
    // If no enemy classes are specified, set up defaults
    if (EnemyClasses.Num() == 0)
    {
        SetupDefaultEnemyClasses();
    }
}

void UGWTEnemySpawner::SetupDefaultEnemyClasses()
{
    // Clear any existing classes
    EnemyClasses.Empty();
    
    // Find available enemy classes
    TArray<UClass*> EnemySubclasses;
    GetDerivedClasses(AGWTEnemyCharacter::StaticClass(), EnemySubclasses);
    
    // Add all found classes
    for (UClass* Class : EnemySubclasses)
    {
        // Skip abstract classes
        if (Class->HasAnyClassFlags(CLASS_Abstract))
        {
            continue;
        }
        
        // Create a default object to get the enemy type
        AGWTEnemyCharacter* DefaultEnemy = Cast<AGWTEnemyCharacter>(Class->GetDefaultObject());
        if (DefaultEnemy)
        {
            EnemyClasses.Add(DefaultEnemy->EnemyType, Class);
            UE_LOG(LogTemp, Display, TEXT("Added enemy class %s of type %s"), 
                   *Class->GetName(), *UEnum::GetValueAsString(DefaultEnemy->EnemyType));
        }
    }
    
    // If no classes were found, create a warning
    if (EnemyClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No enemy classes found. Make sure to implement derived enemy classes."));
        
        // Add at least one entry with the base class
        EnemyClasses.Add(EGWTEnemyType::Goblin, AGWTEnemyCharacter::StaticClass());
    }
}

// Callback for enemy destroyed event
UFUNCTION()
void UGWTEnemySpawner::OnEnemyDestroyed(AActor* DestroyedActor)
{
    // Unregister the destroyed enemy
    AGWTEnemyCharacter* Enemy = Cast<AGWTEnemyCharacter>(DestroyedActor);
    if (Enemy)
    {
        UnregisterEnemy(Enemy);
    }
}