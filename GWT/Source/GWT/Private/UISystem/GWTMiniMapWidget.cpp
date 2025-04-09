// GWTMiniMapWidget.cpp
// Implementation of the minimap widget

#include "GWTMiniMapWidget.h"
#include "GWTLevelGenerator.h"
#include "GWTRoom.h"
#include "GWTPlayerCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGWTMiniMapWidget::UGWTMiniMapWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize properties
    LevelGenerator = nullptr;
    CurrentRoom = nullptr;
    
    // Set default colors
    VisitedRoomColor = FLinearColor(0.2f, 0.2f, 0.8f, 1.0f);      // Blue
    UnvisitedRoomColor = FLinearColor(0.1f, 0.1f, 0.3f, 1.0f);    // Dark blue
    CurrentRoomColor = FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);      // Red
    SpecialRoomColor = FLinearColor(0.8f, 0.8f, 0.2f, 1.0f);      // Yellow
    DoorColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);             // White
    PlayerMarkerColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);     // Green
    
    // Set default sizes
    RoomSize = 20.0f;
    MapScale = 1.0f;
    
    UE_LOG(LogTemp, Verbose, TEXT("Mini-map widget created"));
}

void UGWTMiniMapWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Initialize map overlay
    if (MapOverlay)
    {
        MapOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    
    // Initialize map border
    if (MapBorder)
    {
        MapBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f)); // Semi-transparent black
    }
    
    // Initialize player marker
    if (PlayerMarker)
    {
        PlayerMarker->SetColorAndOpacity(PlayerMarkerColor);
        PlayerMarker->SetVisibility(ESlateVisibility::Visible);
    }
    
    // Find level generator if not already set
    if (!LevelGenerator)
    {
        TArray<AActor*> FoundGenerators;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGWTLevelGenerator::StaticClass(), FoundGenerators);
        
        if (FoundGenerators.Num() > 0)
        {
            LevelGenerator = Cast<AGWTLevelGenerator>(FoundGenerators[0]);
            
            // Create room widgets once we have a level generator
            if (LevelGenerator)
            {
                CreateRoomWidgets();
            }
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Mini-map widget constructed"));
}

void UGWTMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // Update player position on the map
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UpdatePlayerPosition(PlayerPawn->GetActorLocation());
    }
}

void UGWTMiniMapWidget::UpdateMap()
{
    // Update the entire map
    if (!LevelGenerator)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot update map: No level generator"));
        return;
    }
    
    // Clear existing room widgets
    if (MapCanvas)
    {
        MapCanvas->ClearChildren();
    }
    
    // Clear room widget mapping
    RoomWidgets.Empty();
    
    // Create room widgets
    CreateRoomWidgets();
    
    UE_LOG(LogTemp, Verbose, TEXT("Mini-map updated"));
}

void UGWTMiniMapWidget::UpdatePlayerPosition(const FVector& PlayerPosition)
{
    // Update player marker position
    if (!PlayerMarker || !MapCanvas || !LevelGenerator)
    {
        return;
    }
    
    // Convert world position to grid position
    FIntVector GridPos = WorldToGridPosition(PlayerPosition);
    
    // Convert grid position to canvas position
    FVector2D CanvasPos = GridToCanvasPosition(GridPos);
    
    // Set player marker position
    UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot);
    if (MarkerSlot)
    {
        // Center marker on room
        FVector2D MarkerSize = MarkerSlot->GetSize();
        CanvasPos -= MarkerSize * 0.5f;
        
        MarkerSlot->SetPosition(CanvasPos);
    }
    
    // Update current room
    AGWTRoom* Room = LevelGenerator->GetRoom(GridPos.X, GridPos.Y, GridPos.Z);
    if (Room && Room != CurrentRoom)
    {
        CurrentRoom = Room;
        OnRoomVisited(Room);
    }
}

void UGWTMiniMapWidget::OnRoomVisited(AGWTRoom* Room)
{
    // Mark room as visited
    if (Room && !VisitedRooms.Contains(Room))
    {
        VisitedRooms.Add(Room);
        
        // Update room color
        UWidget* RoomWidget = RoomWidgets.FindRef(Room->GridPosition);
        if (RoomWidget)
        {
            UBorder* RoomBorder = Cast<UBorder>(RoomWidget);
            if (RoomBorder)
            {
                RoomBorder->SetBrushColor(GetRoomColor(Room));
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Room at (%d, %d, %d) visited and marked on mini-map"), 
               Room->GridPosition.X, Room->GridPosition.Y, Room->GridPosition.Z);
    }
}

void UGWTMiniMapWidget::SetLevelGenerator(AGWTLevelGenerator* Generator)
{
    // Set the level generator and update the map
    if (Generator)
    {
        LevelGenerator = Generator;
        UpdateMap();
        
        UE_LOG(LogTemp, Display, TEXT("Mini-map level generator set"));
    }
}

void UGWTMiniMapWidget::ResetMap()
{
    // Reset the map state
    CurrentRoom = nullptr;
    VisitedRooms.Empty();
    
    // Update the map
    UpdateMap();
    
    UE_LOG(LogTemp, Display, TEXT("Mini-map reset"));
}

void UGWTMiniMapWidget::SetMapScale(float NewScale)
{
    // Set map scale and update positions
    MapScale = FMath::Max(0.1f, NewScale);
    
    // Update map
    UpdateMap();
    
    UE_LOG(LogTemp, Verbose, TEXT("Mini-map scale set to %.2f"), MapScale);
}

void UGWTMiniMapWidget::ToggleFullscreen()
{
    // Toggle between normal and fullscreen mode
    bIsFullscreen = !bIsFullscreen;
    
    // Adjust map size and position
    if (MapBorder)
    {
        if (bIsFullscreen)
        {
            // Expand to full screen
            MapBorder->SetPadding(FMargin(50, 50, 50, 50));
        }
        else
        {
            // Return to normal size
            MapBorder->SetPadding(FMargin(5, 5, 5, 5));
        }
    }
    
    // Adjust scale
    if (bIsFullscreen)
    {
        SetMapScale(MapScale * A2.0f);
    }
    else
    {
        SetMapScale(MapScale / 2.0f);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Mini-map fullscreen mode: %s"), 
           bIsFullscreen ? TEXT("Enabled") : TEXT("Disabled"));
}

void UGWTMiniMapWidget::CreateRoomWidgets()
{
    // Create widgets for all rooms in the labyrinth
    if (!LevelGenerator || !MapCanvas)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot create room widgets: Missing references"));
        return;
    }
    
    // Get grid dimensions
    int32 GridSizeX = LevelGenerator->GridSizeX;
    int32 GridSizeY = LevelGenerator->GridSizeY;
    int32 GridSizeZ = LevelGenerator->GridSizeZ;
    
    // Create widgets for all rooms
    for (int32 X = 0; X < GridSizeX; X++)
    {
        for (int32 Y = 0; Y < GridSizeY; Y++)
        {
            for (int32 Z = 0; Z < GridSizeZ; Z++)
            {
                AGWTRoom* Room = LevelGenerator->GetRoom(X, Y, Z);
                if (Room)
                {
                    // Create room widget
                    UWidget* RoomWidget = CreateRoomWidget(Room);
                    if (RoomWidget)
                    {
                        // Add to canvas
                        MapCanvas->AddChild(RoomWidget);
                        
                        // Position on canvas
                        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RoomWidget->Slot);
                        if (CanvasSlot)
                        {
                            FVector2D CanvasPos = GridToCanvasPosition(Room->GridPosition);
                            CanvasSlot->SetPosition(CanvasPos);
                            CanvasSlot->SetSize(FVector2D(RoomSize, RoomSize));
                            CanvasSlot->SetZOrder(Room->GridPosition.Z * 10); // Higher Z is above
                        }
                        
                        // Add to mapping
                        RoomWidgets.Add(Room->GridPosition, RoomWidget);
                        
                        // Create door connections
                        for (const auto& Pair : Room->DoorStates)
                        {
                            if (Pair.Value) // If door is open
                            {
                                UWidget* DoorWidget = CreateDoorWidget(Room, Pair.Key);
                                if (DoorWidget)
                                {
                                    MapCanvas->AddChild(DoorWidget);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Created %d room widgets for mini-map"), RoomWidgets.Num());
}

UWidget* UGWTMiniMapWidget::CreateRoomWidget(AGWTRoom* Room)
{
    // Create a widget representing a room
    if (!Room)
    {
        return nullptr;
    }
    
    // Create a border widget
    UBorder* RoomBorder = NewObject<UBorder>(this);
    
    // Set appearance based on room type and state
    RoomBorder->SetBrushColor(GetRoomColor(Room));
    
    // Set border shape
    FSlateBrush Brush;
    Brush.DrawAs = ESlateBrushDrawType::Box;
    Brush.OutlineSettings.Width = 1.0f;
    RoomBorder->SetBrush(Brush);
    
    return RoomBorder;
}

UWidget* UGWTMiniMapWidget::CreateDoorWidget(AGWTRoom* Room, EGWTDirection Direction)
{
    // Create a widget representing a door connection
    if (!Room)
    {
        return nullptr;
    }
    
    // Get positions of connected rooms
    FIntVector RoomPos = Room->GridPosition;
    FVector DirVec = Room->GetDirectionVector(Direction);
    FIntVector ConnectedPos = RoomPos + FIntVector(DirVec.X, DirVec.Y, DirVec.Z);
    
    // Check if connected room exists
    if (!LevelGenerator->GetRoom(ConnectedPos.X, ConnectedPos.Y, ConnectedPos.Z))
    {
        return nullptr;
    }
    
    // Create door line
    UImage* DoorImage = NewObject<UImage>(this);
    DoorImage->SetColorAndOpacity(DoorColor);
    
    // Add to canvas
    UCanvasPanelSlot* DoorSlot = Cast<UCanvasPanelSlot>(DoorImage->Slot);
    if (DoorSlot)
    {
        // Set position and size based on direction
        FVector2D RoomCanvasPos = GridToCanvasPosition(RoomPos);
        FVector2D ConnectedCanvasPos = GridToCanvasPosition(ConnectedPos);
        
        // Calculate center points of rooms
        FVector2D RoomCenter = RoomCanvasPos + FVector2D(RoomSize / 2, RoomSize / 2);
        FVector2D ConnectedCenter = ConnectedCanvasPos + FVector2D(RoomSize / 2, RoomSize / 2);
        
        // Calculate position and size for door line
        FVector2D DoorPos = FMath::Min(RoomCenter.X, ConnectedCenter.X);
        FVector2D DoorSize = FVector2D(
            FMath::Abs(ConnectedCenter.X - RoomCenter.X),
            FMath::Abs(ConnectedCenter.Y - RoomCenter.Y)
        );
        
        // Handle special case for vertical doors (need minimum width)
        if (DoorSize.X < 2.0f)
        {
            DoorSize.X = 2.0f;
            DoorPos.X -= 1.0f;
        }
        
        // Handle special case for horizontal doors (need minimum height)
        if (DoorSize.Y < 2.0f)
        {
            DoorSize.Y = 2.0f;
            DoorPos.Y -= 1.0f;
        }
        
        DoorSlot->SetPosition(DoorPos);
        DoorSlot->SetSize(DoorSize);
        DoorSlot->SetZOrder(5 + Room->GridPosition.Z * 10); // Between rooms
    }
    
    return DoorImage;
}

FVector2D UGWTMiniMapWidget::GridToCanvasPosition(const FIntVector& GridPosition) const
{
    // Convert grid position to canvas position
    float ScaledRoomSize = RoomSize * MapScale;
    float ScaledSpacing = RoomSize * 0.5f * MapScale;
    
    // Use Z axis as depth, Y is up/down, X is left/right
    float X = GridPosition.X * (ScaledRoomSize + ScaledSpacing);
    float Y = GridPosition.Y * (ScaledRoomSize + ScaledSpacing);
    
    // Add Z offset for 3D illusion (shift down and right for higher Z)
    X += GridPosition.Z * (ScaledRoomSize * 0.25f);
    Y += GridPosition.Z * (ScaledRoomSize * 0.25f);
    
    return FVector2D(X, Y);
}

FIntVector UGWTMiniMapWidget::WorldToGridPosition(const FVector& WorldPosition) const
{
    // Convert world position to grid position
    if (!LevelGenerator)
    {
        return FIntVector(0, 0, 0);
    }
    
    // Use room size to calculate grid position
    float WorldRoomSize = LevelGenerator->RoomSize;
    
    int32 X = FMath::Floor(WorldPosition.X / WorldRoomSize);
    int32 Y = FMath::Floor(WorldPosition.Y / WorldRoomSize);
    int32 Z = FMath::Floor(WorldPosition.Z / WorldRoomSize);
    
    // Clamp to grid bounds
    X = FMath::Clamp(X, 0, LevelGenerator->GridSizeX - 1);
    Y = FMath::Clamp(Y, 0, LevelGenerator->GridSizeY - 1);
    Z = FMath::Clamp(Z, 0, LevelGenerator->GridSizeZ - 1);
    
    return FIntVector(X, Y, Z);
}

FLinearColor UGWTMiniMapWidget::GetRoomColor(AGWTRoom* Room) const
{
    // Determine room color based on type and state
    if (!Room)
    {
        return UnvisitedRoomColor;
    }
    
    // Current room is highlighted
    if (Room == CurrentRoom)
    {
        return CurrentRoomColor;
    }
    
    // Special rooms get special colors
    if (Room->RoomType == EGWTRoomType::Treasure || 
        Room->RoomType == EGWTRoomType::Shop || 
        Room->RoomType == EGWTRoomType::Boss)
    {
        // Only show special color if visited
        if (VisitedRooms.Contains(Room))
        {
            return SpecialRoomColor;
        }
    }
    
    // Visited rooms are revealed
    if (VisitedRooms.Contains(Room))
    {
        return VisitedRoomColor;
    }
    
    // Unvisited rooms
    return UnvisitedRoomColor;
}