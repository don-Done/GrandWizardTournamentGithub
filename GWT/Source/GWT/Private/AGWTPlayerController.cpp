// AGWTPlayerController.cpp
// Implementation of the player controller

#include "AGWTPlayerController.h"
#include "AGWTPlayerCharacter.h"
#include "UGWTGrimoire.h"
#include "UGWTInventory.h"
#include "UGWTPlayerProgression.h"
#include "UGWTHUDWidget.h"
#include "UGWTSpellEditorWidget.h"
#include "UGWTInventoryWidget.h"
#include "UGWTMiniMapWidget.h"
#include "UGWTSpell.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AGWTPlayerController::AGWTPlayerController()
{
    // Set this player controller to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Initialize references
    PlayerGrimoire = nullptr;
    PlayerInventory = nullptr;
    PlayerProgression = nullptr;

    // Initialize widget pointers
    HUDWidget = nullptr;
    SpellEditorWidget = nullptr;
    InventoryWidget = nullptr;
    MiniMapWidget = nullptr;

    // Set default UI state
    bShowHUD = true;
    bShowSpellEditor = false;
    bShowInventory = false;

    // Set default active spell
    ActiveSpellIndex = 0;

    UE_LOG(LogTemp, Display, TEXT("Player Controller created"));
}

void AGWTPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get player system references
    GetPlayerReferences();

    // Initialize UI elements
    InitializeHUD();
    InitializeSpellEditor();
    InitializeInventory();
    InitializeMiniMap();

    // Show the main HUD
    ShowHUD();

    // Set up HUD update timer
    GetWorld()->GetTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &AGWTPlayerController::HUDUpdateTick,
        0.1f, // Update every 0.1 seconds
        true // Looping
    );

    UE_LOG(LogTemp, Display, TEXT("Player Controller BeginPlay"));
}

void AGWTPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update UI elements that need constant updating
    UpdateHUD();
}

void AGWTPlayerController::ShowHUD()
{
    // Show the HUD widget
    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Visible);
        bShowHUD = true;
        UE_LOG(LogTemp, Verbose, TEXT("HUD shown"));
    }

    // Show the mini-map widget
    if (MiniMapWidget)
    {
        MiniMapWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AGWTPlayerController::HideHUD()
{
    // Hide the HUD widget
    if (HUDWidget)
    {
        HUDWidget->SetVisibility(ESlateVisibility::Hidden);
        bShowHUD = false;
        UE_LOG(LogTemp, Verbose, TEXT("HUD hidden"));
    }

    // Hide the mini-map widget
    if (MiniMapWidget)
    {
        MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AGWTPlayerController::ToggleSpellEditor()
{
    // Toggle spell editor visibility
    if (bShowSpellEditor)
    {
        CloseSpellEditor();
    }
    else
    {
        OpenSpellEditor();
    }
}

void AGWTPlayerController::OpenSpellEditor()
{
    // Show the spell editor
    if (SpellEditorWidget)
    {
        // Close inventory if open
        if (bShowInventory)
        {
            CloseInventory();
        }

        // Show spell editor
        SpellEditorWidget->SetVisibility(ESlateVisibility::Visible);
        bShowSpellEditor = true;

        // Hide the HUD while editing
        HideHUD();

        // Make mouse cursor visible
        bShowMouseCursor = true;
        SetInputMode(FInputModeGameAndUI());

        UE_LOG(LogTemp, Display, TEXT("Spell editor opened"));
    }
}

void AGWTPlayerController::CloseSpellEditor()
{
    // Hide the spell editor
    if (SpellEditorWidget)
    {
        SpellEditorWidget->SetVisibility(ESlateVisibility::Hidden);
        bShowSpellEditor = false;

        // Show the HUD again
        ShowHUD();

        // Hide mouse cursor
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());

        UE_LOG(LogTemp, Display, TEXT("Spell editor closed"));
    }
}

void AGWTPlayerController::ToggleInventory()
{
    // Toggle inventory visibility
    if (bShowInventory)
    {
        CloseInventory();
    }
    else
    {
        OpenInventory();
    }
}

void AGWTPlayerController::OpenInventory()
{
    // Show the inventory
    if (InventoryWidget)
    {
        // Close spell editor if open
        if (bShowSpellEditor)
        {
            CloseSpellEditor();
        }

        // Show inventory
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        bShowInventory = true;

        // Keep the HUD visible with inventory
        ShowHUD();

        // Make mouse cursor visible
        bShowMouseCursor = true;
        SetInputMode(FInputModeGameAndUI());

        UE_LOG(LogTemp, Display, TEXT("Inventory opened"));
    }
}

void AGWTPlayerController::CloseInventory()
{
    // Hide the inventory
    if (InventoryWidget)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
        bShowInventory = false;

        // Hide mouse cursor
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());

        UE_LOG(LogTemp, Display, TEXT("Inventory closed"));
    }
}

void AGWTPlayerController::UpdateHUD()
{
    // Update HUD elements based on player state
    if (HUDWidget && bShowHUD)
    {
        // Get player character
        AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(GetPawn());
        if (PlayerChar)
        {
            // Update health and mana
            HUDWidget->UpdateHealth(PlayerChar->CurrentHealth, PlayerChar->MaxHealth);
            HUDWidget->UpdateMana(PlayerChar->CurrentMana, PlayerChar->MaxMana);

            // Update active spell
            if (PlayerGrimoire && PlayerGrimoire->GetSpellCount() > 0)
            {
                UGWTSpell* ActiveSpell = PlayerGrimoire->GetSpell(ActiveSpellIndex);
                if (ActiveSpell)
                {
                    HUDWidget->UpdateActiveSpell(ActiveSpell->SpellName.ToString());
                }
            }

            // Update status effects
            // In a full implementation, we would update UI elements showing status effects
        }
    }

    // Update mini-map if visible
    if (MiniMapWidget && bShowHUD)
    {
        // In a full implementation, this would update the player's position on the mini-map
    }
}

void AGWTPlayerController::CycleActiveSpell(bool bForward)
{
    // Make sure we have a grimoire
    if (!PlayerGrimoire)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cycle spells: No grimoire"));
        return;
    }

    // Get number of spells
    int32 SpellCount = PlayerGrimoire->GetSpellCount();

    // If we have no spells, do nothing
    if (SpellCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cycle spells: No spells in grimoire"));
        return;
    }

    // Cycle to next/previous spell
    if (bForward)
    {
        ActiveSpellIndex = (ActiveSpellIndex + 1) % SpellCount;
    }
    else
    {
        ActiveSpellIndex = (ActiveSpellIndex - 1 + SpellCount) % SpellCount;
    }

    // Update the player character's active spell
    AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(GetPawn());
    if (PlayerChar)
    {
        PlayerChar->ActiveSpellIndex = ActiveSpellIndex;
    }

    UE_LOG(LogTemp, Display, TEXT("Cycled to spell %d of %d"), ActiveSpellIndex + 1, SpellCount);

    // Update HUD
    UpdateHUD();
}

void AGWTPlayerCharacter::CastActiveSpell()
{
    // Forward to player character
    AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(GetPawn());
    if (PlayerChar)
    {
        PlayerChar->CastSpell();
    }
}

void AGWTPlayerController::OnGameEnded(bool bWasSuccessful)
{
    // Handle game end
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Display, TEXT("Game completed successfully!"));

        // In a full implementation, this would show a victory screen
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Game over!"));

        // In a full implementation, this would show a game over screen
    }

    // Pause the game and show mouse cursor
    SetPause(true);
    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());
}

void AGWTPlayerController::OnPlayerDied()
{
    // Handle player death
    UE_LOG(LogTemp, Display, TEXT("Player died"));

    // In a full implementation, this would show a death screen and respawn options
}

void AGWTPlayerController::OnWaveCompleted(int32 NewWave)
{
    // Handle wave completion
    UE_LOG(LogTemp, Display, TEXT("Wave completed. Starting wave %d"), NewWave);

    // In a full implementation, this would show a wave transition screen
}

void AGWTPlayerController::InitializeHUD()
{
    // Create HUD widget if it doesn't exist
    if (!HUDWidget)
    {
        // Use widget class specified in blueprint
        TSubclassOf<UGWTHUDWidget> HUDWidgetClass = LoadClass<UGWTHUDWidget>(nullptr, TEXT("/Game/UI/BP_HUDWidget"));

        if (HUDWidgetClass)
        {
            HUDWidget = CreateWidget<UGWTHUDWidget>(this, HUDWidgetClass);

            if (HUDWidget)
            {
                HUDWidget->AddToViewport();
                UE_LOG(LogTemp, Display, TEXT("HUD widget created"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("HUD widget class not found"));
        }
    }
}

void AGWTPlayerController::InitializeSpellEditor()
{
    // Create spell editor widget if it doesn't exist
    if (!SpellEditorWidget)
    {
        // Use widget class specified in blueprint
        TSubclassOf<UGWTSpellEditorWidget> SpellEditorClass = LoadClass<UGWTSpellEditorWidget>(nullptr, TEXT("/Game/UI/BP_SpellEditorWidget"));

        if (SpellEditorClass)
        {
            SpellEditorWidget = CreateWidget<UGWTSpellEditorWidget>(this, SpellEditorClass);

            if (SpellEditorWidget)
            {
                SpellEditorWidget->AddToViewport();
                SpellEditorWidget->SetVisibility(ESlateVisibility::Hidden); // Hidden by default
                UE_LOG(LogTemp, Display, TEXT("Spell editor widget created"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spell editor widget class not found"));
        }
    }
}

void AGWTPlayerController::InitializeInventory()
{
    // Create inventory widget if it doesn't exist
    if (!InventoryWidget)
    {
        // Use widget class specified in blueprint
        TSubclassOf<UGWTInventoryWidget> InventoryClass = LoadClass<UGWTInventoryWidget>(nullptr, TEXT("/Game/UI/BP_InventoryWidget"));

        if (InventoryClass)
        {
            InventoryWidget = CreateWidget<UGWTInventoryWidget>(this, InventoryClass);

            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();
                InventoryWidget->SetVisibility(ESlateVisibility::Hidden); // Hidden by default
                UE_LOG(LogTemp, Display, TEXT("Inventory widget created"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Inventory widget class not found"));
        }
    }
}

void AGWTPlayerController::InitializeMiniMap()
{
    // Create mini-map widget if it doesn't exist
    if (!MiniMapWidget)
    {
        // Use widget class specified in blueprint
        TSubclassOf<UGWTMiniMapWidget> MiniMapClass = LoadClass<UGWTMiniMapWidget>(nullptr, TEXT("/Game/UI/BP_MiniMapWidget"));

        if (MiniMapClass)
        {
            MiniMapWidget = CreateWidget<UGWTMiniMapWidget>(this, MiniMapClass);

            if (MiniMapWidget)
            {
                MiniMapWidget->AddToViewport();
                UE_LOG(LogTemp, Display, TEXT("Mini-map widget created"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Mini-map widget class not found"));
        }
    }
}

void AGWTPlayerController::GetPlayerReferences()
{
    // Get references to player systems from the player character
    AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(GetPawn());
    if (PlayerChar)
    {
        PlayerGrimoire = PlayerChar->Grimoire;
        PlayerInventory = PlayerChar->Inventory;
        PlayerProgression = PlayerChar->Progression;

        UE_LOG(LogTemp, Display, TEXT("Got player system references"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get player character"));
    }
}

void AGWTPlayerController::HUDUpdateTick()
{
    // Update HUD elements that need regular updates
    if (HUDWidget && bShowHUD)
    {
        // Get the game state for time and score
        AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
        if (GameState && GameState->IsA(AGWTGameState::StaticClass()))
        {
            AGWTGameState* GWTGameState = Cast<AGWTGameState>(GameState);

            // Update wave information
            if (GWTGameState)
            {
                // Get the game mode for wave information
                AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
                if (GameMode && GameMode->IsA(AGWTGameMode::StaticClass()))
                {
                    AGWTGameMode* GWTGameMode = Cast<AGWTGameMode>(GameMode);

                    // Update wave display
                    if (GWTGameMode)
                    {
                        HUDWidget->UpdateWave(GWTGameMode->CurrentWave);
                    }
                }

                // Update score
                HUDWidget->UpdateScore(GWTGameState->TotalScore);

                // Update objectives
                if (GameMode && GameMode->IsA(AGWTGameMode::StaticClass()))
                {
                    AGWTGameMode* GWTGameMode = Cast<AGWTGameMode>(GameMode);

                    if (GWTGameMode)
                    {
                        HUDWidget->UpdateObjectives(GWTGameMode->CurrentObjectives);
                    }
                }
            }
        }
    }
}