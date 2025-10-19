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
 * Сериализуемая запись истории диалога
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueHistoryEntry
{
    GENERATED_BODY()

    /** Имя говорящего */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    /** Текст реплики */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FText DialogueText;

    /** ID узла */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName NodeId;

    /** Временная метка */
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
 * Структура для хранения счетчиков посещений узлов одного диалога
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueNodeVisitData
{
    GENERATED_BODY()

    /** ID диалога */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueId;

    /** Счетчики посещений узлов (NodeId -> Count) */
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
 * Сохраненное состояние сессии диалога
 * Содержит все данные для восстановления диалога
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueSessionSaveData
{
    GENERATED_BODY()

    /** ID диалога (ссылка на DialogueAsset) */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueId;

    /** ID текущего узла */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FName CurrentNodeId;

    /** Посещенные узлы */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FName> VisitedNodes;

    /** История разговора */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueHistoryEntry> ConversationHistory;

    /** Кастомные переменные */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, FString> CustomVariables;

    /** Активные теги */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTagContainer ActiveTags;

    /** Базовый уровень отношений */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    float BaseAffinityLevel = 0.0f;

    /** Время начала сессии */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDateTime SessionStartTime;

    /** Путь к Player Actor */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FSoftObjectPath PlayerPath;

    /** Путь к NPC Actor */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FSoftObjectPath NPCPath;

    FDialogueSessionSaveData()
        : DialogueId(NAME_None)
        , CurrentNodeId(NAME_None)
        , BaseAffinityLevel(0.0f)
        , SessionStartTime(FDateTime::Now())
    {}

    /** Проверка валидности */
    bool IsValid() const
    {
        return !DialogueId.IsNone();
    }
};

/**
 * Главный класс SaveGame для системы диалогов
 * Хранит все сохраненные диалоги и глобальное состояние
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UDialogueSaveGame();

    /** Имя слота для сохранения */
    static const FString SaveSlotName;

    /** Индекс пользователя */
    static const int32 UserIndex;

    //~ Begin Saved Data

    /** Активная сессия диалога (если есть) */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDialogueSessionSaveData ActiveSession;

    /** История всех завершенных диалогов (ID диалогов) */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FName> CompletedDialogues;

    /** Счетчики посещений для каждого диалога */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueNodeVisitData> NodeVisitData;

    /** Глобальные флаги памяти */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, bool> GlobalMemoryFlags;

    /** Глобальные переменные */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, FString> GlobalVariables;

    /** Время последнего сохранения */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    FDateTime LastSaveTime;

    /** Версия формата сохранения */
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Dialogue")
    int32 SaveVersion = 1;

    //~ End Saved Data

    //~ Begin API

    /** Сохранить активную сессию */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SaveActiveSession(const FDialogueSessionSaveData& SessionData);

    /** Загрузить активную сессию */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    FDialogueSessionSaveData GetActiveSession() const { return ActiveSession; }

    /** Проверить есть ли активная сессия */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool HasActiveSession() const { return ActiveSession.IsValid(); }

    /** Очистить активную сессию */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void ClearActiveSession();

    /** Отметить диалог как завершенный */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void MarkDialogueCompleted(FName DialogueId);

    /** Был ли диалог завершен */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool WasDialogueCompleted(FName DialogueId) const;

    /** Инкрементировать счетчик посещений узла */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void IncrementNodeVisit(FName DialogueId, FName NodeId);

    /** Получить количество посещений узла */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    int32 GetNodeVisitCount(FName DialogueId, FName NodeId) const;

    /** Установить глобальный флаг памяти */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SetGlobalMemory(FName Key, bool Value);

    /** Получить глобальный флаг памяти */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool GetGlobalMemory(FName Key, bool DefaultValue = false) const;

    /** Установить глобальную переменную */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void SetGlobalVariable(FName Key, const FString& Value);

    /** Получить глобальную переменную */
    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    FString GetGlobalVariable(FName Key, const FString& DefaultValue = TEXT("")) const;

    //~ End API

    //~ Begin Static Helpers

    /** Создать или загрузить SaveGame */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static UDialogueSaveGame* LoadOrCreateSaveGame(UObject* WorldContextObject);

    /** Сохранить SaveGame */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static bool SaveDialogueSaveGame(UObject* WorldContextObject, UDialogueSaveGame* SaveGameObject);

    /** Удалить SaveGame */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame", meta = (WorldContext = "WorldContextObject"))
    static bool DeleteDialogueSaveGame(UObject* WorldContextObject);

    //~ End Static Helpers

protected:
    /** Получить данные посещений для диалога (создать если не существует) */
    FDialogueNodeVisitData* GetOrCreateNodeVisitData(FName DialogueId);

    /** Найти данные посещений для диалога */
    const FDialogueNodeVisitData* FindNodeVisitData(FName DialogueId) const;
};
