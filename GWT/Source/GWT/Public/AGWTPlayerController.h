// AGWTPlayerController.h
// Handles player input and UI

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AGWTPlayerController.generated.h"

// Forward declarations
class UGWTGrimoire;
class UGWTInventory;
class UGWTPlayerProgression;
class UGWTHUDWidget;
class UGWTSpellEditorWidget;
class UGWTInventoryWidget;
class UGWTMiniMapWidget;

/**
 * Player controller for Grand Wizard Tournament
 * Handles player input and manages UI elements
 * Connects player actions to game systems
 */
UCLASS()
class GWT_API AGWTPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AGWTPlayerController();

    // References to player systems
    UPROPERTY(BlueprintReadOnly, Category = "References")
    UGWTGrimoire* PlayerGrimoire;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    UGWTInventory* PlayerInventory;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    UGWTPlayerProgression* PlayerProgression;

    // UI instances
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UGWTHUDWidget* HUDWidget;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UGWTSpellEditorWidget* SpellEditorWidget;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UGWTInventoryWidget* InventoryWidget;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UGWTMiniMapWidget* MiniMapWidget;

    // UI visibility flags
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    bool bShowHUD = true;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    bool bShowSpellEditor = false;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    bool bShowInventory = false;

    // Active spell tracking
    UPROPERTY(BlueprintReadOnly, Category = "Spells")
    int32 ActiveSpellIndex = 0;

    // Methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHUD();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideHUD();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleSpellEditor();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenSpellEditor();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseSpellEditor();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleInventory();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenInventory();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseInventory();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHUD();

    UFUNCTION(BlueprintCallable, Category = "Spells")
    void CycleActiveSpell(bool bForward);

    UFUNCTION(BlueprintCallable, Category = "Spells")
    void CastActiveSpell();

    // Game state events
    UFUNCTION(BlueprintCallable, Category = "Game")
    void OnGameEnded(bool bWasSuccessful);

    UFUNCTION(BlueprintCallable, Category = "Game")
    void OnPlayerDied();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void OnWaveCompleted(int32 NewWave);

protected:
    // Initialize UI elements
    void InitializeHUD();
    void InitializeSpellEditor();
    void InitializeInventory();
    void InitializeMiniMap();

    // Get references to player systems
    void GetPlayerReferences();

    // Update timers
    FTimerHandle HUDUpdateTimerHandle;

    // HUD update timer callback
    UFUNCTION()
    void HUDUpdateTick();
};