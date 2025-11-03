// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_ModifyRelationship.h"
#include "Core/DialogueContext.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/PackageName.h"

void UDialogueEffect_ModifyRelationship::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return;
	}

	AActor* Subject = bApplyToPlayer ? Context->GetPlayer() : Context->GetNPC();
	AActor* Target = bApplyToPlayer ? Context->GetNPC() : Context->GetPlayer();

	if (!Subject || !Target)
	{
		return;
	}

	UActorComponent* RelComp = GetRelationshipComponent(Subject);
	if (!RelComp)
	{
		return;
	}

	switch (Mode)
	{
	case ERelationshipModificationMode::ExecuteAction:
		ExecuteAction(RelComp, Target);
		break;

	case ERelationshipModificationMode::DirectModify:
		DirectModifyDimensions(RelComp, Target);
		break;

	case ERelationshipModificationMode::AddTrait:
		AddTraitToRelationship(RelComp, Target);
		break;

	case ERelationshipModificationMode::RemoveTrait:
		RemoveTraitFromRelationship(RelComp, Target);
		break;

	case ERelationshipModificationMode::ForceStateTransition:
		ForceState(RelComp, Target);
		break;
	}
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
	if (!RelComp || !ActionTag.IsValid())
	{
		return;
	}

	UFunction* ExecuteActionFunc = RelComp->FindFunction(TEXT("ExecuteAction"));
	if (!ExecuteActionFunc)
	{
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
