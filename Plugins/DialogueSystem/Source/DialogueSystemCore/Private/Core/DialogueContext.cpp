// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueState.h"
#include "Core/DialogueParticipants.h"
#include "Core/DialogueVariableStore.h"
#include "SaveGame/DialogueSaveData.h"
#include "GameFramework/Actor.h"

// v1.3.1: Include serializers instead of doing serialization inline
#include "Serialization/DialogueSnapshotSerializer.h"
#include "Serialization/DialogueSaveDataSerializer.h"
#include "Serialization/DialogueJsonSerializer.h"

//==============================================================================
// v1.3.1: Component Initialization
//==============================================================================

void UDialogueSessionContext::Initialize()
{
    if (!State) { State = NewObject<UDialogueState>(this, TEXT("State")); }
    if (!Participants) { Participants = NewObject<UDialogueParticipants>(this, TEXT("Participants")); }
    if (!Variables) { Variables = NewObject<UDialogueVariableStore>(this, TEXT("Variables")); }
}

void UDialogueSessionContext::BeginDestroy() { Super::BeginDestroy(); }

//==============================================================================
// v1.3.1: Facade Delegates - State Component
//==============================================================================

UDialogueNode* UDialogueSessionContext::GetCurrentNode() const { return State ? State->GetCurrentNode() : CurrentNode.Get(); }
void UDialogueSessionContext::SetCurrentNode(UDialogueNode* Node) { if (State) { State->SetCurrentNode(Node); } CurrentNode = Node; }
void UDialogueSessionContext::MarkNodeVisited(FName NodeId) { if (State) { State->MarkNodeVisited(NodeId); } VisitedNodes.AddUnique(NodeId); }
bool UDialogueSessionContext::WasNodeVisited(FName NodeId) const { return State ? State->WasNodeVisited(NodeId) : VisitedNodes.Contains(NodeId); }
void UDialogueSessionContext::AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId) { if (State) { State->AddToHistory(SpeakerName, DialogueText, NodeId); } }

//==============================================================================
// v1.3.1: Facade Delegates - Participants Component
//==============================================================================

AActor* UDialogueSessionContext::GetPlayer() const { return Participants ? Participants->GetPlayer() : Player.Get(); }
AActor* UDialogueSessionContext::GetNPC() const { return Participants ? Participants->GetNPC() : NPC.Get(); }
void UDialogueSessionContext::SetPlayer(AActor* InPlayer) { if (Participants) { Participants->SetPlayer(InPlayer); } Player = InPlayer; }
void UDialogueSessionContext::SetNPC(AActor* InNPC) { if (Participants) { Participants->SetNPC(InNPC); } NPC = InNPC; }

//==============================================================================
// v1.3.1: Facade Delegates - Variable Store Component
//==============================================================================

FString UDialogueSessionContext::GetCustomVariable(FName VariableName, const FString& DefaultValue) const { return Variables ? Variables->GetCustomVariable(VariableName, DefaultValue) : CustomVariables.FindRef(VariableName); }
void UDialogueSessionContext::SetCustomVariable(FName VariableName, const FString& Value) { if (Variables) { Variables->SetCustomVariable(VariableName, Value); } CustomVariables.Add(VariableName, Value); }
void UDialogueSessionContext::AddTag(FGameplayTag Tag) { if (Variables) { Variables->AddTag(Tag); } ActiveTags.AddTag(Tag); }
void UDialogueSessionContext::RemoveTag(FGameplayTag Tag) { if (Variables) { Variables->RemoveTag(Tag); } ActiveTags.RemoveTag(Tag); }
bool UDialogueSessionContext::HasTag(FGameplayTag Tag) const { return Variables ? Variables->HasTag(Tag) : ActiveTags.HasTag(Tag); }

//==============================================================================
// v1.3.1: Type-Safe Variable API (Delegates to VariableStore)
//==============================================================================

void UDialogueSessionContext::SetVariantBool(FName VariableName, bool Value) { if (Variables) { Variables->SetVariantBool(VariableName, Value); } }
void UDialogueSessionContext::SetVariantInt(FName VariableName, int32 Value) { if (Variables) { Variables->SetVariantInt(VariableName, Value); } }
void UDialogueSessionContext::SetVariantFloat(FName VariableName, float Value) { if (Variables) { Variables->SetVariantFloat(VariableName, Value); } }
void UDialogueSessionContext::SetVariantString(FName VariableName, const FString& Value) { if (Variables) { Variables->SetVariantString(VariableName, Value); } }
void UDialogueSessionContext::SetVariantName(FName VariableName, FName Value) { if (Variables) { Variables->SetVariantName(VariableName, Value); } }
void UDialogueSessionContext::SetVariantObject(FName VariableName, UObject* Value) { if (Variables) { Variables->SetVariantObject(VariableName, Value); } }
void UDialogueSessionContext::SetVariantTag(FName VariableName, FGameplayTag Value) { if (Variables) { Variables->SetVariantTag(VariableName, Value); } }

bool UDialogueSessionContext::GetVariantBool(FName VariableName, bool DefaultValue) const { return Variables ? Variables->GetVariantBool(VariableName, DefaultValue) : DefaultValue; }
int32 UDialogueSessionContext::GetVariantInt(FName VariableName, int32 DefaultValue) const { return Variables ? Variables->GetVariantInt(VariableName, DefaultValue) : DefaultValue; }
float UDialogueSessionContext::GetVariantFloat(FName VariableName, float DefaultValue) const { return Variables ? Variables->GetVariantFloat(VariableName, DefaultValue) : DefaultValue; }
FString UDialogueSessionContext::GetVariantString(FName VariableName, const FString& DefaultValue) const { return Variables ? Variables->GetVariantString(VariableName, DefaultValue) : DefaultValue; }
FName UDialogueSessionContext::GetVariantName(FName VariableName, FName DefaultValue) const { return Variables ? Variables->GetVariantName(VariableName, DefaultValue) : DefaultValue; }
UObject* UDialogueSessionContext::GetVariantObject(FName VariableName, UObject* DefaultValue) const { return Variables ? Variables->GetVariantObject(VariableName, DefaultValue) : DefaultValue; }
FGameplayTag UDialogueSessionContext::GetVariantTag(FName VariableName, const FGameplayTag& DefaultValue) const { return Variables ? Variables->GetVariantTag(VariableName, DefaultValue) : DefaultValue; }

//==============================================================================
// v1.3.1: Snapshot API - Delegated to FDialogueSnapshotSerializer
//==============================================================================

FDialogueStateSnapshot UDialogueSessionContext::CreateSnapshot(uint8 DialogueState, const TArray<UDialogueNode*>& NodeHistory) const
{
    return FDialogueSnapshotSerializer::Get().CreateSnapshot(this, DialogueState, NodeHistory);
}

bool UDialogueSessionContext::RestoreFromSnapshot(const FDialogueStateSnapshot& Snapshot)
{
    return FDialogueSnapshotSerializer::Get().RestoreFromSnapshot(this, Snapshot);
}

//==============================================================================
// v1.3.1: Reset State
//==============================================================================

void UDialogueSessionContext::ResetState()
{
    if (State) { State->Reset(); }
    if (Participants) { Participants->Reset(); }
    if (Variables) { Variables->Reset(); }
}

//==============================================================================
// v1.3.1: SaveGame API - Delegated to FDialogueSaveDataSerializer
//==============================================================================

FDialogueSessionSaveData UDialogueSessionContext::ToSaveData(FName DialogueId) const
{
    return FDialogueSaveDataSerializer::Get().ToSaveData(this, DialogueId);
}

bool UDialogueSessionContext::FromSaveData(const FDialogueSessionSaveData& SaveData, AActor* InPlayer, AActor* InNPC)
{
    return FDialogueSaveDataSerializer::Get().FromSaveData(this, SaveData, InPlayer, InNPC);
}

//==============================================================================
// v1.3.1: JSON API - Delegated to FDialogueJsonSerializer
//==============================================================================

void UDialogueSessionContext::SaveToJson(FString& OutJson) const
{
    FDialogueJsonSerializer::Get().SaveToJson(this, OutJson);
}

bool UDialogueSessionContext::LoadFromJson(const FString& Json)
{
    return FDialogueJsonSerializer::Get().LoadFromJson(this, Json);
}
