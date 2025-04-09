// GWTSpellEditorWidget.cpp
// Implementation of the visual spell programming interface

#include "GWTSpellEditorWidget.h"
#include "GWTSpell.h"
#include "GWTSpellNode.h"
#include "GWTNodeWidget.h"
#include "GWTConnectionLineWidget.h"
#include "GWTGrimoire.h"
#include "GWTPlayerController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

UGWTSpellEditorWidget::UGWTSpellEditorWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize editor state
    CurrentSpell = nullptr;
    SelectedNode = nullptr;
    bIsDraggingConnection = false;
    ConnectionSourceNode = nullptr;
    ConnectionSourcePort = -1;
    CurrentMousePosition = FVector2D::ZeroVector;
    Grimoire = nullptr;
}

void UGWTSpellEditorWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Bind button events
    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &UGWTSpellEditorWidget::OnSaveButtonClicked);
    }
    
    if (TestButton)
    {
        TestButton->OnClicked.AddDynamic(this, &UGWTSpellEditorWidget::OnTestButtonClicked);
    }
    
    if (ClearButton)
    {
        ClearButton->OnClicked.AddDynamic(this, &UGWTSpellEditorWidget::OnClearButtonClicked);
    }
    
    // Get grimoire reference from player controller if not already set
    if (!Grimoire)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->IsA(AGWTPlayerController::StaticClass()))
        {
            AGWTPlayerController* GWTPC = Cast<AGWTPlayerController>(PC);
            if (GWTPC)
            {
                Grimoire = GWTPC->PlayerGrimoire;
            }
        }
    }
    
    // Populate the node palette
    PopulateNodePalette();
    
    // Set to hidden by default
    SetVisibility(ESlateVisibility::Hidden);
    
    UE_LOG(LogTemp, Display, TEXT("Spell Editor Widget constructed"));
}

void UGWTSpellEditorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // Update connection lines
    UpdateConnectionLines();
    
    // Update dragging connection if active
    if (bIsDraggingConnection && ConnectionSourceNode)
    {
        // Update the line being dragged to follow mouse cursor
        for (UGWTConnectionLineWidget* Line : ConnectionLines)
        {
            if (Line && Line->SourceNode == ConnectionSourceNode && Line->TargetNode == nullptr)
            {
                Line->UpdateEndPoint(CurrentMousePosition);
            }
        }
    }
}

FReply UGWTSpellEditorWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Store mouse position for connection dragging
    CurrentMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    
    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UGWTSpellEditorWidget::SetSpell(UGWTSpell* Spell)
{
    // Set the current spell and refresh the editor
    if (Spell)
    {
        // Clear the current editor state
        ClearEditor();
        
        // Set the new spell
        CurrentSpell = Spell;
        
        // Update spell name field
        if (SpellNameTextBox)
        {
            SpellNameTextBox->SetText(Spell->SpellName);
        }
        
        // Create widgets for all nodes in the spell
        for (UGWTSpellNode* Node : Spell->AllNodes)
        {
            if (Node)
            {
                // Create a node widget at the saved position
                TSubclassOf<UGWTSpellNode> NodeClass = Node->GetClass();
                UGWTNodeWidget* NodeWidget = AddNode(NodeClass, Node->NodePosition);
                
                if (NodeWidget)
                {
                    // Initialize with existing node instead of creating a new one
                    NodeWidget->SpellNode = Node;
                    NodeWidget->UpdateNodeVisuals();
                }
            }
        }
        
        // Create connection lines
        UpdateConnectionLines();
        
        UE_LOG(LogTemp, Display, TEXT("Spell '%s' loaded into editor"), 
               *Spell->SpellName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot load spell: Null reference"));
    }
}

UGWTNodeWidget* UGWTSpellEditorWidget::AddNode(TSubclassOf<UGWTSpellNode> NodeClass, FVector2D Position)
{
    // Make sure editor canvas exists
    if (!EditorCanvas)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add node: Editor canvas not found"));
        return nullptr;
    }
    
    // Make sure we have a current spell
    if (!CurrentSpell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add node: No current spell"));
        return nullptr;
    }
    
    // Create the node widget
    UGWTNodeWidget* NodeWidget = CreateWidget<UGWTNodeWidget>(this, UGWTNodeWidget::StaticClass());
    
    if (NodeWidget)
    {
        // Create a spell node instance
        UGWTSpellNode* SpellNode = NewObject<UGWTSpellNode>(CurrentSpell, NodeClass);
        
        if (SpellNode)
        {
            // Add node to spell
            CurrentSpell->AddNode(SpellNode);
            
            // Set node position
            SpellNode->NodePosition = Position;
            
            // Initialize the node widget
            NodeWidget->SpellNode = SpellNode;
            NodeWidget->EditorWidget = this;
            NodeWidget->UpdateNodeVisuals();
            
            // Add to editor canvas
            EditorCanvas->AddChild(NodeWidget);
            
            // Position the node widget
            UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NodeWidget->Slot);
            if (CanvasSlot)
            {
                CanvasSlot->SetPosition(Position);
                CanvasSlot->SetSize(FVector2D(200.0f, 150.0f)); // Default size
            }
            
            // Register drag events
            NodeWidget->OnNodeDragDetected.AddUObject(this, &UGWTSpellEditorWidget::OnNodeDragDetected);
            NodeWidget->OnNodeDragEnd.AddUObject(this, &UGWTSpellEditorWidget::OnNodeDragEnd);
            
            // Track node widget
            NodeWidgets.Add(SpellNode, NodeWidget);
            
            UE_LOG(LogTemp, Display, TEXT("Added node of type %s at position (%.1f, %.1f)"), 
                   *NodeClass->GetName(), Position.X, Position.Y);
            
            // Select new node
            SelectNode(NodeWidget);
            
            return NodeWidget;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Failed to create node widget"));
    return nullptr;
}

void UGWTSpellEditorWidget::RemoveNode(UGWTNodeWidget* NodeWidget)
{
    // Make sure editor canvas exists
    if (!EditorCanvas || !NodeWidget || !NodeWidget->SpellNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot remove node: Invalid widget or node"));
        return;
    }
    
    // Make sure we have a current spell
    if (!CurrentSpell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot remove node: No current spell"));
        return;
    }
    
    // Get the spell node
    UGWTSpellNode* SpellNode = NodeWidget->SpellNode;
    
    // Remove node from spell
    CurrentSpell->RemoveNode(SpellNode);
    
    // Remove node widget from tracking
    NodeWidgets.Remove(SpellNode);
    
    // Remove node widget from canvas
    EditorCanvas->RemoveChild(NodeWidget);
    
    // Update connection lines
    UpdateConnectionLines();
    
    // Clear selection if this was the selected node
    if (SelectedNode == NodeWidget)
    {
        SelectNode(nullptr);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Removed node %s"), *SpellNode->NodeTitle.ToString());
}

void UGWTSpellEditorWidget::SelectNode(UGWTNodeWidget* NodeWidget)
{
    // Deselect current node if any
    if (SelectedNode)
    {
        SelectedNode->SetIsSelected(false);
    }
    
    // Select new node
    SelectedNode = NodeWidget;
    
    if (SelectedNode)
    {
        SelectedNode->SetIsSelected(true);
        
        // Update property editor
        UpdatePropertyEditor();
        
        UE_LOG(LogTemp, Verbose, TEXT("Selected node %s"), 
               *SelectedNode->SpellNode->NodeTitle.ToString());
    }
    else
    {
        // Clear property editor
        if (PropertyEditorBox)
        {
            PropertyEditorBox->ClearChildren();
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Node selection cleared"));
    }
}

void UGWTSpellEditorWidget::BeginConnectionDrag(UGWTNodeWidget* SourceNode, int32 OutputIndex)
{
    // Set dragging state
    bIsDraggingConnection = true;
    ConnectionSourceNode = SourceNode;
    ConnectionSourcePort = OutputIndex;
    
    // Create a temporary connection line
    UGWTConnectionLineWidget* Line = CreateConnectionLine();
    
    if (Line)
    {
        // Set source to the node output port
        Line->SourceNode = SourceNode;
        Line->SourcePortIndex = OutputIndex;
        Line->TargetNode = nullptr; // Will be set when drag ends
        Line->TargetPortIndex = -1;
        
        // Get source position
        FVector2D SourcePos = SourceNode->GetOutputPortPosition(OutputIndex);
        Line->UpdateStartPoint(SourcePos);
        Line->UpdateEndPoint(CurrentMousePosition);
        
        UE_LOG(LogTemp, Verbose, TEXT("Started connection drag from node %s port %d"), 
               *SourceNode->SpellNode->NodeTitle.ToString(), OutputIndex);
    }
}

void UGWTSpellEditorWidget::EndConnectionDrag(UGWTNodeWidget* TargetNode, int32 InputIndex)
{
    // Check if we can connect these nodes
    if (bIsDraggingConnection && ConnectionSourceNode && TargetNode && 
        CanConnectNodes(ConnectionSourceNode, TargetNode))
    {
        // Connect the nodes
        UGWTSpellNode* SourceSpellNode = ConnectionSourceNode->SpellNode;
        UGWTSpellNode* TargetSpellNode = TargetNode->SpellNode;
        
        if (SourceSpellNode && TargetSpellNode)
        {
            // Add connection
            TargetSpellNode->AddInputConnection(SourceSpellNode);
            
            UE_LOG(LogTemp, Display, TEXT("Connected node %s to %s"), 
                   *SourceSpellNode->NodeTitle.ToString(), 
                   *TargetSpellNode->NodeTitle.ToString());
            
            // Update connection lines
            UpdateConnectionLines();
        }
    }
    else
    {
        // Remove the temporary connection line
        for (int32 i = ConnectionLines.Num() - 1; i >= 0; i--)
        {
            UGWTConnectionLineWidget* Line = ConnectionLines[i];
            if (Line && Line->SourceNode == ConnectionSourceNode && Line->TargetNode == nullptr)
            {
                RemoveConnectionLine(Line);
                break;
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Connection drag canceled: Invalid connection"));
    }
    
    // Reset dragging state
    bIsDraggingConnection = false;
    ConnectionSourceNode = nullptr;
    ConnectionSourcePort = -1;
}

void UGWTSpellEditorWidget::CancelConnectionDrag()
{
    // Remove the temporary connection line
    if (bIsDraggingConnection)
    {
        for (int32 i = ConnectionLines.Num() - 1; i >= 0; i--)
        {
            UGWTConnectionLineWidget* Line = ConnectionLines[i];
            if (Line && Line->SourceNode == ConnectionSourceNode && Line->TargetNode == nullptr)
            {
                RemoveConnectionLine(Line);
                break;
            }
        }
        
        // Reset dragging state
        bIsDraggingConnection = false;
        ConnectionSourceNode = nullptr;
        ConnectionSourcePort = -1;
        
        UE_LOG(LogTemp, Verbose, TEXT("Connection drag canceled"));
    }
}

void UGWTSpellEditorWidget::UpdateConnectionLines()
{
    // Clear existing connection lines
    ClearConnectionLines();
    
    // Create lines for all connections
    if (CurrentSpell)
    {
        for (UGWTSpellNode* Node : CurrentSpell->AllNodes)
        {
            if (Node)
            {
                // Create a line for each output connection
                for (UGWTSpellNode* OutputNode : Node->OutputNodes)
                {
                    if (OutputNode)
                    {
                        // Find the node widgets
                        UGWTNodeWidget* SourceWidget = NodeWidgets.FindRef(Node);
                        UGWTNodeWidget* TargetWidget = NodeWidgets.FindRef(OutputNode);
                        
                        if (SourceWidget && TargetWidget)
                        {
                            // Create a connection line
                            UGWTConnectionLineWidget* Line = CreateConnectionLine();
                            
                            if (Line)
                            {
                                // Set line properties
                                Line->SourceNode = SourceWidget;
                                Line->SourcePortIndex = 0; // Default to first port
                                Line->TargetNode = TargetWidget;
                                Line->TargetPortIndex = 0; // Default to first port
                                
                                // Update line positions
                                FVector2D SourcePos = SourceWidget->GetOutputPortPosition(0);
                                FVector2D TargetPos = TargetWidget->GetInputPortPosition(0);
                                Line->UpdateStartPoint(SourcePos);
                                Line->UpdateEndPoint(TargetPos);
                            }
                        }
                    }
                }
            }
        }
    }
}

void UGWTSpellEditorWidget::PopulateNodePalette()
{
    // Clear existing palette items
    if (NodePaletteScrollBox)
    {
        NodePaletteScrollBox->ClearChildren();
    }
    
    if (CategoryBox)
    {
        CategoryBox->ClearChildren();
    }
    
    // Get all unlocked node types from grimoire
    if (Grimoire)
    {
        // Group nodes by category
        TMap<FText, TArray<TSubclassOf<UGWTSpellNode>>> NodesByCategory;
        
        // Get unlocked node types
        TArray<TSubclassOf<UGWTSpellNode>> UnlockedNodes = Grimoire->GetUnlockedNodeTypes();
        
        // Group by category
        for (TSubclassOf<UGWTSpellNode> NodeClass : UnlockedNodes)
        {
            UGWTSpellNode* DefaultNode = NodeClass.GetDefaultObject();
            if (DefaultNode)
            {
                if (!NodesByCategory.Contains(DefaultNode->NodeCategory))
                {
                    NodesByCategory.Add(DefaultNode->NodeCategory, TArray<TSubclassOf<UGWTSpellNode>>());
                }
                
                NodesByCategory[DefaultNode->NodeCategory].Add(NodeClass);
            }
        }
        
        // Create widgets for each category
        for (const TPair<FText, TArray<TSubclassOf<UGWTSpellNode>>>& Pair : NodesByCategory)
        {
            // Create category widget
            UWidget* CategoryWidget = CreateNodeCategoryWidget(Pair.Key);
            if (CategoryWidget)
            {
                CategoryBox->AddChild(CategoryWidget);
            }
            
            // Create widgets for each node type in this category
            for (TSubclassOf<UGWTSpellNode> NodeClass : Pair.Value)
            {
                UWidget* NodeItem = CreateNodePaletteItem(NodeClass);
                if (NodeItem)
                {
                    NodePaletteScrollBox->AddChild(NodeItem);
                }
            }
        }
        
        UE_LOG(LogTemp, Display, TEXT("Populated node palette with %d node types"), 
               UnlockedNodes.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot populate node palette: No grimoire reference"));
    }
}

void UGWTSpellEditorWidget::UpdatePropertyEditor()
{
    // Clear existing property widgets
    if (PropertyEditorBox)
    {
        PropertyEditorBox->ClearChildren();
    }
    
    // Add property widgets for selected node
    if (SelectedNode && SelectedNode->SpellNode)
    {
        UGWTSpellNode* Node = SelectedNode->SpellNode;
        
        // Get all properties of the node
        UClass* NodeClass = Node->GetClass();
        for (TFieldIterator<FProperty> It(NodeClass); It; ++It)
        {
            FProperty* Property = *It;
            
            // Only edit properties with EditAnywhere or EditDefaultsOnly specifier
            if (Property->HasAnyPropertyFlags(CPF_Edit))
            {
                UWidget* PropertyWidget = CreatePropertyWidget(Node, Property->GetFName());
                if (PropertyWidget)
                {
                    PropertyEditorBox->AddChild(PropertyWidget);
                }
            }
        }
        
        UE_LOG(LogTemp, Verbose, TEXT("Updated property editor for node %s"), 
               *Node->NodeTitle.ToString());
    }
}

void UGWTSpellEditorWidget::SaveSpell()
{
    // Save the current spell
    if (CurrentSpell)
    {
        // Update spell name
        if (SpellNameTextBox)
        {
            FText NewName = SpellNameTextBox->GetText();
            if (!NewName.IsEmpty())
            {
                CurrentSpell->SpellName = NewName;
            }
        }
        
        // Add to grimoire if needed
        if (Grimoire && !Grimoire->Spells.Contains(CurrentSpell))
        {
            Grimoire->AddSpell(CurrentSpell);
        }
        
        UE_LOG(LogTemp, Display, TEXT("Saved spell '%s'"), 
               *CurrentSpell->SpellName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot save spell: No current spell"));
    }
}

void UGWTSpellEditorWidget::TestSpell()
{
    // Test the current spell
    if (CurrentSpell)
    {
        // Validate the spell
        if (CurrentSpell->ValidateSpell())
        {
            // Get the player controller
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->IsA(AGWTPlayerController::StaticClass()))
            {
                AGWTPlayerController* GWTPC = Cast<AGWTPlayerController>(PC);
                if (GWTPC)
                {
                    // Cast the spell
                    GWTPC->CastActiveSpell();
                    
                    UE_LOG(LogTemp, Display, TEXT("Tested spell '%s'"), 
                           *CurrentSpell->SpellName.ToString());
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot test spell: Validation failed"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot test spell: No current spell"));
    }
}

void UGWTSpellEditorWidget::ClearEditor()
{
    // Clear the editor
    
    // Remove all node widgets
    if (EditorCanvas)
    {
        EditorCanvas->ClearChildren();
    }
    
    // Clear connection lines
    ClearConnectionLines();
    
    // Clear tracking maps
    NodeWidgets.Empty();
    
    // Clear selection
    SelectedNode = nullptr;
    
    // Clear property editor
    if (PropertyEditorBox)
    {
        PropertyEditorBox->ClearChildren();
    }
    
    // Reset spell name
    if (SpellNameTextBox)
    {
        SpellNameTextBox->SetText(FText::FromString("New Spell"));
    }
    
    UE_LOG(LogTemp, Display, TEXT("Cleared spell editor"));
}

void UGWTSpellEditorWidget::SetGrimoire(UGWTGrimoire* InGrimoire)
{
    Grimoire = InGrimoire;
    
    // Repopulate node palette
    PopulateNodePalette();
}

UWidget* UGWTSpellEditorWidget::CreateNodeCategoryWidget(const FText& CategoryName)
{
    // Create a header for a node category
    UBorder* Border = NewObject<UBorder>(this);
    Border->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.3f));
    
    UTextBlock* Text = NewObject<UTextBlock>(this);
    Text->SetText(CategoryName);
    Text->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 1.0f)));
    
    Border->SetContent(Text);
    
    return Border;
}

UWidget* UGWTSpellEditorWidget::CreateNodePaletteItem(TSubclassOf<UGWTSpellNode> NodeClass)
{
    // Create a palette item for a node type
    UButton* Button = NewObject<UButton>(this);
    
    // Get default node properties
    UGWTSpellNode* DefaultNode = NodeClass.GetDefaultObject();
    if (DefaultNode)
    {
        // Create horizontal box for layout
        UHorizontalBox* HBox = NewObject<UHorizontalBox>(this);
        
        // Create color swatch
        UImage* ColorSwatch = NewObject<UImage>(this);
        ColorSwatch->SetColorAndOpacity(DefaultNode->NodeColor);
        
        // Create title text
        UTextBlock* TitleText = NewObject<UTextBlock>(this);
        TitleText->SetText(DefaultNode->NodeTitle);
        
        // Add to layout
        HBox->AddChild(ColorSwatch);
        HBox->AddChild(TitleText);
        
        // Set button content
        Button->SetContent(HBox);
        
        // Bind button click event
        FVector2D CenterPosition = FVector2D(
            EditorCanvas->GetDesiredSize().X * 0.5f,
            EditorCanvas->GetDesiredSize().Y * 0.5f
        );
        
        FScriptDelegate ButtonDelegate;
        ButtonDelegate.BindUFunction(this, "AddNodeFromPalette", NodeClass, CenterPosition);
        Button->OnClicked.Add(ButtonDelegate);
    }
    
    return Button;
}

UWidget* UGWTSpellEditorWidget::CreatePropertyWidget(UGWTSpellNode* Node, FName PropertyName)
{
    // Create widgets for editing a property
    UHorizontalBox* HBox = NewObject<UHorizontalBox>(this);
    
    // Create label
    UTextBlock* Label = NewObject<UTextBlock>(this);
    Label->SetText(FText::FromName(PropertyName));
    
    // Create editor (this would be more complex in a full implementation)
    UEditableTextBox* Editor = NewObject<UEditableTextBox>(this);
    
    // Get property value
    FProperty* Property = Node->GetClass()->FindPropertyByName(PropertyName);
    if (Property)
    {
        FString ValueString;
        if (Property->ExportText_InContainer(0, ValueString, Node, Node, Node, PPF_None))
        {
            Editor->SetText(FText::FromString(ValueString));
        }
    }
    
    // Add to layout
    HBox->AddChild(Label);
    HBox->AddChild(Editor);
    
    return HBox;
}

void UGWTSpellEditorWidget::OnSaveButtonClicked()
{
    SaveSpell();
}

void UGWTSpellEditorWidget::OnTestButtonClicked()
{
    TestSpell();
}

void UGWTSpellEditorWidget::OnClearButtonClicked()
{
    ClearEditor();
    
    // Create a new spell
    CurrentSpell = NewObject<UGWTSpell>();
    CurrentSpell->SpellName = FText::FromString("New Spell");
    
    // Update spell name field
    if (SpellNameTextBox)
    {
        SpellNameTextBox->SetText(CurrentSpell->SpellName);
    }
}

void UGWTSpellEditorWidget::OnNodeDragDetected(UGWTNodeWidget* NodeWidget, const FPointerEvent& MouseEvent)
{
    // Set selected node
    SelectNode(NodeWidget);
}

void UGWTSpellEditorWidget::OnNodeDragEnd(UGWTNodeWidget* NodeWidget, const FPointerEvent& MouseEvent)
{
    // Update node position
    if (NodeWidget && NodeWidget->SpellNode)
    {
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NodeWidget->Slot);
        if (CanvasSlot)
        {
            // Update node position
            FVector2D NewPosition = CanvasSlot->GetPosition();
            NodeWidget->SpellNode->NodePosition = NewPosition;
            
            // Update connection lines
            UpdateConnectionLines();
        }
    }
}

UGWTConnectionLineWidget* UGWTSpellEditorWidget::CreateConnectionLine()
{
    // Create a new connection line widget
    UGWTConnectionLineWidget* Line = CreateWidget<UGWTConnectionLineWidget>(this, UGWTConnectionLineWidget::StaticClass());
    
    if (Line && EditorCanvas)
    {
        // Add to canvas
        EditorCanvas->AddChild(Line);
        
        // Make sure it's drawn behind nodes
        Line->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        Line->SetRenderScale(FVector2D(1.0f, 1.0f));
        
        // Add to tracking array
        ConnectionLines.Add(Line);
        
        return Line;
    }
    
    return nullptr;
}

void UGWTSpellEditorWidget::RemoveConnectionLine(UGWTConnectionLineWidget* LineWidget)
{
    // Remove a connection line
    if (LineWidget && EditorCanvas)
    {
        // Remove from canvas
        EditorCanvas->RemoveChild(LineWidget);
        
        // Remove from tracking array
        ConnectionLines.Remove(LineWidget);
    }
}

void UGWTSpellEditorWidget::ClearConnectionLines()
{
    // Remove all connection lines
    for (UGWTConnectionLineWidget* Line : ConnectionLines)
    {
        if (Line && EditorCanvas)
        {
            EditorCanvas->RemoveChild(Line);
        }
    }
    
    ConnectionLines.Empty();
}

bool UGWTSpellEditorWidget::CanConnectNodes(UGWTNodeWidget* SourceNode, UGWTNodeWidget* TargetNode) const
{
    // Check if these nodes can be connected
    if (!SourceNode || !TargetNode || SourceNode == TargetNode)
    {
        return false;
    }
    
    if (!SourceNode->SpellNode || !TargetNode->SpellNode)
    {
        return false;
    }
    
    // Check if connection would create a cycle
    // (This would require a more complex check in a full implementation)
    
    // Check if source can connect to target
    return SourceNode->SpellNode->CanConnectOutput(TargetNode->SpellNode) && 
           TargetNode->SpellNode->CanConnectInput(SourceNode->SpellNode);
}

UGWTNodeWidget* UGWTSpellEditorWidget::FindNodeAtPosition(FVector2D Position) const
{
    // Find a node at the given position
    for (const TPair<UGWTSpellNode*, UGWTNodeWidget*>& Pair : NodeWidgets)
    {
        UGWTNodeWidget* NodeWidget = Pair.Value;
        if (NodeWidget)
        {
            UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NodeWidget->Slot);
            if (CanvasSlot)
            {
                // Check if position is within node bounds
                FVector2D NodePos = CanvasSlot->GetPosition();
                FVector2D NodeSize = CanvasSlot->GetSize();
                
                if (Position.X >= NodePos.X && Position.X <= NodePos.X + NodeSize.X &&
                    Position.Y >= NodePos.Y && Position.Y <= NodePos.Y + NodeSize.Y)
                {
                    return NodeWidget;
                }
            }
        }
    }
    
    return nullptr;
}

// Add node from palette button click handler
UFUNCTION()
void UGWTSpellEditorWidget::AddNodeFromPalette(TSubclassOf<UGWTSpellNode> NodeClass, FVector2D Position)
{
    AddNode(NodeClass, Position);
}