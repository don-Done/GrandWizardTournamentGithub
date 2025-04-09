// GWTSpellEditorWidget.h
// Visual spell programming interface

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GWTTypes.h"
#include "GWTSpellEditorWidget.generated.h"

// Forward declarations
class UCanvasPanel;
class UScrollBox;
class UVerticalBox;
class UEditableTextBox;
class UButton;
class UGWTSpell;
class UGWTSpellNode;
class UGWTNodeWidget;
class UGWTConnectionLineWidget;
class UGWTGrimoire;

/**
 * Spell editor widget for Grand Wizard Tournament
 * Provides a visual programming interface for creating and editing spells
 * Central to the educational aspect of the game, teaching programming concepts
 */
UCLASS()
class GWT_API UGWTSpellEditorWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UGWTSpellEditorWidget(const FObjectInitializer& ObjectInitializer);
    
    // UI components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCanvasPanel* EditorCanvas;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox* NodePaletteScrollBox;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* PropertyEditorBox;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UEditableTextBox* SpellNameTextBox;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* SaveButton;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* TestButton;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* ClearButton;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* CategoryBox;
    
    // Editor state
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    UGWTSpell* CurrentSpell;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    UGWTNodeWidget* SelectedNode;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    TMap<UGWTSpellNode*, UGWTNodeWidget*> NodeWidgets;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    TArray<UGWTConnectionLineWidget*> ConnectionLines;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    bool bIsDraggingConnection;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    UGWTNodeWidget* ConnectionSourceNode;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    int32 ConnectionSourcePort;
    
    UPROPERTY(BlueprintReadOnly, Category="Editor")
    FVector2D CurrentMousePosition;
    
    // Grimoire reference
    UPROPERTY(BlueprintReadWrite, Category="References")
    UGWTGrimoire* Grimoire;
    
    // Methods
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void SetSpell(UGWTSpell* Spell);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    UGWTNodeWidget* AddNode(TSubclassOf<UGWTSpellNode> NodeClass, FVector2D Position);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void RemoveNode(UGWTNodeWidget* NodeWidget);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void SelectNode(UGWTNodeWidget* NodeWidget);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void BeginConnectionDrag(UGWTNodeWidget* SourceNode, int32 OutputIndex);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void EndConnectionDrag(UGWTNodeWidget* TargetNode, int32 InputIndex);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void CancelConnectionDrag();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void UpdateConnectionLines();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void PopulateNodePalette();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void UpdatePropertyEditor();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void SaveSpell();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void TestSpell();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void ClearEditor();
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    void SetGrimoire(UGWTGrimoire* InGrimoire);
    
protected:
    // UI creation helpers
    UFUNCTION(BlueprintCallable, Category="Editor")
    UWidget* CreateNodeCategoryWidget(const FText& CategoryName);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    UWidget* CreateNodePaletteItem(TSubclassOf<UGWTSpellNode> NodeClass);
    
    UFUNCTION(BlueprintCallable, Category="Editor")
    UWidget* CreatePropertyWidget(UGWTSpellNode* Node, FName PropertyName);
    
    // Event handlers
    UFUNCTION()
    void OnSaveButtonClicked();
    
    UFUNCTION()
    void OnTestButtonClicked();
    
    UFUNCTION()
    void OnClearButtonClicked();
    
    UFUNCTION()
    void OnNodeDragDetected(UGWTNodeWidget* NodeWidget, const FPointerEvent& MouseEvent);
    
    UFUNCTION()
    void OnNodeDragEnd(UGWTNodeWidget* NodeWidget, const FPointerEvent& MouseEvent);
    
    // Connection line management
    UGWTConnectionLineWidget* CreateConnectionLine();
    void RemoveConnectionLine(UGWTConnectionLineWidget* LineWidget);
    void ClearConnectionLines();
    
    // Connection verification
    bool CanConnectNodes(UGWTNodeWidget* SourceNode, UGWTNodeWidget* TargetNode) const;
    
    // Find node at position
    UGWTNodeWidget* FindNodeAtPosition(FVector2D Position) const;
};