// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "SaveGame/DialogueSaveData.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "GameFramework/Actor.h"

void UDialogueSessionContext::BeginDestroy()
{
    Super::BeginDestroy();
}

FString UDialogueSessionContext::GetCustomVariable(FName VariableName, const FString& DefaultValue) const
{
    if (const FString* Value = CustomVariables.Find(VariableName))
    {
        return *Value;
    }
    return DefaultValue;
}

void UDialogueSessionContext::SetCustomVariable(FName VariableName, const FString& Value)
{
    CustomVariables.Add(VariableName, Value);
}

void UDialogueSessionContext::AddTag(FGameplayTag Tag)
{
    ActiveTags.AddTag(Tag);
}

void UDialogueSessionContext::RemoveTag(FGameplayTag Tag)
{
    ActiveTags.RemoveTag(Tag);
}

bool UDialogueSessionContext::HasTag(FGameplayTag Tag) const
{
    return ActiveTags.HasTag(Tag);
}

void UDialogueSessionContext::AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId)
{
    FConversationEntry Entry;
    Entry.SpeakerName = SpeakerName;
    Entry.DialogueText = DialogueText;
    Entry.NodeId = NodeId;
    Entry.Timestamp = FDateTime::Now();
    
    ConversationHistory.Add(Entry);
}

void UDialogueSessionContext::MarkNodeVisited(FName NodeId)
{
    VisitedNodes.AddUnique(NodeId);
}

bool UDialogueSessionContext::WasNodeVisited(FName NodeId) const
{
    return VisitedNodes.Contains(NodeId);
}

FDialogueSessionSaveData UDialogueSessionContext::ToSaveData(FName DialogueId) const
{
    FDialogueSessionSaveData SaveData;
    
    SaveData.DialogueId = DialogueId;
    SaveData.CurrentNodeId = CurrentNode.IsValid() ? CurrentNode->NodeId : NAME_None;
    SaveData.VisitedNodes = VisitedNodes;
    SaveData.CustomVariables = CustomVariables;
    SaveData.ActiveTags = ActiveTags;
    SaveData.BaseAffinityLevel = BaseAffinityLevel;
    SaveData.SessionStartTime = SessionStartTime;
    
    // Конвертация истории разговора
    for (const FConversationEntry& Entry : ConversationHistory)
    {
        FDialogueHistoryEntry HistoryEntry;
        HistoryEntry.SpeakerName = Entry.SpeakerName;
        HistoryEntry.DialogueText = Entry.DialogueText;
        HistoryEntry.NodeId = Entry.NodeId;
        HistoryEntry.Timestamp = Entry.Timestamp;
        SaveData.ConversationHistory.Add(HistoryEntry);
    }
    
    // Сохранить пути к акторам
    if (Player)
    {
        SaveData.PlayerPath = FSoftObjectPath(Player);
    }
    if (NPC)
    {
        SaveData.NPCPath = FSoftObjectPath(NPC);
    }
    
    return SaveData;
}

bool UDialogueSessionContext::FromSaveData(const FDialogueSessionSaveData& SaveData, AActor* InPlayer, AActor* InNPC)
{
    if (!SaveData.IsValid())
    {
        return false;
    }
    
    // Восстановить базовые данные
    Player = InPlayer;
    NPC = InNPC;
    VisitedNodes = SaveData.VisitedNodes;
    CustomVariables = SaveData.CustomVariables;
    ActiveTags = SaveData.ActiveTags;
    BaseAffinityLevel = SaveData.BaseAffinityLevel;
    SessionStartTime = SaveData.SessionStartTime;
    
    // Восстановить историю разговора
    ConversationHistory.Empty();
    for (const FDialogueHistoryEntry& HistoryEntry : SaveData.ConversationHistory)
    {
        FConversationEntry Entry;
        Entry.SpeakerName = HistoryEntry.SpeakerName;
        Entry.DialogueText = HistoryEntry.DialogueText;
        Entry.NodeId = HistoryEntry.NodeId;
        Entry.Timestamp = HistoryEntry.Timestamp;
        ConversationHistory.Add(Entry);
    }
    
    // Примечание: CurrentNode будет установлен DialogueRunner'ом после загрузки
    
    return true;
}

void UDialogueSessionContext::SaveToJson(FString& OutJson) const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    
    // Save custom variables
    TSharedPtr<FJsonObject> VarsObj = MakeShareable(new FJsonObject());
    for (const auto& Pair : CustomVariables)
    {
        VarsObj->SetStringField(Pair.Key.ToString(), Pair.Value);
    }
    JsonObject->SetObjectField(TEXT("CustomVariables"), VarsObj);
    
    // Save visited nodes
    TArray<TSharedPtr<FJsonValue>> VisitedArray;
    for (const FName& NodeId : VisitedNodes)
    {
        VisitedArray.Add(MakeShareable(new FJsonValueString(NodeId.ToString())));
    }
    JsonObject->SetArrayField(TEXT("VisitedNodes"), VisitedArray);
    
    // Save conversation history
    TArray<TSharedPtr<FJsonValue>> HistoryArray;
    for (const FConversationEntry& Entry : ConversationHistory)
    {
        TSharedPtr<FJsonObject> EntryObj = MakeShareable(new FJsonObject());
        EntryObj->SetStringField(TEXT("Speaker"), Entry.SpeakerName.ToString());
        EntryObj->SetStringField(TEXT("Text"), Entry.DialogueText.ToString());
        EntryObj->SetStringField(TEXT("NodeId"), Entry.NodeId.ToString());
        EntryObj->SetStringField(TEXT("Timestamp"), Entry.Timestamp.ToString());
        HistoryArray.Add(MakeShareable(new FJsonValueObject(EntryObj)));
    }
    JsonObject->SetArrayField(TEXT("History"), HistoryArray);
    
    // Save active tags
    TArray<TSharedPtr<FJsonValue>> TagsArray;
    for (const FGameplayTag& Tag : ActiveTags)
    {
        TagsArray.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
    }
    JsonObject->SetArrayField(TEXT("ActiveTags"), TagsArray);
    
    // Save other fields
    JsonObject->SetNumberField(TEXT("BaseAffinity"), BaseAffinityLevel);
    JsonObject->SetStringField(TEXT("SessionStart"), SessionStartTime.ToString());
    
    if (CurrentNode.IsValid())
    {
        JsonObject->SetStringField(TEXT("CurrentNode"), CurrentNode->NodeId.ToString());
    }
    
    // Serialize to string
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}

bool UDialogueSessionContext::LoadFromJson(const FString& Json)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }
    
    // Load custom variables
    if (const TSharedPtr<FJsonObject>* VarsObj; JsonObject->TryGetObjectField(TEXT("CustomVariables"), VarsObj))
    {
        CustomVariables.Empty();
        for (const auto& Pair : (*VarsObj)->Values)
        {
            CustomVariables.Add(FName(*Pair.Key), Pair.Value->AsString());
        }
    }
    
    // Load visited nodes
    const TArray<TSharedPtr<FJsonValue>>* VisitedArray;
    if (JsonObject->TryGetArrayField(TEXT("VisitedNodes"), VisitedArray))
    {
        VisitedNodes.Empty();
        for (const auto& Value : *VisitedArray)
        {
            VisitedNodes.Add(FName(*Value->AsString()));
        }
    }
    
    // Load conversation history
    const TArray<TSharedPtr<FJsonValue>>* HistoryArray;
    if (JsonObject->TryGetArrayField(TEXT("History"), HistoryArray))
    {
        ConversationHistory.Empty();
        for (const auto& Value : *HistoryArray)
        {
            const TSharedPtr<FJsonObject>* EntryObj;
            if (Value->TryGetObject(EntryObj))
            {
                FConversationEntry Entry;
                Entry.SpeakerName = FText::FromString((*EntryObj)->GetStringField(TEXT("Speaker")));
                Entry.DialogueText = FText::FromString((*EntryObj)->GetStringField(TEXT("Text")));
                Entry.NodeId = FName(*(*EntryObj)->GetStringField(TEXT("NodeId")));
                FDateTime::Parse((*EntryObj)->GetStringField(TEXT("Timestamp")), Entry.Timestamp);
                ConversationHistory.Add(Entry);
            }
        }
    }
    
    // Load active tags
    const TArray<TSharedPtr<FJsonValue>>* TagsArray;
    if (JsonObject->TryGetArrayField(TEXT("ActiveTags"), TagsArray))
    {
        ActiveTags.Reset();
        for (const auto& Value : *TagsArray)
        {
            FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*Value->AsString()), false);
            if (Tag.IsValid())
            {
                ActiveTags.AddTag(Tag);
            }
        }
    }
    
    // Load other fields
    BaseAffinityLevel = JsonObject->GetNumberField(TEXT("BaseAffinity"));
    FDateTime::Parse(JsonObject->GetStringField(TEXT("SessionStart")), SessionStartTime);
    
    return true;
}
