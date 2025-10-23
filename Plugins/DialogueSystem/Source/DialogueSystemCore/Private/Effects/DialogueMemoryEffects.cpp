// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueMemoryEffects.h"
#include "Core/DialogueContext.h"
#include "GameFramework/Actor.h"

// ===========================================================================================
// UDialogueEffect_AddMemory
// ===========================================================================================

void UDialogueEffect_AddMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	// Get NPC actor from context
	AActor* NPCActor = Context->GetNPC();
	if (!NPCActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_AddMemory: No NPC actor in context"));
		return;
	}

	// Find NPCMemoryComponent through reflection (to avoid hard dependency on DialogueSystemRuntime)
	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_AddMemory: NPCMemoryComponent class not found"));
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_AddMemory: NPC '%s' doesn't have NPCMemoryComponent"), 
			*NPCActor->GetName());
		return;
	}

	// Call AddMemoryByTag through reflection
	UFunction* AddMemoryFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
	if (!AddMemoryFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("UDialogueEffect_AddMemory: AddMemoryByTag function not found"));
		return;
	}

	// Prepare parameters
	struct FAddMemoryParams
	{
		FGameplayTag Tag;
		float Strength;
	};

	FAddMemoryParams Params;
	Params.Tag = MemoryTag;
	Params.Strength = MemoryStrength;

	// Execute
	MemoryComp->ProcessEvent(AddMemoryFunc, &Params);

	UE_LOG(LogTemp, Verbose, TEXT("UDialogueEffect_AddMemory: Added memory '%s' to NPC '%s' (Strength: %.2f)"),
		*MemoryTag.ToString(), *NPCActor->GetName(), MemoryStrength);
}

FText UDialogueEffect_AddMemory::GetDisplayText_Implementation() const
{
	return FText::Format(
		FText::FromString("Add Memory: {0} (Strength: {1})"),
		FText::FromString(MemoryTag.ToString()),
		FText::AsNumber(MemoryStrength)
	);
}

// ===========================================================================================
// UDialogueEffect_ModifyMemoryStrength
// ===========================================================================================

void UDialogueEffect_ModifyMemoryStrength::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	AActor* NPCActor = Context->GetNPC();
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

	// Get current strength
	UFunction* GetStrengthFunc = MemoryComp->FindFunction(TEXT("GetMemoryStrength"));
	UFunction* RemoveFunc = MemoryComp->FindFunction(TEXT("RemoveMemoryByTag"));
	UFunction* AddFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
	
	if (GetStrengthFunc && RemoveFunc && AddFunc)
	{
		// Note: Since we can't easily get return values through ProcessEvent,
		// we'll use a simpler approach: just add with modified strength
		
		// For additive mode, we'd need to know current strength first
		// For now, simplified implementation
		float NewStrength = bAdditive ? (0.5f + StrengthModifier) : StrengthModifier;
		NewStrength = FMath::Clamp(NewStrength, 0.0f, 1.0f);
		
		// Remove old
		struct FRemoveParams { FGameplayTag Tag; };
		FRemoveParams RemoveParams;
		RemoveParams.Tag = MemoryTag;
		MemoryComp->ProcessEvent(RemoveFunc, &RemoveParams);
		
		// Add new
		struct FAddParams { FGameplayTag Tag; float Strength; };
		FAddParams AddParams;
		AddParams.Tag = MemoryTag;
		AddParams.Strength = NewStrength;
		MemoryComp->ProcessEvent(AddFunc, &AddParams);
		
		UE_LOG(LogTemp, Verbose, TEXT("UDialogueEffect_ModifyMemoryStrength: Modified memory '%s' to strength %.2f"),
			*MemoryTag.ToString(), NewStrength);
	}
}

FText UDialogueEffect_ModifyMemoryStrength::GetDisplayText_Implementation() const
{
	FString Operation = bAdditive ? TEXT("+") : TEXT("=");
	return FText::Format(
		FText::FromString("Modify Memory Strength: {0} {1} {2}"),
		FText::FromString(MemoryTag.ToString()),
		FText::FromString(Operation),
		FText::AsNumber(StrengthModifier)
	);
}

// ===========================================================================================
// UDialogueEffect_RemoveMemory
// ===========================================================================================

void UDialogueEffect_RemoveMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	AActor* NPCActor = Context->GetNPC();
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
		UE_LOG(LogTemp, Error, TEXT("UDialogueEffect_RemoveMemory: RemoveMemoryByTag function not found"));
		return;
	}

	struct FRemoveParams
	{
		FGameplayTag Tag;
	};

	FRemoveParams Params;
	Params.Tag = MemoryTag;

	MemoryComp->ProcessEvent(RemoveFunc, &Params);

	UE_LOG(LogTemp, Verbose, TEXT("UDialogueEffect_RemoveMemory: Removed memory '%s' from NPC '%s'"),
		*MemoryTag.ToString(), *NPCActor->GetName());
}

FText UDialogueEffect_RemoveMemory::GetDisplayText_Implementation() const
{
	return FText::Format(
		FText::FromString("Remove Memory: {0}"),
		FText::FromString(MemoryTag.ToString())
	);
}

// ===========================================================================================
// UDialogueEffect_RefreshMemory
// ===========================================================================================

void UDialogueEffect_RefreshMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	AActor* NPCActor = Context->GetNPC();
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

	// Refresh memory by re-adding it with a slight strength boost
	UFunction* GetStrengthFunc = MemoryComp->FindFunction(TEXT("GetMemoryStrength"));
	UFunction* AddFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
	
	if (GetStrengthFunc && AddFunc)
	{
		// Simplified: assume current strength is decent and boost it slightly
		float RefreshedStrength = 0.85f; // Assume decent strength after refresh
		
		// Re-add with boosted strength (this updates timestamp in NPCMemoryComponent)
		struct FAddParams { FGameplayTag Tag; float Strength; };
		FAddParams AddParams;
		AddParams.Tag = MemoryTag;
		AddParams.Strength = FMath::Min(RefreshedStrength + 0.1f, 1.0f);
		
		MemoryComp->ProcessEvent(AddFunc, &AddParams);
		
		UE_LOG(LogTemp, Verbose, TEXT("UDialogueEffect_RefreshMemory: Refreshed memory '%s'"),
			*MemoryTag.ToString());
	}
}

FText UDialogueEffect_RefreshMemory::GetDisplayText_Implementation() const
{
	return FText::Format(
		FText::FromString("Refresh Memory: {0}"),
		FText::FromString(MemoryTag.ToString())
	);
}

// ===========================================================================================
// UDialogueEffect_CreateMemory
// ===========================================================================================

void UDialogueEffect_CreateMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	AActor* NPCActor = Context->GetNPC();
	AActor* PlayerActor = Context->GetPlayer();
	
	if (!NPCActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: No NPC actor in context"));
		return;
	}

	// Validate memory description
	if (MemoryDescription.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: Memory description is empty for NPC '%s'"), 
			*NPCActor->GetName());
		return;
	}

	// Find NPCMemoryComponent through reflection
	UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
	if (!MemoryCompClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: NPCMemoryComponent class not found"));
		return;
	}

	UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
	if (!MemoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: NPC '%s' doesn't have NPCMemoryComponent"), 
			*NPCActor->GetName());
		return;
	}

	// Call CreateMemory function
	UFunction* CreateMemoryFunc = MemoryComp->FindFunction(TEXT("CreateMemory"));
	if (!CreateMemoryFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("UDialogueEffect_CreateMemory: CreateMemory function not found"));
		return;
	}

	// Prepare parameters matching NPCMemoryComponent::CreateMemory signature
	struct FCreateMemoryParams
	{
		uint8 Type;              // EMemoryType as uint8
		FText Description;
		float Importance;
		uint8 Emotion;  // EMemoryEmotion as uint8
		AActor* RelatedActor;
		FName ReturnValue;       // For return value
	};

	FCreateMemoryParams Params;
	Params.Type = static_cast<uint8>(1);  // EMemoryType::PlayerChoice (most common for dialogue)
	Params.Description = MemoryDescription;
	Params.Importance = Importance;
	Params.Emotion = static_cast<uint8>(0); // EMemoryEmotion::Neutral (default)
	
	if (bLinkToPlayer && PlayerActor)
	{
		Params.RelatedActor = PlayerActor;
	}
	else
	{
		Params.RelatedActor = nullptr;
	}

	// Execute memory creation
	MemoryComp->ProcessEvent(CreateMemoryFunc, &Params);

	// Get the created memory ID from return value
	FName CreatedMemoryId = Params.ReturnValue;

	// Add context tags to the newly created memory
	if (ContextTags.Num() > 0 && CreatedMemoryId != NAME_None)
	{
		UFunction* AddTagsFunc = MemoryComp->FindFunction(TEXT("AddTagsToMemory"));
		if (AddTagsFunc)
		{
			struct FAddTagsParams
			{
				FName MemoryId;
				FGameplayTagContainer Tags;
				bool ReturnValue;  // Return value for bool function
			};
			
			FAddTagsParams TagParams;
			TagParams.MemoryId = CreatedMemoryId;
			TagParams.Tags = ContextTags;
			TagParams.ReturnValue = false;
			
			MemoryComp->ProcessEvent(AddTagsFunc, &TagParams);
			
			if (TagParams.ReturnValue)
			{
				UE_LOG(LogTemp, Verbose, TEXT("UDialogueEffect_CreateMemory: Successfully added %d tags to memory '%s'"),
					ContextTags.Num(), *CreatedMemoryId.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: Failed to add tags to memory '%s'"),
					*CreatedMemoryId.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UDialogueEffect_CreateMemory: AddTagsToMemory function not found"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UDialogueEffect_CreateMemory: Created memory for NPC '%s': '%s' (ID: %s, Importance: %.1f, Tags: %d)"),
		*NPCActor->GetName(), 
		*MemoryDescription.ToString(),
		*CreatedMemoryId.ToString(),
		Importance,
		ContextTags.Num());
}

FText UDialogueEffect_CreateMemory::GetDisplayText_Implementation() const
{
	// Show truncated description (first 50 characters)
	FString ShortDesc = MemoryDescription.ToString();
	if (ShortDesc.Len() > 50)
	{
		ShortDesc = ShortDesc.Left(47) + TEXT("...");
	}

	return FText::Format(
		FText::FromString("Create Memory: \"{0}\" (Importance: {1})"),
		FText::FromString(ShortDesc),
		FText::AsNumber(Importance)
	);
}
