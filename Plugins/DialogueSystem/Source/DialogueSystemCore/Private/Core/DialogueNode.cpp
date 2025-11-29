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
    ListenerId = InData.ListenerId;  // NEW v1.18: Copy ListenerId
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
        // Replace special placeholders
        if (Context->GetPlayer())
        {
            ResolvedString = ResolvedString.Replace(TEXT("{Player}"), *Context->GetPlayer()->GetName());
        }
  
        // Replace {Affinity} with current affinity level
        float Affinity = Context->GetParticipants() ? Context->GetParticipants()->GetBaseAffinityLevel() : 0.0f;
        FString AffinityString = FString::Printf(TEXT("%.0f"), Affinity);
        ResolvedString = ResolvedString.Replace(TEXT("{Affinity}"), *AffinityString);

        // Replace custom variables
        TArray<FName> Keys = Context->GetVariables()->GetAllVariableKeys();
        for (const FName& Key : Keys)
        {
            FString Placeholder = FString::Printf(TEXT("{%s}"), *Key.ToString());
            FString Value = Context->GetCustomVariable(Key);
            ResolvedString = ResolvedString.Replace(*Placeholder, *Value);
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
