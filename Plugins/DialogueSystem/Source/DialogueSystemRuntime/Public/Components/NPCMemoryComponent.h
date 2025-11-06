// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NPCMemoryComponent.generated.h"

/**
 */
UENUM(BlueprintType)
enum class EMemoryType : uint8
{
	DialogueEvent,
	
	PlayerChoice,
	
	ItemGiven,
	
	QuestEvent,
	
	WorldEvent,
	
	Promise,
	
	Secret,
	
	PersonalMemory
};

/**
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
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FNPCMemoryEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FName MemoryId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	EMemoryType Type = EMemoryType::DialogueEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	EMemoryEmotion Emotion = EMemoryEmotion::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "0", ClampMax = "100"))
	float Importance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FDateTime Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	TWeakObjectPtr<AActor> RelatedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FGameplayTagContainer ContextTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FString MetaData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	int32 RecallCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	FDateTime LastRecalledTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	float Freshness = 1.0f;

	FNPCMemoryEntry()
		: MemoryId(NAME_None)
		, Timestamp(FDateTime::Now())
		, LastRecalledTime(FDateTime::Now())
	{}

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
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FConversationTopic
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic")
	FName TopicName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topic")
	int32 DiscussionCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topic")
	FDateTime LastDiscussed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic", meta = (ClampMin = "0", ClampMax = "100"))
	float Interest = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topic")
	FGameplayTagContainer RelatedTags;

	FConversationTopic()
		: TopicName(NAME_None)
		, LastDiscussed(FDateTime::Now())
	{}
};

/**
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryAdded, const FNPCMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryRecalled, const FNPCMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryForgotten, FName, MemoryId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTopicDiscussed, FName, TopicName, int32, TotalCount);

/**
 * 
 * 
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UNPCMemoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNPCMemoryComponent();

	//~ Begin Events

	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryAdded OnMemoryAdded;

	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryRecalled OnMemoryRecalled;

	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnMemoryForgotten OnMemoryForgotten;

	UPROPERTY(BlueprintAssignable, Category = "Memory|Events")
	FOnTopicDiscussed OnTopicDiscussed;

	//~ End Events

protected:
	//~ Begin Memory Storage

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
	TArray<FNPCMemoryEntry> Memories;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "10", ClampMax = "1000"))
	int32 MaxMemories = 100;

	//~ End Memory Storage

	//~ Begin Topics

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Topics")
	TArray<FConversationTopic> Topics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Topics", meta = (ClampMin = "5", ClampMax = "100"))
	int32 MaxTopics = 50;

	//~ End Topics

	//~ Begin Memory Decay Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	bool bEnableMemoryDecay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseDecayRate = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay", meta = (ClampMin = "0", ClampMax = "100"))
	float MinImportanceThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	float EmotionalMemoryBonus = 0.5f;

	//~ End Memory Decay Settings

	//~ Begin NPC Personality

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MemoryRetentionMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float NegativeBias = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality")
	TMap<EMemoryType, float> TypePreferences;

	//~ End NPC Personality

	//~ Begin Internal State

	float TimeSinceLastDecayUpdate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float DecayUpdateInterval = 60.0f;

	//~ End Internal State

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin Memory Management API

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void AddMemory(const FNPCMemoryEntry& Memory);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	FName CreateMemory(EMemoryType Type, const FText& Description, float Importance = 50.0f,
	                   EMemoryEmotion Emotion = EMemoryEmotion::Neutral, AActor* RelatedActor = nullptr);

	UFUNCTION(BlueprintPure, Category = "Memory")
	bool GetMemory(FName MemoryId, FNPCMemoryEntry& OutMemory) const;

	UFUNCTION(BlueprintCallable, Category = "Memory")
	bool ForgetMemory(FName MemoryId);

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void RecallMemory(FName MemoryId);

	/** 
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory")
	bool AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetAllMemories() const { return Memories; }

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesByType(EMemoryType Type) const;

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesByEmotion(EMemoryEmotion Emotion) const;

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesAboutActor(AActor* Actor) const;

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMemoriesWithTags(const FGameplayTagContainer& Tags, bool bRequireAll = false) const;

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetRecentMemories(int32 Count = 10) const;

	UFUNCTION(BlueprintPure, Category = "Memory")
	TArray<FNPCMemoryEntry> GetMostImportantMemories(int32 Count = 10) const;

	UFUNCTION(BlueprintCallable, Category = "Memory")
	TArray<FNPCMemoryEntry> FindRelevantMemories(const FGameplayTagContainer& ContextTags,
	                                              AActor* RelatedActor = nullptr,
	                                              int32 MaxResults = 5) const;

	//~ End Memory Management API

	//~ Begin Topic Management API

	UFUNCTION(BlueprintCallable, Category = "Topics")
	void DiscussTopic(FName TopicName, float InterestModifier = 0.0f);

	UFUNCTION(BlueprintPure, Category = "Topics")
	bool GetTopic(FName TopicName, FConversationTopic& OutTopic) const;

	UFUNCTION(BlueprintPure, Category = "Topics")
	TArray<FConversationTopic> GetAllTopics() const { return Topics; }

	UFUNCTION(BlueprintPure, Category = "Topics")
	TArray<FConversationTopic> GetInterestingTopics(int32 Count = 5) const;

	UFUNCTION(BlueprintPure, Category = "Topics")
	bool WasTopicDiscussed(FName TopicName) const;

	UFUNCTION(BlueprintPure, Category = "Topics")
	int32 GetTopicDiscussionCount(FName TopicName) const;

	//~ End Topic Management API


	UFUNCTION(BlueprintCallable, Category = "Memory|Tags")
	void AddMemoryByTag(FGameplayTag MemoryTag, float Strength = 1.0f);

	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	bool HasMemoryOfTag(FGameplayTag MemoryTag) const;

	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	float GetMemoryStrength(FGameplayTag MemoryTag) const;

	UFUNCTION(BlueprintCallable, Category = "Memory|Tags")
	void RemoveMemoryByTag(FGameplayTag MemoryTag);

	UFUNCTION(BlueprintPure, Category = "Memory|Tags")
	float GetTimeSinceLastUpdate(FGameplayTag MemoryTag) const;

	//~ End Memory Tag API

	//~ Begin Query API

	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	bool RemembersEvent(FName MemoryId) const;

	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	int32 GetMemoryCount() const { return Memories.Num(); }

	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	int32 GetMemoryCountByType(EMemoryType Type) const;

	UFUNCTION(BlueprintPure, Category = "Memory|Query")
	bool HasMemoriesAbout(AActor* Actor) const;

	//~ End Query API

	//~ Begin Save/Load API

	UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
	void SaveToJson(FString& OutJson) const;

	UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
	bool LoadFromJson(const FString& Json);

	//~ End Save/Load API

	//~ Begin Utility

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void ClearAllMemories();

	UFUNCTION(BlueprintCallable, Category = "Memory")
	void PruneOldMemories(float MaxAgeInDays = 30.0f);

	//~ End Utility

protected:
	void ProcessMemoryDecay(float DeltaTime);

	float CalculateDecayRate(const FNPCMemoryEntry& Memory) const;

	void EnforceMemoryLimit();

	float CalculateRelevance(const FNPCMemoryEntry& Memory,
	                         const FGameplayTagContainer& ContextTags,
	                         AActor* RelatedActor) const;

	FName GenerateMemoryId() const;

	void UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime);
};
