// AGWTLevelGenerator.h
// Generates the procedural labyrinth for each wave

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWTTypes.h"
#include "AGWTLevelGenerator.generated.h"

// Forward declarations
class AGWTRoom;

/**
 * Level generator that creates the procedural labyrinth
 * Handles creation, connection, and manipulation of rooms
 * Implements the 3D grid system with cube swapping and rotation
 */
UCLASS()
class GWT_API AGWTLevelGenerator : public AActor
{
    GENERATED_BODY()

public:
    AGWTLevelGenerator();

    // Grid dimensions
    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 GridSizeX = 3;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 GridSizeY = 3;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 GridSizeZ = 3;

    // Room size
    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    float RoomSize = 1000.0f;

    // Room templates
    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<TSubclassOf<AGWTRoom>> CombatRoomTemplates;

    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<TSubclassOf<AGWTRoom>> TreasureRoomTemplates;

    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<TSubclassOf<AGWTRoom>> ShopRoomTemplates;

    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<TSubclassOf<AGWTRoom>> PuzzleRoomTemplates;

    UPROPERTY(EditDefaultsOnly, Category = "Rooms")
    TArray<TSubclassOf<AGWTRoom>> EmptyRoomTemplates;

    // Level configuration
    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 MinCombatRooms = 3;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 MaxCombatRooms = 8;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 MinTreasureRooms = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    int32 MaxTreasureRooms = 3;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    float ShopRoomChance = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = "Generation")
    float PuzzleRoomChance = 0.3f;

    // Room grid storage
    UPROPERTY()
    TArray<TArray<TArray<AGWTRoom*>>> RoomGrid;

    // Methods
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Generation")
    void GenerateLevel(int32 WaveNumber);

    UFUNCTION(BlueprintCallable, Category = "Generation")
    void ClearExistingRooms();

    UFUNCTION(BlueprintCallable, Category = "Rooms")
    AGWTRoom* SpawnRoom(TSubclassOf<AGWTRoom> RoomClass, int32 X, int32 Y, int32 Z);

    UFUNCTION(BlueprintCallable, Category = "Rooms")
    void ConnectRooms();

    UFUNCTION(BlueprintCallable, Category = "Rooms")
    void PlaceObjectives();

    UFUNCTION(BlueprintCallable, Category = "Manipulation")
    void SwapCubes(int32 X1, int32 Y1, int32 Z1, int32 X2, int32 Y2, int32 Z2);

    UFUNCTION(BlueprintCallable, Category = "Manipulation")
    void RotatePlane(EGWTPlaneType Plane, int32 Index, float Angle);

    // Helper methods
    UFUNCTION(BlueprintCallable, Category = "Generation")
    TSubclassOf<AGWTRoom> SelectRoomTemplate(EGWTRoomType RoomType);

    UFUNCTION(BlueprintCallable, Category = "Generation")
    EGWTRoomType GetRoomTypeForPosition(int32 X, int32 Y, int32 Z, int32 WaveNumber);

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    AGWTRoom* GetRoom(int32 X, int32 Y, int32 Z) const;

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    FIntVector GetSpawnRoomPosition() const;

    UFUNCTION(BlueprintCallable, Category = "Navigation")
    AGWTRoom* GetSpawnRoom() const;

protected:
    // Initialize the room grid
    void InitializeRoomGrid();

    // Check if position is valid
    bool IsValidPosition(int32 X, int32 Y, int32 Z) const;

    // Room placement constraints
    bool IsPositionSuitableForRoomType(int32 X, int32 Y, int32 Z, EGWTRoomType RoomType) const;

    // Get adjacent room positions
    TArray<FIntVector> GetAdjacentRoomPositions(int32 X, int32 Y, int32 Z) const;
};