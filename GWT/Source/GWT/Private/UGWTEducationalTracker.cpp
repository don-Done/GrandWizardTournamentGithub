// UGWTEducationalTracker.cpp
// Implementation of educational tracking system

#include "UGWTEducationalTracker.h"
#include "UGWTSpell.h"
#include "UGWTSpellNode.h"
#include "UGWTConditionNode.h"
#include "UGWTVariableNode.h"
#include "UGWTFlowNode.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UGWTEducationalTracker::UGWTEducationalTracker()
{
    // Initialize concept mastery levels to 0
    ConceptMastery.Add(EGWTProgrammingConcept::SequentialLogic, 0.0f);
    ConceptMastery.Add(EGWTProgrammingConcept::ConditionalLogic, 0.0f);
    ConceptMastery.Add(EGWTProgrammingConcept::Variables, 0.0f);
    ConceptMastery.Add(EGWTProgrammingConcept::Loops, 0.0f);
    ConceptMastery.Add(EGWTProgrammingConcept::Functions, 0.0f);

    UE_LOG(LogTemp, Display, TEXT("Educational Tracker initialized"));
}

void UGWTEducationalTracker::AnalyzeSpellUsage(UGWTSpell* Spell, bool bWasSuccessful)
{
    // Check if spell is valid
    if (!Spell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot analyze spell: Invalid spell"));
        return;
    }

    // Create usage data entry
    FGWTSpellUsageData UsageData;
    UsageData.SpellID = Spell->NodeID;
    UsageData.SpellName = Spell->SpellName.ToString();
    UsageData.TimeStamp = FDateTime::Now();
    UsageData.WasSuccessful = bWasSuccessful;
    UsageData.NodeCount = Spell->CountNodes();
    UsageData.ConnectionCount = Spell->CountConnections();

    // Add to usage history
    SpellUsageHistory.Add(UsageData);

    // Analyze spell structure
    AnalyzeSpellStructure(Spell);

    // Update mastery based on success/failure
    float SuccessFactor = bWasSuccessful ? 0.05f : -0.02f;

    // Update all concept masteries that are used in this spell
    if (Spell->GetNodesOfType(EGWTSpellComponentType::Condition).Num() > 0)
    {
        UpdateConceptMastery(EGWTProgrammingConcept::ConditionalLogic, SuccessFactor);
    }

    if (Spell->GetNodesOfType(EGWTSpellComponentType::Variable).Num() > 0)
    {
        UpdateConceptMastery(EGWTProgrammingConcept::Variables, SuccessFactor);
    }

    if (Spell->GetNodesOfType(EGWTSpellComponentType::Flow).Num() > 0)
    {
        UpdateConceptMastery(EGWTProgrammingConcept::Loops, SuccessFactor);
    }

    // Sequential logic is always used
    UpdateConceptMastery(EGWTProgrammingConcept::SequentialLogic, SuccessFactor);

    UE_LOG(LogTemp, Display, TEXT("Analyzed spell usage: %s, Success: %s"),
        *Spell->SpellName.ToString(), bWasSuccessful ? TEXT("True") : TEXT("False"));
}

void UGWTEducationalTracker::AnalyzeSpellStructure(UGWTSpell* Spell)
{
    // Check if spell is valid
    if (!Spell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot analyze spell structure: Invalid spell"));
        return;
    }

    // Analyze each concept
    float SequentialScore = AnalyzeSequentialLogic(Spell);
    float ConditionalScore = AnalyzeConditionalLogic(Spell);
    float VariableScore = AnalyzeVariableUsage(Spell);
    float LoopScore = AnalyzeLoopUsage(Spell);
    float FunctionScore = AnalyzeFunctionAbstraction(Spell);

    // Update mastery values based on analysis
    UpdateConceptMastery(EGWTProgrammingConcept::SequentialLogic, SequentialScore * 0.02f);
    UpdateConceptMastery(EGWTProgrammingConcept::ConditionalLogic, ConditionalScore * 0.02f);
    UpdateConceptMastery(EGWTProgrammingConcept::Variables, VariableScore * 0.02f);
    UpdateConceptMastery(EGWTProgrammingConcept::Loops, LoopScore * 0.02f);
    UpdateConceptMastery(EGWTProgrammingConcept::Functions, FunctionScore * 0.02f);

    UE_LOG(LogTemp, Verbose, TEXT("Analyzed spell structure for: %s"), *Spell->SpellName.ToString());
}

float UGWTEducationalTracker::GetConceptMastery(EGWTProgrammingConcept Concept) const
{
    // Get the mastery level for a specific concept
    if (ConceptMastery.Contains(Concept))
    {
        return ConceptMastery[Concept];
    }

    return 0.0f;
}

void UGWTEducationalTracker::UpdateConceptMastery(EGWTProgrammingConcept Concept, float DeltaMastery)
{
    // Update mastery level for a concept
    if (ConceptMastery.Contains(Concept))
    {
        // Get current mastery
        float CurrentMastery = ConceptMastery[Concept];

        // Apply delta
        float NewMastery = FMath::Clamp(CurrentMastery + DeltaMastery, 0.0f, 1.0f);

        // Update value
        ConceptMastery[Concept] = NewMastery;

        UE_LOG(LogTemp, Verbose, TEXT("Updated mastery for concept %d: %.2f -> %.2f"),
            (int32)Concept, CurrentMastery, NewMastery);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot update concept mastery: Invalid concept %d"),
            (int32)Concept);
    }
}

TArray<EGWTProgrammingConcept> UGWTEducationalTracker::GetMasteredConcepts(float MasteryThreshold) const
{
    // Get all concepts with mastery above threshold
    TArray<EGWTProgrammingConcept> MasteredConcepts;

    for (const auto& Pair : ConceptMastery)
    {
        if (Pair.Value >= MasteryThreshold)
        {
            MasteredConcepts.Add(Pair.Key);
        }
    }

    return MasteredConcepts;
}

TArray<EGWTProgrammingConcept> UGWTEducationalTracker::GetStruggleConcepts(float MasteryThreshold) const
{
    // Get all concepts with mastery below threshold
    TArray<EGWTProgrammingConcept> StruggleConcepts;

    for (const auto& Pair : ConceptMastery)
    {
        if (Pair.Value < MasteryThreshold)
        {
            StruggleConcepts.Add(Pair.Key);
        }
    }

    return StruggleConcepts;
}

FString UGWTEducationalTracker::GenerateProgressReport() const
{
    // Create a text report of educational progress
    FString Report = TEXT("Programming Concept Mastery Report\n");
    Report += TEXT("================================\n\n");

    for (const auto& Pair : ConceptMastery)
    {
        // Format concept name
        FString ConceptName;

        switch (Pair.Key)
        {
        case EGWTProgrammingConcept::SequentialLogic:
            ConceptName = TEXT("Sequential Logic");
            break;

        case EGWTProgrammingConcept::ConditionalLogic:
            ConceptName = TEXT("Conditional Logic");
            break;

        case EGWTProgrammingConcept::Variables:
            ConceptName = TEXT("Variables");
            break;

        case EGWTProgrammingConcept::Loops:
            ConceptName = TEXT("Loops");
            break;

        case EGWTProgrammingConcept::Functions:
            ConceptName = TEXT("Functions");
            break;

        default:
            ConceptName = TEXT("Unknown Concept");
            break;
        }

        // Format mastery level
        int32 MasteryPercent = FMath::RoundToInt(Pair.Value * 100.0f);

        // Add to report
        Report += FString::Printf(TEXT("%s: %d%%\n"), *ConceptName, MasteryPercent);
    }

    // Add spell usage stats
    Report += TEXT("\nSpell Usage Statistics\n");
    Report += TEXT("=====================\n\n");

    int32 TotalSpellUses = SpellUsageHistory.Num();
    int32 SuccessfulUses = 0;

    for (const FGWTSpellUsageData& Data : SpellUsageHistory)
    {
        if (Data.WasSuccessful)
        {
            SuccessfulUses++;
        }
    }

    float SuccessRate = (TotalSpellUses > 0) ? ((float)SuccessfulUses / TotalSpellUses) * 100.0f : 0.0f;

    Report += FString::Printf(TEXT("Total Spell Uses: %d\n"), TotalSpellUses);
    Report += FString::Printf(TEXT("Successful Uses: %d\n"), SuccessfulUses);
    Report += FString::Printf(TEXT("Success Rate: %.1f%%\n"), SuccessRate);

    return Report;
}

void UGWTEducationalTracker::SaveAnalytics(const FString& FileName)
{
    // Save analytics data to a file
    // In a full implementation, this would serialize to JSON or another format

    FString Data = GenerateProgressReport();

    // Write to file
    if (FFileHelper::SaveStringToFile(Data, *FileName))
    {
        UE_LOG(LogTemp, Display, TEXT("Analytics saved to file: %s"), *FileName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save analytics to file: %s"), *FileName);
    }
}

void UGWTEducationalTracker::LoadAnalytics(const FString& FileName)
{
    // Load analytics data from a file
    // In a full implementation, this would deserialize from JSON or another format

    FString Data;

    // Read from file
    if (FFileHelper::LoadFileToString(Data, *FileName))
    {
        UE_LOG(LogTemp, Display, TEXT("Analytics loaded from file: %s"), *FileName);
        // Parse data and update state
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load analytics from file: %s"), *FileName);
    }
}

float UGWTEducationalTracker::AnalyzeSequentialLogic(UGWTSpell* Spell) const
{
    // Analyze sequential logic usage
    // Higher score for more complex sequences

    if (!Spell)
        return 0.0f;

    int32 NodeCount = Spell->CountNodes();
    int32 ConnectionCount = Spell->CountConnections();

    // Simple metric: connections / nodes ratio
    float SequentialComplexity = (NodeCount > 1) ? (float)ConnectionCount / (float)NodeCount : 0.0f;

    // Scale to 0-1
    return FMath::Min(SequentialComplexity, 1.0f);
}

float UGWTEducationalTracker::AnalyzeConditionalLogic(UGWTSpell* Spell) const
{
    // Analyze conditional logic usage

    if (!Spell)
        return 0.0f;

    // Get all condition nodes
    TArray<UGWTSpellNode*> ConditionNodes = Spell->GetNodesOfType(EGWTSpellComponentType::Condition);

    // Check complexity - do they use both true and false paths?
    float ComplexityScore = 0.0f;

    for (UGWTSpellNode* Node : ConditionNodes)
    {
        UGWTConditionNode* ConditionNode = Cast<UGWTConditionNode>(Node);
        if (ConditionNode)
        {
            // Higher score if both true and false paths are connected
            if (ConditionNode->TruePathNode && ConditionNode->FalsePathNode)
            {
                ComplexityScore += 1.0f;
            }
            else if (ConditionNode->TruePathNode || ConditionNode->FalsePathNode)
            {
                ComplexityScore += 0.5f;
            }
        }
    }

    // Scale by number of condition nodes (max 5 for full score)
    float NodeCountFactor = FMath::Min((float)ConditionNodes.Num() / 5.0f, 1.0f);

    // Combine factors
    return (ConditionNodes.Num() > 0) ?
        FMath::Min(ComplexityScore / (float)ConditionNodes.Num() * NodeCountFactor, 1.0f) :
        0.0f;
}

float UGWTEducationalTracker::AnalyzeVariableUsage(UGWTSpell* Spell) const
{
    // Analyze variable usage

    if (!Spell)
        return 0.0f;

    // Get all variable nodes
    TArray<UGWTSpellNode*> VariableNodes = Spell->GetNodesOfType(EGWTSpellComponentType::Variable);

    // Check for different operation types
    int32 ReadOps = 0;
    int32 WriteOps = 0;
    int32 ModifyOps = 0;

    for (UGWTSpellNode* Node : VariableNodes)
    {
        UGWTVariableNode* VarNode = Cast<UGWTVariableNode>(Node);
        if (VarNode)
        {
            // Count operation types
            switch (VarNode->Operation)
            {
            case UGWTVariableNode::EVariableOperation::Read:
                ReadOps++;
                break;

            case UGWTVariableNode::EVariableOperation::Write:
                WriteOps++;
                break;

            default:
                ModifyOps++;
                break;
            }
        }
    }

    // Higher score for balanced usage of different operations
    float OperationBalance = 0.0f;
    if (VariableNodes.Num() > 0)
    {
        float ReadRatio = (float)ReadOps / VariableNodes.Num();
        float WriteRatio = (float)WriteOps / VariableNodes.Num();
        float ModifyRatio = (float)ModifyOps / VariableNodes.Num();

        // Balanced usage = more even distribution
        OperationBalance = (ReadRatio > 0.0f ? 0.33f : 0.0f) +
            (WriteRatio > 0.0f ? 0.33f : 0.0f) +
            (ModifyRatio > 0.0f ? 0.34f : 0.0f);
    }

    // Scale by number of variable nodes (max 5 for full score)
    float NodeCountFactor = FMath::Min((float)VariableNodes.Num() / 5.0f, 1.0f);

    // Combine factors
    return OperationBalance * NodeCountFactor;
}

float UGWTEducationalTracker::AnalyzeLoopUsage(UGWTSpell* Spell) const
{
    // Analyze loop usage

    if (!Spell)
        return 0.0f;

    // Get all flow nodes
    TArray<UGWTSpellNode*> FlowNodes = Spell->GetNodesOfType(EGWTSpellComponentType::Flow);

    // Count loop types
    int32 RepeatLoops = 0;
    int32 WhileLoops = 0;
    int32 ForEachLoops = 0;

    for (UGWTSpellNode* Node : FlowNodes)
    {
        UGWTFlowNode* FlowNode = Cast<UGWTFlowNode>(Node);
        if (FlowNode)
        {
            // Count loop types
            switch (FlowNode->FlowType)
            {
            case EGWTFlowType::Repeat:
                RepeatLoops++;
                break;

            case EGWTFlowType::While:
                WhileLoops++;
                break;

            case EGWTFlowType::ForEach:
                ForEachLoops++;
                break;

            default:
                break;
            }

            // Check loop body complexity by counting nodes inside
            if (FlowNode->BodyNode)
            {
                // More body nodes = more complex loop usage
                // This would need a more sophisticated implementation to count
                // all nodes in the body branch
            }
        }
    }

    // Higher score for using different loop types
    float LoopVariety = 0.0f;
    if (FlowNodes.Num() > 0)
    {
        LoopVariety = (RepeatLoops > 0 ? 0.33f : 0.0f) +
            (WhileLoops > 0 ? 0.33f : 0.0f) +
            (ForEachLoops > 0 ? 0.34f : 0.0f);
    }

    // Scale by number of flow nodes (max 3 for full score)
    float NodeCountFactor = FMath::Min((float)FlowNodes.Num() / 3.0f, 1.0f);

    // Combine factors
    return LoopVariety * NodeCountFactor;
}

float UGWTEducationalTracker::AnalyzeFunctionAbstraction(UGWTSpell* Spell) const
{
    // Analyze function abstraction - this would be more complex in a real implementation
    // In this example, we'll use a simpler metric

    if (!Spell)
        return 0.0f;

    // For now, we'll simply return a base value if the spell has multiple sections
    // In a full implementation, this would analyze spell structure more deeply

    int32 NodeCount = Spell->CountNodes();
    int32 EffectNodes = Spell->GetNodesOfType(EGWTSpellComponentType::Effect).Num();
    int32 MagicNodes = Spell->GetNodesOfType(EGWTSpellComponentType::Magic).Num();

    // If spell has a good mix of different node types, consider it better abstraction
    float NodeTypeDiversity = (float)(EffectNodes + MagicNodes) / (float)NodeCount;

    // Scale by total node count (larger spells with diverse node types = better abstraction)
    float NodeCountFactor = FMath::Min((float)NodeCount / 10.0f, 1.0f);

    return NodeTypeDiversity * NodeCountFactor;
}

int32 UGWTEducationalTracker::GetSpellIterationCount(UGWTSpell* Spell) const
{
    // Count how many times this spell has been modified
    // In a full implementation, this would track edits to the spell over time

    if (!Spell)
        return 0;

    // For now, check the usage history for instances of this spell
    int32 IterationCount = 0;

    for (const FGWTSpellUsageData& Data : SpellUsageHistory)
    {
        if (Data.SpellID == Spell->NodeID)
        {
            IterationCount++;
        }
    }

    return IterationCount;
}