// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_ModifyRelationship.h"
#include "Core/DialogueContext.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/PackageName.h"

void UDialogueEffect_ModifyRelationship::Execute_Implementation(UDialogueSessionContext* Context)
{
	UE_LOG(LogTemp, Warning, TEXT("============================================"));
	UE_LOG(LogTemp, Warning, TEXT("DialogueEffect_ModifyRelationship::Execute CALLED"));
	UE_LOG(LogTemp, Warning, TEXT("  Mode: %d"), (int32)Mode);
	UE_LOG(LogTemp, Warning, TEXT("  bApplyToPlayer: %s"), bApplyToPlayer ? TEXT("TRUE") : TEXT("FALSE"));

	if (!Context)
	{
		UE_LOG(LogTemp, Error, TEXT("? Context is NULL!"));
		UE_LOG(LogTemp, Warning, TEXT("============================================"));
		return;
	}

	AActor* Subject = bApplyToPlayer ? Context->GetPlayer() : Context->GetNPC();
	AActor* Target = bApplyToPlayer ? Context->GetNPC() : Context->GetPlayer();

	UE_LOG(LogTemp, Warning, TEXT("  Subject: %s"), *GetNameSafe(Subject));
	UE_LOG(LogTemp, Warning, TEXT("  Target: %s"), *GetNameSafe(Target));

	if (!Subject || !Target)
	{
		UE_LOG(LogTemp, Error, TEXT("? Subject or Target is NULL!"));
		UE_LOG(LogTemp, Warning, TEXT("============================================"));
		return;
	}

	UActorComponent* RelComp = GetRelationshipComponent(Subject);
	
	UE_LOG(LogTemp, Warning, TEXT("  RelComp: %s"), RelComp ? *RelComp->GetName() : TEXT("NULL"));

	if (!RelComp)
	{
		UE_LOG(LogTemp, Error, TEXT("? RelationshipComponent NOT FOUND on Subject!"));
		UE_LOG(LogTemp, Error, TEXT("Make sure %s has NPCRelationshipComponent!"), *GetNameSafe(Subject));
		UE_LOG(LogTemp, Warning, TEXT("============================================"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("? Executing mode: %d"), (int32)Mode);

	switch (Mode)
	{
	case ERelationshipModificationMode::ExecuteAction:
		UE_LOG(LogTemp, Warning, TEXT("  ? ExecuteAction(%s)"), *ActionTag.ToString());
		ExecuteAction(RelComp, Target);
		break;

	case ERelationshipModificationMode::DirectModify:
		UE_LOG(LogTemp, Warning, TEXT("  ? DirectModifyDimensions (count: %d)"), DimensionModifiers.Num());
		for (const FRelationshipDimensionModifier& Mod : DimensionModifiers)
		{
			UE_LOG(LogTemp, Warning, TEXT("    - %s: %.2f"), *Mod.DimensionTag.ToString(), Mod.Delta);
		}
		DirectModifyDimensions(RelComp, Target);
		break;

	case ERelationshipModificationMode::AddTrait:
		UE_LOG(LogTemp, Warning, TEXT("  ? AddTrait(%s)"), *TraitTag.ToString());
		AddTraitToRelationship(RelComp, Target);
		break;

	case ERelationshipModificationMode::RemoveTrait:
		UE_LOG(LogTemp, Warning, TEXT("  ? RemoveTrait(%s)"), *TraitTag.ToString());
		RemoveTraitFromRelationship(RelComp, Target);
		break;

	case ERelationshipModificationMode::ForceStateTransition:
		UE_LOG(LogTemp, Warning, TEXT("  ? ForceState(%s)"), *TargetStateTag.ToString());
		ForceState(RelComp, Target);
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("? DialogueEffect_ModifyRelationship::Execute COMPLETED"));
	UE_LOG(LogTemp, Warning, TEXT("============================================"));
}

FText UDialogueEffect_ModifyRelationship::GetDisplayText_Implementation() const
{
	FString ModeStr;
	FString DetailStr;

	switch (Mode)
	{
	case ERelationshipModificationMode::ExecuteAction:
		ModeStr = TEXT("Execute Action");
		DetailStr = ActionTag.ToString();
		break;

	case ERelationshipModificationMode::DirectModify:
		ModeStr = TEXT("Modify Dimensions");
		DetailStr = FString::Printf(TEXT("%d modifiers"), DimensionModifiers.Num());
		break;

	case ERelationshipModificationMode::AddTrait:
		ModeStr = TEXT("Add Trait");
		DetailStr = TraitTag.ToString();
		break;

	case ERelationshipModificationMode::RemoveTrait:
		ModeStr = TEXT("Remove Trait");
		DetailStr = TraitTag.ToString();
		break;

	case ERelationshipModificationMode::ForceStateTransition:
		ModeStr = TEXT("Force State");
		DetailStr = TargetStateTag.ToString();
		break;

	default:
		ModeStr = TEXT("Unknown");
		break;
	}

	return FText::FromString(FString::Printf(TEXT("Modify Relationship: %s (%s)"), *ModeStr, *DetailStr));
}

UActorComponent* UDialogueEffect_ModifyRelationship::GetRelationshipComponent(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	UClass* RelCompClass = FindObject<UClass>(nullptr, TEXT("/Script/RelationshipSystemCore.NPCRelationshipComponent"));
	if (!RelCompClass)
	{
		return nullptr;
	}

	return Actor->GetComponentByClass(RelCompClass);
}

void UDialogueEffect_ModifyRelationship::ExecuteAction(UActorComponent* RelComp, AActor* Target)
{
	if (!RelComp)
	{
		UE_LOG(LogTemp, Error, TEXT("  ? RelComp is NULL!"));
		return;
	}

	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("  ? Target is NULL!"));
		return;
	}

	// ? ENSURE RELATIONSHIP EXISTS BEFORE EXECUTING ACTION
	AActor* Subject = RelComp->GetOwner();
	if (Subject && Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("  ? Ensuring relationship exists: %s -> %s"), *GetNameSafe(Subject), *GetNameSafe(Target));
		
		// Check if relationship exists - CORRECT FUNCTION NAME!
		UFunction* HasRelFunc = RelComp->FindFunction(TEXT("HasRelationshipWith"));
		if (HasRelFunc)
		{
			struct FHasRelationshipParams
			{
				AActor* TargetActor;
				bool ReturnValue;
			};
			
			FHasRelationshipParams HasParams;
			HasParams.TargetActor = Target;
			HasParams.ReturnValue = false;
			
			RelComp->ProcessEvent(HasRelFunc, &HasParams);
			
			if (!HasParams.ReturnValue)
			{
				UE_LOG(LogTemp, Warning, TEXT("?? Relationship DOESN'T exist! Creating it now..."));
				
				// Create relationship - CORRECT FUNCTION NAME!
				UFunction* CreateRelFunc = RelComp->FindFunction(TEXT("CreateRelationshipWith"));
				if (CreateRelFunc)
				{
					struct FCreateRelationshipParams
					{
						AActor* TargetActor;
						bool ReturnValue;
					};
					
					FCreateRelationshipParams CreateParams;
					CreateParams.TargetActor = Target;
					CreateParams.ReturnValue = false;
					
					RelComp->ProcessEvent(CreateRelFunc, &CreateParams);
					
					UE_LOG(LogTemp, Warning, TEXT("  ? Relationship created: %s"), CreateParams.ReturnValue ? TEXT("SUCCESS") : TEXT("FAILED"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("  ? CreateRelationshipWith function NOT FOUND!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  ? Relationship already exists"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  ? HasRelationshipWith function NOT FOUND!"));
		}
	}

	// NOW check ActionTag
	if (!ActionTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("  ?? ActionTag is INVALID, skipping action execution"));
		return;
	}

	// ? CHECK IF ACTION CAN BE EXECUTED (handles cooldowns gracefully)
	UFunction* CanExecuteFunc = RelComp->FindFunction(TEXT("CanExecuteAction"));
	if (CanExecuteFunc)
	{
		struct FCanExecuteActionParams
		{
			AActor* TargetActor;
			FGameplayTag ActionTag;
			bool ReturnValue;
		};

		FCanExecuteActionParams CanParams;
		CanParams.TargetActor = Target;
		CanParams.ActionTag = ActionTag;
		CanParams.ReturnValue = false;

		RelComp->ProcessEvent(CanExecuteFunc, &CanParams);

		if (!CanParams.ReturnValue)
		{
			UE_LOG(LogTemp, Warning, TEXT("  ?? Action '%s' cannot be executed (likely on cooldown), skipping"), *ActionTag.ToString());
			return;
		}
	}

	UFunction* ExecuteActionFunc = RelComp->FindFunction(TEXT("ExecuteAction"));
	if (!ExecuteActionFunc)
	{
		UE_LOG(LogTemp, Error, TEXT("  ? ExecuteAction function NOT FOUND!"));
		return;
	}

	struct FExecuteActionParams
	{
		AActor* TargetActor;
		FGameplayTag ActionTag;
		bool ReturnValue;
	};

	FExecuteActionParams Params;
	Params.TargetActor = Target;
	Params.ActionTag = ActionTag;
	Params.ReturnValue = false;

	RelComp->ProcessEvent(ExecuteActionFunc, &Params);
	
	UE_LOG(LogTemp, Warning, TEXT("  ? ExecuteAction called, result: %s"), Params.ReturnValue ? TEXT("SUCCESS") : TEXT("FAILED"));
}

void UDialogueEffect_ModifyRelationship::DirectModifyDimensions(UActorComponent* RelComp, AActor* Target)
{
	if (!RelComp || DimensionModifiers.Num() == 0)
	{
		return;
	}

	UFunction* ModifyFunc = RelComp->FindFunction(TEXT("ModifyDimensionValue"));
	if (!ModifyFunc)
	{
		return;
	}

	struct FModifyDimensionParams
	{
		AActor* TargetActor;
		FGameplayTag DimensionTag;
		float Delta;
		bool ReturnValue;
	};

	for (const FRelationshipDimensionModifier& Modifier : DimensionModifiers)
	{
		if (!Modifier.DimensionTag.IsValid())
		{
			continue;
		}

		FModifyDimensionParams Params;
		Params.TargetActor = Target;
		Params.DimensionTag = Modifier.DimensionTag;
		Params.Delta = Modifier.Delta;
		Params.ReturnValue = false;

		RelComp->ProcessEvent(ModifyFunc, &Params);
	}
}

void UDialogueEffect_ModifyRelationship::AddTraitToRelationship(UActorComponent* RelComp, AActor* Target)
{
	if (!RelComp || !TraitTag.IsValid())
	{
		return;
	}

	UFunction* AddTraitFunc = RelComp->FindFunction(TEXT("AddTrait"));
	if (!AddTraitFunc)
	{
		return;
	}

	struct FAddTraitParams
	{
		AActor* TargetActor;
		FGameplayTag TraitTag;
		bool ReturnValue;
	};

	FAddTraitParams Params;
	Params.TargetActor = Target;
	Params.TraitTag = TraitTag;
	Params.ReturnValue = false;

	RelComp->ProcessEvent(AddTraitFunc, &Params);
}

void UDialogueEffect_ModifyRelationship::RemoveTraitFromRelationship(UActorComponent* RelComp, AActor* Target)
{
	if (!RelComp || !TraitTag.IsValid())
	{
		return;
	}

	UFunction* RemoveTraitFunc = RelComp->FindFunction(TEXT("RemoveTrait"));
	if (!RemoveTraitFunc)
	{
		return;
	}

	struct FRemoveTraitParams
	{
		AActor* TargetActor;
		FGameplayTag TraitTag;
		bool ReturnValue;
	};

	FRemoveTraitParams Params;
	Params.TargetActor = Target;
	Params.TraitTag = TraitTag;
	Params.ReturnValue = false;

	RelComp->ProcessEvent(RemoveTraitFunc, &Params);
}

void UDialogueEffect_ModifyRelationship::ForceState(UActorComponent* RelComp, AActor* Target)
{
	if (!RelComp || !TargetStateTag.IsValid())
	{
		return;
	}

	UFunction* ForceStateFunc = RelComp->FindFunction(TEXT("ForceStateChange"));
	if (!ForceStateFunc)
	{
		return;
	}

	struct FForceStateParams
	{
		AActor* TargetActor;
		FGameplayTag NewState;
		bool ReturnValue;
	};

	FForceStateParams Params;
	Params.TargetActor = Target;
	Params.NewState = TargetStateTag;
	Params.ReturnValue = false;

	RelComp->ProcessEvent(ForceStateFunc, &Params);
}
