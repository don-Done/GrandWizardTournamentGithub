// UGWTEnemySpawner.h
// Manages enemy spawning

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GWTTypes.h"
#include "UGWTEnemySpawner.generated.h"

// Forward declarations
class AGWTRoom;
class AGWTEnemyCharacter;

/**
 * Enemy spawner component for Grand Wizard Tournament
 * Handles spawning enemies in rooms based on wave number and difficulty
 * Manages enemy populations and distributions
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GWT_API UGWTEnemySpawner : public UActorComponent
{
    GENERATED_BODY()

public:
    UGWTEnemySpawner();

    // Spawn settings
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TArray<FGWTEnemyWaveInfo> WaveConfigurations;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    float MinSpawnDistance = 500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    float MaxEnemiesPerRoom = 5;

    // Enemy classes
    UPROPERTY(EditDefaultsOnly, Category = "Enemies")
    TMap<EGWTEnemyType, TSubclassOf<AGWTEnemyCharacter>> EnemyClasses;

    // Global enemy cap
    UPROPERTY(EditDefaultsOnly, Category = "Limits")
    int32 MaxConcurrentEnemies = 20;

    // Active enemies
    UPROPERTY(BlueprintReadOnly, Category = "Tracking")
    TArray<AGWTEnemyCharacter*> ActiveEnemies;

    // Methods
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemiesForWave(int32 WaveNumber, AGWTRoom* Room);

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AGWTEnemyCharacter* SpawnEnemy(TSubclassOf<AGWTEnemyCharacter> EnemyClass, FVector Location, int32 WaveNumber);

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    TSubclassOf<AGWTEnemyCharacter> SelectEnemyTypeForWave(int32 WaveNumber);

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    int32 CalculateEnemyCountForRoom(AGWTRoom* Room, int32 WaveNumber);

    // Enemy type selection
    UFUNCTION(BlueprintCallable, Category = "Enemies")
    TArray<EGWTEnemyType> GetEnemyTypesForWave(int32 WaveNumber) const;

    UFUNCTION(BlueprintCallable, Category = "Enemies")
    EGWTEnemyType GetRandomEnemyType(const TArray<EGWTEnemyType>& EnemyTypes) const;

    // Spawn point calculation
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector GetRandomSpawnPointInRoom(AGWTRoom* Room) const;

    // Enemy tracking
    UFUNCTION(BlueprintCallable, Category = "Tracking")
    void RegisterEnemy(AGWTEnemyCharacter* Enemy);

    UFUNCTION(BlueprintCallable, Category = "Tracking")
    void UnregisterEnemy(AGWTEnemyCharacter* Enemy);

    UFUNCTION(BlueprintCallable, Category = "Tracking")
    int32 GetActiveEnemyCount() const;

    UFUNCTION(BlueprintCallable, Category = "Tracking")
    void CleanupDeadEnemies();

    // Wave-specific configuration
    UFUNCTION(BlueprintCallable, Category = "Configuration")
    FGWTEnemyWaveInfo GetWaveConfiguration(int32 WaveNumber) const;

protected:
    // Initialize enemy classes
    void InitializeEnemyClasses();

    // Enemy classes when none are specified in editor
    void SetupDefaultEnemyClasses();
};