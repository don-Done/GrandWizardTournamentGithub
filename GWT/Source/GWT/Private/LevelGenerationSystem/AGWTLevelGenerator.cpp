// GWTLevelGenerator.cpp
// Implementation of the level generator

#include "GWTLevelGenerator.h"
#include "GWTRoom.h"
#include "GWTTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGWTLevelGenerator::AGWTLevelGenerator()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = false;
    
    // Set default properties
    GridSizeX = 3;
    GridSizeY = 3;
    GridSizeZ = 3;
    RoomSize = 1000.0f;
    
    // Room count defaults
    MinCombatRooms = 3;
    MaxCombatRooms = 8;
    MinTreasureRooms = 1;
    MaxTreasureRooms = 3;
    ShopRoomChance = 0.2f;
    PuzzleRoomChance = 0.3f;
    
    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("Level Generator created"));
}

void AGWTLevelGenerator::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize the room grid
    InitializeRoomGrid();
}

void AGWTLevelGenerator::GenerateLevel(int32 WaveNumber)
{
    // Clear any existing rooms
    ClearExistingRooms();
    
    // Initialize the room grid
    InitializeRoomGrid();
    
    UE_LOG(LogTemp, Display, TEXT("Generating level for wave %d"), WaveNumber);
    
    // Determine room counts based on wave number
    int32 CombatRoomCount = FMath::Min(MinCombatRooms + WaveNumber / 2, MaxCombatRooms);
    int32 TreasureRoomCount = FMath::Min(MinTreasureRooms + WaveNumber / 3, MaxTreasureRooms);
    
    UE_LOG(LogTemp, Display, TEXT("Room counts - Combat: %d, Treasure: %d"), CombatRoomCount, TreasureRoomCount);
    
    // First, place a spawn room at the center
    int32 CenterX = GridSizeX / 2;
    int32 CenterY = GridSizeY / 2;
    int32 CenterZ = GridSizeZ / 2;
    
    TSubclassOf<AGWTRoom> EmptyRoomClass = SelectRoomTemplate(EGWTRoomType::Empty);
    AGWTRoom* SpawnRoom = SpawnRoom(EmptyRoomClass, CenterX, CenterY, CenterZ);
    if (SpawnRoom)
    {
        SpawnRoom->RoomType = EGWTRoomType::Empty;
        UE_LOG(LogTemp, Display, TEXT("Placed spawn room at (%d, %d, %d)"), CenterX, CenterY, CenterZ);
    }
    
    // Place combat rooms
    int32 PlacedCombatRooms = 0;
    while (PlacedCombatRooms < CombatRoomCount)
    {
        // Choose a random position
        int32 X = FMath::RandRange(0, GridSizeX - 1);
        int32 Y = FMath::RandRange(0, GridSizeY - 1);
        int32 Z = FMath::RandRange(0, GridSizeZ - 1);
        
        // Skip if position already has a room
        if (GetRoom(X, Y, Z) != nullptr)
            continue;
        
        // Check if position is suitable for combat room
        if (IsPositionSuitableForRoomType(X, Y, Z, EGWTRoomType::Combat))
        {
            TSubclassOf<AGWTRoom> CombatRoomClass = SelectRoomTemplate(EGWTRoomType::Combat);
            AGWTRoom* CombatRoom = SpawnRoom(CombatRoomClass, X, Y, Z);
            if (CombatRoom)
            {
                CombatRoom->RoomType = EGWTRoomType::Combat;
                PlacedCombatRooms++;
                UE_LOG(LogTemp, Verbose, TEXT("Placed combat room at (%d, %d, %d)"), X, Y, Z);
            }
        }
    }
    
    // Place treasure rooms
    int32 PlacedTreasureRooms = 0;
    while (PlacedTreasureRooms < TreasureRoomCount)
    {
        // Choose a random position
        int32 X = FMath::RandRange(0, GridSizeX - 1);
        int32 Y = FMath::RandRange(0, GridSizeY - 1);
        int32 Z = FMath::RandRange(0, GridSizeZ - 1);
        
        // Skip if position already has a room
        if (GetRoom(X, Y, Z) != nullptr)
            continue;
        
        // Check if position is suitable for treasure room
        if (IsPositionSuitableForRoomType(X, Y, Z, EGWTRoomType::Treasure))
        {
            TSubclassOf<AGWTRoom> TreasureRoomClass = SelectRoomTemplate(EGWTRoomType::Treasure);
            AGWTRoom* TreasureRoom = SpawnRoom(TreasureRoomClass, X, Y, Z);
            if (TreasureRoom)
            {
                TreasureRoom->RoomType = EGWTRoomType::Treasure;
                PlacedTreasureRooms++;
                UE_LOG(LogTemp, Verbose, TEXT("Placed treasure room at (%d, %d, %d)"), X, Y, Z);
            }
        }
    }
    
    // Place shop room (if chance permits)
    if (FMath::FRand() < ShopRoomChance)
    {
        bool bPlacedShop = false;
        int32 Attempts = 0;
        
        while (!bPlacedShop && Attempts < 10)
        {
            // Choose a random position
            int32 X = FMath::RandRange(0, GridSizeX - 1);
            int32 Y = FMath::RandRange(0, GridSizeY - 1);
            int32 Z = FMath::RandRange(0, GridSizeZ - 1);
            
            // Skip if position already has a room
            if (GetRoom(X, Y, Z) != nullptr)
            {
                Attempts++;
                continue;
            }
            
            // Check if position is suitable for shop room
            if (IsPositionSuitableForRoomType(X, Y, Z, EGWTRoomType::Shop))
            {
                TSubclassOf<AGWTRoom> ShopRoomClass = SelectRoomTemplate(EGWTRoomType::Shop);
                AGWTRoom* ShopRoom = SpawnRoom(ShopRoomClass, X, Y, Z);
                if (ShopRoom)
                {
                    ShopRoom->RoomType = EGWTRoomType::Shop;
                    bPlacedShop = true;
                    UE_LOG(LogTemp, Display, TEXT("Placed shop room at (%d, %d, %d)"), X, Y, Z);
                }
            }
            
            Attempts++;
        }
    }
    
    // Place puzzle room (if chance permits)
    if (FMath::FRand() < PuzzleRoomChance)
    {
        bool bPlacedPuzzle = false;
        int32 Attempts = 0;
        
        while (!bPlacedPuzzle && Attempts < 10)
        {
            // Choose a random position
            int32 X = FMath::RandRange(0, GridSizeX - 1);
            int32 Y = FMath::RandRange(0, GridSizeY - 1);
            int32 Z = FMath::RandRange(0, GridSizeZ - 1);
            
            // Skip if position already has a room
            if (GetRoom(X, Y, Z) != nullptr)
            {
                Attempts++;
                continue;
            }
            
            // Check if position is suitable for puzzle room
            if (IsPositionSuitableForRoomType(X, Y, Z, EGWTRoomType::Puzzle))
            {
                TSubclassOf<AGWTRoom> PuzzleRoomClass = SelectRoomTemplate(EGWTRoomType::Puzzle);
                AGWTRoom* PuzzleRoom = SpawnRoom(PuzzleRoomClass, X, Y, Z);
                if (PuzzleRoom)
                {
                    PuzzleRoom->RoomType = EGWTRoomType::Puzzle;
                    bPlacedPuzzle = true;
                    UE_LOG(LogTemp, Display, TEXT("Placed puzzle room at (%d, %d, %d)"), X, Y, Z);
                }
            }
            
            Attempts++;
        }
    }
    
    // Fill remaining empty spaces with empty rooms
    for (int32 X = 0; X < GridSizeX; X++)
    {
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            for (int32 Z = 0; Z < GridSizeZ; Z++)
            {
                if (GetRoom(X, Y, Z) == nullptr)
                {
                    TSubclassOf<AGWTRoom> EmptyRoomClass = SelectRoomTemplate(EGWTRoomType::Empty);
                    AGWTRoom* EmptyRoom = SpawnRoom(EmptyRoomClass, X, Y, Z);
                    if (EmptyRoom)
                    {
                        EmptyRoom->RoomType = EGWTRoomType::Empty;
                        UE_LOG(LogTemp, Verbose, TEXT("Placed empty room at (%d, %d, %d)"), X, Y, Z);
                    }
                }
            }
        }
    }
    
    // Connect all rooms with doors
    ConnectRooms();
    
    // Place objectives
    PlaceObjectives();
    
    UE_LOG(LogTemp, Display, TEXT("Level generation complete for wave %d"), WaveNumber);
}

void AGWTLevelGenerator::ClearExistingRooms()
{
    // Destroy all existing rooms
    for (int32 X = 0; X < RoomGrid.Num(); X++)
    {
        for (int32 Y = 0; Y < RoomGrid[X].Num(); Y++)
        {
            for (int32 Z = 0; Z < RoomGrid[X][Y].Num(); Z++)
            {
                AGWTRoom* Room = RoomGrid[X][Y][Z];
                if (Room)
                {
                    Room->Destroy();
                    RoomGrid[X][Y][Z] = nullptr;
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Cleared existing rooms"));
}

AGWTRoom* AGWTLevelGenerator::SpawnRoom(TSubclassOf<AGWTRoom> RoomClass, int32 X, int32 Y, int32 Z)
{
    // Check if position is valid
    if (!IsValidPosition(X, Y, Z))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid room position: (%d, %d, %d)"), X, Y, Z);
        return nullptr;
    }
    
    // Check if room class is valid
    if (!RoomClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid room class for position (%d, %d, %d)"), X, Y, Z);
        return nullptr;
    }
    
    // Calculate world position
    FVector RoomLocation = FVector(
        X * RoomSize,
        Y * RoomSize,
        Z * RoomSize
    );
    
    // Spawn the room
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    AGWTRoom* NewRoom = GetWorld()->SpawnActor<AGWTRoom>(
        RoomClass,
        RoomLocation,
        FRotator::ZeroRotator,
        SpawnParams
    );
    
    if (NewRoom)
    {
        // Set room properties
        NewRoom->GridPosition = FIntVector(X, Y, Z);
        
        // Add to grid
        RoomGrid[X][Y][Z] = NewRoom;
        
        UE_LOG(LogTemp, Verbose, TEXT("Spawned room at (%d, %d, %d)"), X, Y, Z);
        return NewRoom;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Failed to spawn room at (%d, %d, %d)"), X, Y, Z);
    return nullptr;
}

void AGWTLevelGenerator::ConnectRooms()
{
    // Connect adjacent rooms with doors
    for (int32 X = 0; X < GridSizeX; X++)
    {
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            for (int32 Z = 0; Z < GridSizeZ; Z++)
            {
                AGWTRoom* CurrentRoom = GetRoom(X, Y, Z);
                if (!CurrentRoom)
                    continue;
                
                // Connect to adjacent rooms
                
                // Check East (X+1)
                if (X < GridSizeX - 1)
                {
                    AGWTRoom* EastRoom = GetRoom(X + 1, Y, Z);
                    if (EastRoom)
                    {
                        // Enable East door in current room
                        CurrentRoom->EnableDoor(EGWTDirection::East, true);
                        
                        // Enable West door in adjacent room
                        EastRoom->EnableDoor(EGWTDirection::West, true);
                        
                        UE_LOG(LogTemp, Verbose, TEXT("Connected rooms at (%d,%d,%d) and (%d,%d,%d) with doors"),
                              X, Y, Z, X + 1, Y, Z);
                    }
                }
                
                // Check North (Y+1)
                if (Y < GridSizeY - 1)
                {
                    AGWTRoom* NorthRoom = GetRoom(X, Y + 1, Z);
                    if (NorthRoom)
                    {
                        // Enable North door in current room
                        CurrentRoom->EnableDoor(EGWTDirection::North, true);
                        
                        // Enable South door in adjacent room
                        NorthRoom->EnableDoor(EGWTDirection::South, true);
                        
                        UE_LOG(LogTemp, Verbose, TEXT("Connected rooms at (%d,%d,%d) and (%d,%d,%d) with doors"),
                              X, Y, Z, X, Y + 1, Z);
                    }
                }
                
                // Check Up (Z+1)
                if (Z < GridSizeZ - 1)
                {
                    AGWTRoom* UpRoom = GetRoom(X, Y, Z + 1);
                    if (UpRoom)
                    {
                        // Enable Up door in current room
                        CurrentRoom->EnableDoor(EGWTDirection::Up, true);
                        
                        // Enable Down door in adjacent room
                        UpRoom->EnableDoor(EGWTDirection::Down, true);
                        
                        UE_LOG(LogTemp, Verbose, TEXT("Connected rooms at (%d,%d,%d) and (%d,%d,%d) with doors"),
                              X, Y, Z, X, Y, Z + 1);
                    }
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Connected all rooms with doors"));
}

void AGWTLevelGenerator::PlaceObjectives()
{
    // This would typically be implemented in a more complex way
    // For this example, we'll just log that objectives would be placed
    
    UE_LOG(LogTemp, Display, TEXT("Objectives would be placed here"));
    
    // In a full implementation, this would:
    // 1. Place boss in a strategic location
    // 2. Place keys/tokens in specific rooms
    // 3. Set up puzzle requirements
    // 4. Place special collectibles
}

void AGWTLevelGenerator::SwapCubes(int32 X1, int32 Y1, int32 Z1, int32 X2, int32 Y2, int32 Z2)
{
    // Check if both positions are valid
    if (!IsValidPosition(X1, Y1, Z1) || !IsValidPosition(X2, Y2, Z2))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid positions for cube swap: (%d,%d,%d) and (%d,%d,%d)"),
              X1, Y1, Z1, X2, Y2, Z2);
        return;
    }
    
    // Get rooms at both positions
    AGWTRoom* Room1 = GetRoom(X1, Y1, Z1);
    AGWTRoom* Room2 = GetRoom(X2, Y2, Z2);
    
    // Check if both rooms exist
    if (!Room1 || !Room2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing rooms for cube swap"));
        return;
    }
    
    // Swap positions in grid
    RoomGrid[X1][Y1][Z1] = Room2;
    RoomGrid[X2][Y2][Z2] = Room1;
    
    // Update rooms' grid positions
    Room1->GridPosition = FIntVector(X2, Y2, Z2);
    Room2->GridPosition = FIntVector(X1, Y1, Z1);
    
    // Calculate world positions
    FVector Position1 = FVector(X1 * RoomSize, Y1 * RoomSize, Z1 * RoomSize);
    FVector Position2 = FVector(X2 * RoomSize, Y2 * RoomSize, Z2 * RoomSize);
    
    // Update actor positions
    Room1->SetActorLocation(Position2);
    Room2->SetActorLocation(Position1);
    
    // Reconnect doors
    ConnectRooms();
    
    UE_LOG(LogTemp, Display, TEXT("Swapped cubes at positions (%d,%d,%d) and (%d,%d,%d)"),
          X1, Y1, Z1, X2, Y2, Z2);
}

void AGWTLevelGenerator::RotatePlane(EGWTPlaneType Plane, int32 Index, float Angle)
{
    // In a full implementation, this would rotate all rooms in a plane
    // This is a simplified version that just logs the operation
    
    UE_LOG(LogTemp, Display, TEXT("Would rotate plane %d at index %d by angle %f"),
          (int32)Plane, Index, Angle);
    
    // A complete implementation would:
    // 1. Select all rooms in the specified plane
    // 2. Calculate new positions based on rotation
    // 3. Update room positions and grid references
    // 4. Reconnect doors
}

TSubclassOf<AGWTRoom> AGWTLevelGenerator::SelectRoomTemplate(EGWTRoomType RoomType)
{
    // Select a room template based on room type
    switch (RoomType)
    {
        case EGWTRoomType::Combat:
            if (CombatRoomTemplates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, CombatRoomTemplates.Num() - 1);
                return CombatRoomTemplates[Index];
            }
            break;
            
        case EGWTRoomType::Treasure:
            if (TreasureRoomTemplates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, TreasureRoomTemplates.Num() - 1);
                return TreasureRoomTemplates[Index];
            }
            break;
            
        case EGWTRoomType::Shop:
            if (ShopRoomTemplates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, ShopRoomTemplates.Num() - 1);
                return ShopRoomTemplates[Index];
            }
            break;
            
        case EGWTRoomType::Puzzle:
            if (PuzzleRoomTemplates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, PuzzleRoomTemplates.Num() - 1);
                return PuzzleRoomTemplates[Index];
            }
            break;
            
        case EGWTRoomType::Empty:
        default:
            if (EmptyRoomTemplates.Num() > 0)
            {
                int32 Index = FMath::RandRange(0, EmptyRoomTemplates.Num() - 1);
                return EmptyRoomTemplates[Index];
            }
            break;
    }
    
    // Fallback to first empty room template or nullptr
    return EmptyRoomTemplates.Num() > 0 ? EmptyRoomTemplates[0] : nullptr;
}

EGWTRoomType AGWTLevelGenerator::GetRoomTypeForPosition(int32 X, int32 Y, int32 Z, int32 WaveNumber)
{
    // This would implement more complex logic to determine appropriate room types
    // For this example, we'll return a basic distribution
    
    // Center is always empty (spawn room)
    if (X == GridSizeX / 2 && Y == GridSizeY / 2 && Z == GridSizeZ / 2)
    {
        return EGWTRoomType::Empty;
    }
    
    // Random distribution
    float Random = FMath::FRand();
    
    if (Random < 0.4f)
    {
        return EGWTRoomType::Combat;
    }
    else if (Random < 0.6f)
    {
        return EGWTRoomType::Treasure;
    }
    else if (Random < 0.7f)
    {
        return EGWTRoomType::Shop;
    }
    else if (Random < 0.8f)
    {
        return EGWTRoomType::Puzzle;
    }
    else
    {
        return EGWTRoomType::Empty;
    }
}

AGWTRoom* AGWTLevelGenerator::GetRoom(int32 X, int32 Y, int32 Z) const
{
    // Check if position is valid
    if (!IsValidPosition(X, Y, Z))
    {
        return nullptr;
    }
    
    return RoomGrid[X][Y][Z];
}

FIntVector AGWTLevelGenerator::GetSpawnRoomPosition() const
{
    // Spawn room is at the center of the grid
    return FIntVector(GridSizeX / 2, GridSizeY / 2, GridSizeZ / 2);
}

AGWTRoom* AGWTLevelGenerator::GetSpawnRoom() const
{
    // Get the spawn room (center of grid)
    FIntVector SpawnPos = GetSpawnRoomPosition();
    return GetRoom(SpawnPos.X, SpawnPos.Y, SpawnPos.Z);
}

void AGWTLevelGenerator::InitializeRoomGrid()
{
    // Resize grid to match dimensions
    RoomGrid.SetNum(GridSizeX);
    for (int32 X = 0; X < GridSizeX; X++)
    {
        RoomGrid[X].SetNum(GridSizeY);
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            RoomGrid[X][Y].SetNum(GridSizeZ);
            for (int32 Z = 0; Z < GridSizeZ; Z++)
            {
                RoomGrid[X][Y][Z] = nullptr;
            }
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Initialized room grid: %d x %d x %d"), GridSizeX, GridSizeY, GridSizeZ);
}

bool AGWTLevelGenerator::IsValidPosition(int32 X, int32 Y, int32 Z) const
{
    // Check if position is within grid bounds
    return X >= 0 && X < GridSizeX && 
           Y >= 0 && Y < GridSizeY && 
           Z >= 0 && Z < GridSizeZ;
}

bool AGWTLevelGenerator::IsPositionSuitableForRoomType(int32 X, int32 Y, int32 Z, EGWTRoomType RoomType) const
{
    // This would implement more complex logic based on room type and neighboring rooms
    // For this example, we'll use a simplified version
    
    // Check if position is valid and empty
    if (!IsValidPosition(X, Y, Z) || GetRoom(X, Y, Z) != nullptr)
    {
        return false;
    }
    
    // Check adjacent rooms for compatibility
    TArray<FIntVector> AdjacentPositions = GetAdjacentRoomPositions(X, Y, Z);
    
    // For this example, we'll implement a few basic rules:
    
    // Treasure rooms shouldn't be adjacent to other treasure rooms
    if (RoomType == EGWTRoomType::Treasure)
    {
        for (const FIntVector& Pos : AdjacentPositions)
        {
            AGWTRoom* AdjacentRoom = GetRoom(Pos.X, Pos.Y, Pos.Z);
            if (AdjacentRoom && AdjacentRoom->RoomType == EGWTRoomType::Treasure)
            {
                return false;
            }
        }
    }
    
    // Shop rooms should have at least one adjacent empty or combat room
    if (RoomType == EGWTRoomType::Shop)
    {
        bool bHasCompatibleNeighbor = false;
        for (const FIntVector& Pos : AdjacentPositions)
        {
            AGWTRoom* AdjacentRoom = GetRoom(Pos.X, Pos.Y, Pos.Z);
            if (AdjacentRoom && (AdjacentRoom->RoomType == EGWTRoomType::Empty || 
                                AdjacentRoom->RoomType == EGWTRoomType::Combat))
            {
                bHasCompatibleNeighbor = true;
                break;
            }
        }
        
        if (!bHasCompatibleNeighbor && AdjacentPositions.Num() > 0)
        {
            return false;
        }
    }
    
    // Default to suitable
    return true;
}

TArray<FIntVector> AGWTLevelGenerator::GetAdjacentRoomPositions(int32 X, int32 Y, int32 Z) const
{
    TArray<FIntVector> AdjacentPositions;
    
    // Check all six directions
    const FIntVector Directions[] = {
        FIntVector(1, 0, 0),  // East
        FIntVector(-1, 0, 0), // West
        FIntVector(0, 1, 0),  // North
        FIntVector(0, -1, 0), // South
        FIntVector(0, 0, 1),  // Up
        FIntVector(0, 0, -1)  // Down
    };
    
    for (const FIntVector& Dir : Directions)
    {
        int32 NewX = X + Dir.X;
        int32 NewY = Y + Dir.Y;
        int32 NewZ = Z + Dir.Z;
        
        if (IsValidPosition(NewX, NewY, NewZ))
        {
            AdjacentPositions.Add(FIntVector(NewX, NewY, NewZ));
        }
    }
    
    return AdjacentPositions;
}