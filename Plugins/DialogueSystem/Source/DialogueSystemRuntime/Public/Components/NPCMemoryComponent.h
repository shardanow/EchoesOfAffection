// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NPCMemoryComponent.generated.h"

/**
 * Тип памяти о событии
 */
UENUM(BlueprintType)
enum class EMemoryType : uint8
{
	/** Событие из диалога */
	DialogueEvent,
	
	/** Выбор игрока */
	PlayerChoice,
	
	/** Подарок/предмет */
	ItemGiven,
	
	/** Квестовое событие */
	QuestEvent,
	
	/** Важное мировое событие */
	WorldEvent,
	
	/** Обещание */
	Promise,
	
	/** Секрет */
	Secret,
	
	/** Личное воспоминание */
	PersonalMemory
};

/**
 * Эмоциональная окраска воспоминания
 */
UENUM(BlueprintType)
enum class EMemoryEmotion : uint8
{
	Neutral,
	Positive,
	Negative,
	Bittersweet,
	Traumatic,
	Joyful,
	Regretful,
	Nostalgic
};

/**
 * Отдельное воспоминание
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FNPCMemoryEntry
{
	GENERATED_BODY()

	/** Уникальный идентификатор воспоминания */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FName MemoryId;

	/** Тип воспоминания */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	EMemoryType Type = EMemoryType::DialogueEvent;

	/** Описание события */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FText Description;

	/** Эмоциональная окраска */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	EMemoryEmotion Emotion = EMemoryEmotion::Neutral;

	/** Важность (0-100) - влияет на вероятность забывания */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "0", ClampMax = "100"))
	float Importance = 50.0f;

	/** Время когда произошло событие */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FDateTime Timestamp;

	/** Участник/актер связанный с памятью */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	TWeakObjectPtr<AActor> RelatedActor;

	/** Контекстные теги */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FGameplayTagContainer ContextTags;

	/** Дополнительные метаданные (JSON) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FString MetaData;

	/** Сколько раз было вспомнено */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	int32 RecallCount = 0;

	/** Последний раз вспоминалось */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	FDateTime LastRecalledTime;

	/** Текущая "свежесть" воспоминания (0-1) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	float Freshness = 1.0f;

	FNPCMemoryEntry()
		: MemoryId(NAME_None)
		, Timestamp(FDateTime::Now())
		, LastRecalledTime(FDateTime::Now())
	{}

	/** Конструктор с основными параметрами */
	FNPCMemoryEntry(FName InId, EMemoryType InType, const FText& InDescription, float InImportance = 50.0f)
		: MemoryId(InId)
		, Type(InType)
		, Description(InDescription)
		, Importance(InImportance)
		, Timestamp(FDateTime::Now())
		, LastRecalledTime(FDateTime::Now())
	{}
};

/**
 * Тема разговора
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FConversationTopic
{
	GENERATED_BODY()

	/** Имя темы */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic")
	FName TopicName;

	/** Сколько раз обсуждалась */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topic")
	int32 DiscussionCount = 0;

	/** Последнее обсуждение */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topic")
	FDateTime LastDiscussed;

	/** Интерес NPC к теме (0-100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic", meta = (ClampMin = "0", ClampMax = "100"))
	float Interest = 50.0f;

	/** Теги связанные с темой */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic")
	FGameplayTagContainer RelatedTags;

	FConversationTopic()
		: TopicName(NAME_None)
		, LastDiscussed(FDateTime::Now())
	{}
};

/**
 * События
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryAdded, const FNPCMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryRecalled, const FNPCMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryForgotten, FName, MemoryId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTopicDiscussed, FName, TopicName, int32, TotalCount);

/**
 * Компонент долгосрочной памяти NPC
 * 
 * Функции:
 * - Хранение воспоминаний о важных событиях
 * - Система забывания с учетом важности и времени
 * - Отслеживание тем разговоров
 * - Вспоминание релевантных событий
 * - Влияние на поведение и диалоги
 * 
 * Память деградирует со временем, но:
 * - Важные события помнятся дольше
 * - Эмоционально окрашенные события ярче
 * - Частое вспоминание укрепляет память
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UNPCMemoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNPCMemoryComponent();

	//~ Begin Events

	/** Событие при добавлении памяти */
	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryAdded OnMemoryAdded;

	/** Событие при вспоминании */
	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryRecalled OnMemoryRecalled;

	/** Событие при забывании */
	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryForgotten OnMemoryForgotten;

	/** Событие при обсуждении темы */
	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnTopicDiscussed OnTopicDiscussed;

	//~ End Events

protected:
	//~ Begin Memory Storage

	/** Все воспоминания NPC */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	TArray<FNPCMemoryEntry> Memories;

	/** Максимальное количество воспоминаний */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "10", ClampMax = "1000"))
	int32 MaxMemories = 100;

	//~ End Memory Storage

	//~ Begin Topics

	/** Отслеживаемые темы разговоров */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topics")
	TArray<FConversationTopic> Topics;

	/** Максимальное количество тем */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topics", meta = (ClampMin = "5", ClampMax = "100"))
	int32 MaxTopics = 50;

	//~ End Topics

	//~ Begin Memory Decay Settings

	/** Включить автоматическое забывание */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	bool bEnableMemoryDecay = true;

	/** Базовая скорость забывания (за день) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseDecayRate = 0.05f;

	/** Минимальная важность для сохранения памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay", meta = (ClampMin = "0", ClampMax = "100"))
	float MinImportanceThreshold = 10.0f;

	/** Влияние эмоций на устойчивость памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	float EmotionalMemoryBonus = 0.5f;

	//~ End Memory Decay Settings

	//~ Begin NPC Personality

	/** Насколько хорошо NPC помнит детали (0.5 = плохая память, 2.0 = отличная) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MemoryRetentionMultiplier = 1.0f;

	/** Склонность помнить негативное (0 = позитивное, 1 = нейтрально, 2 = негативное) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float NegativeBias = 1.0f;

	/** Склонность фокусироваться на определенных типах памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
	TMap<EMemoryType, float> TypePreferences;

	//~ End NPC Personality

	//~ Begin Internal State

	/** Время последнего обновления памяти */
	float TimeSinceLastDecayUpdate = 0.0f;

	/** Частота обновления decay (секунды) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float DecayUpdateInterval = 60.0f;

	//~ End Internal State

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin Memory Management API

	/** Добавить новое воспоминание */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void AddMemory(const FNPCMemoryEntry& Memory);

	/** Создать и добавить воспоминание (упрощенный метод) */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	FName CreateMemory(EMemoryType Type, const FText& Description, float Importance = 50.0f,
	                   EMemoryEmotion Emotion = EMemoryEmotion::Neutral, AActor* RelatedActor = nullptr);

	/** Получить воспоминание по ID */
	UFUNCTION(BlueprintPure, Category = "Memory")
	bool GetMemory(FName MemoryId, FNPCMemoryEntry& OutMemory) const;

	/** Удалить воспоминание */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	bool ForgetMemory(FName MemoryId);

	/** Вспомнить (отметить что воспоминание было использовано) */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void RecallMemory(FName MemoryId);

	/** Получить все воспоминания */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetAllMemories() const { return Memories; }

	/** Получить воспоминания по типу */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesByType(EMemoryType Type) const;

	/** Получить воспоминания по эмоции */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesByEmotion(EMemoryEmotion Emotion) const;

	/** Получить воспоминания связанные с актером */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesAboutActor(AActor* Actor) const;

	/** Получить воспоминания с тегами */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesWithTags(const FGameplayTagContainer& Tags, bool bRequireAll = false) const;

	/** Получить самые свежие воспоминания */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetRecentMemories(int32 Count = 10) const;

	/** Получить самые важные воспоминания */
	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMostImportantMemories(int32 Count = 10) const;

	/** Поиск релевантных воспоминаний для контекста */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	TArray<FNPCMemoryEntry> FindRelevantMemories(const FGameplayTagContainer& ContextTags,
	                                              AActor* RelatedActor = nullptr,
	                                              int32 MaxResults = 5) const;

	//~ End Memory Management API

	//~ Begin Topic Management API

	/** Отметить обсуждение темы */
	UFUNCTION(BlueprintCallable, Category = "Topics")
	void DiscussTopic(FName TopicName, float InterestModifier = 0.0f);

	/** Получить информацию о теме */
	UFUNCTION(BlueprintPure, Category = "Topics")
	bool GetTopic(FName TopicName, FConversationTopic& OutTopic) const;

	/** Получить все темы */
	UFUNCTION(BlueprintPure, Category = "Topics")
	TArray<FConversationTopic> GetAllTopics() const { return Topics; }

	/** Получить интересные темы (отсортированные по интересу) */
	UFUNCTION(BlueprintPure, Category = "Topics")
	TArray<FConversationTopic> GetInterestingTopics(int32 Count = 5) const;

	/** Проверить была ли тема обсуждена */
	UFUNCTION(BlueprintPure, Category = "Topics")
	bool WasTopicDiscussed(FName TopicName) const;

	/** Сколько раз обсуждалась тема */
	UFUNCTION(BlueprintPure, Category = "Topics")
	int32 GetTopicDiscussionCount(FName TopicName) const;

	//~ End Topic Management API

	//~ Begin Memory Tag API (для использования с Conditions/Effects)

	/** Добавить память с тегом (упрощенный метод для эффектов) */
	UFUNCTION(BlueprintCallable, Category = "Memory|Tags")
	void AddMemoryByTag(FGameplayTag MemoryTag, float Strength = 1.0f);

	/** Проверить наличие памяти с тегом */
	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	bool HasMemoryOfTag(FGameplayTag MemoryTag) const;

	/** Получить силу памяти с тегом */
	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	float GetMemoryStrength(FGameplayTag MemoryTag) const;

	/** Удалить память с тегом */
	UFUNCTION(BlueprintCallable, Category = "Memory|Tags")
	void RemoveMemoryByTag(FGameplayTag MemoryTag);

	/** Получить время с последнего обновления памяти с тегом */
	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	float GetTimeSinceLastUpdate(FGameplayTag MemoryTag) const;

	//~ End Memory Tag API

	//~ Begin Query API

	/** Проверить помнит ли NPC о событии */
	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	bool RemembersEvent(FName MemoryId) const;

	/** Получить общее количество воспоминаний */
	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	int32 GetMemoryCount() const { return Memories.Num(); }

	/** Получить количество воспоминаний определенного типа */
	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	int32 GetMemoryCountByType(EMemoryType Type) const;

	/** Есть ли воспоминания об акторе */
	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	bool HasMemoriesAbout(AActor* Actor) const;

	//~ End Query API

	//~ Begin Save/Load API

	/** Сохранить в JSON */
	UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
	void SaveToJson(FString& OutJson) const;

	/** Загрузить из JSON */
	UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
	bool LoadFromJson(const FString& Json);

	//~ End Save/Load API

	//~ Begin Utility

	/** Очистить всю память */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void ClearAllMemories();

	/** Очистить старые воспоминания */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void PruneOldMemories(float MaxAgeInDays = 30.0f);

	//~ End Utility

protected:
	/** Обработка деградации памяти */
	void ProcessMemoryDecay(float DeltaTime);

	/** Вычислить скорость забывания для воспоминания */
	float CalculateDecayRate(const FNPCMemoryEntry& Memory) const;

	/** Удалить самые слабые воспоминания если превышен лимит */
	void EnforceMemoryLimit();

	/** Вычислить релевантность воспоминания */
	float CalculateRelevance(const FNPCMemoryEntry& Memory,
	                         const FGameplayTagContainer& ContextTags,
	                         AActor* RelatedActor) const;

	/** Генерация уникального ID для воспоминания */
	FName GenerateMemoryId() const;

	/** Обновить freshness воспоминания */
	void UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime);
};
