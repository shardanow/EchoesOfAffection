// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/RelationshipSystemBridge.h"

// TEMPORARY: Bridge implementation commented out until GameEventBus integration is complete
// TODO: Uncomment when FGameEvent struct is available from GameEventBus plugin

#if 0 // Disabled for now

void URelationshipSystemBridge::OnMemoryEventReceived(const FGameEvent& Event)
{
	if (!Event.EventTag.MatchesTag("NPC.Memory.Created"))
	{
		return;
	}

	// Проверяем конфигурацию
	if (!MappingConfig || !MappingConfig->bEnableMemoryToRelationship)
	{
		return;
	}

	// Извлекаем данные из события
	FString MemoryType = Event.Payload.FindRef("MemoryType");
	float Importance = FCString::Atof(*Event.Payload.FindRef("Importance"));
	float Freshness = FCString::Atof(*Event.Payload.FindRef("Freshness"));
	FString Emotion = Event.Payload.FindRef("Emotion");

	// Проверяем через конфиг, должна ли память влиять
	if (!MappingConfig->ShouldMemoryAffectRelationship(Importance, Freshness))
	{
		UE_LOG(LogRelationshipSystem, Verbose, 
			TEXT("Memory importance/freshness too low: %.1f/%.2f"), Importance, Freshness);
		return;
	}

	// Конвертируем через data-driven маппинг
	ConvertMemoryToRelationshipChange(
		Event.Instigator, 
		Event.Target,
		MemoryType, 
		Importance,
		Freshness,
		Emotion);
}

void URelationshipSystemBridge::ConvertMemoryToRelationshipChange(
	AActor* NPC, AActor* Target,
	const FString& MemoryType, float Importance, float Freshness, const FString& Emotion)
{
	if (!NPC || !MappingConfig)
	{
		return;
	}

	UNPCRelationshipComponent* RelComp = 
		NPC->FindComponentByClass<UNPCRelationshipComponent>();
	if (!RelComp)
	{
		return;
	}

	// 1. Найти действие для типа памяти (через Data Asset!)
	FGameplayTag ActionTag;
	if (MappingConfig->FindActionForMemoryType(MemoryType, Importance, ActionTag))
	{
		// Выполнить действие (вся логика в DA_Action_*)

		RelComp->ExecuteAction(Target, ActionTag);
		
		UE_LOG(LogRelationshipSystem, Verbose,
			TEXT("Memory '%s' triggered action '%s'"), 
			*MemoryType, *ActionTag.ToString());
	}

	// 2. Применить эмоциональные модификаторы (тоже через Data Asset!)
	ApplyEmotionModifiers(NPC, Target, Emotion, Importance);
}

void URelationshipSystemBridge::ApplyEmotionModifiers(
	AActor* NPC, AActor* Target, const FString& Emotion, float Importance)
{
	if (!MappingConfig || Emotion.IsEmpty())
	{
		return;
	}

	UNPCRelationshipComponent* RelComp = 
		NPC->FindComponentByClass<UNPCRelationshipComponent>();
	if (!RelComp)
	{
		return;
	}

	// Получаем модификаторы из Data Asset
	TArray<FEmotionDimensionModifier> Modifiers = 
		MappingConfig->GetModifiersForEmotion(Emotion);

	// Применяем каждый модификатор
	for (const FEmotionDimensionModifier& Modifier : Modifiers)
	{
		if (!Modifier.DimensionTag.IsValid())
		{
			continue;
		}

		// Вычисляем delta на основе важности и множителя
		float Delta = Importance * Modifier.ImportanceMultiplier;
		if (!Modifier.bPositiveInfluence)
		{
			Delta = -Delta;
		}

		// Применяем к dimension
		RelComp->ModifyDimensionValue(Target, Modifier.DimensionTag, Delta);

		UE_LOG(LogRelationshipSystem, Verbose,
			TEXT("Emotion '%s' modified '%s' by %.1f"), 
			*Emotion, *Modifier.DimensionTag.ToString(), Delta);
	}
}

#endif // Disabled Bridge
