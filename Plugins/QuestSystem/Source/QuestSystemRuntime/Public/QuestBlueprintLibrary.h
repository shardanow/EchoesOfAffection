// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuestTypes.h"
#include "QuestAsset.h"
#include "QuestBlueprintLibrary.generated.h"

/**
 * Quest Blueprint Function Library
 * Convenience functions for Blueprint usage
 */
UCLASS()
class QUESTSYSTEMRUNTIME_API UQuestBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Quest Management

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool StartQuest(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool CompleteQuest(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool FailQuest(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool AbandonQuest(UObject* WorldContextObject, FName QuestId);

	// Quest Queries

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static EQuestState GetQuestState(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool IsQuestActive(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool IsQuestCompleted(UObject* WorldContextObject, FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static TArray<FQuestSaveData> GetActiveQuests(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static UQuestAsset* GetQuestAsset(UObject* WorldContextObject, FName QuestId);

	// Objective Management

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool UpdateObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static bool CompleteObjective(UObject* WorldContextObject, FName QuestId, FName ObjectiveId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static void GetObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, 
		int32& CurrentCount, int32& TargetCount, bool& bIsCompleted);

	// Event Emission

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitItemAcquired(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitItemUsed(UObject* WorldContextObject, FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitNpcTalked(UObject* WorldContextObject, FName NpcId, AActor* Player, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitDialogChoice(UObject* WorldContextObject, FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitEnterArea(UObject* WorldContextObject, FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "Tags"))
	static void EmitActorKilled(UObject* WorldContextObject, AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitCustomEvent(UObject* WorldContextObject, FGameplayTag EventTag, FName StringParam, int32 IntParam, float FloatParam);

	// Utility

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	static FText QuestStateToText(EQuestState State);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	static FText ObjectiveStyleToText(EObjectiveStyle Style);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	static FLinearColor QuestStateToColor(EQuestState State);

private:
	static class UQuestSubsystem* GetQuestSubsystem(UObject* WorldContextObject);
	static class UQuestEventBus* GetEventBus(UObject* WorldContextObject);
};
