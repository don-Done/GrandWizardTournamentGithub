// UGWTGrimoire.h
// Manages the player's spell collection

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTGrimoire.generated.h"

// Forward declarations
class UGWTSpell;
class UGWTSpellNode;

/**
 * Grimoire class that manages the player's collection of spells
 * Acts as a library for all spells the player has created or learned
 * Also tracks available spell components (nodes) the player has unlocked
 */
UCLASS(BlueprintType)
class GWT_API UGWTGrimoire : public UObject
{
    GENERATED_BODY()

public:
    UGWTGrimoire();

    // Spell collection
    UPROPERTY(BlueprintReadOnly, Category = "Spells")
    TArray<UGWTSpell*> Spells;

    // Available components
    UPROPERTY(BlueprintReadOnly, Category = "Components")
    TArray<TSubclassOf<UGWTSpellNode>> UnlockedNodeTypes;

    // Methods
    UFUNCTION(BlueprintCallable, Category = "Spells")
    void AddSpell(UGWTSpell* Spell);

    UFUNCTION(BlueprintCallable, Category = "Spells")
    void RemoveSpell(UGWTSpell* Spell);

    UFUNCTION(BlueprintCallable, Category = "Spells")
    UGWTSpell* GetSpell(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Components")
    void UnlockNodeType(TSubclassOf<UGWTSpellNode> NodeType);

    UFUNCTION(BlueprintCallable, Category = "Components")
    bool IsNodeTypeUnlocked(TSubclassOf<UGWTSpellNode> NodeType);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveToString(FString& OutString);

    UFUNCTION(BlueprintCallable, Category = "Save")
    static UGWTGrimoire* LoadFromString(const FString& GrimoireData);

    // Utility methods
    UFUNCTION(BlueprintCallable, Category = "Spells")
    int32 GetSpellCount() const;

    UFUNCTION(BlueprintCallable, Category = "Spells")
    UGWTSpell* CreateNewSpell(const FString& SpellName = TEXT("New Spell"));

    UFUNCTION(BlueprintCallable, Category = "Components")
    TArray<TSubclassOf<UGWTSpellNode>> GetUnlockedNodeTypes() const;

    UFUNCTION(BlueprintCallable, Category = "Components")
    TArray<FText> GetUnlockedNodeCategories() const;

    UFUNCTION(BlueprintCallable, Category = "Components")
    TArray<TSubclassOf<UGWTSpellNode>> GetNodeTypesInCategory(const FText& Category) const;
};