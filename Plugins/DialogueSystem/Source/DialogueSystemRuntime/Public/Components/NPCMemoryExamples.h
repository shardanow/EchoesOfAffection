// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/NPCMemoryComponent.h"
#include "NPCMemoryExamples.generated.h"

// Forward declarations
class URelationshipComponent;

/**
 * Примеры использования UNPCMemoryComponent
 * 
 * Этот класс демонстрирует различные способы использования компонента памяти NPC
 */
UCLASS()
class DIALOGUESYSTEMRUNTIME_API UNPCMemoryExamples : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Пример 1: Базовое использование - добавление воспоминания
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_AddBasicMemory(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Простой способ создать воспоминание
		FName MemoryId = MemoryComponent->CreateMemory(
			EMemoryType::DialogueEvent,
			FText::FromString("Player asked about the mysterious stranger"),
			60.0f, // Средняя важность
			EMemoryEmotion::Neutral,
			Player
		);
		
		UE_LOG(LogTemp, Log, TEXT("Created memory with ID: %s"), *MemoryId.ToString());
	}

	/**
	 * Пример 2: Создание подробного воспоминания с тегами
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_AddDetailedMemory(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Создаем структуру воспоминания вручную для большего контроля
		FNPCMemoryEntry Memory;
		Memory.MemoryId = FName("PlayerBroughtGift_001");
		Memory.Type = EMemoryType::ItemGiven;
		Memory.Description = FText::FromString("Player brought me a beautiful rose from the garden");
		Memory.Emotion = EMemoryEmotion::Joyful;
		Memory.Importance = 85.0f; // Высокая важность - это подарок!
		Memory.RelatedActor = Player;
		
		// Добавляем контекстные теги
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift.Flower")));
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Emotion.Happy")));
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Location.Garden")));
		
		// Можно сохранить дополнительные данные в JSON
		Memory.MetaData = TEXT("{\"ItemName\":\"Rose\",\"Value\":50,\"Rarity\":\"Rare\"}");
		
		MemoryComponent->AddMemory(Memory);
	}

	/**
	 * Пример 3: Поиск релевантных воспоминаний для диалога
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_FindRelevantMemories(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Создаем контекст - о чем сейчас разговор
		FGameplayTagContainer ContextTags;
		ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Romance")));
		ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
		
		// Ищем релевантные воспоминания
		TArray<FNPCMemoryEntry> RelevantMemories = MemoryComponent->FindRelevantMemories(
			ContextTags,
			Player, // Связанные с игроком
			5 // Максимум 5 воспоминаний
		);
		
		// Используем их в диалоге
		for (const FNPCMemoryEntry& Memory : RelevantMemories)
		{
			UE_LOG(LogTemp, Log, TEXT("Relevant memory: %s (Importance: %.1f)"),
				*Memory.Description.ToString(), Memory.Importance);
			
			// NPC может упомянуть это в диалоге:
			// "I remember when you brought me that rose... it was so thoughtful"
		}
	}

	/**
	 * Пример 4: Отслеживание тем разговора
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_TrackConversationTopics(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Отмечаем что обсуждали определенную тему
		MemoryComponent->DiscussTopic(FName("Family"), 10.0f); // +10 к интересу
		MemoryComponent->DiscussTopic(FName("War"), -5.0f); // -5 к интересу (неприятная тема)
		
		// Проверяем сколько раз обсуждали
		int32 FamilyCount = MemoryComponent->GetTopicDiscussionCount(FName("Family"));
		
		// Получаем самые интересные темы для NPC
		TArray<FConversationTopic> InterestingTopics = MemoryComponent->GetInterestingTopics(3);
		
		for (const FConversationTopic& Topic : InterestingTopics)
		{
			UE_LOG(LogTemp, Log, TEXT("Interesting topic: %s (Interest: %.1f)"),
				*Topic.TopicName.ToString(), Topic.Interest);
		}
		
		// Можно использовать это для:
		// - NPC сам предлагает поговорить о любимой теме
		// - NPC избегает неприятных тем
		// - Блокировка повторения одного и того же разговора
	}

	/**
	 * Пример 5: Обработка обещаний и секретов
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_PromisesAndSecrets(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// NPC запоминает обещание игрока
		FNPCMemoryEntry Promise;
		Promise.Type = EMemoryType::Promise;
		Promise.Description = FText::FromString("Player promised to help find my lost brother");
		Promise.Importance = 95.0f; // Обещания очень важны!
		Promise.Emotion = EMemoryEmotion::Nostalgic;
		Promise.RelatedActor = Player;
		Promise.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Quest.FindBrother")));
		
		MemoryComponent->AddMemory(Promise);
		
		// Позже можно проверить выполнил ли игрок обещание
		TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);
		for (const FNPCMemoryEntry& P : Promises)
		{
			// Проверяем статус квеста и обновляем память
			// Если игрок не выполнил - снижаем доверие через RelationshipComponent
		}
		
		// Секрет, который игрок рассказал NPC
		FNPCMemoryEntry Secret;
		Secret.Type = EMemoryType::Secret;
		Secret.Description = FText::FromString("Player revealed they are actually a prince in hiding");
		Secret.Importance = 100.0f; // Максимальная важность - секрет!
		Secret.Emotion = EMemoryEmotion::Bittersweet;
		Secret.RelatedActor = Player;
		Secret.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Secret.Identity")));
		
		MemoryComponent->AddMemory(Secret);
		
		// Секреты могут:
		// - Влиять на будущие диалоги (новые реплики)
		// - Создавать риски (NPC может проговориться при определенных условиях)
		// - Укреплять отношения (доверие)
	}

	/**
	 * Пример 6: Интеграция с системой отношений
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_IntegrationWithRelationships(UNPCMemoryComponent* MemoryComponent, 
	                                                  URelationshipComponent* RelationshipComp,
	                                                  AActor* Player)
	{
		if (!MemoryComponent || !RelationshipComp)
		{
			return;
		}
		
		// Создаем воспоминание о негативном событии
		FNPCMemoryEntry BadMemory;
		BadMemory.Type = EMemoryType::PlayerChoice;
		BadMemory.Description = FText::FromString("Player betrayed my trust by revealing secret to guards");
		BadMemory.Importance = 90.0f;
		BadMemory.Emotion = EMemoryEmotion::Traumatic;
		BadMemory.RelatedActor = Player;
		BadMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")));
		
		MemoryComponent->AddMemory(BadMemory);
		
		// Это воспоминание влияет на отношения
		// NOTE: Uncomment these lines when you have URelationshipComponent available
		// RelationshipComp->ModifyAffinity(-30.0f);
		// RelationshipComp->ModifyTrust(-50.0f);
		// RelationshipComp->ModifyFear(20.0f);
		
		// В будущих диалогах можно проверять:
		TArray<FNPCMemoryEntry> BetrayalMemories = MemoryComponent->GetMemoriesWithTags(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal"))),
			false
		);
		
		if (BetrayalMemories.Num() > 0)
		{
			// NPC помнит предательство - меняем диалог
			// "I trusted you once... I won't make that mistake again"
		}
	}

	/**
	 * Пример 7: Динамическая генерация диалогов на основе памяти
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static FText Example_GenerateContextualDialogue(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return FText::FromString("Hello.");
		}
		
		// Получаем недавние воспоминания об игроке
		TArray<FNPCMemoryEntry> RecentMemories = MemoryComponent->GetMemoriesAboutActor(Player);
		
		// Сортируем по времени
		RecentMemories.Sort([](const FNPCMemoryEntry& A, const FNPCMemoryEntry& B)
		{
			return A.Timestamp > B.Timestamp;
		});
		
		if (RecentMemories.Num() > 0)
		{
			const FNPCMemoryEntry& LastMemory = RecentMemories[0];
			
			// Генерируем приветствие на основе последнего воспоминания
			switch (LastMemory.Emotion)
			{
			case EMemoryEmotion::Joyful:
				return FText::FromString("Welcome back! I still remember the wonderful gift you brought me.");
				
			case EMemoryEmotion::Negative:
			case EMemoryEmotion::Traumatic:
				return FText::FromString("You again... I haven't forgotten what happened.");
				
			case EMemoryEmotion::Nostalgic:
				return FText::FromString("Ah, it's good to see you. Remember when we talked about...?");
				
			default:
				break;
			}
		}
		
		// Проверяем сколько раз общались
		int32 DialogueCount = MemoryComponent->GetMemoryCountByType(EMemoryType::DialogueEvent);
		
		if (DialogueCount > 10)
		{
			return FText::FromString("My old friend! It's always a pleasure.");
		}
		else if (DialogueCount > 5)
		{
			return FText::FromString("Good to see you again.");
		}
		
		return FText::FromString("Hello, stranger.");
	}

	/**
	 * Пример 8: Сохранение и загрузка памяти
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_SaveLoadMemory(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Сохранение
		FString JsonData;
		MemoryComponent->SaveToJson(JsonData);
		
		// Здесь вы можете сохранить JsonData в SaveGame или файл
		UE_LOG(LogTemp, Log, TEXT("Saved memory data: %d characters"), JsonData.Len());
		
		// Загрузка
		bool bSuccess = MemoryComponent->LoadFromJson(JsonData);
		
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded %d memories"), 
				MemoryComponent->GetMemoryCount());
		}
	}

	/**
	 * Пример 9: Настройка личности через параметры памяти
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_ConfigureNPCPersonality(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// Настройка для NPC с отличной памятью (ученый, мудрец)
		// MemoryComponent->MemoryRetentionMultiplier = 2.0f;
		// MemoryComponent->BaseDecayRate = 0.02f;
		
		// Настройка для забывчивого NPC (пьяница, старик)
		// MemoryComponent->MemoryRetentionMultiplier = 0.5f;
		// MemoryComponent->BaseDecayRate = 0.1f;
		
		// Настройка для обидчивого NPC (помнит зло)
		// MemoryComponent->NegativeBias = 2.0f;
		
		// Настройка для оптимиста (помнит хорошее)
		// MemoryComponent->NegativeBias = 0.5f;
		
		// Настройка приоритетов типов памяти
		// TMap<EMemoryType, float> Preferences;
		// Preferences.Add(EMemoryType::Promise, 3.0f); // Честный NPC помнит обещания
		// Preferences.Add(EMemoryType::Secret, 2.5f); // И секреты
		// Preferences.Add(EMemoryType::ItemGiven, 0.5f); // Но не особо ценит подарки
		
		UE_LOG(LogTemp, Log, TEXT("Configured NPC personality through memory parameters"));
	}

	/**
	 * Пример 10: Использование в Condition'ах диалога
	 */
	UFUNCTION(BlueprintPure, Category = "Examples|Memory")
	static bool Example_CheckMemoryCondition(UNPCMemoryComponent* MemoryComponent, 
	                                          FName MemoryId, 
	                                          float MinImportance = 50.0f)
	{
		if (!MemoryComponent)
		{
			return false;
		}
		
		// Проверяем помнит ли NPC определенное событие
		FNPCMemoryEntry Memory;
		if (MemoryComponent->GetMemory(MemoryId, Memory))
		{
			// Проверяем важность и свежесть
			return Memory.Importance >= MinImportance && Memory.Freshness > 0.3f;
		}
		
		return false;
	}
};
