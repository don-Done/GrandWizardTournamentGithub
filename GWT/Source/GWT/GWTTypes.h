// GWTTypes.h
// Contains common enums and structs used throughout the Grand Wizard Tournament game

#pragma once

#include "CoreMinimal.h"
#include "GWTTypes.generated.h"

// Forward declarations
class AActor;
class UGWTSpell;
class UGWTSpellNode;

// Element types for spells
UENUM(BlueprintType)
enum class EGWTElementType : uint8
{
    None    UMETA(DisplayName = "None"),
    Fire    UMETA(DisplayName = "Fire"),
    Water   UMETA(DisplayName = "Water"),
    Earth   UMETA(DisplayName = "Earth"),
    Air     UMETA(DisplayName = "Air"),
    Lightning UMETA(DisplayName = "Lightning"),
    Ice     UMETA(DisplayName = "Ice"),
    Light   UMETA(DisplayName = "Light"),
    Void    UMETA(DisplayName = "Void")
};

// Room types
UENUM(BlueprintType)
enum class EGWTRoomType : uint8
{
    Empty   UMETA(DisplayName = "Empty"),
    Combat  UMETA(DisplayName = "Combat"),
    Treasure UMETA(DisplayName = "Treasure"),
    Shop    UMETA(DisplayName = "Shop"),
    Puzzle  UMETA(DisplayName = "Puzzle"),
    Boss    UMETA(DisplayName = "Boss")
};

// Direction enum
UENUM(BlueprintType)
enum class EGWTDirection : uint8
{
    North   UMETA(DisplayName = "North"),
    East    UMETA(DisplayName = "East"),
    South   UMETA(DisplayName = "South"),
    West    UMETA(DisplayName = "West"),
    Up      UMETA(DisplayName = "Up"),
    Down    UMETA(DisplayName = "Down")
};

// Plane type for cube rotation
UENUM(BlueprintType)
enum class EGWTPlaneType : uint8
{
    XY      UMETA(DisplayName = "XY Plane"),
    XZ      UMETA(DisplayName = "XZ Plane"),
    YZ      UMETA(DisplayName = "YZ Plane")
};

// Status effect types
UENUM(BlueprintType)
enum class EGWTStatusEffectType : uint8
{
    Burning     UMETA(DisplayName = "Burning"),
    Frozen      UMETA(DisplayName = "Frozen"),
    Poisoned    UMETA(DisplayName = "Poisoned"),
    Electrified UMETA(DisplayName = "Electrified"),
    Shielded    UMETA(DisplayName = "Shielded"),
    SpeedBoosted UMETA(DisplayName = "Speed Boosted"),
    ManaRegen   UMETA(DisplayName = "Mana Regeneration"),
    HealthRegen UMETA(DisplayName = "Health Regeneration")
};

// Status effect
USTRUCT(BlueprintType)
struct FGWTStatusEffect
{
    GENERATED_BODY()
    
    // Type of effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGWTStatusEffectType EffectType = EGWTStatusEffectType::Burning;
    
    // How long the effect lasts
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 5.0f;
    
    // Power/intensity of the effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Strength = 1.0f;
    
    // Who/what caused this effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Causer = nullptr;
    
    // Time remaining until effect expires
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeRemaining = 5.0f;
};

// Equipment slots
UENUM(BlueprintType)
enum class EGWTEquipmentSlot : uint8
{
    Wand    UMETA(DisplayName = "Wand"),
    Robe    UMETA(DisplayName = "Robe"),
    Hat     UMETA(DisplayName = "Hat"),
    Ring1   UMETA(DisplayName = "Ring 1"),
    Ring2   UMETA(DisplayName = "Ring 2"),
    Broom   UMETA(DisplayName = "Broom")
};

// Enemy types
UENUM(BlueprintType)
enum class EGWTEnemyType : uint8
{
    Goblin      UMETA(DisplayName = "Goblin"),
    Rat         UMETA(DisplayName = "Rat"),
    Slime       UMETA(DisplayName = "Slime"),
    Skeleton    UMETA(DisplayName = "Skeleton"),
    DarkWizard  UMETA(DisplayName = "Dark Wizard"),
    Mimic       UMETA(DisplayName = "Mimic"),
    EliteGoblin UMETA(DisplayName = "Elite Goblin"),
    EliteSkeleton UMETA(DisplayName = "Elite Skeleton"),
    EliteWizard UMETA(DisplayName = "Elite Wizard"),
    Boss        UMETA(DisplayName = "Boss")
};

// Item rarity
UENUM(BlueprintType)
enum class EGWTItemRarity : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

// Stat types for items
UENUM(BlueprintType)
enum class EGWTStatType : uint8
{
    MaxHealth       UMETA(DisplayName = "Max Health"),
    MaxMana         UMETA(DisplayName = "Max Mana"),
    HealthRegen     UMETA(DisplayName = "Health Regeneration"),
    ManaRegen       UMETA(DisplayName = "Mana Regeneration"),
    SpellDamage     UMETA(DisplayName = "Spell Damage"),
    SpellRange      UMETA(DisplayName = "Spell Range"),
    MovementSpeed   UMETA(DisplayName = "Movement Speed"),
    DefenseRating   UMETA(DisplayName = "Defense Rating")
};

// Item stat
USTRUCT(BlueprintType)
struct FGWTItemStat
{
    GENERATED_BODY()
    
    // Type of stat
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGWTStatType StatType = EGWTStatType::MaxHealth;
    
    // Value of the stat
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value = 0.0f;
    
    // Whether value is a percentage or flat
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPercentage = false;
};

// Consumable types
UENUM(BlueprintType)
enum class EGWTConsumableType : uint8
{
    HealthPotion    UMETA(DisplayName = "Health Potion"),
    ManaPotion      UMETA(DisplayName = "Mana Potion"),
    StatusEffect    UMETA(DisplayName = "Status Effect"),
    TemporaryBoost  UMETA(DisplayName = "Temporary Boost"),
    SpellScroll     UMETA(DisplayName = "Spell Scroll")
};

// Spell component types
UENUM(BlueprintType)
enum class EGWTSpellComponentType : uint8
{
    Magic       UMETA(DisplayName = "Magic"),
    Trigger     UMETA(DisplayName = "Trigger"),
    Effect      UMETA(DisplayName = "Effect"),
    Condition   UMETA(DisplayName = "Condition"),
    Variable    UMETA(DisplayName = "Variable"),
    Flow        UMETA(DisplayName = "Flow")
};

// Trigger types for spell nodes
UENUM(BlueprintType)
enum class EGWTTriggerType : uint8
{
    OnCast          UMETA(DisplayName = "On Cast"),
    OnHit           UMETA(DisplayName = "On Hit"),
    OnEnemyEnter    UMETA(DisplayName = "On Enemy Enter"),
    OnHealthBelow   UMETA(DisplayName = "On Health Below"),
    OnManaAbove     UMETA(DisplayName = "On Mana Above"),
    OnTimerExpired  UMETA(DisplayName = "On Timer Expired")
};

// Effect types for spell nodes
UENUM(BlueprintType)
enum class EGWTEffectType : uint8
{
    Damage      UMETA(DisplayName = "Damage"),
    Heal        UMETA(DisplayName = "Heal"),
    ApplyStatus UMETA(DisplayName = "Apply Status"),
    Teleport    UMETA(DisplayName = "Teleport"),
    Knockback   UMETA(DisplayName = "Knockback"),
    Shield      UMETA(DisplayName = "Shield"),
    Summon      UMETA(DisplayName = "Summon")
};

// Condition types for spell nodes
UENUM(BlueprintType)
enum class EGWTConditionType : uint8
{
    HealthCheck         UMETA(DisplayName = "Health Check"),
    ManaCheck           UMETA(DisplayName = "Mana Check"),
    DistanceCheck       UMETA(DisplayName = "Distance Check"),
    ElementalCheck      UMETA(DisplayName = "Elemental Check"),
    StatusEffectCheck   UMETA(DisplayName = "Status Effect Check"),
    RandomChance        UMETA(DisplayName = "Random Chance")
};

// Flow types for spell nodes
UENUM(BlueprintType)
enum class EGWTFlowType : uint8
{
    Repeat      UMETA(DisplayName = "Repeat"),
    While       UMETA(DisplayName = "While"),
    ForEach     UMETA(DisplayName = "For Each"),
    Delay       UMETA(DisplayName = "Delay")
};

// Variable types for spell nodes
UENUM(BlueprintType)
enum class EGWTVariableType : uint8
{
    Float   UMETA(DisplayName = "Float"),
    Int     UMETA(DisplayName = "Integer"),
    Bool    UMETA(DisplayName = "Boolean"),
    Vector  UMETA(DisplayName = "Vector"),
    Target  UMETA(DisplayName = "Target")
};

// Variable value struct for spell execution
USTRUCT(BlueprintType)
struct FGWTVariableValue
{
    GENERATED_BODY()
    
    // Type of the variable
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGWTVariableType Type = EGWTVariableType::Float;
    
    // Float value (if Type is Float)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValue = 0.0f;
    
    // Integer value (if Type is Int)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IntValue = 0;
    
    // Boolean value (if Type is Bool)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool BoolValue = false;
    
    // Vector value (if Type is Vector)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector VectorValue = FVector::ZeroVector;
    
    // Target value (if Type is Target)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* TargetValue = nullptr;
};

// Rank tiers for competitive play
UENUM(BlueprintType)
enum class EGWTRankTier : uint8
{
    Bronze      UMETA(DisplayName = "Bronze"),
    Silver      UMETA(DisplayName = "Silver"),
    Gold        UMETA(DisplayName = "Gold"),
    Platinum    UMETA(DisplayName = "Platinum"),
    Diamond     UMETA(DisplayName = "Diamond"),
    Grandmaster UMETA(DisplayName = "Grandmaster")
};

// Wave configuration for enemy spawning
USTRUCT(BlueprintType)
struct FGWTEnemyWaveInfo
{
    GENERATED_BODY()
    
    // Wave number
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WaveNumber = 1;
    
    // Enemy types to spawn in this wave
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<class AGWTEnemyCharacter>> EnemyTypes;
    
    // How frequently enemies spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnFrequency = 5.0f;
    
    // Multiplier for enemy difficulty
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DifficultyMultiplier = 1.0f;
};

// Programming concept definition
USTRUCT(BlueprintType)
struct FGWTProgrammingConcept
{
    GENERATED_BODY()
    
    // Unique concept identifier
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ConceptName;
    
    // Display title for the concept
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ConceptTitle;
    
    // Explanation of the concept
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ConceptDescription;
    
    // How difficult the concept is (1-10)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DifficultyLevel = 1;
    
    // Concepts that should be learned first
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> PrerequisiteConcepts;
    
    // Tips for understanding the concept
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FText> Hints;
};

// Spell complexity snapshot for educational tracking
USTRUCT(BlueprintType)
struct FGWTSpellComplexitySnapshot
{
    GENERATED_BODY()
    
    // Which wave number the snapshot was taken
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WaveNumber = 1;
    
    // Number of nodes in the spell
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NodeCount = 0;
    
    // Number of connections between nodes
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ConnectionCount = 0;
    
    // Programming concepts used in the spell
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> ConceptsUsed;
    
    // When the snapshot was taken
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeStamp = 0.0f;
};

// Tutorial step
USTRUCT(BlueprintType)
struct FGWTTutorialStep
{
    GENERATED_BODY()
    
    // Title of the step
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText StepTitle;
    
    // Instructions for the step
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText StepDescription;
    
    // UI element to highlight
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName HighlightElement;
    
    // What event completes this step
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CompletionTrigger;
    
    // Whether to automatically go to next step
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAutoAdvance = false;
};

// Tutorial sequence
USTRUCT(BlueprintType)
struct FGWTTutorialSequence
{
    GENERATED_BODY()
    
    // Unique sequence identifier
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SequenceName;
    
    // Display title for the sequence
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText SequenceTitle;
    
    // Steps in this tutorial
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGWTTutorialStep> Steps;
};