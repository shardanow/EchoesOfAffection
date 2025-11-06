// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/MemoryConditions.h"
#include "GameFramework/Actor.h"




bool UHasMemoryCondition::EvaluateCondition_Implementation(AActor* NPCActor) const
{
	if (!NPCActor)
	{
		return false;
	}
	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return false;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return false;
	}
	UFunction* HasMemoryFunc = MemoryComp->FindFunction(TEXT("HasMemoryOfTag"));
	if (!HasMemoryFunc)
	{
		return false;
	}

	struct FHasMemoryParams
	{
		FGameplayTag MemoryTag;
		bool ReturnValue;
	};

	FHasMemoryParams Params;
	Params.MemoryTag = MemoryTag;
	Params.ReturnValue = false;

	MemoryComp->ProcessEvent(HasMemoryFunc, &Params);

	return Params.ReturnValue;
}

bool UMemoryStrengthCondition::EvaluateCondition_Implementation(AActor* NPCActor) const
{
	if (!NPCActor)
	{
		return false;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return false;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return false;
	}

	UFunction* GetStrengthFunc = MemoryComp->FindFunction(TEXT("GetMemoryStrength"));
	if (!GetStrengthFunc)
	{
		return false;
	}

	struct FGetStrengthParams
	{
		FGameplayTag MemoryTag;
		float ReturnValue;
	};

	FGetStrengthParams Params;
	Params.MemoryTag = MemoryTag;
	Params.ReturnValue = 0.0f;

	MemoryComp->ProcessEvent(GetStrengthFunc, &Params);

	float CurrentStrength = Params.ReturnValue;

	switch (ComparisonType)
	{
	case EMemoryComparisonType::GreaterThan:
		return CurrentStrength > TargetStrength;
	case EMemoryComparisonType::GreaterOrEqual:
		return CurrentStrength >= TargetStrength;
	case EMemoryComparisonType::Equal:
		return FMath::IsNearlyEqual(CurrentStrength, TargetStrength, 0.01f);
	case EMemoryComparisonType::LessOrEqual:
		return CurrentStrength <= TargetStrength;
	case EMemoryComparisonType::LessThan:
		return CurrentStrength < TargetStrength;
	default:
		return false;
	}
}

bool UMemoryDecayCondition::EvaluateCondition_Implementation(AActor* NPCActor) const
{
	if (!NPCActor)
	{
		return false;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return false;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return false;
	}

	UFunction* GetStrengthFunc = MemoryComp->FindFunction(TEXT("GetMemoryStrength"));
	if (!GetStrengthFunc)
	{
		return false;
	}

	struct FGetStrengthParams
	{
		FGameplayTag MemoryTag;
		float ReturnValue;
	};

	FGetStrengthParams Params;
	Params.MemoryTag = MemoryTag;
	Params.ReturnValue = 0.0f;

	MemoryComp->ProcessEvent(GetStrengthFunc, &Params);

	float CurrentStrength = Params.ReturnValue;

	if (bCheckForDecayed)
	{
		return CurrentStrength < DecayThreshold;
	}
	else
	{
		return CurrentStrength >= DecayThreshold;
	}
}

bool URecentMemoryCondition::EvaluateCondition_Implementation(AActor* NPCActor) const
{
	if (!NPCActor)
	{
		return false;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return false;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return false;
	}

	UFunction* GetTimeSinceFunc = MemoryComp->FindFunction(TEXT("GetTimeSinceLastUpdate"));
	if (!GetTimeSinceFunc)
	{
		return false;
	}

	struct FGetTimeParams
	{
		FGameplayTag MemoryTag;
		float ReturnValue;
	};

	FGetTimeParams Params;
	Params.MemoryTag = MemoryTag;
	Params.ReturnValue = 0.0f;

	MemoryComp->ProcessEvent(GetTimeSinceFunc, &Params);

	float TimeSince = Params.ReturnValue;

	return TimeSince <= MaxTimeSinceUpdate;
}
