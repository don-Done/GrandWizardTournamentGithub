// UGWTHUDWidget.cpp
// Implementation of the gameplay HUD widget

#include "UGWTHUDWidget.h"
#include "UGWTObjective.h"
#include "GWTTypes.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"

UGWTHUDWidget::UGWTHUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize animation properties
    bLowHealthPulse = false;
    PulseAlpha = 0.0f;
}

void UGWTHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Set default values
    if (HealthBar)
    {
        HealthBar->SetPercent(1.0f);
    }

    if (ManaBar)
    {
        ManaBar->SetPercent(1.0f);
    }

    if (WaveText)
    {
        WaveText->SetText(FText::FromString("Wave: 1"));
    }

    if (ScoreText)
    {
        ScoreText->SetText(FText::FromString("Score: 0"));
    }

    if (ActiveSpellText)
    {
        ActiveSpellText->SetText(FText::FromString("No Spell"));
    }

    if (GoldText)
    {
        GoldText->SetText(FText::FromString("Gold: 0"));
    }

    if (TimeText)
    {
        TimeText->SetText(FText::FromString("Time: 5:00"));
    }

    UE_LOG(LogTemp, Display, TEXT("HUD Widget constructed"));
}

void UGWTHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Update animation effects
    UpdateLowHealthPulse(InDeltaTime);
}

void UGWTHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    // Update health bar
    if (HealthBar)
    {
        float HealthPercent = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
        HealthBar->SetPercent(HealthPercent);

        // Set low health pulse effect if health is below 30%
        bLowHealthPulse = (HealthPercent < 0.3f);

        // Set bar color based on health
        if (HealthPercent < 0.3f)
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f)); // Red
        }
        else if (HealthPercent < 0.6f)
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.5f, 0.0f)); // Orange
        }
        else
        {
            HealthBar->SetFillColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f)); // Green
        }
    }
}

void UGWTHUDWidget::UpdateMana(float CurrentMana, float MaxMana)
{
    // Update mana bar
    if (ManaBar)
    {
        float ManaPercent = (MaxMana > 0.0f) ? (CurrentMana / MaxMana) : 0.0f;
        ManaBar->SetPercent(ManaPercent);

        // Set bar color based on mana
        if (ManaPercent < 0.3f)
        {
            ManaBar->SetFillColorAndOpacity(FLinearColor(0.5f, 0.5f, 1.0f)); // Light blue
        }
        else
        {
            ManaBar->SetFillColorAndOpacity(FLinearColor(0.0f, 0.0f, 1.0f)); // Blue
        }
    }
}

void UGWTHUDWidget::UpdateWave(int32 CurrentWave)
{
    // Update wave text
    if (WaveText)
    {
        WaveText->SetText(FText::Format(
            NSLOCTEXT("GWT", "WaveFormat", "Wave: {0}"),
            FText::AsNumber(CurrentWave)
        ));
    }
}

void UGWTHUDWidget::UpdateScore(int32 CurrentScore)
{
    // Update score text
    if (ScoreText)
    {
        ScoreText->SetText(FText::Format(
            NSLOCTEXT("GWT", "ScoreFormat", "Score: {0}"),
            FText::AsNumber(CurrentScore)
        ));
    }
}

void UGWTHUDWidget::UpdateActiveSpell(const FString& SpellName)
{
    // Update active spell text
    if (ActiveSpellText)
    {
        ActiveSpellText->SetText(FText::Format(
            NSLOCTEXT("GWT", "SpellFormat", "Spell: {0}"),
            FText::FromString(SpellName)
        ));
    }
}

void UGWTHUDWidget::UpdateObjectives(const TArray<UGWTObjective*>& Objectives)
{
    // Update objectives list
    if (ObjectivesBox)
    {
        // Clear existing objective widgets
        ObjectivesBox->ClearChildren();

        // Add widget for each objective
        for (UGWTObjective* Objective : Objectives)
        {
            if (Objective)
            {
                UWidget* ObjectiveWidget = CreateObjectiveWidget(Objective);
                if (ObjectiveWidget)
                {
                    ObjectivesBox->AddChild(ObjectiveWidget);
                }
            }
        }
    }
}

void UGWTHUDWidget::UpdateMiniMap()
{
    // This would update the mini map image
    // In a full implementation, this would render a top-down view of the labyrinth

    if (MiniMapImage)
    {
        // For this example, we'll just update the visibility
        MiniMapImage->SetVisibility(ESlateVisibility::Visible);
    }
}

void UGWTHUDWidget::UpdateGold(int32 CurrentGold)
{
    // Update gold text
    if (GoldText)
    {
        GoldText->SetText(FText::Format(
            NSLOCTEXT("GWT", "GoldFormat", "Gold: {0}"),
            FText::AsNumber(CurrentGold)
        ));
    }
}

void UGWTHUDWidget::UpdateTime(float RemainingTime)
{
    // Update time text
    if (TimeText)
    {
        FString FormattedTime = FormatTime(RemainingTime);

        TimeText->SetText(FText::Format(
            NSLOCTEXT("GWT", "TimeFormat", "Time: {0}"),
            FText::FromString(FormattedTime)
        ));

        // Change color if time is running low
        if (RemainingTime < 30.0f)
        {
            TimeText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f))); // Red
        }
        else if (RemainingTime < 60.0f)
        {
            TimeText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.5f, 0.0f))); // Orange
        }
        else
        {
            TimeText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f))); // White
        }
    }
}

void UGWTHUDWidget::UpdateStatusEffects(const TArray<FGWTStatusEffect>& StatusEffects)
{
    // Update status effect icons
    if (StatusEffectsBox)
    {
        // Clear existing status effect widgets
        StatusEffectsBox->ClearChildren();

        // Add widget for each status effect
        for (const FGWTStatusEffect& Effect : StatusEffects)
        {
            UWidget* EffectWidget = CreateStatusEffectWidget(Effect);
            if (EffectWidget)
            {
                StatusEffectsBox->AddChild(EffectWidget);
            }
        }
    }
}

UWidget* UGWTHUDWidget::CreateObjectiveWidget(UGWTObjective* Objective)
{
    // Create a widget to display a single objective

    if (!Objective)
        return nullptr;

    // Create horizontal box to hold objective info
    UHorizontalBox* HBox = NewObject<UHorizontalBox>(this);

    // Create border for the objective background
    UBorder* Border = NewObject<UBorder>(this);
    Border->SetBrushColor(Objective->bIsPrimary ?
        FLinearColor(0.1f, 0.1f, 0.5f) : // Primary: blue
        FLinearColor(0.3f, 0.3f, 0.3f)); // Secondary: gray

    // Create text block for objective title
    UTextBlock* TitleText = NewObject<UTextBlock>(this);
    TitleText->SetText(Objective->ObjectiveTitle);
    TitleText->SetColorAndOpacity(FSlateColor(
        Objective->IsCompleted() ? FLinearColor(0.0f, 1.0f, 0.0f) : // Completed: green
        FLinearColor(1.0f, 1.0f, 1.0f)   // Not completed: white
    ));

    // Create text block for progress
    UTextBlock* ProgressText = NewObject<UTextBlock>(this);
    ProgressText->SetText(FText::FromString(Objective->GetProgressText()));

    // Add components to layout
    Border->SetContent(HBox);
    HBox->AddChild(TitleText);
    HBox->AddChild(ProgressText);

    return Border;
}

UWidget* UGWTHUDWidget::CreateStatusEffectWidget(const FGWTStatusEffect& StatusEffect)
{
    // Create a widget to display a single status effect

    // Create border for the effect background
    UBorder* Border = NewObject<UBorder>(this);

    // Set color based on effect type
    FLinearColor EffectColor;
    switch (StatusEffect.EffectType)
    {
    case EGWTStatusEffectType::Burning:
        EffectColor = FLinearColor(1.0f, 0.2f, 0.0f); // Red-orange
        break;

    case EGWTStatusEffectType::Frozen:
        EffectColor = FLinearColor(0.0f, 0.8f, 1.0f); // Light blue
        break;

    case EGWTStatusEffectType::Poisoned:
        EffectColor = FLinearColor(0.0f, 0.7f, 0.0f); // Green
        break;

    case EGWTStatusEffectType::Electrified:
        EffectColor = FLinearColor(0.7f, 0.0f, 1.0f); // Purple
        break;

    case EGWTStatusEffectType::Shielded:
        EffectColor = FLinearColor(0.8f, 0.8f, 0.0f); // Yellow
        break;

    default:
        EffectColor = FLinearColor(0.5f, 0.5f, 0.5f); // Gray
        break;
    }

    Border->SetBrushColor(EffectColor);

    // Create vertical box to hold effect info
    UVerticalBox* VBox = NewObject<UVerticalBox>(this);

    // Create text block for effect name
    UTextBlock* NameText = NewObject<UTextBlock>(this);
    NameText->SetText(FText::FromString(UEnum::GetValueAsString(StatusEffect.EffectType)));

    // Create text block for duration
    UTextBlock* DurationText = NewObject<UTextBlock>(this);
    DurationText->SetText(FText::Format(
        NSLOCTEXT("GWT", "DurationFormat", "{0}s"),
        FText::AsNumber(FMath::RoundToInt(StatusEffect.TimeRemaining))
    ));

    // Add components to layout
    Border->SetContent(VBox);
    VBox->AddChild(NameText);
    VBox->AddChild(DurationText);

    return Border;
}

FString UGWTHUDWidget::FormatTime(float TimeInSeconds) const
{
    // Format time as MM:SS
    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(TimeInSeconds) % 60;

    return FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
}

void UGWTHUDWidget::UpdateLowHealthPulse(float DeltaTime)
{
    // Update low health pulse effect
    if (bLowHealthPulse && HealthBar)
    {
        // Oscillate alpha value
        PulseAlpha += DeltaTime * 3.0f; // Controls pulse speed
        float PulseValue = (FMath::Sin(PulseAlpha) + 1.0f) * 0.5f; // 0.0 to 1.0

        // Update border opacity based on pulse
        FLinearColor Color = HealthBar->GetFillColorAndOpacity();
        Color.A = FMath::Lerp(0.5f, 1.0f, PulseValue); // Pulse between 50% and 100% opacity
        HealthBar->SetFillColorAndOpacity(Color);
    }
}