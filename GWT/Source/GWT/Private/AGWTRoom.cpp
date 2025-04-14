// AGWTRoom.cpp
// Implementation of a single room in the labyrinth

#include "GWTRoom.h"
#include "GWTCharacter.h"
#include "GWTEnemyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"

AGWTRoom::AGWTRoom()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Create root component
    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(RootComp);

    // Create floor mesh
    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    FloorMesh->SetupAttachment(RootComponent);

    // Create ceiling mesh
    CeilingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilingMesh"));
    CeilingMesh->SetupAttachment(RootComponent);

    // Create spawn points
    EnemySpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EnemySpawnPoint"));
    EnemySpawnPoint->SetupAttachment(RootComponent);

    ItemSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnPoint"));
    ItemSpawnPoint->SetupAttachment(RootComponent);

    // Default room properties
    RoomType = EGWTRoomType::Empty;
    MaxEnemies = 5;
    MaxItems = 3;
    GridPosition = FIntVector::ZeroValue;

    // Log initialization
    UE_LOG(LogTemp, Verbose, TEXT("Room created"));
}

void AGWTRoom::BeginPlay()
{
    Super::BeginPlay();

    // Set up room components
    SetupRoomComponents();

    // Initialize door states
    SetupDoors();

    // Initialize room state
    InitializeRoomState();

    // Register overlap events for player detection
    UBoxComponent* TriggerBox = FindComponentByClass<UBoxComponent>();
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGWTRoom::OnRoomBeginOverlap);
        TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AGWTRoom::OnRoomEndOverlap);
    }

    UE_LOG(LogTemp, Verbose, TEXT("Room initialized at grid position (%d, %d, %d)"),
        GridPosition.X, GridPosition.Y, GridPosition.Z);
}

void AGWTRoom::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Room-specific tick logic
    // (Empty for base implementation)
}

void AGWTRoom::EnableDoor(EGWTDirection Direction, bool bEnabled)
{
    // Store door state
    DoorStates.Add(Direction, bEnabled);

    // Get door component index
    int32 DoorIndex = GetDoorIndex(Direction);

    // Ensure door components array is big enough
    while (DoorComponents.Num() <= DoorIndex)
    {
        UChildActorComponent* NewDoorComponent = NewObject<UChildActorComponent>(this);
        NewDoorComponent->RegisterComponent();
        NewDoorComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        DoorComponents.Add(NewDoorComponent);
    }

    // Get door component
    UChildActorComponent* DoorComponent = DoorComponents[DoorIndex];

    // Set door visibility based on enabled state
    if (DoorComponent)
    {
        DoorComponent->SetVisibility(bEnabled);

        // Set door transform
        DoorComponent->SetRelativeTransform(GetDoorTransform(Direction));

        UE_LOG(LogTemp, Verbose, TEXT("Door %d (%s) enabled: %s"),
            DoorIndex, *UEnum::GetValueAsString(Direction), bEnabled ? TEXT("true") : TEXT("false"));
    }

    // Update room visual appearance
    UpdateRoomAppearance();
}

bool AGWTRoom::HasDoor(EGWTDirection Direction) const
{
    // Check if door is enabled
    return DoorStates.Contains(Direction) && DoorStates[Direction];
}

void AGWTRoom::SpawnEnemies(int32 WaveNumber)
{
    // Don't spawn enemies in non-combat rooms
    if (RoomType != EGWTRoomType::Combat && RoomType != EGWTRoomType::Boss)
    {
        return;
    }

    // Calculate number of enemies based on wave number and room type
    int32 EnemyCount = FMath::Min(MaxEnemies, 2 + WaveNumber / 2);

    if (RoomType == EGWTRoomType::Boss)
    {
        // Boss rooms have fewer, stronger enemies
        EnemyCount = FMath::Min(MaxEnemies / 2, 1 + WaveNumber / 4);
    }

    UE_LOG(LogTemp, Display, TEXT("Spawning %d enemies in room (%d, %d, %d)"),
        EnemyCount, GridPosition.X, GridPosition.Y, GridPosition.Z);

    // In a real implementation, this would use an enemy spawner to create appropriate enemies
    // For this example, we'll just log that enemies would be spawned

    for (int32 i = 0; i < EnemyCount; i++)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Would spawn enemy %d of %d at %s"),
            i + 1, EnemyCount, *EnemySpawnPoint->GetComponentLocation().ToString());
    }
}

void AGWTRoom::SpawnItems(int32 WaveNumber)
{
    // Don't spawn items in combat rooms until cleared
    if ((RoomType == EGWTRoomType::Combat || RoomType == EGWTRoomType::Boss) && !bIsCleared)
    {
        return;
    }

    // Calculate number of items based on room type
    int32 ItemCount = 0;

    switch (RoomType)
    {
    case EGWTRoomType::Treasure:
        ItemCount = FMath::Min(MaxItems, 2 + WaveNumber / 3);
        break;

    case EGWTRoomType::Shop:
        ItemCount = FMath::Min(MaxItems, 3 + WaveNumber / 2);
        break;

    case EGWTRoomType::Combat:
        ItemCount = FMath::RandRange(0, 2); // Few items in combat rooms
        break;

    case EGWTRoomType::Boss:
        ItemCount = FMath::RandRange(2, 4); // More items in boss rooms
        break;

    case EGWTRoomType::Puzzle:
        ItemCount = FMath::RandRange(1, 2); // Some items in puzzle rooms
        break;

    case EGWTRoomType::Empty:
        ItemCount = FMath::RandRange(0, 1); // Rare items in empty rooms
        break;
    }

    UE_LOG(LogTemp, Display, TEXT("Spawning %d items in room (%d, %d, %d)"),
        ItemCount, GridPosition.X, GridPosition.Y, GridPosition.Z);

    // In a real implementation, this would use an item spawner to create appropriate items
    // For this example, we'll just log that items would be spawned

    for (int32 i = 0; i < ItemCount; i++)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Would spawn item %d of %d at %s"),
            i + 1, ItemCount, *ItemSpawnPoint->GetComponentLocation().ToString());
    }
}

void AGWTRoom::OnPlayerEntered(AGWTCharacter* Player)
{
    if (!Player)
        return;

    UE_LOG(LogTemp, Display, TEXT("Player entered room (%d, %d, %d) of type %s"),
        GridPosition.X, GridPosition.Y, GridPosition.Z, *UEnum::GetValueAsString(RoomType));

    // Mark room as visited
    bHasBeenVisited = true;

    // Spawn enemies if not cleared yet
    if (!bIsCleared && (RoomType == EGWTRoomType::Combat || RoomType == EGWTRoomType::Boss))
    {
        // Get wave number from game mode
        int32 WaveNumber = 1; // Default
        AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
        if (GameMode && GameMode->IsA(AGWTGameMode::StaticClass()))
        {
            WaveNumber = Cast<AGWTGameMode>(GameMode)->CurrentWave;
        }

        // Spawn enemies
        SpawnEnemies(WaveNumber);
    }

    // Update room appearance
    UpdateRoomAppearance();
}

void AGWTRoom::OnPlayerExited(AGWTCharacter* Player)
{
    if (!Player)
        return;

    UE_LOG(LogTemp, Display, TEXT("Player exited room (%d, %d, %d)"),
        GridPosition.X, GridPosition.Y, GridPosition.Z);

    // Update room appearance
    UpdateRoomAppearance();
}

void AGWTRoom::MarkAsCleared()
{
    // Mark room as cleared
    bIsCleared = true;

    UE_LOG(LogTemp, Display, TEXT("Room (%d, %d, %d) marked as cleared"),
        GridPosition.X, GridPosition.Y, GridPosition.Z);

    // Spawn items now that room is cleared
    int32 WaveNumber = 1; // Default
    AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
    if (GameMode && GameMode->IsA(AGWTGameMode::StaticClass()))
    {
        WaveNumber = Cast<AGWTGameMode>(GameMode)->CurrentWave;
    }

    SpawnItems(WaveNumber);

    // Update room appearance
    UpdateRoomAppearance();
}

int32 AGWTRoom::GetDoorIndex(EGWTDirection Direction) const
{
    // Convert direction to index
    switch (Direction)
    {
    case EGWTDirection::North: return 0;
    case EGWTDirection::East: return 1;
    case EGWTDirection::South: return 2;
    case EGWTDirection::West: return 3;
    case EGWTDirection::Up: return 4;
    case EGWTDirection::Down: return 5;
    default: return -1;
    }
}

FVector AGWTRoom::GetDirectionVector(EGWTDirection Direction) const
{
    // Convert direction to vector
    switch (Direction)
    {
    case EGWTDirection::North: return FVector(0.0f, 1.0f, 0.0f);
    case EGWTDirection::East: return FVector(1.0f, 0.0f, 0.0f);
    case EGWTDirection::South: return FVector(0.0f, -1.0f, 0.0f);
    case EGWTDirection::West: return FVector(-1.0f, 0.0f, 0.0f);
    case EGWTDirection::Up: return FVector(0.0f, 0.0f, 1.0f);
    case EGWTDirection::Down: return FVector(0.0f, 0.0f, -1.0f);
    default: return FVector::ZeroVector;
    }
}

FTransform AGWTRoom::GetDoorTransform(EGWTDirection Direction) const
{
    // Get room size from actor bounds
    FVector RoomExtent = GetComponentsBoundingBox().GetExtent();
    float RoomSize = FMath::Min(RoomExtent.X, RoomExtent.Y) * 2.0f;

    // Position based on direction
    FVector DoorPosition = GetDirectionVector(Direction) * RoomSize * 0.5f;

    // Rotation based on direction
    FRotator DoorRotation = FRotator::ZeroRotator;

    switch (Direction)
    {
    case EGWTDirection::North:
        DoorRotation = FRotator(0.0f, 0.0f, 0.0f);
        break;

    case EGWTDirection::East:
        DoorRotation = FRotator(0.0f, 90.0f, 0.0f);
        break;

    case EGWTDirection::South:
        DoorRotation = FRotator(0.0f, 180.0f, 0.0f);
        break;

    case EGWTDirection::West:
        DoorRotation = FRotator(0.0f, 270.0f, 0.0f);
        break;

    case EGWTDirection::Up:
        DoorRotation = FRotator(-90.0f, 0.0f, 0.0f);
        break;

    case EGWTDirection::Down:
        DoorRotation = FRotator(90.0f, 0.0f, 0.0f);
        break;
    }

    return FTransform(DoorRotation, DoorPosition);
}

void AGWTRoom::SetupRoomComponents()
{
    // Create a trigger box for player detection if it doesn't exist
    UBoxComponent* TriggerBox = FindComponentByClass<UBoxComponent>();
    if (!TriggerBox)
    {
        TriggerBox = NewObject<UBoxComponent>(this, TEXT("TriggerBox"));
        TriggerBox->RegisterComponent();
        TriggerBox->SetupAttachment(RootComponent);
        TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
        TriggerBox->SetGenerateOverlapEvents(true);

        // Set box size based on the room size
        FVector RoomExtent = GetComponentsBoundingBox().GetExtent();
        if (!RoomExtent.IsZero())
        {
            TriggerBox->SetBoxExtent(RoomExtent * 0.9f); // Slightly smaller than room
        }
        else
        {
            TriggerBox->SetBoxExtent(FVector(400.0f, 400.0f, 200.0f)); // Default size
        }
    }

    // Create wall meshes if needed
    if (WallMeshes.Num() < 4)
    {
        const FString WallNames[] = { TEXT("NorthWall"), TEXT("EastWall"), TEXT("SouthWall"), TEXT("WestWall") };

        // Create missing walls
        while (WallMeshes.Num() < 4)
        {
            int32 Index = WallMeshes.Num();
            UStaticMeshComponent* WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(*WallNames[Index]);
            WallMesh->SetupAttachment(RootComponent);
            WallMeshes.Add(WallMesh);
        }
    }

    // Position enemy spawn point if it exists
    if (EnemySpawnPoint)
    {
        // Center of room, slightly raised
        EnemySpawnPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    }

    // Position item spawn point if it exists
    if (ItemSpawnPoint)
    {
        // Offset from center
        ItemSpawnPoint->SetRelativeLocation(FVector(100.0f, 100.0f, 50.0f));
    }
}

void AGWTRoom::SetupDoors()
{
    // Initialize door states to closed
    DoorStates.Add(EGWTDirection::North, false);
    DoorStates.Add(EGWTDirection::East, false);
    DoorStates.Add(EGWTDirection::South, false);
    DoorStates.Add(EGWTDirection::West, false);
    DoorStates.Add(EGWTDirection::Up, false);
    DoorStates.Add(EGWTDirection::Down, false);

    // Create door components if needed
    for (int32 i = 0; i < 6; i++) // One for each direction
    {
        if (i >= DoorComponents.Num())
        {
            UChildActorComponent* DoorComponent = NewObject<UChildActorComponent>(this);
            DoorComponent->RegisterComponent();
            DoorComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            DoorComponent->SetVisibility(false); // Start with door closed

            DoorComponents.Add(DoorComponent);
        }
    }
}

void AGWTRoom::InitializeRoomState()
{
    // Initialize room state variables
    bHasBeenVisited = false;
    bIsCleared = false;

    // Room-specific initialization based on type
    switch (RoomType)
    {
    case EGWTRoomType::Empty:
        // Empty rooms are automatically cleared
        bIsCleared = true;
        break;

    case EGWTRoomType::Combat:
        // Combat rooms need to be cleared by defeating enemies
        bIsCleared = false;
        break;

    case EGWTRoomType::Treasure:
        // Treasure rooms are automatically cleared
        bIsCleared = true;
        break;

    case EGWTRoomType::Shop:
        // Shop rooms are automatically cleared
        bIsCleared = true;
        break;

    case EGWTRoomType::Puzzle:
        // Puzzle rooms need to be cleared by solving the puzzle
        bIsCleared = false;
        break;

    case EGWTRoomType::Boss:
        // Boss rooms need to be cleared by defeating the boss
        bIsCleared = false;
        break;
    }
}

void AGWTRoom::OnRoomBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the overlapping actor is a player
    AGWTCharacter* Player = Cast<AGWTCharacter>(OtherActor);
    if (Player)
    {
        OnPlayerEntered(Player);
    }
}

void AGWTRoom::OnRoomEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Check if the overlapping actor is a player
    AGWTCharacter* Player = Cast<AGWTCharacter>(OtherActor);
    if (Player)
    {
        OnPlayerExited(Player);
    }
}