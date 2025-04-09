// GWTRoom.h
// Represents a single room in the labyrinth

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWTTypes.h"
#include "GWTRoom.generated.h"

// Forward declarations
class UStaticMeshComponent;
class UChildActorComponent;
class AGWTCharacter;

/**
 * Room class that represents a single cell in the labyrinth
 * Handles doors, enemy spawning, and player interaction
 * Rooms are connected in a 3D grid to form the complete labyrinth
 */
UCLASS()
class GWT_API AGWTRoom : public AActor
{
    GENERATED_BODY()
    
public:
    AGWTRoom();
    
    // Room properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Room")
    EGWTRoomType RoomType;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Room")
    int32 MaxEnemies = 5;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Room")
    int32 MaxItems = 3;
    
    // Room visuals
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* FloorMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* CeilingMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TArray<UStaticMeshComponent*> WallMeshes;
    
    // Door components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TArray<UChildActorComponent*> DoorComponents;
    
    // Gameplay components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* EnemySpawnPoint;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* ItemSpawnPoint;
    
    // Tracking
    UPROPERTY(BlueprintReadOnly, Category="State")
    bool bHasBeenVisited = false;
    
    UPROPERTY(BlueprintReadOnly, Category="State")
    bool bIsCleared = false;
    
    // Grid position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Position")
    FIntVector GridPosition;
    
    // Door states
    UPROPERTY(BlueprintReadOnly, Category="Doors")
    TMap<EGWTDirection, bool> DoorStates;
    
    // Methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category="Doors")
    void EnableDoor(EGWTDirection Direction, bool bEnabled = true);
    
    UFUNCTION(BlueprintCallable, Category="Doors")
    bool HasDoor(EGWTDirection Direction) const;
    
    UFUNCTION(BlueprintCallable, Category="Spawning")
    void SpawnEnemies(int32 WaveNumber);
    
    UFUNCTION(BlueprintCallable, Category="Spawning")
    void SpawnItems(int32 WaveNumber);
    
    UFUNCTION(BlueprintCallable, Category="Interaction")
    void OnPlayerEntered(AGWTCharacter* Player);
    
    UFUNCTION(BlueprintCallable, Category="Interaction")
    void OnPlayerExited(AGWTCharacter* Player);
    
    UFUNCTION(BlueprintCallable, Category="State")
    void MarkAsCleared();
    
    UFUNCTION(BlueprintImplementableEvent, Category="Visualization")
    void UpdateRoomAppearance();
    
    // Helper methods
    UFUNCTION(BlueprintCallable, Category="Doors")
    int32 GetDoorIndex(EGWTDirection Direction) const;
    
    UFUNCTION(BlueprintCallable, Category="Navigation")
    FVector GetDirectionVector(EGWTDirection Direction) const;
    
    UFUNCTION(BlueprintCallable, Category="Navigation")
    FTransform GetDoorTransform(EGWTDirection Direction) const;
    
protected:
    // Setup methods
    void SetupRoomComponents();
    void SetupDoors();
    void InitializeRoomState();
    
    // Event handlers
    UFUNCTION()
    void OnRoomBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                             bool bFromSweep, const FHitResult& SweepResult);
    
    UFUNCTION()
    void OnRoomEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};