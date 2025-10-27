// Copyright Epic Games, Inc. All Rights Reserved.

#include "Library/NeedsBlueprintLibrary.h"
#include "Components/CharacterNeedsComponent.h"
#include "Subsystems/NeedsSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// ========================================
// SUBSYSTEM ACCESS
// ========================================

UNeedsSubsystem* UNeedsBlueprintLibrary::GetNeedsSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return World->GetSubsystem<UNeedsSubsystem>();
}

// ========================================
// COMPONENT ACCESS
// ========================================

UCharacterNeedsComponent* UNeedsBlueprintLibrary::GetNeedsComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UCharacterNeedsComponent>();
}

UCharacterNeedsComponent* UNeedsBlueprintLibrary::GetOrCreateNeedsComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component)
	{
		Component = NewObject<UCharacterNeedsComponent>(Actor, NAME_None, RF_Transactional);
		Actor->AddInstanceComponent(Component);
		Component->RegisterComponent();
	}

	return Component;
}

// ========================================
// QUICK OPERATIONS
// ========================================

bool UNeedsBlueprintLibrary::ApplyEffectToActor(AActor* Actor, UNeedEffectAsset* Effect, int32 StackCount)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component || !Effect)
	{
		return false;
	}

	Component->ApplyEffect_Implementation(Effect, StackCount);
	return true;
}

bool UNeedsBlueprintLibrary::AddModifierToActor(AActor* Actor, UNeedModifierAsset* Modifier)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component || !Modifier)
	{
		return false;
	}

	Component->AddModifier_Implementation(Modifier);
	return true;
}

bool UNeedsBlueprintLibrary::SetActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag, float Value)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component)
	{
		return false;
	}

	Component->SetNeedValue_Implementation(NeedTag, Value);
	return true;
}

bool UNeedsBlueprintLibrary::ModifyActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag, float Delta)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component)
	{
		return false;
	}

	Component->ModifyNeedValue_Implementation(NeedTag, Delta);
	return true;
}

float UNeedsBlueprintLibrary::GetActorNeedValue(AActor* Actor, const FGameplayTag& NeedTag)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component)
	{
		return 0.0f;
	}

	return Component->GetNeedValue_Implementation(NeedTag);
}

float UNeedsBlueprintLibrary::GetActorMood(AActor* Actor)
{
	UCharacterNeedsComponent* Component = GetNeedsComponent(Actor);
	if (!Component)
	{
		return 0.0f;
	}

	return Component->GetCurrentMood_Implementation();
}

// ========================================
// HANDLE UTILITIES
// ========================================

bool UNeedsBlueprintLibrary::IsHandleValid(const FNeedHandle& Handle)
{
	return Handle.IsValid();
}

float UNeedsBlueprintLibrary::GetNeedValueFromHandle(const FNeedHandle& Handle, UCharacterNeedsComponent* Component)
{
	if (!Component || !Handle.IsValid())
	{
		return 0.0f;
	}

	// Verify owner ID matches
	if (Handle.OwnerID != Component->GetComponentID())
	{
		return 0.0f;
	}

	return Component->GetNeedValue_Implementation(Handle.NeedTag);
}

// ========================================
// THRESHOLD UTILITIES
// ========================================

FLinearColor UNeedsBlueprintLibrary::ThresholdToColor(ENeedThresholdLevel Threshold)
{
	switch (Threshold)
	{
		case ENeedThresholdLevel::Normal:
			return FLinearColor::Green;
		case ENeedThresholdLevel::Warning:
			return FLinearColor::Yellow;
		case ENeedThresholdLevel::Critical:
			return FLinearColor::Red;
		case ENeedThresholdLevel::Recovered:
			return FLinearColor(0.0f, 0.8f, 1.0f); // Light blue
		default:
			return FLinearColor::White;
	}
}

FText UNeedsBlueprintLibrary::ThresholdToText(ENeedThresholdLevel Threshold)
{
	return StaticEnum<ENeedThresholdLevel>()->GetDisplayNameTextByValue((int64)Threshold);
}

// ========================================
// LOD UTILITIES
// ========================================

FText UNeedsBlueprintLibrary::LODToText(ENeedLODLevel LOD)
{
	return StaticEnum<ENeedLODLevel>()->GetDisplayNameTextByValue((int64)LOD);
}

FLinearColor UNeedsBlueprintLibrary::LODToColor(ENeedLODLevel LOD)
{
	switch (LOD)
	{
		case ENeedLODLevel::Near:
			return FLinearColor::Green;
		case ENeedLODLevel::Far:
			return FLinearColor::Yellow;
		case ENeedLODLevel::Parked:
			return FLinearColor::Red;
		default:
			return FLinearColor::White;
	}
}

// ========================================
// MOOD UTILITIES
// ========================================

FText UNeedsBlueprintLibrary::MoodToText(float MoodValue)
{
	if (MoodValue >= 80.0f)
	{
		return FText::FromString(TEXT("Excellent"));
	}
	else if (MoodValue >= 40.0f)
	{
		return FText::FromString(TEXT("Good"));
	}
	else if (MoodValue >= 0.0f)
	{
		return FText::FromString(TEXT("Neutral"));
	}
	else if (MoodValue >= -40.0f)
	{
		return FText::FromString(TEXT("Bad"));
	}
	else
	{
		return FText::FromString(TEXT("Terrible"));
	}
}

FLinearColor UNeedsBlueprintLibrary::MoodToColor(float MoodValue)
{
	// Gradient from red (bad) to green (good)
	const float Normalized = (MoodValue + 100.0f) / 200.0f; // 0..1

	if (Normalized >= 0.5f)
	{
		// Good mood: yellow to green
		const float T = (Normalized - 0.5f) * 2.0f;
		return FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, T);
	}
	else
	{
		// Bad mood: red to yellow
		const float T = Normalized * 2.0f;
		return FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Yellow, T);
	}
}

int32 UNeedsBlueprintLibrary::GetMoodLevel(float MoodValue)
{
	// 0 = Terrible, 1 = Bad, 2 = Neutral, 3 = Good, 4 = Excellent
	if (MoodValue >= 80.0f) return 4;
	if (MoodValue >= 40.0f) return 3;
	if (MoodValue >= 0.0f) return 2;
	if (MoodValue >= -40.0f) return 1;
	return 0;
}
