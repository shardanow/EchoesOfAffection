// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "DialogueSaveData.generated.h"

// Forward declarations
class UDialogueDataAsset;
class UDialogueNode;

/**
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueHistoryEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FText DialogueText;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName NodeId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDateTime Timestamp;

    FDialogueHistoryEntry()
        : SpeakerName(FText::GetEmpty())
        , DialogueText(FText::GetEmpty())
        , NodeId(NAME_None)
        , Timestamp(FDateTime::Now())
    {}

    FDialogueHistoryEntry(const FText& InSpeaker, const FText& InText, FName InNodeId)
        : SpeakerName(InSpeaker)
        , DialogueText(InText)
        , NodeId(InNodeId)
        , Timestamp(FDateTime::Now())
    {}
};

/**
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueNodeVisitData
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, int32> NodeVisitCounts;

    FDialogueNodeVisitData()
        : DialogueId(NAME_None)
    {}

    FDialogueNodeVisitData(FName InDialogueId)
        : DialogueId(InDialogueId)
    {}
};

/**
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueSessionSaveData
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName CurrentNodeId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FName> VisitedNodes;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueHistoryEntry> ConversationHistory;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, FString> CustomVariables;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTagContainer ActiveTags;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    float BaseAffinityLevel = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDateTime SessionStartTime;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FSoftObjectPath PlayerPath;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FSoftObjectPath NPCPath;

    FDialogueSessionSaveData()
        : DialogueId(NAME_None)
        , CurrentNodeId(NAME_None)
        , BaseAffinityLevel(0.0f)
        , SessionStartTime(FDateTime::Now())
    {}

    bool IsValid() const
    {
        return !DialogueId.IsNone();
    }
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UDialogueSaveGame();

    static const FString SaveSlotName;

    static const int32 UserIndex;

    //~ Begin Saved Data

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDialogueSessionSaveData ActiveSession;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FName> CompletedDialogues;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueNodeVisitData> NodeVisitData;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, bool> GlobalMemoryFlags;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, FString> GlobalVariables;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDateTime LastSaveTime;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    int32 SaveVersion = 1;

    //~ End Saved Data

    //~ Begin API

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SaveActiveSession(const FDialogueSessionSaveData& SessionData);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    FDialogueSessionSaveData GetActiveSession() const { return ActiveSession; }

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool HasActiveSession() const { return ActiveSession.IsValid(); }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void ClearActiveSession();

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void MarkDialogueCompleted(FName DialogueId);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool WasDialogueCompleted(FName DialogueId) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void IncrementNodeVisit(FName DialogueId, FName NodeId);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    int32 GetNodeVisitCount(FName DialogueId, FName NodeId) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SetGlobalMemory(FName Key, bool Value);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool GetGlobalMemory(FName Key, bool DefaultValue = false) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SetGlobalVariable(FName Key, const FString& Value);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    FString GetGlobalVariable(FName Key, const FString& DefaultValue = TEXT("")) const;

    //~ End API

    //~ Begin Static Helpers

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static UDialogueSaveGame* LoadOrCreateSaveGame(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static bool SaveDialogueSaveGame(UObject* WorldContextObject, UDialogueSaveGame* SaveGameObject);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static bool DeleteDialogueSaveGame(UObject* WorldContextObject);

    //~ End Static Helpers

protected:
    FDialogueNodeVisitData* GetOrCreateNodeVisitData(FName DialogueId);

    const FDialogueNodeVisitData* FindNodeVisitData(FName DialogueId) const;
};
