// GWTHUDWidget.h
// Gameplay heads-up display

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GWTHUDWidget.generated.h"

// Forward declarations
class UProgressBar;
class UTextBlock;
class UVerticalBox;
class UImage;
class UGWTObjective;

/**
 * HUD widget for Grand Wizard Tournament
 * Displays player stats, active spell, objectives, and other game information
 * Provides real-time feedback during gameplay
 */
UCLASS()
class GWT_API UGWTHUDWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UGWTHUDWidget(const FObjectInitializer& ObjectInitializer);
    
    // UI elements
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* HealthBar;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* ManaBar;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* WaveText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ScoreText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ActiveSpellText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ObjectivesBox;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* MiniMapImage;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* GoldText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* TimeText;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* StatusEffectsBox;
    
    // Methods
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateMana(float CurrentMana, float MaxMana);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateWave(int32 CurrentWave);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateScore(int32 CurrentScore);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateActiveSpell(const FString& SpellName);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateObjectives(const TArray<UGWTObjective*>& Objectives);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateMiniMap();
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateGold(int32 CurrentGold);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateTime(float RemainingTime);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateStatusEffects(const TArray<FGWTStatusEffect>& StatusEffects);
    
    // UI element creation
    UFUNCTION(BlueprintCallable, Category="HUD")
    UWidget* CreateObjectiveWidget(UGWTObjective* Objective);
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    UWidget* CreateStatusEffectWidget(const FGWTStatusEffect& StatusEffect);
    
protected:
    // Helper to format time
    FString FormatTime(float TimeInSeconds) const;
    
    // Animation properties
    UPROPERTY(BlueprintReadOnly, Category="Animation")
    bool bLowHealthPulse = false;
    
    UPROPERTY(BlueprintReadOnly, Category="Animation")
    float PulseAlpha = 0.0f;
    
    // Pulse animation for low health
    void UpdateLowHealthPulse(float DeltaTime);
};