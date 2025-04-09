// GWTTutorialManager.cpp
// Implementation of the tutorial manager

#include "GWTTutorialManager.h"
#include "GWTSpellEditorWidget.h"
#include "GWTSpell.h"
#include "GWTSpellNode.h"
#include "GWTMagicNode.h"
#include "GWTEffectNode.h"
#include "GWTConditionNode.h"
#include "GWTVariableNode.h"
#include "GWTFlowNode.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGWTTutorialManager::UGWTTutorialManager()
{
    // Initialize with default tutorials
    InitializeDefaultTutorials();
    
    UE_LOG(LogTemp, Display, TEXT("Tutorial Manager initialized"));
}

void UGWTTutorialManager::StartTutorial(int32 SequenceIndex)
{
    // Check if the sequence index is valid
    if (!TutorialSequences.IsValidIndex(SequenceIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid tutorial sequence index: %d"), SequenceIndex);
        return;
    }
    
    // Set tutorial state
    CurrentSequenceIndex = SequenceIndex;
    CurrentStepIndex = 0;
    bTutorialActive = true;
    
    UE_LOG(LogTemp, Display, TEXT("Started tutorial sequence: %s"), 
           *TutorialSequences[CurrentSequenceIndex].SequenceName);
    
    // Execute the first step
    if (TutorialSequences[CurrentSequenceIndex].Steps.IsValidIndex(CurrentStepIndex))
    {
        const FGWTTutorialStep& Step = TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
        ExecuteStepAction(Step);
    }
}

void UGWTTutorialManager::EndTutorial()
{
    // End the current tutorial
    bTutorialActive = false;
    CurrentSequenceIndex = -1;
    CurrentStepIndex = -1;
    
    // Clean up any tutorial-specific state
    CleanupPreviousStep();
    
    UE_LOG(LogTemp, Display, TEXT("Tutorial ended"));
}

void UGWTTutorialManager::NextStep()
{
    // Check if tutorial is active
    if (!bTutorialActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot go to next step: No active tutorial"));
        return;
    }
    
    // Check if current step is complete
    if (!IsCurrentStepComplete())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot advance: Current step is not complete"));
        return;
    }
    
    // Clean up previous step
    CleanupPreviousStep();
    
    // Advance to next step
    CurrentStepIndex++;
    
    // Check if we reached the end of the tutorial
    if (!TutorialSequences[CurrentSequenceIndex].Steps.IsValidIndex(CurrentStepIndex))
    {
        UE_LOG(LogTemp, Display, TEXT("Tutorial sequence completed: %s"), 
               *TutorialSequences[CurrentSequenceIndex].SequenceName);
        
        // End the tutorial
        bTutorialActive = false;
        return;
    }
    
    // Execute the new step
    const FGWTTutorialStep& Step = TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
    ExecuteStepAction(Step);
    
    UE_LOG(LogTemp, Display, TEXT("Advanced to step %d: %s"), 
           CurrentStepIndex, *Step.StepTitle);
}

void UGWTTutorialManager::PreviousStep()
{
    // Check if tutorial is active
    if (!bTutorialActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot go to previous step: No active tutorial"));
        return;
    }
    
    // Check if we're already at the first step
    if (CurrentStepIndex <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already at first step of tutorial"));
        return;
    }
    
    // Clean up previous step
    CleanupPreviousStep();
    
    // Go back to previous step
    CurrentStepIndex--;
    
    // Execute the new step
    const FGWTTutorialStep& Step = TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
    ExecuteStepAction(Step);
    
    UE_LOG(LogTemp, Display, TEXT("Went back to step %d: %s"), 
           CurrentStepIndex, *Step.StepTitle);
}

void UGWTTutorialManager::SkipToStep(int32 StepIndex)
{
    // Check if tutorial is active
    if (!bTutorialActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot skip step: No active tutorial"));
        return;
    }
    
    // Check if step index is valid
    if (!TutorialSequences[CurrentSequenceIndex].Steps.IsValidIndex(StepIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid step index: %d"), StepIndex);
        return;
    }
    
    // Clean up previous step
    CleanupPreviousStep();
    
    // Set new step index
    CurrentStepIndex = StepIndex;
    
    // Execute the new step
    const FGWTTutorialStep& Step = TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
    ExecuteStepAction(Step);
    
    UE_LOG(LogTemp, Display, TEXT("Skipped to step %d: %s"), 
           CurrentStepIndex, *Step.StepTitle);
}

FGWTTutorialStep UGWTTutorialManager::GetCurrentStep() const
{
    // Return the current step
    if (bTutorialActive && 
        TutorialSequences.IsValidIndex(CurrentSequenceIndex) && 
        TutorialSequences[CurrentSequenceIndex].Steps.IsValidIndex(CurrentStepIndex))
    {
        return TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
    }
    
    // Return empty step if no active tutorial
    return FGWTTutorialStep();
}

FString UGWTTutorialManager::GetCurrentInstructions() const
{
    // Get instructions for the current step
    FGWTTutorialStep CurrentStep = GetCurrentStep();
    
    return CurrentStep.Instructions;
}

bool UGWTTutorialManager::IsCurrentStepComplete() const
{
    // Check if the current step is complete
    if (!bTutorialActive)
    {
        return false;
    }
    
    // Get current step
    if (TutorialSequences.IsValidIndex(CurrentSequenceIndex) && 
        TutorialSequences[CurrentSequenceIndex].Steps.IsValidIndex(CurrentStepIndex))
    {
        const FGWTTutorialStep& Step = TutorialSequences[CurrentSequenceIndex].Steps[CurrentStepIndex];
        return CheckStepCompletion(Step);
    }
    
    return false;
}

bool UGWTTutorialManager::IsTutorialComplete() const
{
    // Check if the entire tutorial is complete
    if (!bTutorialActive)
    {
        return false;
    }
    
    // If we're at the last step and it's complete, the tutorial is complete
    if (TutorialSequences.IsValidIndex(CurrentSequenceIndex))
    {
        int32 LastStepIndex = TutorialSequences[CurrentSequenceIndex].Steps.Num() - 1;
        return (CurrentStepIndex == LastStepIndex) && IsCurrentStepComplete();
    }
    
    return false;
}

void UGWTTutorialManager::LoadTutorialData(const FString& FileName)
{
    // Load tutorial data from a JSON file
    FString JsonString;
    
    // Read the file
    if (FFileHelper::LoadFileToString(JsonString, *FileName))
    {
        // Parse JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
        
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Clear existing tutorials
            TutorialSequences.Empty();
            
            // Parse sequences array
            TArray<TSharedPtr<FJsonValue>> SequencesArray = JsonObject->GetArrayField(TEXT("sequences"));
            
            for (TSharedPtr<FJsonValue> SequenceValue : SequencesArray)
            {
                TSharedPtr<FJsonObject> SequenceObject = SequenceValue->AsObject();
                
                FGWTTutorialSequence Sequence;
                Sequence.SequenceName = SequenceObject->GetStringField(TEXT("name"));
                Sequence.SequenceDescription = SequenceObject->GetStringField(TEXT("description"));
                
                // Parse steps array
                TArray<TSharedPtr<FJsonValue>> StepsArray = SequenceObject->GetArrayField(TEXT("steps"));
                
                for (TSharedPtr<FJsonValue> StepValue : StepsArray)
                {
                    TSharedPtr<FJsonObject> StepObject = StepValue->AsObject();
                    
                    FGWTTutorialStep Step;
                    Step.StepTitle = StepObject->GetStringField(TEXT("title"));
                    Step.Instructions = StepObject->GetStringField(TEXT("instructions"));
                    
                    // Add step to sequence
                    Sequence.Steps.Add(Step);
                }
                
                // Add sequence to tutorials
                TutorialSequences.Add(Sequence);
            }
            
            UE_LOG(LogTemp, Display, TEXT("Loaded %d tutorial sequences from %s"), 
                   TutorialSequences.Num(), *FileName);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to parse tutorial JSON from %s"), *FileName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load tutorial file: %s"), *FileName);
    }
}

void UGWTTutorialManager::InitializeDefaultTutorials()
{
    // Create default tutorial sequences if none are loaded
    TutorialSequences.Empty();
    
    // Tutorial 1: Basic Spell Crafting
    {
        FGWTTutorialSequence Sequence;
        Sequence.SequenceName = TEXT("Basic Spell Crafting");
        Sequence.SequenceDescription = TEXT("Learn the basics of creating and casting spells.");
        
        // Step 1: Introduction
        {
            FGWTTutorialStep Step;
            Step.StepTitle = TEXT("Welcome to Spell Crafting");
            Step.Instructions = TEXT("Welcome to the Grand Wizard Tournament! In this tutorial, you'll learn how to create your first spell. Click Next to continue.");
            Sequence.Steps.Add(Step);
        }
        
        // Step 2: Opening the Spell Editor
        {
            FGWTTutorialStep Step;
            Step.StepTitle = TEXT("The Spell Editor");
            Step.Instructions = TEXT("This is the Spell Editor where you'll create your spells. Notice the node palette on the left which contains all the different spell components you can use.");
            Sequence.Steps.Add(Step);
        }
        
        // Step 3: Adding a Magic Node
        {
            FGWTTutorialStep Step;
            Step.StepTitle = TEXT("Your First Node");
            Step.Instructions = TEXT("Let's create a simple fireball spell. Drag a Magic Node from the palette to the canvas. This will be the core of your spell.");
            Sequence.Steps.Add(Step);
        }
        
        // Add more steps...
        
        TutorialSequences.Add(Sequence);
    }
    
    // Tutorial 2: Conditional Logic
    {
        FGWTTutorialSequence Sequence;
        Sequence.SequenceName = TEXT("Conditional Logic");
        Sequence.SequenceDescription = TEXT("Learn how to use if/then statements in your spells.");
        
        // Add steps...
        
        TutorialSequences.Add(Sequence);
    }
    
    // Tutorial 3: Variables
    {
        FGWTTutorialSequence Sequence;
        Sequence.SequenceName = TEXT("Variables");
        Sequence.SequenceDescription = TEXT("Learn how to store and manipulate data in your spells.");
        
        // Add steps...
        
        TutorialSequences.Add(Sequence);
    }
    
    // Tutorial 4: Loops
    {
        FGWTTutorialSequence Sequence;
        Sequence.SequenceName = TEXT("Loops");
        Sequence.SequenceDescription = TEXT("Learn how to repeat actions in your spells.");
        
        // Add steps...
        
        TutorialSequences.Add(Sequence);
    }
    
    UE_LOG(LogTemp, Display, TEXT("Initialized %d default tutorial sequences"), TutorialSequences.Num());
}

void UGWTTutorialManager::ExecuteStepAction(const FGWTTutorialStep& Step)
{
    // Execute any actions required for this step
    // For example, highlighting UI elements, setting up the editor, etc.
    
    // Set up the editor for this step
    SetupEditorForStep(Step);
    
    // In a full implementation, this would handle various actions like:
    // - Highlighting specific UI elements
    // - Adding sample nodes to the editor
    // - Restricting available nodes
    // - Setting up objective conditions
    
    UE_LOG(LogTemp, Verbose, TEXT("Executed actions for step: %s"), *Step.StepTitle);
}

bool UGWTTutorialManager::CheckStepCompletion(const FGWTTutorialStep& Step) const
{
    // Check if the step has been completed by the player
    // This would check conditions specific to the step
    
    // In a real implementation, this would check for various completion criteria:
    // - Has the player added required nodes?
    // - Is the spell structured correctly?
    // - Has the player completed the specific task?
    
    // For this example, we'll assume steps are completed by clicking Next
    // In a real tutorial, most steps would have actual completion criteria
    
    return true;
}

void UGWTTutorialManager::SetupEditorForStep(const FGWTTutorialStep& Step)
{
    // Set up the spell editor for this tutorial step
    if (!SpellEditorWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot set up editor: No spell editor reference"));
        return;
    }
    
    // Different setup based on step requirements
    // In a full implementation, this would create example nodes, restrict the palette, etc.
    
    // Example: If this was a step about adding a specific node type
    if (Step.StepTitle.Contains(TEXT("Your First Node")))
    {
        // Highlight the Magic node in the palette
        // Restrict palette to only show Magic nodes
        // etc.
    }
}

void UGWTTutorialManager::CleanupPreviousStep()
{
    // Clean up any state from the previous step
    // For example, removing highlight effects, temporary nodes, etc.
    
    // This would be implemented based on specific tutorial needs
}