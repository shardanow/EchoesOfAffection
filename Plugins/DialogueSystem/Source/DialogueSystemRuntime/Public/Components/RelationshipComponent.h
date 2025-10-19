// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RelationshipComponent.generated.h"

/**
 * Тип стимула для изменения отношений
 */
UENUM(BlueprintType)
enum class ERelationshipStimulusType : uint8
{
    /** Подарок */
    Gift,
    
    /** Комплимент */
    Compliment,
    
    /** Оскорбление */
    Insult,
    
    /** Помощь */
    Help,
    
    /** Предательство */
    Betrayal,
    
    /** Флирт */
    Flirt,
    
    /** Угроза */
    Threat,
    
    /** Общее позитивное действие */
    Positive,
    
    /** Общее негативное действие */
    Negative
};

/**
 * Данные стимула
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FRelationshipStimulus
{
    GENERATED_BODY()

    /** Тип стимула */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    ERelationshipStimulusType Type = ERelationshipStimulusType::Positive;

    /** Сила воздействия (-100 to +100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    float Intensity = 10.0f;

    /** Источник (кто вызвал) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    TObjectPtr<AActor> Source;

    /** Дополнительный контекст (теги) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    FGameplayTagContainer ContextTags;

    /** Описание (для логов) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    FText Description;
};

/**
 * Состояние настроения NPC
 */
UENUM(BlueprintType)
enum class ENPCMoodState : uint8
{
    Neutral,
    Happy,
    Sad,
    Angry,
    Fearful,
    Excited,
    Bored,
    Anxious
};

/**
 * Делегаты
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRelationshipChanged, FName, AttributeName, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoodChanged, ENPCMoodState, OldMood, ENPCMoodState, NewMood);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRelationshipThresholdReached, FName, ThresholdName);

/**
 * Компонент управления отношениями NPC с игроком
 * 
 * Атрибуты:
 * - Affinity (Симпатия): -100 to +100
 * - Trust (Доверие): 0 to 100
 * - Respect (Уважение): 0 to 100
 * - Romance (Романтика): 0 to 100
 * - Fear (Страх): 0 to 100
 * - Jealousy (Ревность): 0 to 100
 * 
 * Особенности:
 * - Автоматический decay (медленное возвращение к нейтральному)
 * - Threshold события (например, Affinity >= 50 => "Friend" ранг)
 * - Модификаторы от персоны NPC (застенчивый, гордый, и т.д.)
 * - Интеграция с памятью (запоминает важные события)
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API URelationshipComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URelationshipComponent();

    //~ Begin Events

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnRelationshipChanged OnRelationshipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnMoodChanged OnMoodChanged;

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnRelationshipThresholdReached OnThresholdReached;

    //~ End Events

protected:
    //~ Begin Core Attributes

    /** Симпатия (-100 враг, 0 нейтрал, +100 лучший друг) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
    float Affinity = 0.0f;

    /** Доверие (0 - не доверяет, 100 - полное доверие) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Trust = 50.0f;

    /** Уважение */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Respect = 50.0f;

    /** Романтика (для любовных отношений) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Romance = 0.0f;

    /** Страх перед игроком */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Fear = 0.0f;

    /** Ревность */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Jealousy = 0.0f;

    //~ End Core Attributes

    //~ Begin Mood System

    /** Текущее настроение */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
    ENPCMoodState CurrentMood = ENPCMoodState::Neutral;

    /** Сколько времени NPC находится в текущем настроении */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mood")
    float MoodDuration = 0.0f;

    /** Минимальная длительность настроения (сек) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
    float MinMoodDuration = 30.0f;

    //~ End Mood System

    //~ Begin Decay Settings

    /** Включен ли автоматический decay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    bool bEnableDecay = true;

    /** Скорость decay для Affinity (единиц в секунду) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float AffinityDecayRate = 0.5f;

    /** Целевое значение для decay Affinity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float AffinityDecayTarget = 0.0f;

    /** Скорость decay для других атрибутов */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float OtherDecayRate = 1.0f;

    //~ End Decay Settings

    //~ Begin Modifiers (from Persona)

    /** Модификатор реакции на позитивные стимулы */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float PositiveSensitivity = 1.0f;

    /** Модификатор реакции на негативные стимулы */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float NegativeSensitivity = 1.0f;

    /** Насколько легко NPC прощает */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float ForgivenessRate = 1.0f;

    /** Насколько сильно NPC запоминает обиды */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float GrudgeStrength = 1.0f;

    //~ End Modifiers

    //~ Begin Memory

    /** История важных событий */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
    TArray<FRelationshipStimulus> EventHistory;

    /** Максимум хранимых событий */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 MaxEventHistory = 20;

    //~ End Memory

public:
    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    //~ Begin Public API

    /** Применить стимул */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ApplyStimulus(const FRelationshipStimulus& Stimulus);

    /** Изменить Affinity напрямую */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyAffinity(float Delta);

    /** Изменить Trust */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyTrust(float Delta);

    /** Изменить Respect */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyRespect(float Delta);

    /** Изменить Romance */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyRomance(float Delta);

    /** Изменить Fear */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyFear(float Delta);

    /** Установить настроение принудительно */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void SetMood(ENPCMoodState NewMood);

    /** Получить текущий ранг отношений (строка для UI) */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    FText GetRelationshipRank() const;

    /** Получить Affinity */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetAffinity() const { return Affinity; }

    /** Получить Trust */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetTrust() const { return Trust; }

    /** Получить Romance */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetRomance() const { return Romance; }

    /** Сбросить все атрибуты */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ResetRelationship();

    /** Сериализация */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void SaveToJson(FString& OutJson) const;

    UFUNCTION(BlueprintCallable, Category = "Relationship")
    bool LoadFromJson(const FString& Json);

    //~ End Public API

protected:
    /** Обработка decay */
    void ProcessDecay(float DeltaTime);

    /** Обработка настроения */
    void ProcessMood(float DeltaTime);

    /** Обновить настроение на основе атрибутов */
    void UpdateMoodFromAttributes();

    /** Проверка threshold'ов */
    void CheckThresholds(float OldAffinity, float NewAffinity);

    /** Clamp атрибута */
    void ClampAttribute(float& Value, float Min, float Max);
};
