// AGWTEnemyCharacter.h
// Base class for all enemy types

#pragma once

#include "CoreMinimal.h"
#include "AGWTCharacter.h"
#include "GWTTypes.h"
#include "AGWTEnemyCharacter.generated.h"

// Forward declarations
class UAIPerceptionComponent;
class UPawnSensingComponent;
class UGWTSpell;

/**
 * Base enemy character class for Grand Wizard Tournament
 * Provides AI behavior, perception, and combat functionality
 * Serves as the foundation for all different enemy types
 */
UCLASS()
class GWT_API AGWTEnemyCharacter : public AGWTCharacter
{
    GENERATED_BODY()

public:
    AGWTEnemyCharacter();

    // Enemy properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    EGWTEnemyType EnemyType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    int32 ExperienceValue = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    int32 GoldValue = 5;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float DetectionRadius = 1000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float AttackRange = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float MaxAggroRange = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float MinAttackCooldown = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
    float MaxAttackCooldown = 4.0f;

    // AI components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAIPerceptionComponent* PerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPawnSensingComponent* SensingComponent;

    // Current target
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    AActor* CurrentTarget;

    // AI state flags
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsAggressive = true;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsPatrolling = true;

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool bIsAttacking = false;

    // Patrol points
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    TArray<FVector> PatrolPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Patrol")
    int32 CurrentPatrolIndex = 0;

    // Spells
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells")
    TArray<UGWTSpell*> EnemySpells;

    UPROPERTY(BlueprintReadOnly, Category = "Spells")
    int32 CurrentSpellIndex = 0;

    // Methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnDeath() override;

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void DetectPlayer();

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void ChaseTarget();

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void AttackTarget();

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void Patrol();

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void SelectSpell();

    UFUNCTION(BlueprintCallable, Category = "AI")
    virtual void CastSpell();

    UFUNCTION(BlueprintCallable, Category = "Rewards")
    virtual void DropLoot();

    UFUNCTION(BlueprintCallable, Category = "Rewards")
    virtual void GrantExperience();

    // Sensory events
    UFUNCTION()
    void OnSeePlayer(APawn* Pawn);

    UFUNCTION()
    void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

    // Set difficulty based on wave
    UFUNCTION(BlueprintCallable, Category = "Difficulty")
    void SetDifficultyLevel(int32 WaveNumber);

    // Check if target is in range
    UFUNCTION(BlueprintCallable, Category = "AI")
    bool IsTargetInRange(float Range) const;

    // Get closest player
    UFUNCTION(BlueprintCallable, Category = "AI")
    APawn* GetClosestPlayer() const;

protected:
    // AI behavior timers
    FTimerHandle AttackTimerHandle;
    FTimerHandle PatrolTimerHandle;

    // Current attack cooldown
    float CurrentAttackCooldown = 2.0f;

    // Initialize AI behavior
    virtual void InitializeAI();

    // Set up patrol points
    virtual void SetupPatrolPoints();

    // Choose a random attack cooldown
    virtual float GetRandomAttackCooldown() const;

    // Timer callbacks
    UFUNCTION()
    void AttackTimerCallback();

    UFUNCTION()
    void PatrolTimerCallback();

    // Get movement destination for patrol
    virtual FVector GetNextPatrolPoint() const;

    // Create and initialize enemy spells
    virtual void InitializeSpells();
};