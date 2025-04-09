// GWTNodeWidget.cpp
// Implementation of the node widget for the spell editor

#include "GWTNodeWidget.h"
#include "GWTSpellNode.h"
#include "GWTSpellEditorWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

UGWTNodeWidget::UGWTNodeWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize node state
    SpellNode = nullptr;
    EditorWidget = nullptr;
    bIsSelected = false;
    bIsDragging = false;
    DragOffset = FVector2D::ZeroVector;
}

void UGWTNodeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set default visuals
    if (NodeBorder)
    {
        NodeBorder->SetBrushColor(FLinearColor(0.3f, 0.3f, 0.3f));
    }
    
    // Update node visuals
    UpdateNodeVisuals();
    
    UE_LOG(LogTemp, Verbose, TEXT("Node Widget constructed"));
}

void UGWTNodeWidget::SetSpellNode(UGWTSpellNode* Node)
{
    // Set the spell node
    SpellNode = Node;
    
    // Update visuals
    UpdateNodeVisuals();
}

void UGWTNodeWidget::UpdateNodeVisuals()
{
    // Update the node's visual appearance based on its properties
    if (SpellNode)
    {
        // Set title
        if (TitleText)
        {
            TitleText->SetText(SpellNode->NodeTitle);
        }
        
        // Set border color
        if (NodeBorder)
        {
            NodeBorder->SetBrushColor(bIsSelected ? 
                                     FLinearColor(1.0f, 1.0f, 0.3f) : // Selected: yellow
                                     SpellNode->NodeColor);           // Normal: node color
        }
        
        // Create input/output ports
        CreateInputPorts();
        CreateOutputPorts();
    }
    else
    {
        // Set default title
        if (TitleText)
        {
            TitleText->SetText(FText::FromString("No Node"));
        }
    }
}

void UGWTNodeWidget::SetIsSelected(bool bSelected)
{
    // Set selection state
    bIsSelected = bSelected;
    
    // Update visuals
    if (NodeBorder && SpellNode)
    {
        NodeBorder->SetBrushColor(bIsSelected ? 
                                 FLinearColor(1.0f, 1.0f, 0.3f) : // Selected: yellow
                                 SpellNode->NodeColor);           // Normal: node color
    }
}

void UGWTNodeWidget::OnNodeMoved(FVector2D NewPosition)
{
    // Update node position in data model
    if (SpellNode)
    {
        SpellNode->NodePosition = NewPosition;
    }
}

FVector2D UGWTNodeWidget::GetInputPortPosition(int32 InputIndex) const
{
    // Calculate the position of an input port
    UCanvasPanelSlot* CanvasSlot = GetCanvasSlot();
    if (CanvasSlot && InputsBox)
    {
        FVector2D NodePos = CanvasSlot->GetPosition();
        float PortY = NodePos.Y + InputsBox->GetDesiredSize().Y * 0.5f;
        
        // If multiple ports, adjust Y based on index
        if (InputsBox->GetChildrenCount() > 1 && InputIndex < InputsBox->GetChildrenCount())
        {
            UWidget* PortWidget = InputsBox->GetChildAt(InputIndex);
            if (PortWidget)
            {
                PortY = NodePos.Y + PortWidget->GetCachedGeometry().GetAbsolutePosition().Y;
            }
        }
        
        return FVector2D(NodePos.X, PortY);
    }
    
    return FVector2D::ZeroVector;
}

FVector2D UGWTNodeWidget::GetOutputPortPosition(int32 OutputIndex) const
{
    // Calculate the position of an output port
    UCanvasPanelSlot* CanvasSlot = GetCanvasSlot();
    if (CanvasSlot && OutputsBox)
    {
        FVector2D NodePos = CanvasSlot->GetPosition();
        FVector2D NodeSize = CanvasSlot->GetSize();
        float PortY = NodePos.Y + OutputsBox->GetDesiredSize().Y * 0.5f;
        
        // If multiple ports, adjust Y based on index
        if (OutputsBox->GetChildrenCount() > 1 && OutputIndex < OutputsBox->GetChildrenCount())
        {
            UWidget* PortWidget = OutputsBox->GetChildAt(OutputIndex);
            if (PortWidget)
            {
                PortY = NodePos.Y + PortWidget->GetCachedGeometry().GetAbsolutePosition().Y;
            }
        }
        
        return FVector2D(NodePos.X + NodeSize.X, PortY);
    }
    
    return FVector2D::ZeroVector;
}

FReply UGWTNodeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Handle mouse button press
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Start dragging
        bIsDragging = true;
        DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
        
        // Notify editor of drag start
        if (EditorWidget)
        {
            EditorWidget->SelectNode(this);
            OnNodeDragDetected.Broadcast(this, InMouseEvent);
        }
        
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }
    
    return FReply::Unhandled();
}

FReply UGWTNodeWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Handle mouse button release
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
    {
        // End dragging
        bIsDragging = false;
        
        // Notify editor of drag end
        OnNodeDragEnd.Broadcast(this, InMouseEvent);
        
        return FReply::Handled().ReleaseMouseCapture();
    }
    
    return FReply::Unhandled();
}

FReply UGWTNodeWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Handle mouse movement
    if (bIsDragging && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        // Calculate new position
        FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
        FVector2D Delta = MousePos - DragOffset;
        
        // Move node on canvas
        UCanvasPanelSlot* CanvasSlot = GetCanvasSlot();
        if (CanvasSlot)
        {
            FVector2D NewPosition = CanvasSlot->GetPosition() + Delta;
            CanvasSlot->SetPosition(NewPosition);
            
            // Update node position in data model
            OnNodeMoved(NewPosition);
        }
        
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

void UGWTNodeWidget::CreateInputPorts()
{
    // Create input ports based on node type
    if (InputsBox)
    {
        // Clear existing ports
        InputsBox->ClearChildren();
        
        // Create a port for each input connection
        if (SpellNode)
        {
            // For simplicity, we'll just create a single input port
            UWidget* PortWidget = CreatePortWidget(true, 0);
            if (PortWidget)
            {
                InputsBox->AddChild(PortWidget);
            }
        }
    }
}

void UGWTNodeWidget::CreateOutputPorts()
{
    // Create output ports based on node type
    if (OutputsBox)
    {
        // Clear existing ports
        OutputsBox->ClearChildren();
        
        // Create a port for each output type
        if (SpellNode)
        {
            // For simplicity, we'll just create a single output port
            UWidget* PortWidget = CreatePortWidget(false, 0);
            if (PortWidget)
            {
                OutputsBox->AddChild(PortWidget);
            }
        }
    }
}

UWidget* UGWTNodeWidget::CreatePortWidget(bool bIsInput, int32 PortIndex)
{
    // Create a widget for a port
    UButton* PortButton = NewObject<UButton>(this);
    
    // Set port visual
    UImage* PortImage = NewObject<UImage>(this);
    PortImage->SetColorAndOpacity(bIsInput ? 
                                 FLinearColor(0.2f, 0.8f, 0.2f) : // Input: green
                                 FLinearColor(0.8f, 0.2f, 0.2f)); // Output: red
    
    PortButton->SetContent(PortImage);
    
    // Set click handler
    FScriptDelegate ClickDelegate;
    if (bIsInput)
    {
        ClickDelegate.BindUFunction(this, "HandleInputPortClicked", PortIndex);
    }
    else
    {
        ClickDelegate.BindUFunction(this, "HandleOutputPortClicked", PortIndex);
    }
    
    PortButton->OnClicked.Add(ClickDelegate);
    
    return PortButton;
}

void UGWTNodeWidget::HandleInputPortClicked(int32 PortIndex)
{
    // Handle input port click
    if (EditorWidget && EditorWidget->bIsDraggingConnection)
    {
        // End connection drag to this port
        EditorWidget->EndConnectionDrag(this, PortIndex);
    }
}

void UGWTNodeWidget::HandleOutputPortClicked(int32 PortIndex)
{
    // Handle output port click
    if (EditorWidget)
    {
        // Start connection drag from this port
        EditorWidget->BeginConnectionDrag(this, PortIndex);
    }
}

UCanvasPanelSlot* UGWTNodeWidget::GetCanvasSlot() const
{
    return Cast<UCanvasPanelSlot>(Slot);
}