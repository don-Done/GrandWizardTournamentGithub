// GWTMiniMapWidget.h
// Displays labyrinth map for navigation

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GWTTypes.h"
#include "GWTMiniMapWidget.generated.h"

// Forward declarations
class UImage;
class UCanvasPanel;
class UCanvasPanelSlot;
class UBorder;
class UOverlay;
class AGWTRoom;
class AGWTLevelGenerator;

/**
 * Mini-map widget for Grand Wizard Tournament
 * Displays a top-down view of the labyrinth for navigation
 * Shows room positions, connections, and player location
 */
UCLASS()
class GWT_API UGWTMiniMapWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UGWTMiniMapWidget(const FObjectInitializer& ObjectInitializer);
    
    // UI elements
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCanvasPanel* MapCanvas;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UOverlay* MapOverlay;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* MapBorder;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* PlayerMarker;
    
    // Map properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    float RoomSize = 20.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    float MapScale = 1.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor VisitedRoomColor = FLinearColor(0.2f, 0.2f, 0.8f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor UnvisitedRoomColor = FLinearColor(0.1f, 0.1f, 0.3f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor CurrentRoomColor = FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor SpecialRoomColor = FLinearColor(0.8f, 0.8f, 0.2f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor DoorColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    FLinearColor PlayerMarkerColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    
    // Level generator reference
    UPROPERTY(BlueprintReadWrite, Category="References")
    AGWTLevelGenerator* LevelGenerator;
    
    // Room widgets mapping
    UPROPERTY()
    TMap<FIntVector, UWidget*> RoomWidgets;
    
    // Methods
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void UpdateMap();
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void UpdatePlayerPosition(const FVector& PlayerPosition);
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void OnRoomVisited(AGWTRoom* Room);
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void SetLevelGenerator(AGWTLevelGenerator* Generator);
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void ResetMap();
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void SetMapScale(float NewScale);
    
    UFUNCTION(BlueprintCallable, Category="Map")
    void ToggleFullscreen();
    
protected:
    // Is the map in fullscreen mode
    bool bIsFullscreen = false;
    
    // Current player room
    UPROPERTY()
    AGWTRoom* CurrentRoom;
    
    // Visited rooms
    UPROPERTY()
    TArray<AGWTRoom*> VisitedRooms;
    
    // Create room markers and connections
    void CreateRoomWidgets();
    
    // Create a single room widget
    UWidget* CreateRoomWidget(AGWTRoom* Room);
    
    // Create a door connection widget
    UWidget* CreateDoorWidget(AGWTRoom* Room, EGWTDirection Direction);
    
    // Convert grid position to canvas position
    FVector2D GridToCanvasPosition(const FIntVector& GridPosition) const;
    
    // Convert world position to grid position
    FIntVector WorldToGridPosition(const FVector& WorldPosition) const;
    
    // Get room color based on type and state
    FLinearColor GetRoomColor(AGWTRoom* Room) const;
};