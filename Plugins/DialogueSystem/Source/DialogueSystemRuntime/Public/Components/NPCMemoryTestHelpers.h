// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/NPCMemoryComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/DialogueComponent.h"
#include "NPCMemoryTestHelpers.generated.h"

/**
 * Helper функции для тестирования и отладки NPC Memory System
 */
UCLASS()
class DIALOGUESYSTEMRUNTIME_API UNPCMemoryTestHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Создает тестовый набор воспоминаний для NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing", meta = (WorldContext = "WorldContextObject"))
	static void CreateTestMemories(UObject* WorldContextObject, AActor* NPC, AActor* Player)
	{
		if (!NPC || !Player)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC doesn't have NPCMemoryComponent"));
			return;
		}

		// 1. Первая встреча
		MemoryComp->CreateMemory(
			EMemoryType::DialogueEvent,
			FText::FromString("First meeting with the stranger"),
			40.0f,
			EMemoryEmotion::Neutral,
			Player
		);

		// 2. Игрок помог
		FNPCMemoryEntry HelpMemory;
		HelpMemory.Type = EMemoryType::PlayerChoice;
		HelpMemory.Description = FText::FromString("Player helped defend the village");
		HelpMemory.Importance = 85.0f;
		HelpMemory.Emotion = EMemoryEmotion::Joyful;
		HelpMemory.RelatedActor = Player;
		HelpMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Help")));
		HelpMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Location.Village")));
		MemoryComp->AddMemory(HelpMemory);

		// 3. Подарок
		FNPCMemoryEntry GiftMemory;
		GiftMemory.MemoryId = FName("GiftMemory_Rose");
		GiftMemory.Type = EMemoryType::ItemGiven;
		GiftMemory.Description = FText::FromString("Player gave me a beautiful rose");
		GiftMemory.Importance = 70.0f;
		GiftMemory.Emotion = EMemoryEmotion::Positive;
		GiftMemory.RelatedActor = Player;
		GiftMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
		GiftMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Item.Flower")));
		MemoryComp->AddMemory(GiftMemory);

		// 4. Обещание
		FNPCMemoryEntry Promise;
		Promise.MemoryId = FName("Promise_FindSister");
		Promise.Type = EMemoryType::Promise;
		Promise.Description = FText::FromString("Player promised to find my lost sister");
		Promise.Importance = 95.0f;
		Promise.Emotion = EMemoryEmotion::Nostalgic;
		Promise.RelatedActor = Player;
		Promise.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Quest.FindSister")));
		MemoryComp->AddMemory(Promise);

		// 5. Негативное событие
		FNPCMemoryEntry Negative;
		Negative.Type = EMemoryType::PlayerChoice;
		Negative.Description = FText::FromString("Player was rude during conversation");
		Negative.Importance = 50.0f;
		Negative.Emotion = EMemoryEmotion::Negative;
		Negative.RelatedActor = Player;
		MemoryComp->AddMemory(Negative);

		// Добавляем темы
		MemoryComp->DiscussTopic(FName("Family"), 15.0f);
		MemoryComp->DiscussTopic(FName("Village"), 10.0f);
		MemoryComp->DiscussTopic(FName("Politics"), -5.0f);

		UE_LOG(LogTemp, Log, TEXT("Created test memories for NPC"));
	}

	/**
	 * Выводит все воспоминания NPC в лог
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void PrintMemories(AActor* NPC)
	{
		if (!NPC)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC doesn't have NPCMemoryComponent"));
			return;
		}

		TArray<FNPCMemoryEntry> Memories = MemoryComp->GetAllMemories();

		UE_LOG(LogTemp, Log, TEXT("========== NPC Memories =========="));
		UE_LOG(LogTemp, Log, TEXT("Total Count: %d"), Memories.Num());

		for (const FNPCMemoryEntry& Memory : Memories)
		{
			FString TypeStr = UEnum::GetValueAsString(Memory.Type);
			FString EmotionStr = UEnum::GetValueAsString(Memory.Emotion);
			
			UE_LOG(LogTemp, Log, TEXT("---"));
			UE_LOG(LogTemp, Log, TEXT("ID: %s"), *Memory.MemoryId.ToString());
			UE_LOG(LogTemp, Log, TEXT("Type: %s"), *TypeStr);
			UE_LOG(LogTemp, Log, TEXT("Description: %s"), *Memory.Description.ToString());
			UE_LOG(LogTemp, Log, TEXT("Emotion: %s"), *EmotionStr);
			UE_LOG(LogTemp, Log, TEXT("Importance: %.1f"), Memory.Importance);
			UE_LOG(LogTemp, Log, TEXT("Freshness: %.2f"), Memory.Freshness);
			UE_LOG(LogTemp, Log, TEXT("Recall Count: %d"), Memory.RecallCount);
			UE_LOG(LogTemp, Log, TEXT("Age: %.1f days"), (FDateTime::Now() - Memory.Timestamp).GetTotalDays());
		}

		UE_LOG(LogTemp, Log, TEXT("=================================="));
	}

	/**
	 * Выводит темы разговоров
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void PrintTopics(AActor* NPC)
	{
		if (!NPC)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			return;
		}

		TArray<FConversationTopic> Topics = MemoryComp->GetAllTopics();

		UE_LOG(LogTemp, Log, TEXT("========== Conversation Topics =========="));
		UE_LOG(LogTemp, Log, TEXT("Total Count: %d"), Topics.Num());

		for (const FConversationTopic& Topic : Topics)
		{
			UE_LOG(LogTemp, Log, TEXT("Topic: %s | Interest: %.1f | Count: %d"),
				*Topic.TopicName.ToString(),
				Topic.Interest,
				Topic.DiscussionCount);
		}

		UE_LOG(LogTemp, Log, TEXT("========================================="));
	}

	/**
	 * Тест системы релевантности
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void TestRelevance(AActor* NPC, AActor* Player)
	{
		if (!NPC || !Player)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			return;
		}

		// Создаем различные контексты и тестируем
		FGameplayTagContainer Context1;
		Context1.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
		
		TArray<FNPCMemoryEntry> Relevant = MemoryComp->FindRelevantMemories(Context1, Player, 5);

		UE_LOG(LogTemp, Log, TEXT("========== Relevance Test =========="));
		UE_LOG(LogTemp, Log, TEXT("Context: Event.Gift"));
		UE_LOG(LogTemp, Log, TEXT("Found %d relevant memories:"), Relevant.Num());

		for (const FNPCMemoryEntry& Memory : Relevant)
		{
			UE_LOG(LogTemp, Log, TEXT("- %s (Importance: %.1f)"), 
				*Memory.Description.ToString(), 
				Memory.Importance);
		}

		UE_LOG(LogTemp, Log, TEXT("===================================="));
	}

	/**
	 * Симулирует течение времени для тестирования decay
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void SimulateTimePassing(AActor* NPC, float Days)
	{
		if (!NPC)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			return;
		}

		// Симулируем decay на указанное количество дней
		float SecondsPerDay = 86400.0f;
		float TotalSeconds = Days * SecondsPerDay;
		float UpdateInterval = 60.0f; // Update every minute of in-game time
		
		int32 Updates = FMath::CeilToInt(TotalSeconds / UpdateInterval);

		UE_LOG(LogTemp, Log, TEXT("Simulating %.1f days of memory decay (%d updates)"), Days, Updates);

		// Принудительно вызываем decay обновления
		for (int32 i = 0; i < Updates; ++i)
		{
			// Напрямую вызывать ProcessMemoryDecay нельзя (protected), 
			// но можно эмулировать через Tick
			// В реальности используйте timer или wait в тестах
		}

		UE_LOG(LogTemp, Log, TEXT("Simulation complete. Print memories to see changes."));
	}

	/**
	 * Полный тест интеграции памяти и отношений
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void TestMemoryRelationshipIntegration(AActor* NPC, AActor* Player)
	{
		if (!NPC || !Player)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		URelationshipComponent* RelComp = NPC->FindComponentByClass<URelationshipComponent>();

		if (!MemoryComp || !RelComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC missing required components"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("========== Integration Test =========="));

		// Начальное состояние
		float InitialAffinity = RelComp->GetAffinity();
		int32 InitialMemoryCount = MemoryComp->GetMemoryCount();

		UE_LOG(LogTemp, Log, TEXT("Initial State:"));
		UE_LOG(LogTemp, Log, TEXT("- Affinity: %.1f"), InitialAffinity);
		UE_LOG(LogTemp, Log, TEXT("- Memories: %d"), InitialMemoryCount);

		// Симулируем позитивное событие
		FNPCMemoryEntry PositiveMemory;
		PositiveMemory.Type = EMemoryType::PlayerChoice;
		PositiveMemory.Description = FText::FromString("Player saved my life");
		PositiveMemory.Importance = 100.0f;
		PositiveMemory.Emotion = EMemoryEmotion::Joyful;
		PositiveMemory.RelatedActor = Player;
		MemoryComp->AddMemory(PositiveMemory);

		RelComp->ModifyAffinity(30.0f);
		RelComp->ModifyTrust(25.0f);

		// Финальное состояние
		float FinalAffinity = RelComp->GetAffinity();
		int32 FinalMemoryCount = MemoryComp->GetMemoryCount();

		UE_LOG(LogTemp, Log, TEXT("After Positive Event:"));
		UE_LOG(LogTemp, Log, TEXT("- Affinity: %.1f (Delta: +%.1f)"), 
			FinalAffinity, FinalAffinity - InitialAffinity);
		UE_LOG(LogTemp, Log, TEXT("- Memories: %d (Delta: +%d)"), 
			FinalMemoryCount, FinalMemoryCount - InitialMemoryCount);

		// Проверяем что воспоминания об игроке соответствуют affinity
		TArray<FNPCMemoryEntry> PlayerMemories = MemoryComp->GetMemoriesAboutActor(Player);
		int32 PositiveCount = 0;
		int32 NegativeCount = 0;

		for (const FNPCMemoryEntry& Memory : PlayerMemories)
		{
			if (Memory.Emotion == EMemoryEmotion::Positive || Memory.Emotion == EMemoryEmotion::Joyful)
			{
				PositiveCount++;
			}
			else if (Memory.Emotion == EMemoryEmotion::Negative || Memory.Emotion == EMemoryEmotion::Traumatic)
			{
				NegativeCount++;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Memory Analysis:"));
		UE_LOG(LogTemp, Log, TEXT("- Total about player: %d"), PlayerMemories.Num());
		UE_LOG(LogTemp, Log, TEXT("- Positive: %d"), PositiveCount);
		UE_LOG(LogTemp, Log, TEXT("- Negative: %d"), NegativeCount);
		UE_LOG(LogTemp, Log, TEXT("======================================"));
	}

	/**
	 * Тест производительности
	 */
	UFUNCTION(BlueprintCallable, Category = "Memory|Testing")
	static void PerformanceTest(AActor* NPC, int32 MemoryCount)
	{
		if (!NPC)
		{
			return;
		}

		UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
		if (!MemoryComp)
		{
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("========== Performance Test =========="));
		UE_LOG(LogTemp, Log, TEXT("Creating %d memories..."), MemoryCount);

		double StartTime = FPlatformTime::Seconds();

		// Создаем много воспоминаний
		for (int32 i = 0; i < MemoryCount; ++i)
		{
			MemoryComp->CreateMemory(
				EMemoryType::DialogueEvent,
				FText::FromString(FString::Printf(TEXT("Test Memory %d"), i)),
				FMath::RandRange(20.0f, 80.0f),
				EMemoryEmotion::Neutral,
				nullptr
			);
		}

		double CreationTime = FPlatformTime::Seconds() - StartTime;

		UE_LOG(LogTemp, Log, TEXT("Creation time: %.4f seconds (%.2f ms per memory)"),
			CreationTime,
			(CreationTime / MemoryCount) * 1000.0);

		// Тестируем поиск
		StartTime = FPlatformTime::Seconds();

		TArray<FNPCMemoryEntry> AllMemories = MemoryComp->GetAllMemories();

		double RetrievalTime = FPlatformTime::Seconds() - StartTime;

		UE_LOG(LogTemp, Log, TEXT("Retrieval time: %.4f seconds"), RetrievalTime);

		// Тестируем поиск с фильтрами
		FGameplayTagContainer TestTags;
		TestTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Test.Tag")));

		StartTime = FPlatformTime::Seconds();

		TArray<FNPCMemoryEntry> FilteredMemories = MemoryComp->FindRelevantMemories(
			TestTags,
			nullptr,
			10
		);

		double SearchTime = FPlatformTime::Seconds() - StartTime;

		UE_LOG(LogTemp, Log, TEXT("Search time: %.4f seconds"), SearchTime);
		UE_LOG(LogTemp, Log, TEXT("======================================"));
	}
};
