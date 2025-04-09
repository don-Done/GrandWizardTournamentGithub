// GWTPlayerCharacter.h
// Player-specific character implementation

#pragma once

#include "CoreMinimal.h"
#include "GWTCharacter.h"
#include "GWTPlayerCharacter.generated.h"

// Forward declarations
class USpringArmComponent;
class UCameraComponent;
class UGWTInventory;
class UGWTGrimoire;
class UGWTPlayerProgression;

/**
 * Player character class for Grand Wizard Tournament
 * Extends the base character with player-specific functionality
 * Handles player movement, spell casting, and equipment
 */
UCLASS()
class GWT_API AGWTPlayerCharacter : public AGWTCharacter
{
    GENERATED_BODY()
    
public:
    AGWTPlayerCharacter();
    
    // Camera components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    USpringArmComponent* CameraBoom;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    UCameraComponent* FollowCamera;
    
    // Player systems
    UPROPERTY(BlueprintReadOnly, Category="Player")
    UGWTInventory* Inventory;
    
    UPROPERTY(BlueprintReadOnly, Category="Player")
    UGWTGrimoire* Grimoire;
    
    UPROPERTY(BlueprintReadOnly, Category="Player")
    UGWTPlayerProgression* Progression;
    
    // Spell casting properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Casting")
    float CastingRange = 1000.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Casting")
    bool bIsCasting = false;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Casting")
    float CastingSpeed = 1.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Casting")
    int32 ActiveSpellIndex = 0;
    
    // Override base class methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void OnDeath() override;
    
    // Movement methods
    UFUNCTION(BlueprintCallable, Category="Movement")
    void MoveForward(float Value);
    
    UFUNCTION(BlueprintCallable, Category="Movement")
    void MoveRight(float Value);
    
    UFUNCTION(BlueprintCallable, Category="Movement")
    void Turn(float Value);
    
    UFUNCTION(BlueprintCallable, Category="Movement")
    void LookUp(float Value);
    
    // Spell casting
    UFUNCTION(BlueprintCallable, Category="Spells")
    void StartSpellCast();
    
    UFUNCTION(BlueprintCallable, Category="Spells")
    void EndSpellCast();
    
    UFUNCTION(BlueprintCallable, Category="Spells")
    void CycleActiveSpell(bool bForward);
    
    UFUNCTION(BlueprintCallable, Category="Spells")
    void CastSpell();
    
    // Equipment management
    UFUNCTION(BlueprintCallable, Category="Equipment")
    void EquipItem(class UGWTItem* Item);
    
    UFUNCTION(BlueprintCallable, Category="Equipment")
    void UnequipItem(EGWTEquipmentSlot Slot);
    
    // UI interaction
    UFUNCTION(BlueprintCallable, Category="UI")
    void ToggleInventory();
    
    UFUNCTION(BlueprintCallable, Category="UI")
    void ToggleSpellEditor();
    
    // Interact with world
    UFUNCTION(BlueprintCallable, Category="Interaction")
    void Interact();
    
    // Get camera target for spell aiming
    UFUNCTION(BlueprintCallable, Category="Casting")
    bool GetCameraTarget(FHitResult& OutHitResult, float MaxDistance = 10000.0f);
    
    // Override status effect handling for player-specific behavior
    virtual void ApplyStatusEffect(FGWTStatusEffect Effect) override;
    
protected:
    // Input flags
    bool bMoveForward = false;
    bool bMoveRight = false;
    
    // Visual feedback for spell casting
    UFUNCTION(BlueprintImplementableEvent, Category="Casting")
    void OnBeginCasting();
    
    UFUNCTION(BlueprintImplementableEvent, Category="Casting")
    void OnEndCasting();
    
    UFUNCTION(BlueprintImplementableEvent, Category="Casting")
    void OnSpellCast(int32 SpellIndex);
    
    // Initialize player systems
    virtual void InitializeInventory();
    virtual void InitializeGrimoire();
    virtual void InitializeProgression();
};