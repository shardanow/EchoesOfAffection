// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialogueConnection.h"
#include "DialogueNode.generated.h"

// Forward declarations
class UDialogueCondition;
class UDialogueEffect;
class UDialogueSessionContext;

/**
 * Тип узла диалога
 */
UENUM(BlueprintType)
enum class EDialogueNodeType : uint8
{
    /** Стандартная реплика NPC */
    NpcLine,
    
    /** Выбор игрока (branch point) */
    PlayerChoice,
    
    /** Проверка условия (invisible node) */
    Condition,
    
    /** Случайный выбор (weighted random) */
    Random,
    
    /** Событие (trigger only) */
    Event,
    
    /** Завершение диалога */
    End,
    
    /** AI-генерируемая реплика */
    AIGenerated
};

/**
 * Приоритет эмоции для анимации
 */
UENUM(BlueprintType)
enum class EEmotionIntensity : uint8
{
    Neutral,
    Mild,
    Moderate,
    Strong,
    Extreme
};

/**
 * Метаданные для камеры и анимации
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialoguePresentationData
{
    GENERATED_BODY()

    /** Тег эмоции (GameplayTag: Emotion.Happy, Emotion.Angry, etc) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FGameplayTag EmotionTag;

    /** Интенсивность эмоции */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    EEmotionIntensity Intensity = EEmotionIntensity::Neutral;

    /** Название анимации лица/тела */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FName AnimationName;

    /** Аудиофайл озвучки */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    TSoftObjectPtr<class USoundBase> VoiceAudio;

    /** Время паузы перед следующей репликой (сек) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    float PauseAfter = 0.5f;

    /** Камера должна сфокусироваться на говорящем */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    bool bFocusCamera = true;

    /** Дистанция камеры (близко/далеко) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    float CameraDistance = 200.0f;
};

/**
 * Базовая структура узла диалога
 * Используется в DataTable и runtime кэше
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueNodeData
{
    GENERATED_BODY()

    /** Уникальный ID узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName NodeId;

    /** Тип узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    EDialogueNodeType NodeType = EDialogueNodeType::NpcLine;

    /** ID персонажа-говорящего (для NpcLine) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName SpeakerId;

    /** Текст реплики (поддерживает {placeholders}) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = true))
    FText DialogueText;

    /** Текст выбора игрока (для PlayerChoice) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    FText ChoiceText;

    /** Иконка последствий (опционально) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TSoftObjectPtr<class UTexture2D> ChoiceIcon;

    /** Presentation метаданные */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FDialoguePresentationData Presentation;

    /** Строка условий (DSL) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
    FString ConditionExpression;

    /** Строка эффектов (DSL) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
    FString EffectExpression;

    /** Теги для фильтрации/поиска */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    FGameplayTagContainer NodeTags;

    /** Вес для Random узлов */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    float RandomWeight = 1.0f;

    /** Можно показывать только 1 раз? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    bool bOneTimeOnly = false;

    /** Автоматический переход (без ожидания input) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    bool bAutoAdvance = false;

    /** Время задержки автоперехода */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta", meta = (EditCondition = "bAutoAdvance"))
    float AutoAdvanceDelay = 2.0f;

    /** Подсказка для AI (если AIGenerated) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MultiLine = true))
    FString AIPromptHint;
};

/**
 * Ребро графа — переход между узлами
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueEdgeData
{
    GENERATED_BODY()

    /** Узел источник */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FName FromNodeId;

    /** Узел назначение */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FName ToNodeId;

    /** Условие перехода (опционально) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FString EdgeCondition;

    /** Приоритет (для сортировки) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    int32 Priority = 0;
};

/**
 * Runtime представление узла (создаётся из FDialogueNodeData)
 * Добавляет состояние и кэшированные данные
 */
//UCLASS(BlueprintType)
UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType, Blueprintable)
class DIALOGUESYSTEMCORE_API UDialogueNode : public UObject
{
    GENERATED_BODY()

public:
    UDialogueNode();

    /** Уникальный ID узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NodeId;

    /** Тип узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    EDialogueNodeType NodeType;

    /** ID персонажа-говорящего */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName SpeakerId;

    /** Текст реплики */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = true))
    FText DialogueText;

    /** Текст выбора игрока (для PlayerChoice) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    /** Иконка говорящего (опционально) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UTexture2D> SpeakerPortrait;

    /** Presentation метаданные */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FDialoguePresentationData Presentation;

    /** Тег эмоции (GameplayTag: Emotion.Happy, Emotion.Angry, etc) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTag EmotionTag;

    /** Автоматический переход */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bAutoAdvance;

    /** Время задержки автоперехода */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "bAutoAdvance"))
    float AutoAdvanceDelay;

    /** Теги узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTagContainer Tags;

    /** Вес для Random узлов */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float RandomWeight;

    /** Можно показывать только 1 раз? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bOneTimeOnly;

    /** Условие (объект) */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TObjectPtr<UDialogueCondition> Condition;

    /** Эффекты узла */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TArray<TObjectPtr<UDialogueEffect>> Effects;

    /** Соединения с другими узлами */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueConnection> Connections;

    /** Был ли узел посещён в текущей сессии */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bVisitedThisSession;

    /** Количество посещений (для статистики) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    int32 VisitCount;

    /** Метод для инициализации */
    void Initialize(const FDialogueNodeData& InData);

    /** Отметить как посещённый */
    void MarkVisited()
    {
        bVisitedThisSession = true;
        VisitCount++;
    }

    /** Получить финальный текст с подстановкой плейсхолдеров */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FText GetResolvedText(const UDialogueSessionContext* Context) const;

    /** Проверить доступность узла */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool IsAvailable(const UDialogueSessionContext* Context) const;

    /** Get dialogue text (wrapper for compatibility) */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetDialogueText() const { return DialogueText; }

    /** Get choice text (wrapper for compatibility) */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetChoiceText() const { return ChoiceText; }

    /** Get speaker name */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetSpeakerName() const { return FText::FromName(SpeakerId); }

    /** Get emotion tag */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FGameplayTag GetEmotionTag() const { return EmotionTag; }

    /** Get speaker portrait */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    UTexture2D* GetSpeakerPortrait() const { return SpeakerPortrait.LoadSynchronous(); }
};
