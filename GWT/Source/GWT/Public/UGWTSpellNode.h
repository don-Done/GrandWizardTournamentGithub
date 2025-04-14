// UGWTSpellNode.h
// Base class for all spell nodes in the visual programming system

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWTTypes.h"
#include "UGWTSpellNode.generated.h"

// Forward declarations
class UGWTSpellExecutionContext;

/**
 * Base class for all spell nodes in the visual programming system
 * Nodes can be connected to create complex spells
 */
UCLASS(Abstract, BlueprintType)
class GWT_API UGWTSpellNode : public UObject
{
    GENERATED_BODY()

public:
    UGWTSpellNode();

    // Node identity
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Node")
    FGuid NodeID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Node")
    FText NodeTitle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Node")
    FText NodeDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Node")
    FText NodeCategory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Node")
    FLinearColor NodeColor;

    // Node connections
    UPROPERTY()
    TArray<UGWTSpellNode*> InputNodes;

    UPROPERTY()
    TArray<UGWTSpellNode*> OutputNodes;

    // Node position in editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Editor")
    FVector2D NodePosition;

    // Execution methods
    UFUNCTION(BlueprintCallable, Category = "Execution")
    virtual void Execute(UGWTSpellExecutionContext* Context);

    // Validation methods
    UFUNCTION(BlueprintCallable, Category = "Validation")
    virtual bool ValidateConnections();

    // Connection methods
    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual bool CanConnectInput(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual bool CanConnectOutput(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual void AddInputConnection(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual void AddOutputConnection(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual void RemoveInputConnection(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    virtual void RemoveOutputConnection(UGWTSpellNode* Node);

    // Serialization
    UFUNCTION(BlueprintCallable, Category = "Serialization")
    virtual void Serialize(FArchive& Ar);

    // Utility methods
    UFUNCTION(BlueprintCallable, Category = "Utility")
    virtual FString GetNodeTypeAsString() const;

    UFUNCTION(BlueprintCallable, Category = "Utility")
    virtual EGWTSpellComponentType GetNodeType() const;
};