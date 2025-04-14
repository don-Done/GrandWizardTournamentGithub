// UGWTSpellComponent.h
// Item that unlocks spell nodes in the Grimoire

#pragma once

#include "CoreMinimal.h"
#include "UGWTItem.h"
#include "GWTTypes.h"
#include "UGWTSpellComponent.generated.h"

// Forward declarations
class UGWTSpellNode;

/**
 * Spell Component item class for Grand Wizard Tournament
 * Represents items that unlock spell nodes when used
 * Core to the spell crafting progression system
 */
UCLASS(BlueprintType)
class GWT_API UGWTSpellComponent : public UGWTItem
{
    GENERATED_BODY()

public:
    UGWTSpellComponent();

    // Spell component properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    TSubclassOf<UGWTSpellNode> NodeType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    EGWTSpellComponentType ComponentType = EGWTSpellComponentType::Magic;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    EGWTElementType ElementType = EGWTElementType::None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    TArray<TSubclassOf<UGWTSpellNode>> AdditionalNodeTypes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    bool bIsPermanentUnlock = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell Component")
    int32 UsageCount = 0; // 0 = unlimited if permanent

    // Override base item methods
    virtual bool Use(AGWTCharacter* Character) override;
    virtual FString GetItemDescription() const override;
    virtual UGWTItem* CreateCopy() const override;

    // Spell component-specific methods
    UFUNCTION(BlueprintCallable, Category = "Spell Component")
    bool UnlockNodeInGrimoire(class UGWTGrimoire* Grimoire);

    UFUNCTION(BlueprintCallable, Category = "Spell Component")
    FString GetComponentDescription() const;

    UFUNCTION(BlueprintCallable, Category = "Spell Component")
    FLinearColor GetComponentColor() const;
};