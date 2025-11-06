// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/NPCMemoryComponent.h"
#include "NPCMemoryExamples.generated.h"

// Forward declarations
class URelationshipComponent;

/**
 * ������� ������������� UNPCMemoryComponent
 * 
 * ���� ����� ������������� ��������� ������� ������������� ���������� ������ NPC
 */
UCLASS()
class DIALOGUESYSTEMRUNTIME_API UNPCMemoryExamples : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * ������ 1: ������� ������������� - ���������� ������������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_AddBasicMemory(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		FName MemoryId = MemoryComponent->CreateMemory(
			EMemoryType::DialogueEvent,
			FText::FromString("Player asked about the mysterious stranger"),
			60.0f, // ������� ��������
			EMemoryEmotion::Neutral,
			Player
		);
		
		UE_LOG(LogTemp, Log, TEXT("Created memory with ID: %s"), *MemoryId.ToString());
	}

	/**
	 * ������ 2: �������� ���������� ������������ � ������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_AddDetailedMemory(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		FNPCMemoryEntry Memory;
		Memory.MemoryId = FName("PlayerBroughtGift_001");
		Memory.Type = EMemoryType::ItemGiven;
		Memory.Description = FText::FromString("Player brought me a beautiful rose from the garden");
		Memory.Emotion = EMemoryEmotion::Joyful;
		Memory.Importance = 85.0f; // ������� �������� - ��� �������!
		Memory.RelatedActor = Player;
		
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift.Flower")));
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Emotion.Happy")));
		Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Location.Garden")));
		
		Memory.MetaData = TEXT("{\"ItemName\":\"Rose\",\"Value\":50,\"Rarity\":\"Rare\"}");
		
		MemoryComponent->AddMemory(Memory);
	}

	/**
	 * ������ 3: ����� ����������� ������������ ��� �������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_FindRelevantMemories(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		FGameplayTagContainer ContextTags;
		ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Romance")));
		ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
		
		TArray<FNPCMemoryEntry> RelevantMemories = MemoryComponent->FindRelevantMemories(
			ContextTags,
			Player, // ��������� � �������
			5 // �������� 5 ������������
		);
		
		for (const FNPCMemoryEntry& Memory : RelevantMemories)
		{
			UE_LOG(LogTemp, Log, TEXT("Relevant memory: %s (Importance: %.1f)"),
				*Memory.Description.ToString(), Memory.Importance);
			
			// "I remember when you brought me that rose... it was so thoughtful"
		}
	}

	/**
	 * ������ 4: ������������ ��� ���������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_TrackConversationTopics(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		MemoryComponent->DiscussTopic(FName("Family"), 10.0f); // +10 � ��������
		MemoryComponent->DiscussTopic(FName("War"), -5.0f); // -5 � �������� (���������� ����)
		
		int32 FamilyCount = MemoryComponent->GetTopicDiscussionCount(FName("Family"));
		
		TArray<FConversationTopic> InterestingTopics = MemoryComponent->GetInterestingTopics(3);
		
		for (const FConversationTopic& Topic : InterestingTopics)
		{
			UE_LOG(LogTemp, Log, TEXT("Interesting topic: %s (Interest: %.1f)"),
				*Topic.TopicName.ToString(), Topic.Interest);
		}
		
	}

	/**
	 * ������ 5: ��������� �������� � ��������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_PromisesAndSecrets(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		FNPCMemoryEntry Promise;
		Promise.Type = EMemoryType::Promise;
		Promise.Description = FText::FromString("Player promised to help find my lost brother");
		Promise.Importance = 95.0f; // �������� ����� �����!
		Promise.Emotion = EMemoryEmotion::Nostalgic;
		Promise.RelatedActor = Player;
		Promise.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Quest.FindBrother")));
		
		MemoryComponent->AddMemory(Promise);
		
		TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);
		for (const FNPCMemoryEntry& P : Promises)
		{
		}
		
		FNPCMemoryEntry Secret;
		Secret.Type = EMemoryType::Secret;
		Secret.Description = FText::FromString("Player revealed they are actually a prince in hiding");
		Secret.Importance = 100.0f; // ������������ �������� - ������!
		Secret.Emotion = EMemoryEmotion::Bittersweet;
		Secret.RelatedActor = Player;
		Secret.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Secret.Identity")));
		
		MemoryComponent->AddMemory(Secret);
		
	}

	/**
	 * ������ 6: ���������� � �������� ���������
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
		
		FNPCMemoryEntry BadMemory;
		BadMemory.Type = EMemoryType::PlayerChoice;
		BadMemory.Description = FText::FromString("Player betrayed my trust by revealing secret to guards");
		BadMemory.Importance = 90.0f;
		BadMemory.Emotion = EMemoryEmotion::Traumatic;
		BadMemory.RelatedActor = Player;
		BadMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")));
		
		MemoryComponent->AddMemory(BadMemory);
		
		// NOTE: Uncomment these lines when you have URelationshipComponent available
		// RelationshipComp->ModifyAffinity(-30.0f);
		// RelationshipComp->ModifyTrust(-50.0f);
		// RelationshipComp->ModifyFear(20.0f);
		
		TArray<FNPCMemoryEntry> BetrayalMemories = MemoryComponent->GetMemoriesWithTags(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal"))),
			false
		);
		
		if (BetrayalMemories.Num() > 0)
		{
			// "I trusted you once... I won't make that mistake again"
		}
	}

	/**
	 * ������ 7: ������������ ��������� �������� �� ������ ������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static FText Example_GenerateContextualDialogue(UNPCMemoryComponent* MemoryComponent, AActor* Player)
	{
		if (!MemoryComponent)
		{
			return FText::FromString("Hello.");
		}
		
		TArray<FNPCMemoryEntry> RecentMemories = MemoryComponent->GetMemoriesAboutActor(Player);
		
		RecentMemories.Sort([](const FNPCMemoryEntry& A, const FNPCMemoryEntry& B)
		{
			return A.Timestamp > B.Timestamp;
		});
		
		if (RecentMemories.Num() > 0)
		{
			const FNPCMemoryEntry& LastMemory = RecentMemories[0];
			
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
	 * ������ 8: ���������� � �������� ������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_SaveLoadMemory(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		FString JsonData;
		MemoryComponent->SaveToJson(JsonData);
		
		UE_LOG(LogTemp, Log, TEXT("Saved memory data: %d characters"), JsonData.Len());
		
		bool bSuccess = MemoryComponent->LoadFromJson(JsonData);
		
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded %d memories"), 
				MemoryComponent->GetMemoryCount());
		}
	}

	/**
	 * ������ 9: ��������� �������� ����� ��������� ������
	 */
	UFUNCTION(BlueprintCallable, Category = "Examples|Memory")
	static void Example_ConfigureNPCPersonality(UNPCMemoryComponent* MemoryComponent)
	{
		if (!MemoryComponent)
		{
			return;
		}
		
		// MemoryComponent->MemoryRetentionMultiplier = 2.0f;
		// MemoryComponent->BaseDecayRate = 0.02f;
		
		// MemoryComponent->MemoryRetentionMultiplier = 0.5f;
		// MemoryComponent->BaseDecayRate = 0.1f;
		
		// MemoryComponent->NegativeBias = 2.0f;
		
		// MemoryComponent->NegativeBias = 0.5f;
		
		// TMap<EMemoryType, float> Preferences;
		
		UE_LOG(LogTemp, Log, TEXT("Configured NPC personality through memory parameters"));
	}

	/**
	 * ������ 10: ������������� � Condition'�� �������
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
		
		FNPCMemoryEntry Memory;
		if (MemoryComponent->GetMemory(MemoryId, Memory))
		{
			return Memory.Importance >= MinImportance && Memory.Freshness > 0.3f;
		}
		
		return false;
	}
};
