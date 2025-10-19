// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/NPCMemoryComponent.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "GameFramework/Actor.h"
#include "Math/NumericLimits.h"

UNPCMemoryComponent::UNPCMemoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 1.0f; // Обновляем раз в секунду
}

void UNPCMemoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Инициализация предпочтений типов памяти по умолчанию
	if (TypePreferences.Num() == 0)
	{
		TypePreferences.Add(EMemoryType::DialogueEvent, 1.0f);
		TypePreferences.Add(EMemoryType::PlayerChoice, 1.5f);
		TypePreferences.Add(EMemoryType::ItemGiven, 1.2f);
		TypePreferences.Add(EMemoryType::QuestEvent, 1.3f);
		TypePreferences.Add(EMemoryType::WorldEvent, 1.0f);
		TypePreferences.Add(EMemoryType::Promise, 2.0f);
		TypePreferences.Add(EMemoryType::Secret, 1.8f);
		TypePreferences.Add(EMemoryType::PersonalMemory, 1.5f);
	}
}

void UNPCMemoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bEnableMemoryDecay)
	{
		ProcessMemoryDecay(DeltaTime);
	}
}

void UNPCMemoryComponent::AddMemory(const FNPCMemoryEntry& Memory)
{
	// Создаем копию с уникальным ID если не задан
	FNPCMemoryEntry NewMemory = Memory;
	if (NewMemory.MemoryId.IsNone())
	{
		NewMemory.MemoryId = GenerateMemoryId();
	}
	
	// Проверяем не существует ли уже
	int32 ExistingIndex = Memories.IndexOfByPredicate([&](const FNPCMemoryEntry& Entry) 
	{
		return Entry.MemoryId == NewMemory.MemoryId;
	});
	
	if (ExistingIndex != INDEX_NONE)
	{
		// Обновляем существующее воспоминание
		Memories[ExistingIndex] = NewMemory;
		UE_LOG(LogTemp, Warning, TEXT("NPCMemory: Updated existing memory '%s'"), *NewMemory.MemoryId.ToString());
	}
	else
	{
		// Добавляем новое
		Memories.Add(NewMemory);
		OnMemoryAdded.Broadcast(NewMemory);
		
		UE_LOG(LogTemp, Log, TEXT("NPCMemory: Added memory '%s' (Type: %d, Importance: %.1f)"), 
			*NewMemory.MemoryId.ToString(), 
			static_cast<int32>(NewMemory.Type), 
			NewMemory.Importance);
		
		// Проверяем лимит
		EnforceMemoryLimit();
	}
}

FName UNPCMemoryComponent::CreateMemory(EMemoryType Type, const FText& Description, float Importance,
                                         EMemoryEmotion Emotion, AActor* RelatedActor)
{
	FNPCMemoryEntry NewMemory;
	NewMemory.MemoryId = GenerateMemoryId();
	NewMemory.Type = Type;
	NewMemory.Description = Description;
	NewMemory.Importance = FMath::Clamp(Importance, 0.0f, 100.0f);
	NewMemory.Emotion = Emotion;
	NewMemory.RelatedActor = RelatedActor;
	NewMemory.Timestamp = FDateTime::Now();
	NewMemory.LastRecalledTime = FDateTime::Now();
	NewMemory.Freshness = 1.0f;
	
	AddMemory(NewMemory);
	
	return NewMemory.MemoryId;
}

bool UNPCMemoryComponent::GetMemory(FName MemoryId, FNPCMemoryEntry& OutMemory) const
{
	const FNPCMemoryEntry* Found = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.MemoryId == MemoryId;
	});
	
	if (Found)
	{
		OutMemory = *Found;
		return true;
	}
	
	return false;
}

bool UNPCMemoryComponent::ForgetMemory(FName MemoryId)
{
	int32 Index = Memories.IndexOfByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.MemoryId == MemoryId;
	});
	
	if (Index != INDEX_NONE)
	{
		UE_LOG(LogTemp, Log, TEXT("NPCMemory: Forgetting memory '%s'"), *MemoryId.ToString());
		Memories.RemoveAt(Index);
		OnMemoryForgotten.Broadcast(MemoryId);
		return true;
	}
	
	return false;
}

void UNPCMemoryComponent::RecallMemory(FName MemoryId)
{
	FNPCMemoryEntry* Found = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.MemoryId == MemoryId;
	});
	
	if (Found)
	{
		Found->RecallCount++;
		Found->LastRecalledTime = FDateTime::Now();
		Found->Freshness = FMath::Min(Found->Freshness + 0.1f, 1.0f); // Вспоминание укрепляет память
		
		OnMemoryRecalled.Broadcast(*Found);
		
		UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Recalled memory '%s' (Count: %d)"), 
			*MemoryId.ToString(), Found->RecallCount);
	}
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetMemoriesByType(EMemoryType Type) const
{
	TArray<FNPCMemoryEntry> Result;
	
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		if (Memory.Type == Type)
		{
			Result.Add(Memory);
		}
	}
	
	return Result;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetMemoriesByEmotion(EMemoryEmotion Emotion) const
{
	TArray<FNPCMemoryEntry> Result;
	
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		if (Memory.Emotion == Emotion)
		{
			Result.Add(Memory);
		}
	}
	
	return Result;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetMemoriesAboutActor(AActor* Actor) const
{
	TArray<FNPCMemoryEntry> Result;
	
	if (!Actor)
	{
		return Result;
	}
	
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		if (Memory.RelatedActor.IsValid() && Memory.RelatedActor.Get() == Actor)
		{
			Result.Add(Memory);
		}
	}
	
	return Result;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetMemoriesWithTags(const FGameplayTagContainer& Tags, bool bRequireAll) const
{
	TArray<FNPCMemoryEntry> Result;
	
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		bool bMatches = false;
		
		if (bRequireAll)
		{
			bMatches = Memory.ContextTags.HasAll(Tags);
		}
		else
		{
			bMatches = Memory.ContextTags.HasAny(Tags);
		}
		
		if (bMatches)
		{
			Result.Add(Memory);
		}
	}
	
	return Result;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetRecentMemories(int32 Count) const
{
	TArray<FNPCMemoryEntry> SortedMemories = Memories;
	
	// Сортируем по времени (новые первыми)
	SortedMemories.Sort([](const FNPCMemoryEntry& A, const FNPCMemoryEntry& B)
	{
		return A.Timestamp > B.Timestamp;
	});
	
	// Возвращаем только нужное количество
	if (SortedMemories.Num() > Count)
	{
		SortedMemories.SetNum(Count);
	}
	
	return SortedMemories;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::GetMostImportantMemories(int32 Count) const
{
	TArray<FNPCMemoryEntry> SortedMemories = Memories;
	
	// Сортируем по важности (важные первыми)
	SortedMemories.Sort([](const FNPCMemoryEntry& A, const FNPCMemoryEntry& B)
	{
		return A.Importance > B.Importance;
	});
	
	// Возвращаем только нужное количество
	if (SortedMemories.Num() > Count)
	{
		SortedMemories.SetNum(Count);
	}
	
	return SortedMemories;
}

TArray<FNPCMemoryEntry> UNPCMemoryComponent::FindRelevantMemories(const FGameplayTagContainer& ContextTags,
                                                                   AActor* RelatedActor,
                                                                   int32 MaxResults) const
{
	// Структура для хранения памяти с её релевантностью
	struct FMemoryWithScore
	{
		FNPCMemoryEntry Memory;
		float Relevance;
	};
	
	TArray<FMemoryWithScore> ScoredMemories;
	
	// Оцениваем релевантность каждого воспоминания
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		float Relevance = CalculateRelevance(Memory, ContextTags, RelatedActor);
		
		if (Relevance > 0.1f) // Минимальный порог релевантности
		{
			FMemoryWithScore Scored;
			Scored.Memory = Memory;
			Scored.Relevance = Relevance;
			ScoredMemories.Add(Scored);
		}
	}
	
	// Сортируем по релевантности
	ScoredMemories.Sort([](const FMemoryWithScore& A, const FMemoryWithScore& B)
	{
		return A.Relevance > B.Relevance;
	});
	
	// Возвращаем только самые релевантные
	TArray<FNPCMemoryEntry> Result;
	int32 Count = FMath::Min(MaxResults, ScoredMemories.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		Result.Add(ScoredMemories[i].Memory);
		
		// Отмечаем что воспоминание было вспомнено
		const_cast<UNPCMemoryComponent*>(this)->RecallMemory(ScoredMemories[i].Memory.MemoryId);
	}
	
	return Result;
}

void UNPCMemoryComponent::DiscussTopic(FName TopicName, float InterestModifier)
{
	// Ищем существующую тему
	FConversationTopic* ExistingTopic = Topics.FindByPredicate([&](const FConversationTopic& Topic)
	{
		return Topic.TopicName == TopicName;
	});
	
	if (ExistingTopic)
	{
		ExistingTopic->DiscussionCount++;
		ExistingTopic->LastDiscussed = FDateTime::Now();
		ExistingTopic->Interest = FMath::Clamp(ExistingTopic->Interest + InterestModifier, 0.0f, 100.0f);
		
		OnTopicDiscussed.Broadcast(TopicName, ExistingTopic->DiscussionCount);
	}
	else
	{
		// Создаем новую тему
		FConversationTopic NewTopic;
		NewTopic.TopicName = TopicName;
		NewTopic.DiscussionCount = 1;
		NewTopic.LastDiscussed = FDateTime::Now();
		NewTopic.Interest = FMath::Clamp(50.0f + InterestModifier, 0.0f, 100.0f);
		
		Topics.Add(NewTopic);
		OnTopicDiscussed.Broadcast(TopicName, 1);
		
		// Проверяем лимит тем
		if (Topics.Num() > MaxTopics)
		{
			// Удаляем самую неинтересную и давно обсуждавшуюся тему
			Topics.Sort([](const FConversationTopic& A, const FConversationTopic& B)
			{
				float ScoreA = A.Interest + (FDateTime::Now() - A.LastDiscussed).GetTotalDays();
				float ScoreB = B.Interest + (FDateTime::Now() - B.LastDiscussed).GetTotalDays();
				return ScoreA > ScoreB;
			});
			Topics.SetNum(MaxTopics);
		}
	}
	
	UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Discussed topic '%s'"), *TopicName.ToString());
}

bool UNPCMemoryComponent::GetTopic(FName TopicName, FConversationTopic& OutTopic) const
{
	const FConversationTopic* Found = Topics.FindByPredicate([&](const FConversationTopic& Topic)
	{
		return Topic.TopicName == TopicName;
	});
	
	if (Found)
	{
		OutTopic = *Found;
		return true;
	}
	
	return false;
}

TArray<FConversationTopic> UNPCMemoryComponent::GetInterestingTopics(int32 Count) const
{
	TArray<FConversationTopic> SortedTopics = Topics;
	
	// Сортируем по интересу
	SortedTopics.Sort([](const FConversationTopic& A, const FConversationTopic& B)
	{
		return A.Interest > B.Interest;
	});
	
	// Возвращаем только нужное количество
	if (SortedTopics.Num() > Count)
	{
		SortedTopics.SetNum(Count);
	}
	
	return SortedTopics;
}

bool UNPCMemoryComponent::WasTopicDiscussed(FName TopicName) const
{
	return Topics.ContainsByPredicate([&](const FConversationTopic& Topic)
	{
		return Topic.TopicName == TopicName;
	});
}

int32 UNPCMemoryComponent::GetTopicDiscussionCount(FName TopicName) const
{
	const FConversationTopic* Found = Topics.FindByPredicate([&](const FConversationTopic& Topic)
	{
		return Topic.TopicName == TopicName;
	});
	
	return Found ? Found->DiscussionCount : 0;
}

bool UNPCMemoryComponent::RemembersEvent(FName MemoryId) const
{
	return Memories.ContainsByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.MemoryId == MemoryId;
	});
}

int32 UNPCMemoryComponent::GetMemoryCountByType(EMemoryType Type) const
{
	return Memories.FilterByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.Type == Type;
	}).Num();
}

bool UNPCMemoryComponent::HasMemoriesAbout(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}
	
	return Memories.ContainsByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.RelatedActor.IsValid() && Entry.RelatedActor.Get() == Actor;
	});
}

void UNPCMemoryComponent::SaveToJson(FString& OutJson) const
{
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
	
	// Сохраняем воспоминания
	TArray<TSharedPtr<FJsonValue>> MemoriesArray;
	for (const FNPCMemoryEntry& Memory : Memories)
	{
		TSharedPtr<FJsonObject> MemoryObj = MakeShareable(new FJsonObject());
		
		MemoryObj->SetStringField(TEXT("MemoryId"), Memory.MemoryId.ToString());
		MemoryObj->SetNumberField(TEXT("Type"), static_cast<int32>(Memory.Type));
		MemoryObj->SetStringField(TEXT("Description"), Memory.Description.ToString());
		MemoryObj->SetNumberField(TEXT("Emotion"), static_cast<int32>(Memory.Emotion));
		MemoryObj->SetNumberField(TEXT("Importance"), Memory.Importance);
		MemoryObj->SetStringField(TEXT("Timestamp"), Memory.Timestamp.ToString());
		MemoryObj->SetNumberField(TEXT("RecallCount"), Memory.RecallCount);
		MemoryObj->SetStringField(TEXT("LastRecalledTime"), Memory.LastRecalledTime.ToString());
		MemoryObj->SetNumberField(TEXT("Freshness"), Memory.Freshness);
		MemoryObj->SetStringField(TEXT("MetaData"), Memory.MetaData);
		
		// Сохраняем теги
		TArray<TSharedPtr<FJsonValue>> TagsArray;
		for (const FGameplayTag& Tag : Memory.ContextTags)
		{
			TagsArray.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
		}
		MemoryObj->SetArrayField(TEXT("ContextTags"), TagsArray);
		
		MemoriesArray.Add(MakeShareable(new FJsonValueObject(MemoryObj)));
	}
	RootObject->SetArrayField(TEXT("Memories"), MemoriesArray);
	
	// Сохраняем темы
	TArray<TSharedPtr<FJsonValue>> TopicsArray;
	for (const FConversationTopic& Topic : Topics)
	{
		TSharedPtr<FJsonObject> TopicObj = MakeShareable(new FJsonObject());
		
		TopicObj->SetStringField(TEXT("TopicName"), Topic.TopicName.ToString());
		TopicObj->SetNumberField(TEXT("DiscussionCount"), Topic.DiscussionCount);
		TopicObj->SetStringField(TEXT("LastDiscussed"), Topic.LastDiscussed.ToString());
		TopicObj->SetNumberField(TEXT("Interest"), Topic.Interest);
		
		TopicsArray.Add(MakeShareable(new FJsonValueObject(TopicObj)));
	}
	RootObject->SetArrayField(TEXT("Topics"), TopicsArray);
	
	// Сериализация в строку
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
}

bool UNPCMemoryComponent::LoadFromJson(const FString& Json)
{
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
	
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		return false;
	}
	
	// Загружаем воспоминания
	const TArray<TSharedPtr<FJsonValue>>* MemoriesArray;
	if (RootObject->TryGetArrayField(TEXT("Memories"), MemoriesArray))
	{
		Memories.Empty();
		
		for (const TSharedPtr<FJsonValue>& Value : *MemoriesArray)
		{
			const TSharedPtr<FJsonObject>* MemoryObj;
			if (Value->TryGetObject(MemoryObj))
			{
				FNPCMemoryEntry Memory;
				
				Memory.MemoryId = FName(*(*MemoryObj)->GetStringField(TEXT("MemoryId")));
				Memory.Type = static_cast<EMemoryType>((*MemoryObj)->GetIntegerField(TEXT("Type")));
				Memory.Description = FText::FromString((*MemoryObj)->GetStringField(TEXT("Description")));
				Memory.Emotion = static_cast<EMemoryEmotion>((*MemoryObj)->GetIntegerField(TEXT("Emotion")));
				Memory.Importance = (*MemoryObj)->GetNumberField(TEXT("Importance"));
				FDateTime::Parse((*MemoryObj)->GetStringField(TEXT("Timestamp")), Memory.Timestamp);
				Memory.RecallCount = (*MemoryObj)->GetIntegerField(TEXT("RecallCount"));
				FDateTime::Parse((*MemoryObj)->GetStringField(TEXT("LastRecalledTime")), Memory.LastRecalledTime);
				Memory.Freshness = (*MemoryObj)->GetNumberField(TEXT("Freshness"));
				Memory.MetaData = (*MemoryObj)->GetStringField(TEXT("MetaData"));
				
				// Загружаем теги
				const TArray<TSharedPtr<FJsonValue>>* TagsArray;
				if ((*MemoryObj)->TryGetArrayField(TEXT("ContextTags"), TagsArray))
				{
					for (const TSharedPtr<FJsonValue>& TagValue : *TagsArray)
					{
						FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagValue->AsString()), false);
						if (Tag.IsValid())
						{
							Memory.ContextTags.AddTag(Tag);
						}
					}
				}
				
				Memories.Add(Memory);
			}
		}
	}
	
	// Загружаем темы
	const TArray<TSharedPtr<FJsonValue>>* TopicsArray;
	if (RootObject->TryGetArrayField(TEXT("Topics"), TopicsArray))
	{
		Topics.Empty();
		
		for (const TSharedPtr<FJsonValue>& Value : *TopicsArray)
		{
			const TSharedPtr<FJsonObject>* TopicObj;
			if (Value->TryGetObject(TopicObj))
			{
				FConversationTopic Topic;
				
				Topic.TopicName = FName(*(*TopicObj)->GetStringField(TEXT("TopicName")));
				Topic.DiscussionCount = (*TopicObj)->GetIntegerField(TEXT("DiscussionCount"));
				FDateTime::Parse((*TopicObj)->GetStringField(TEXT("LastDiscussed")), Topic.LastDiscussed);
				Topic.Interest = (*TopicObj)->GetNumberField(TEXT("Interest"));
				
				Topics.Add(Topic);
			}
		}
	}
	
	return true;
}

void UNPCMemoryComponent::ClearAllMemories()
{
	UE_LOG(LogTemp, Warning, TEXT("NPCMemory: Clearing all memories"));
	Memories.Empty();
	Topics.Empty();
}

void UNPCMemoryComponent::PruneOldMemories(float MaxAgeInDays)
{
	FDateTime Now = FDateTime::Now();
	int32 RemovedCount = 0;
	
	for (int32 i = Memories.Num() - 1; i >= 0; --i)
	{
		FTimespan Age = Now - Memories[i].Timestamp;
		if (Age.GetTotalDays() > MaxAgeInDays)
		{
			UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Pruning old memory '%s' (Age: %.1f days)"),
				*Memories[i].MemoryId.ToString(), Age.GetTotalDays());
			
			OnMemoryForgotten.Broadcast(Memories[i].MemoryId);
			Memories.RemoveAt(i);
			RemovedCount++;
		}
	}
	
	if (RemovedCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("NPCMemory: Pruned %d old memories"), RemovedCount);
	}
}

void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
	TimeSinceLastDecayUpdate += DeltaTime;
	
	// Обновляем decay не чаще чем раз в DecayUpdateInterval
	if (TimeSinceLastDecayUpdate < DecayUpdateInterval)
	{
		return;
	}
	
	TimeSinceLastDecayUpdate = 0.0f;
	
	// Обновляем freshness для всех воспоминаний
	for (int32 i = Memories.Num() - 1; i >= 0; --i)
	{
		FNPCMemoryEntry& Memory = Memories[i];
		UpdateMemoryFreshness(Memory, DecayUpdateInterval);
		
		// Проверяем нужно ли забыть
		if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
		{
			UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Forgetting faded memory '%s' (Freshness: %.2f, Importance: %.1f)"),
				*Memory.MemoryId.ToString(), Memory.Freshness, Memory.Importance);
			
			OnMemoryForgotten.Broadcast(Memory.MemoryId);
			Memories.RemoveAt(i);
		}
	}
}

float UNPCMemoryComponent::CalculateDecayRate(const FNPCMemoryEntry& Memory) const
{
	float DecayRate = BaseDecayRate;
	
	// Учитываем важность
	float ImportanceFactor = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 100.0f), FVector2D(2.0f, 0.5f), Memory.Importance);
	DecayRate *= ImportanceFactor;
	
	// Учитываем эмоциональную окраску
	if (Memory.Emotion != EMemoryEmotion::Neutral)
	{
		DecayRate *= (1.0f - EmotionalMemoryBonus);
	}
	
	// Учитываем память NPC
	DecayRate /= MemoryRetentionMultiplier;
	
	// Учитываем негативный bias
	if (Memory.Emotion == EMemoryEmotion::Negative || 
	    Memory.Emotion == EMemoryEmotion::Traumatic || 
	    Memory.Emotion == EMemoryEmotion::Regretful)
	{
		DecayRate /= NegativeBias;
	}
	
	// Учитываем предпочтения типов
	if (const float* TypePref = TypePreferences.Find(Memory.Type))
	{
		DecayRate /= *TypePref;
	}
	
	// Учитываем частоту вспоминания
	if (Memory.RecallCount > 0)
	{
		float RecallFactor = 1.0f / (1.0f + FMath::Sqrt(static_cast<float>(Memory.RecallCount)));
		DecayRate *= RecallFactor;
	}
	
	return DecayRate;
}

void UNPCMemoryComponent::EnforceMemoryLimit()
{
	if (Memories.Num() <= MaxMemories)
	{
		return;
	}
	
	// Удаляем самые слабые воспоминания
	Memories.Sort([this](const FNPCMemoryEntry& A, const FNPCMemoryEntry& B)
	{
		// Вычисляем "силу" памяти
		float StrengthA = A.Importance * A.Freshness * (1.0f + A.RecallCount * 0.1f);
		float StrengthB = B.Importance * B.Freshness * (1.0f + B.RecallCount * 0.1f);
		return StrengthA > StrengthB;
	});
	
	int32 ToRemove = Memories.Num() - MaxMemories;
	for (int32 i = 0; i < ToRemove; ++i)
	{
		int32 Index = Memories.Num() - 1;
		UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Removing weakest memory '%s' due to limit"),
			*Memories[Index].MemoryId.ToString());
		
		OnMemoryForgotten.Broadcast(Memories[Index].MemoryId);
		Memories.RemoveAt(Index);
	}
}

float UNPCMemoryComponent::CalculateRelevance(const FNPCMemoryEntry& Memory,
                                               const FGameplayTagContainer& ContextTags,
                                               AActor* RelatedActor) const
{
	float Relevance = 0.0f;
	
	// Базовая релевантность от важности и свежести
	Relevance += Memory.Importance * 0.01f; // 0-1
	Relevance += Memory.Freshness; // 0-1
	
	// Совпадение тегов
	if (ContextTags.Num() > 0 && Memory.ContextTags.Num() > 0)
	{
		int32 MatchingTags = 0;
		for (const FGameplayTag& Tag : ContextTags)
		{
			if (Memory.ContextTags.HasTag(Tag))
			{
				MatchingTags++;
			}
		}
		
		float TagRelevance = static_cast<float>(MatchingTags) / static_cast<float>(ContextTags.Num());
		Relevance += TagRelevance * 2.0f; // Теги важны
	}
	
	// Совпадение актера
	if (RelatedActor && Memory.RelatedActor.IsValid() && Memory.RelatedActor.Get() == RelatedActor)
	{
		Relevance += 1.5f; // Актор очень важен
	}
	
	// Бонус за частоту вспоминания
	Relevance += FMath::Min(Memory.RecallCount * 0.1f, 0.5f);
	
	// Недавние воспоминания более релевантны
	FTimespan Age = FDateTime::Now() - Memory.Timestamp;
	float AgeDays = Age.GetTotalDays();
	float AgeFactor = FMath::Exp(-AgeDays * 0.1f); // Экспоненциальный спад
	Relevance *= AgeFactor;
	
	return Relevance;
}

FName UNPCMemoryComponent::GenerateMemoryId() const
{
	// Генерируем уникальный ID на основе времени и случайности
	FString IdString = FString::Printf(TEXT("Memory_%lld_%d"), 
		FDateTime::Now().GetTicks(), 
		FMath::Rand());
	
	return FName(*IdString);
}

void UNPCMemoryComponent::UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime)
{
	float DecayRate = CalculateDecayRate(Memory);
	
	// Конвертируем скорость в изменение за секунду
	float DecayPerSecond = DecayRate / (24.0f * 60.0f * 60.0f); // Из "за день" в "за секунду"
	
	// Обновляем freshness
	Memory.Freshness = FMath::Max(Memory.Freshness - DecayPerSecond * DeltaTime, 0.0f);
}

// Реализация методов для работы с тегами памяти

void UNPCMemoryComponent::AddMemoryByTag(FGameplayTag MemoryTag, float Strength)
{
	// Создаем или обновляем память с этим тегом
	FName MemoryId = FName(*MemoryTag.ToString());
	
	// Ищем существующую память с этим тегом
	FNPCMemoryEntry* ExistingMemory = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.ContextTags.HasTag(MemoryTag);
	});
	
	if (ExistingMemory)
	{
		// Обновляем существующую память
		ExistingMemory->Importance = FMath::Clamp(Strength * 100.0f, 0.0f, 100.0f);
		ExistingMemory->Freshness = 1.0f;
		ExistingMemory->Timestamp = FDateTime::Now();
		ExistingMemory->LastRecalledTime = FDateTime::Now();
		
		UE_LOG(LogTemp, Verbose, TEXT("NPCMemory: Updated memory with tag '%s' (Strength: %.2f)"),
			*MemoryTag.ToString(), Strength);
	}
	else
	{
		// Создаем новую память
		FNPCMemoryEntry NewMemory;
		NewMemory.MemoryId = GenerateMemoryId();
		NewMemory.Type = EMemoryType::DialogueEvent;
		NewMemory.Description = FText::FromString(FString::Printf(TEXT("Memory: %s"), *MemoryTag.ToString()));
		NewMemory.Importance = FMath::Clamp(Strength * 100.0f, 0.0f, 100.0f);
		NewMemory.Emotion = EMemoryEmotion::Neutral;
		NewMemory.Timestamp = FDateTime::Now();
		NewMemory.LastRecalledTime = FDateTime::Now();
		NewMemory.Freshness = 1.0f;
		NewMemory.ContextTags.AddTag(MemoryTag);
		
		AddMemory(NewMemory);
		
		UE_LOG(LogTemp, Log, TEXT("NPCMemory: Added memory with tag '%s' (Strength: %.2f)"),
			*MemoryTag.ToString(), Strength);
	}
}

bool UNPCMemoryComponent::HasMemoryOfTag(FGameplayTag MemoryTag) const
{
	return Memories.ContainsByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.ContextTags.HasTag(MemoryTag);
	});
}

float UNPCMemoryComponent::GetMemoryStrength(FGameplayTag MemoryTag) const
{
	const FNPCMemoryEntry* Memory = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.ContextTags.HasTag(MemoryTag);
	});
	
	if (Memory)
	{
		// Возвращаем силу памяти как комбинацию важности и свежести
		return (Memory->Importance / 100.0f) * Memory->Freshness;
	}
	
	return 0.0f;
}

void UNPCMemoryComponent::RemoveMemoryByTag(FGameplayTag MemoryTag)
{
	int32 Index = Memories.IndexOfByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.ContextTags.HasTag(MemoryTag);
	});
	
	if (Index != INDEX_NONE)
	{
		FName MemoryId = Memories[Index].MemoryId;
		UE_LOG(LogTemp, Log, TEXT("NPCMemory: Removing memory with tag '%s'"), *MemoryTag.ToString());
		
		Memories.RemoveAt(Index);
		OnMemoryForgotten.Broadcast(MemoryId);
	}
}

float UNPCMemoryComponent::GetTimeSinceLastUpdate(FGameplayTag MemoryTag) const
{
	const FNPCMemoryEntry* Memory = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
	{
		return Entry.ContextTags.HasTag(MemoryTag);
	});
	
	if (Memory)
	{
		FTimespan TimeSince = FDateTime::Now() - Memory->LastRecalledTime;
		return static_cast<float>(TimeSince.GetTotalSeconds());
	}
	
	return TNumericLimits<float>::Max(); // Возвращаем максимальное значение если память не найдена
}
