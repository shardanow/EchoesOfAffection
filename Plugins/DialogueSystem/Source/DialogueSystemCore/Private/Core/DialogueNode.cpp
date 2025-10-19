// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "Conditions/DialogueConditionEvaluator.h"

UDialogueNode::UDialogueNode()
    : NodeType(EDialogueNodeType::NpcLine)
    , bAutoAdvance(false)
    , AutoAdvanceDelay(2.0f)
    , RandomWeight(1.0f)
    , bOneTimeOnly(false)
    , bVisitedThisSession(false)
    , VisitCount(0)
{
}

void UDialogueNode::Initialize(const FDialogueNodeData& InData)
{
    NodeId = InData.NodeId;
    NodeType = InData.NodeType;
    SpeakerId = InData.SpeakerId;
    DialogueText = InData.DialogueText;
    ChoiceText = InData.ChoiceText;
    Presentation = InData.Presentation;
    EmotionTag = InData.Presentation.EmotionTag;
    Tags = InData.NodeTags;
    RandomWeight = InData.RandomWeight;
    bOneTimeOnly = InData.bOneTimeOnly;
    bAutoAdvance = InData.bAutoAdvance;
    AutoAdvanceDelay = InData.AutoAdvanceDelay;

    // TODO: Parse condition/effect DSL expressions and create runtime instances
}

FText UDialogueNode::GetResolvedText(const UDialogueSessionContext* Context) const
{
    FString ResolvedString = DialogueText.ToString();

    // Simple placeholder replacement
    if (Context)
    {
        // Replace {Player} with player name
        if (Context->Player)
        {
            ResolvedString = ResolvedString.Replace(TEXT("{Player}"), *Context->Player->GetName());
        }

        // Replace {Affinity} with current affinity level from context
        FString AffinityString = FString::Printf(TEXT("%.0f"), Context->BaseAffinityLevel);
        ResolvedString = ResolvedString.Replace(TEXT("{Affinity}"), *AffinityString);

        // Replace custom variables
        for (const TPair<FName, FString>& Pair : Context->CustomVariables)
        {
            FString Placeholder = FString::Printf(TEXT("{%s}"), *Pair.Key.ToString());
            ResolvedString = ResolvedString.Replace(*Placeholder, *Pair.Value);
        }
    }

    return FText::FromString(ResolvedString);
}

bool UDialogueNode::IsAvailable(const UDialogueSessionContext* Context) const
{
    // Check one-time only
    if (bOneTimeOnly && bVisitedThisSession)
    {
        return false;
    }

    // Check condition
    if (Condition)
    {
        return Condition->Evaluate(Context);
    }

    return true;
}
