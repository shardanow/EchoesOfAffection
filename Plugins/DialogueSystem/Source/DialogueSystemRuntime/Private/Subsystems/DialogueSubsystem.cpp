// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/DialogueSubsystem.h"
#include "Core/DialogueRunner.h"
#include "Data/DialogueDataAsset.h"
#include "Core/DialogueContext.h"
#include "Engine/AssetManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem Initialized"));
}

void UDialogueSubsystem::Deinitialize()
{
    // End any active dialogue
    if (ActiveDialogue)
    {
        EndActiveDialogue();
    }
    
    // Clear cache
    ClearCache();
    
    Super::Deinitialize();
}

UDialogueRunner* UDialogueSubsystem::StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC)
{
    if (!DialogueAsset || !Player || !NPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueSubsystem::StartDialogue - Invalid parameters"));
        return nullptr;
    }
    
    // End previous dialogue if any
    if (ActiveDialogue)
    {
        EndActiveDialogue();
    }
    
    // Create new runner
    ActiveDialogue = NewObject<UDialogueRunner>(this);
    
    // Bind to dialogue ended event
    ActiveDialogue->OnDialogueEnded.AddDynamic(this, &UDialogueSubsystem::HandleDialogueEnded);
    
    // Prepare participants array
    TArray<UObject*> Participants;
    Participants.Add(Player);
    Participants.Add(NPC);
    
    // Start dialogue
    ActiveDialogue->StartDialogue(DialogueAsset, Participants);
    
    // Add to history
    DialogueHistory.Add(DialogueAsset->DialogueId);
    
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Started dialogue '%s'"), *DialogueAsset->DialogueId.ToString());
    
    return ActiveDialogue;
}

void UDialogueSubsystem::EndActiveDialogue()
{
    if (!ActiveDialogue)
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Ending active dialogue"));
    
    ActiveDialogue->EndDialogue();
    ActiveDialogue = nullptr;
}

UDialogueDataAsset* UDialogueSubsystem::LoadDialogue(FName DialogueId)
{
    // Check cache first
    if (TObjectPtr<UDialogueDataAsset>* CachedAsset = LoadedDialogues.Find(DialogueId))
    {
        return *CachedAsset;
    }
    
    // Try to load from asset manager or search for asset
    // In a real project, you would use proper asset references
    // This is a simplified example
    
    UE_LOG(LogTemp, Warning, TEXT("DialogueSubsystem::LoadDialogue - Asset loading not fully implemented for '%s'"), *DialogueId.ToString());
    
    return nullptr;
}

void UDialogueSubsystem::PreloadDialogues(const TArray<FName>& DialogueIds)
{
    for (FName DialogueId : DialogueIds)
    {
        LoadDialogue(DialogueId);
    }
}

void UDialogueSubsystem::ClearCache()
{
    LoadedDialogues.Empty();
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Cache cleared"));
}

FString UDialogueSubsystem::SaveStateToJson() const
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    
    // Save settings
    RootObject->SetNumberField(TEXT("TypewriterSpeed"), TypewriterSpeed);
    RootObject->SetBoolField(TEXT("AutoAdvanceEnabled"), bAutoAdvanceEnabled);
    RootObject->SetNumberField(TEXT("AutoAdvanceDelay"), AutoAdvanceDelay);
    
    // Save history
    TArray<TSharedPtr<FJsonValue>> HistoryArray;
    for (const FName& DialogueId : DialogueHistory)
    {
        HistoryArray.Add(MakeShareable(new FJsonValueString(DialogueId.ToString())));
    }
    RootObject->SetArrayField(TEXT("DialogueHistory"), HistoryArray);
    
    // Save active dialogue state if any
    if (ActiveDialogue && ActiveDialogue->IsActive())
    {
        TSharedPtr<FJsonObject> ActiveDialogueObject = MakeShareable(new FJsonObject());
        
        if (UDialogueSessionContext* Context = ActiveDialogue->GetContext())
        {
            FString ContextJson;
            // Context->SaveToJson(ContextJson); // Not implemented yet
            ActiveDialogueObject->SetStringField(TEXT("Context"), ContextJson);
        }
        
        if (UDialogueNode* CurrentNode = ActiveDialogue->GetCurrentNode())
        {
            ActiveDialogueObject->SetStringField(TEXT("CurrentNodeId"), CurrentNode->NodeId.ToString());
        }
        
        RootObject->SetObjectField(TEXT("ActiveDialogue"), ActiveDialogueObject);
    }
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UDialogueSubsystem::LoadStateFromJson(const FString& Json)
{
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    
    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        return false;
    }
    
    // Load settings
    TypewriterSpeed = RootObject->GetNumberField(TEXT("TypewriterSpeed"));
    bAutoAdvanceEnabled = RootObject->GetBoolField(TEXT("AutoAdvanceEnabled"));
    AutoAdvanceDelay = RootObject->GetNumberField(TEXT("AutoAdvanceDelay"));
    
    // Load history
    DialogueHistory.Empty();
    const TArray<TSharedPtr<FJsonValue>>* HistoryArray;
    if (RootObject->TryGetArrayField(TEXT("DialogueHistory"), HistoryArray))
    {
        for (const TSharedPtr<FJsonValue>& Value : *HistoryArray)
        {
            DialogueHistory.Add(FName(*Value->AsString()));
        }
    }
    
    // Note: Loading active dialogue state would require more complex logic
    // to restore the dialogue runner and its state
    
    return true;
}

void UDialogueSubsystem::HandleDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Dialogue ended callback"));
    
    // Cleanup
    ActiveDialogue = nullptr;
}
