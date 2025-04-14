// UGWTSpell.h
// Complete spell composed of nodes

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTSpell.generated.h"

// Forward declarations
class UGWTSpellNode;
class UGWTSpellExecutionContext;

/**
 * Represents a complete spell composed of multiple connected nodes
 * Acts as a container for the node graph and handles spell execution
 * This is the primary object players will create and modify
 */
UCLASS(BlueprintType)
class GWT_API UGWTSpell : public UObject
{
    GENERATED_BODY()

public:
    UGWTSpell();

    // Spell identity
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell")
    FText SpellName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell")
    FText SpellDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell")
    UTexture2D* SpellIcon;

    // Nodes
    UPROPERTY()
    TArray<UGWTSpellNode*> RootNodes;

    UPROPERTY()
    TArray<UGWTSpellNode*> AllNodes;

    // Spell stats
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float TotalManaCost = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float BaseDamage = 0.0f;

    // Methods
    UFUNCTION(BlueprintCallable, Category = "Spell")
    void Cast(AActor* Caster, AActor* Target = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Spell")
    void AddNode(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Spell")
    void RemoveNode(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Spell")
    bool ValidateSpell();

    UFUNCTION(BlueprintCallable, Category = "Spell")
    float CalculateManaCost();

    UFUNCTION(BlueprintCallable, Category = "Spell")
    float CalculateBaseDamage();

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveToString(FString& OutString);

    UFUNCTION(BlueprintCallable, Category = "Save")
    static UGWTSpell* LoadFromString(const FString& SpellData);

    // Complexity analysis for educational tracking
    UFUNCTION(BlueprintCallable, Category = "Analysis")
    int32 CountNodes() const;

    UFUNCTION(BlueprintCallable, Category = "Analysis")
    int32 CountConnections() const;

    UFUNCTION(BlueprintCallable, Category = "Analysis")
    TArray<UGWTSpellNode*> GetNodesOfType(EGWTSpellComponentType NodeType) const;

    // Editor utilities
    UFUNCTION(BlueprintCallable, Category = "Editor")
    UGWTSpellNode* FindNodeByID(const FGuid& NodeID) const;

    UFUNCTION(BlueprintCallable, Category = "Editor")
    void UpdateNodeConnections();
};