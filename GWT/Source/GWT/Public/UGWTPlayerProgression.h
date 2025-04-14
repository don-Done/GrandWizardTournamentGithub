// UGWTPlayerProgression.h
// Manages player level and unlocks

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTPlayerProgression.generated.h"

// Forward declarations
class UGWTGrimoire;

/**
 * Player progression system for Grand Wizard Tournament
 * Handles experience, levels, and unlocking new content
 * Provides rewards for educational advancement
 */
UCLASS(BlueprintType)
class GWT_API UGWTPlayerProgression : public UObject
{
    GENERATED_BODY()

public:
    UGWTPlayerProgression();

    // Progression stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 Experience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 ExperienceToNextLevel = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 SkillPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    TArray<FGWTUnlockData> UnlockedContent;

    // Grimoire reference
    UPROPERTY(BlueprintReadWrite, Category = "References")
    UGWTGrimoire* Grimoire;

    // Unlock trackers
    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    TArray<TSubclassOf<UGWTSpellNode>> AvailableNodeTypes;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    TMap<EGWTSkillTreeCategory, int32> SkillLevels;

    // Methods
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddXP(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool UnlockContent(EGWTUnlockableType Type, FName ContentID);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool IsContentUnlocked(EGWTUnlockableType Type, FName ContentID) const;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    TArray<FGWTUnlockData> GetAllUnlocks() const;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    TArray<FGWTUnlockData> GetUnlocksOfType(EGWTUnlockableType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Skills")
    bool SpendSkillPoint(EGWTSkillTreeCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Skills")
    int32 GetSkillLevel(EGWTSkillTreeCategory Category) const;

    UFUNCTION(BlueprintCallable, Category = "Skills")
    TArray<FGWTSkillData> GetAvailableSkills() const;

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void SaveProgression(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool LoadProgression(const FString& SlotName);

protected:
    // Initialize skill tree
    void InitializeSkillTree();

    // Calculate XP requirements for each level
    int32 CalculateXPForLevel(int32 TargetLevel) const;

    // Get unlocks for a level
    TArray<FGWTUnlockData> GetUnlocksForLevel(int32 Level) const;

    // Apply unlocks to game systems
    void ApplyUnlock(const FGWTUnlockData& Unlock);

    // Level progression data
    TMap<int32, int32> LevelXPRequirements;

    // Available skills by category
    TMap<EGWTSkillTreeCategory, TArray<FGWTSkillData>> SkillTree;
};