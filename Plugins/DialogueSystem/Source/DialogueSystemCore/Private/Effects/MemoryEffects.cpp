// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/MemoryEffects.h"
#include "GameFramework/Actor.h"

void UAddMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
	if (!NPCActor)
	{
		return;
	}
	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return;
	}
	UFunction* AddMemoryFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
	if (!AddMemoryFunc)
	{
		return;
	}

	struct FAddMemoryParams
	{
		FGameplayTag Tag;
		float Strength;
	};

	FAddMemoryParams Params;
	Params.Tag = MemoryTag;
	Params.Strength = MemoryStrength;

	MemoryComp->ProcessEvent(AddMemoryFunc, &Params);
}

void UModifyMemoryStrengthEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
	if (!NPCActor)
	{
		return;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return;
	}

	UFunction* ModifyFunc = MemoryComp->FindFunction(TEXT("ModifyMemoryStrength"));
	if (!ModifyFunc)
	{
		return;
	}

	struct FModifyParams
	{
		FGameplayTag Tag;
		float Modifier;
		bool bAdditive;
	};

	FModifyParams Params;
	Params.Tag = MemoryTag;
	Params.Modifier = StrengthModifier;
	Params.bAdditive = bAdditive;

	MemoryComp->ProcessEvent(ModifyFunc, &Params);
}

void URemoveMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
	if (!NPCActor)
	{
		return;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return;
	}

	UFunction* RemoveFunc = MemoryComp->FindFunction(TEXT("RemoveMemoryByTag"));
	if (!RemoveFunc)
	{
		return;
	}

	struct FRemoveParams
	{
		FGameplayTag Tag;
	};

	FRemoveParams Params;
	Params.Tag = MemoryTag;

	MemoryComp->ProcessEvent(RemoveFunc, &Params);
}

void URefreshMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
	if (!NPCActor)
	{
		return;
	}

	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		return;
	}

	UFunction* RefreshFunc = MemoryComp->FindFunction(TEXT("RefreshMemory"));
	if (!RefreshFunc)
	{
		return;
	}

	struct FRefreshParams
	{
		FGameplayTag Tag;
	};

	FRefreshParams Params;
	Params.Tag = MemoryTag;

	MemoryComp->ProcessEvent(RefreshFunc, &Params);
}
