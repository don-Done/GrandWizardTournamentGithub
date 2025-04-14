// UGWTPlayerProgression.cpp
// Implementation of player progression system

#include "UGWTPlayerProgression.h"
#include "UGWTGrimoire.h"
#include "UGWTSpellNode.h"
#include "UGWTMagicNode.h"
#include "UGWTTriggerNode.h"
#include "UGWTEffectNode.h"
#include "UGWTConditionNode.h"
#include "UGWTVariableNode.h"
#include "UGWTFlowNode.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

UGWTPlayerProgression::UGWTPlayerProgression()
{
    // Initialize progression
    Level = 1;
    Experience = 0;
    ExperienceToNextLevel = 100;
    SkillPoints = 0;

    // Set up level requirements
    for (int32 i = 1; i <= 20; i++)
    {
        LevelXPRequirements.Add(i, CalculateXPForLevel(i));
    }

    // Initialize skill levels
    SkillLevels.Add(EGWTSkillTreeCategory::SpellPower, 0);
    SkillLevels.Add(EGWTSkillTreeCategory::ManaEfficiency, 0);
    SkillLevels.Add(EGWTSkillTreeCategory::CastingSpeed, 0);
    SkillLevels.Add(EGWTSkillTreeCategory::SpellComplexity, 0);
    SkillLevels.Add(EGWTSkillTreeCategory::ElementalMastery, 0);

    // Initialize skill tree
    InitializeSkillTree();

    // Initialize available node types (starting nodes)
    AvailableNodeTypes.Add(UGWTMagicNode::StaticClass());
    AvailableNodeTypes.Add(UGWTTriggerNode::StaticClass());

    UE_LOG(LogTemp, Display, TEXT("Player Progression initialized"));
}

void UGWTPlayerProgression::AddXP(int32 Amount)
{
    // Add experience points
    int32 OldExperience = Experience;
    Experience += Amount;

    UE_LOG(LogTemp, Display, TEXT("Added %d XP. Total: %d"), Amount, Experience);

    // Check for level up
    while (Experience >= ExperienceToNextLevel)
    {
        LevelUp();
    }
}

void UGWTPlayerProgression::LevelUp()
{
    // Level up the player
    Level++;

    // Calculate new XP requirement
    ExperienceToNextLevel = LevelXPRequirements.Contains(Level + 1) ?
        LevelXPRequirements[Level + 1] :
        CalculateXPForLevel(Level + 1);

    // Award skill point
    SkillPoints++;

    UE_LOG(LogTemp, Display, TEXT("Level up! New level: %d, Skill points: %d"), Level, SkillPoints);

    // Process level-specific unlocks
    TArray<FGWTUnlockData> LevelUnlocks = GetUnlocksForLevel(Level);

    for (const FGWTUnlockData& Unlock : LevelUnlocks)
    {
        // Apply unlock
        ApplyUnlock(Unlock);

        // Add to unlocked content
        UnlockedContent.Add(Unlock);

        UE_LOG(LogTemp, Display, TEXT("Unlocked content: %s"), *Unlock.ContentName);
    }
}

bool UGWTPlayerProgression::UnlockContent(EGWTUnlockableType Type, FName ContentID)
{
    // Check if already unlocked
    for (const FGWTUnlockData& Unlock : UnlockedContent)
    {
        if (Unlock.ContentID == ContentID && Unlock.Type == Type)
        {
            UE_LOG(LogTemp, Warning, TEXT("Content already unlocked: %s"), *ContentID.ToString());
            return false;
        }
    }

    // Create unlock data
    FGWTUnlockData NewUnlock;
    NewUnlock.Type = Type;
    NewUnlock.ContentID = ContentID;

    // Set content name based on type
    switch (Type)
    {
    case EGWTUnlockableType::NodeType:
        NewUnlock.ContentName = FString::Printf(TEXT("Spell Node: %s"), *ContentID.ToString());
        break;

    case EGWTUnlockableType::Element:
        NewUnlock.ContentName = FString::Printf(TEXT("Element: %s"), *ContentID.ToString());
        break;

    case EGWTUnlockableType::Equipment:
        NewUnlock.ContentName = FString::Printf(TEXT("Equipment: %s"), *ContentID.ToString());
        break;

    case EGWTUnlockableType::Room:
        NewUnlock.ContentName = FString::Printf(TEXT("Room Feature: %s"), *ContentID.ToString());
        break;

    default:
        NewUnlock.ContentName = ContentID.ToString();
        break;
    }

    // Apply the unlock
    ApplyUnlock(NewUnlock);

    // Add to unlocked content
    UnlockedContent.Add(NewUnlock);

    UE_LOG(LogTemp, Display, TEXT("Manually unlocked content: %s"), *NewUnlock.ContentName);

    return true;
}

bool UGWTPlayerProgression::IsContentUnlocked(EGWTUnlockableType Type, FName ContentID) const
{
    // Check if content is unlocked
    for (const FGWTUnlockData& Unlock : UnlockedContent)
    {
        if (Unlock.ContentID == ContentID && Unlock.Type == Type)
        {
            return true;
        }
    }

    return false;
}

TArray<FGWTUnlockData> UGWTPlayerProgression::GetAllUnlocks() const
{
    return UnlockedContent;
}

TArray<FGWTUnlockData> UGWTPlayerProgression::GetUnlocksOfType(EGWTUnlockableType Type) const
{
    // Get all unlocks of a specific type
    TArray<FGWTUnlockData> TypedUnlocks;

    for (const FGWTUnlockData& Unlock : UnlockedContent)
    {
        if (Unlock.Type == Type)
        {
            TypedUnlocks.Add(Unlock);
        }
    }

    return TypedUnlocks;
}

bool UGWTPlayerProgression::SpendSkillPoint(EGWTSkillTreeCategory Category)
{
    // Check if player has skill points
    if (SkillPoints <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spend skill point: No skill points available"));
        return false;
    }

    // Check if category is valid
    if (!SkillLevels.Contains(Category))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spend skill point: Invalid category %d"), (int32)Category);
        return false;
    }

    // Check if skill is at max level
    int32 CurrentLevel = SkillLevels[Category];
    if (CurrentLevel >= 5) // Assuming max level is 5
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot spend skill point: Skill already at max level"));
        return false;
    }

    // Spend the skill point
    SkillLevels[Category] = CurrentLevel + 1;
    SkillPoints--;

    UE_LOG(LogTemp, Display, TEXT("Spent skill point on %d. New level: %d, Remaining points: %d"),
        (int32)Category, SkillLevels[Category], SkillPoints);

    return true;
}

int32 UGWTPlayerProgression::GetSkillLevel(EGWTSkillTreeCategory Category) const
{
    // Get the level of a specific skill
    if (SkillLevels.Contains(Category))
    {
        return SkillLevels[Category];
    }

    return 0;
}

TArray<FGWTSkillData> UGWTPlayerProgression::GetAvailableSkills() const
{
    // Get all skills that are available based on player level
    TArray<FGWTSkillData> AvailableSkills;

    // For each category
    for (const auto& Pair : SkillTree)
    {
        // Get current level in this category
        int32 CurrentLevel = SkillLevels.Contains(Pair.Key) ? SkillLevels[Pair.Key] : 0;

        // If there are more skills available in this category
        if (Pair.Value.IsValidIndex(CurrentLevel))
        {
            AvailableSkills.Add(Pair.Value[CurrentLevel]);
        }
    }

    return AvailableSkills;
}

void UGWTPlayerProgression::SaveProgression(const FString& SlotName)
{
    // Save progression to a save file
    // This would be implemented using UGameplayStatics::SaveGameToSlot

    UE_LOG(LogTemp, Display, TEXT("Saved player progression to slot: %s"), *SlotName);
}

bool UGWTPlayerProgression::LoadProgression(const FString& SlotName)
{
    // Load progression from a save file
    // This would be implemented using UGameplayStatics::LoadGameFromSlot

    UE_LOG(LogTemp, Display, TEXT("Loaded player progression from slot: %s"), *SlotName);

    return true;
}

void UGWTPlayerProgression::InitializeSkillTree()
{
    // Initialize the skill tree with available skills
    // Each category has multiple levels

    // Spell Power category
    {
        TArray<FGWTSkillData> Skills;

        FGWTSkillData Skill1;
        Skill1.SkillName = TEXT("Novice Power");
        Skill1.Description = TEXT("Increases spell damage by 10%");
        Skill1.Category = EGWTSkillTreeCategory::SpellPower;
        Skill1.Level = 1;
        Skills.Add(Skill1);

        FGWTSkillData Skill2;
        Skill2.SkillName = TEXT("Apprentice Power");
        Skill2.Description = TEXT("Increases spell damage by 20%");
        Skill2.Category = EGWTSkillTreeCategory::SpellPower;
        Skill2.Level = 2;
        Skills.Add(Skill2);

        FGWTSkillData Skill3;
        Skill3.SkillName = TEXT("Adept Power");
        Skill3.Description = TEXT("Increases spell damage by 30%");
        Skill3.Category = EGWTSkillTreeCategory::SpellPower;
        Skill3.Level = 3;
        Skills.Add(Skill3);

        FGWTSkillData Skill4;
        Skill4.SkillName = TEXT("Expert Power");
        Skill4.Description = TEXT("Increases spell damage by 40%");
        Skill4.Category = EGWTSkillTreeCategory::SpellPower;
        Skill4.Level = 4;
        Skills.Add(Skill4);

        FGWTSkillData Skill5;
        Skill5.SkillName = TEXT("Master Power");
        Skill5.Description = TEXT("Increases spell damage by 50%");
        Skill5.Category = EGWTSkillTreeCategory::SpellPower;
        Skill5.Level = 5;
        Skills.Add(Skill5);

        SkillTree.Add(EGWTSkillTreeCategory::SpellPower, Skills);
    }

    // Mana Efficiency category
    {
        TArray<FGWTSkillData> Skills;

        FGWTSkillData Skill1;
        Skill1.SkillName = TEXT("Mana Conservation I");
        Skill1.Description = TEXT("Reduces spell mana cost by 10%");
        Skill1.Category = EGWTSkillTreeCategory::ManaEfficiency;
        Skill1.Level = 1;
        Skills.Add(Skill1);

        // Add more levels...

        SkillTree.Add(EGWTSkillTreeCategory::ManaEfficiency, Skills);
    }

    // Add more categories...

    UE_LOG(LogTemp, Display, TEXT("Initialized skill tree with %d categories"), SkillTree.Num());
}

int32 UGWTPlayerProgression::CalculateXPForLevel(int32 TargetLevel) const
{
    // Calculate XP required for a level using a formula
    // Simple formula: 100 * level^2
    return 100 * TargetLevel * TargetLevel;
}

TArray<FGWTUnlockData> UGWTPlayerProgression::GetUnlocksForLevel(int32 Level) const
{
    // Get unlocks that happen automatically at a specific level
    TArray<FGWTUnlockData> Unlocks;

    // Level-specific unlocks
    switch (Level)
    {
    case 2:
        // Level 2: Unlock Effect node
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::NodeType;
        Unlock.ContentID = FName("EffectNode");
        Unlock.ContentName = TEXT("Effect Node");
        Unlocks.Add(Unlock);
    }
    break;

    case 3:
        // Level 3: Unlock Condition node
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::NodeType;
        Unlock.ContentID = FName("ConditionNode");
        Unlock.ContentName = TEXT("Condition Node");
        Unlocks.Add(Unlock);
    }
    break;

    case 4:
        // Level 4: Unlock Variable node
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::NodeType;
        Unlock.ContentID = FName("VariableNode");
        Unlock.ContentName = TEXT("Variable Node");
        Unlocks.Add(Unlock);
    }
    break;

    case 5:
        // Level 5: Unlock Flow node
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::NodeType;
        Unlock.ContentID = FName("FlowNode");
        Unlock.ContentName = TEXT("Flow Node");
        Unlocks.Add(Unlock);
    }
    break;

    case 6:
        // Level 6: Unlock Ice element
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::Element;
        Unlock.ContentID = FName("Ice");
        Unlock.ContentName = TEXT("Ice Element");
        Unlocks.Add(Unlock);
    }
    break;

    case 8:
        // Level 8: Unlock Lightning element
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::Element;
        Unlock.ContentID = FName("Lightning");
        Unlock.ContentName = TEXT("Lightning Element");
        Unlocks.Add(Unlock);
    }
    break;

    case 10:
        // Level 10: Unlock Earth element
    {
        FGWTUnlockData Unlock;
        Unlock.Type = EGWTUnlockableType::Element;
        Unlock.ContentID = FName("Earth");
        Unlock.ContentName = TEXT("Earth Element");
        Unlocks.Add(Unlock);
    }
    break;

    // Add more level unlocks...
    }

    return Unlocks;
}

void UGWTPlayerProgression::ApplyUnlock(const FGWTUnlockData& Unlock)
{
    // Apply the unlock to game systems
    switch (Unlock.Type)
    {
    case EGWTUnlockableType::NodeType:
        // Unlock spell node type
        if (Unlock.ContentID == FName("EffectNode"))
        {
            AvailableNodeTypes.AddUnique(UGWTEffectNode::StaticClass());

            // Add to grimoire if available
            if (Grimoire)
            {
                Grimoire->UnlockNodeType(UGWTEffectNode::StaticClass());
            }
        }
        else if (Unlock.ContentID == FName("ConditionNode"))
        {
            AvailableNodeTypes.AddUnique(UGWTConditionNode::StaticClass());

            // Add to grimoire if available
            if (Grimoire)
            {
                Grimoire->UnlockNodeType(UGWTConditionNode::StaticClass());
            }
        }
        else if (Unlock.ContentID == FName("VariableNode"))
        {
            AvailableNodeTypes.AddUnique(UGWTVariableNode::StaticClass());

            // Add to grimoire if available
            if (Grimoire)
            {
                Grimoire->UnlockNodeType(UGWTVariableNode::StaticClass());
            }
        }
        else if (Unlock.ContentID == FName("FlowNode"))
        {
            AvailableNodeTypes.AddUnique(UGWTFlowNode::StaticClass());

            // Add to grimoire if available
            if (Grimoire)
            {
                Grimoire->UnlockNodeType(UGWTFlowNode::StaticClass());
            }
        }
        break;

    case EGWTUnlockableType::Element:
        // Unlock elemental type
        // This would be implemented based on how elements are stored
        break;

    case EGWTUnlockableType::Equipment:
        // Unlock equipment type
        // This would be implemented based on inventory system
        break;

    case EGWTUnlockableType::Room:
        // Unlock room feature
        // This would be implemented based on level generation system
        break;
    }
}