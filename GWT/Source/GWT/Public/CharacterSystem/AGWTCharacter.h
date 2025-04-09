// GWTCharacter.h
// Base character class with stats

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GWTTypes.h"
#include "GWTCharacter.generated.h"

/**
 * Base character class for Grand Wizard Tournament
 * Serves as the foundation for both player and enemy characters
 * Handles health, mana, and status effects
 */
UCLASS()
class GWT_API AGWTCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    AGWTCharacter();
    
    // Character stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MaxHealth = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float CurrentHealth = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MaxMana = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float CurrentMana = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float ManaRegenRate = 5.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
    float MovementSpeed = 600.0f;
    
    // Status effects
    UPROPERTY(BlueprintReadOnly, Category="Effects")
    TArray<FGWTStatusEffect> ActiveEffects;
    
    // Visual Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* WandMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* HatMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USkeletalMeshComponent* RobeMesh;
    
    // Methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category="Health")
    virtual void TakeDamage(float Damage, EGWTElementType DamageType, AActor* DamageCauser);
    
    UFUNCTION(BlueprintCallable, Category="Health")
    virtual void Heal(float Amount);
    
    UFUNCTION(BlueprintCallable, Category="Mana")
    virtual void ConsumeMana(float Amount);
    
    UFUNCTION(BlueprintCallable, Category="Mana")
    virtual void RegenerateMana(float Amount);
    
    UFUNCTION(BlueprintCallable, Category="Effects")
    virtual void ApplyStatusEffect(FGWTStatusEffect Effect);
    
    UFUNCTION(BlueprintCallable, Category="Effects")
    virtual void RemoveStatusEffect(EGWTStatusEffectType EffectType);
    
    UFUNCTION(BlueprintCallable, Category="Effects")
    virtual void ProcessStatusEffects(float DeltaTime);
    
    UFUNCTION(BlueprintCallable, Category="Death")
    virtual void OnDeath();
    
    // Equipment methods
    UFUNCTION(BlueprintCallable, Category="Equipment")
    virtual void EquipWand(class UGWTWand* Wand);
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    virtual void EquipHat(class UGWTHat* Hat);
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    virtual void EquipRobe(class UGWTRobe* Robe);
    
    // Status getters
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Stats")
    float GetHealthPercent() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Stats")
    float GetManaPercent() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Effects")
    bool HasStatusEffect(EGWTStatusEffectType EffectType) const;
    
protected:
    // Apply damage over time from status effects
    void ApplyStatusEffectDamage(float DeltaTime);
    
    // Update status effect durations
    void UpdateStatusEffectDurations(float DeltaTime);
    
    // Mana regeneration timer
    FTimerHandle ManaRegenTimerHandle;
    
    // Mana regeneration tick
    UFUNCTION()
    void ManaRegenTick();
};