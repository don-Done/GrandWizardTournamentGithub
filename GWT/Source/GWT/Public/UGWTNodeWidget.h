// UGWTNodeWidget.h
// Visual representation of a spell node

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UGWTNodeWidget.generated.h"

// Forward declarations
class UBorder;
class UTextBlock;
class UVerticalBox;
class UGWTSpellNode;
class UGWTSpellEditorWidget;
struct FPointerEvent;

/**
 * Node widget for Grand Wizard Tournament spell editor
 * Provides a visual representation of a spell node
 * Handles user interaction with nodes in the editor
 */
UCLASS()
class GWT_API UGWTNodeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UGWTNodeWidget(const FObjectInitializer& ObjectInitializer);

    // UI elements
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* NodeBorder;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* TitleText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* InputsBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* OutputsBox;

    // Node data
    UPROPERTY(BlueprintReadOnly, Category = "Node")
    UGWTSpellNode* SpellNode;

    UPROPERTY(BlueprintReadOnly, Category = "Node")
    UGWTSpellEditorWidget* EditorWidget;

    UPROPERTY(BlueprintReadOnly, Category = "Node")
    bool bIsSelected = false;

    UPROPERTY(BlueprintReadOnly, Category = "Node")
    bool bIsDragging = false;

    UPROPERTY(BlueprintReadOnly, Category = "Node")
    FVector2D DragOffset;

    // Events
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNodeDragDetected, UGWTNodeWidget*, const FPointerEvent&);
    FOnNodeDragDetected OnNodeDragDetected;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNodeDragEnd, UGWTNodeWidget*, const FPointerEvent&);
    FOnNodeDragEnd OnNodeDragEnd;

    // Methods
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Node")
    void SetSpellNode(UGWTSpellNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Node")
    void UpdateNodeVisuals();

    UFUNCTION(BlueprintCallable, Category = "Node")
    void SetIsSelected(bool bSelected);

    UFUNCTION(BlueprintCallable, Category = "Node")
    void OnNodeMoved(FVector2D NewPosition);

    UFUNCTION(BlueprintCallable, Category = "Node")
    FVector2D GetInputPortPosition(int32 InputIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Node")
    FVector2D GetOutputPortPosition(int32 OutputIndex) const;

    // Input handling
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
    // UI creation helpers
    void CreateInputPorts();
    void CreateOutputPorts();

    UWidget* CreatePortWidget(bool bIsInput, int32 PortIndex);

    // Interaction handlers
    void HandleInputPortClicked(int32 PortIndex);
    void HandleOutputPortClicked(int32 PortIndex);

    // Helper methods
    UCanvasPanelSlot* GetCanvasSlot() const;
};