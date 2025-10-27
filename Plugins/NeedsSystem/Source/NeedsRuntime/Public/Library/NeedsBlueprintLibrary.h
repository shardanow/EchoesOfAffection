// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/NeedsTypes.h"
#include "NeedsBlueprintLibrary.generated.h"

// Forward declarations
class UCharacterNeedsComponent;
class UNeedsSubsystem;
class UNeedEffectAsset;
class UNeedModifierAsset;

/**
 * Blueprint Function Library for Needs System
 * Convenience functions for common operations
 */
UCLASS()
class NEEDSRUNTIME_API UNeedsBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ========================================
	// SUBSYSTEM ACCESS
	// ========================================

	/** Get Needs Subsystem */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs", meta = (WorldContext = "WorldContextObject"))
	static UNeedsSubsystem* GetNeedsSubsystem(const UObject* WorldContextObject);

	// ========================================
	// COMPONENT ACCESS
	// ========================================

	/** Get Needs Component from Actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static UCharacterNeedsComponent* GetNeedsComponent(AActor* Actor);

	/** Get Needs Component or create if missing */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static UCharacterNeedsComponent* GetOrCreateNeedsComponent(AActor* Actor);

	// ========================================
	// QUICK OPERATIONS
	// ========================================

	/** Quick: Apply effect to actor */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static bool ApplyEffectToActor(AActor* Actor, UNeedEffectAsset* Effect, int32 StackCount = 1);

	/** Quick: Add modifier to actor */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static bool AddModifierToActor(AActor* Actor, UNeedModifierAsset* Modifier);

	/** Quick: Set need value on actor */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static bool SetActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag, float Value);

	/** Quick: Modify need value on actor */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static bool ModifyActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag, float Delta);

	/** Quick: Get need value from actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static float GetActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag);

	/** Quick: Get mood from actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs", meta = (DefaultToSelf = "Actor"))
	static float GetActorMood(AActor* Actor);

	// ========================================
	// HANDLE UTILITIES
	// ========================================

	/** Check if handle is valid */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Handle")
	static bool IsHandleValid(const FNeedHandle& Handle);

	/** Get need value from handle (requires component) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Handle")
	static float GetNeedValueFromHandle(const FNeedHandle& Handle, UCharacterNeedsComponent* Component);

	// ========================================
	// THRESHOLD UTILITIES
	// ========================================

	/** Convert threshold level to color */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Threshold")
	static FLinearColor ThresholdToColor(ENeedThresholdLevel Threshold);

	/** Convert threshold level to text */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Threshold")
	static FText ThresholdToText(ENeedThresholdLevel Threshold);

	// ========================================
	// LOD UTILITIES
	// ========================================

	/** Convert LOD level to text */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|LOD")
	static FText LODToText(ENeedLODLevel LOD);

	/** Convert LOD level to color */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|LOD")
	static FLinearColor LODToColor(ENeedLODLevel LOD);

	// ========================================
	// MOOD UTILITIES
	// ========================================

	/** Mood value to mood description text */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Mood")
	static FText MoodToText(float MoodValue);

	/** Mood value to color */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Mood")
	static FLinearColor MoodToColor(float MoodValue);

	/** Get mood level from value (-100..100 -> 0..4) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Mood")
	static int32 GetMoodLevel(float MoodValue);
};
