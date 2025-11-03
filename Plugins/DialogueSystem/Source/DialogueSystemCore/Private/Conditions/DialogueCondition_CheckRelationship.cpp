// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/DialogueCondition_CheckRelationship.h"
#include "Core/DialogueContext.h"

bool UDialogueCondition_CheckRelationship::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
	if (!Context)
	{
		return false;
	}

	AActor* Subject = bCheckPlayerRelationship ? Context->GetPlayer() : Context->GetNPC();
	AActor* Target = bCheckPlayerRelationship ? Context->GetNPC() : Context->GetPlayer();

	if (!Subject)
	{
		return false;
	}

	UActorComponent* RelComp = GetRelationshipComponent(Subject);
	if (!RelComp)
	{

		UE_LOG(LogTemp, Verbose, TEXT("DialogueCondition_CheckRelationship: RelationshipComponent not found (plugin may not be enabled)"));
		return false;
	}

	switch (CheckMode)
	{
	case ERelationshipCheckMode::CheckDimension:
		return CheckDimensionValue(RelComp, Target);

	case ERelationshipCheckMode::CheckState:
		return CheckCurrentState(RelComp, Target);

	case ERelationshipCheckMode::CheckNotState:
		return !CheckCurrentState(RelComp, Target);

	case ERelationshipCheckMode::CheckTrait:
		return CheckHasTrait(RelComp, Target);

	case ERelationshipCheckMode::CheckNoTrait:
		return !CheckHasTrait(RelComp, Target);

	default:
		return false;
	}
}

FString UDialogueCondition_CheckRelationship::GetDescription_Implementation() const
{
	FString Description;

	switch (CheckMode)
	{
	case ERelationshipCheckMode::CheckDimension:
	{
		FString OpStr;
		switch (ComparisonOp)
		{
		case EDialogueRelationshipComparisonOp::Greater: OpStr = TEXT(">"); break;
		case EDialogueRelationshipComparisonOp::GreaterOrEqual: OpStr = TEXT(">="); break;
		case EDialogueRelationshipComparisonOp::Less: OpStr = TEXT("<"); break;
		case EDialogueRelationshipComparisonOp::LessOrEqual: OpStr = TEXT("<="); break;
		case EDialogueRelationshipComparisonOp::Equal: OpStr = TEXT("="); break;
		case EDialogueRelationshipComparisonOp::NotEqual: OpStr = TEXT("!="); break;
		}
		Description = FString::Printf(TEXT("%s %s %.1f"), 
			*DimensionTag.ToString(), *OpStr, ThresholdValue);
		break;
	}

	case ERelationshipCheckMode::CheckState:
		Description = FString::Printf(TEXT("State = %s"), *StateOrTraitTag.ToString());
		break;

	case ERelationshipCheckMode::CheckNotState:
		Description = FString::Printf(TEXT("State != %s"), *StateOrTraitTag.ToString());
		break;

	case ERelationshipCheckMode::CheckTrait:
		Description = FString::Printf(TEXT("Has Trait: %s"), *StateOrTraitTag.ToString());
		break;

	case ERelationshipCheckMode::CheckNoTrait:
		Description = FString::Printf(TEXT("NO Trait: %s"), *StateOrTraitTag.ToString());
		break;
	}

	return FString::Printf(TEXT("Check Relationship: %s"), *Description);
}

UActorComponent* UDialogueCondition_CheckRelationship::GetRelationshipComponent(AActor* Actor) const
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

bool UDialogueCondition_CheckRelationship::CheckDimensionValue(UActorComponent* RelComp, AActor* Target) const
{
	if (!RelComp || !DimensionTag.IsValid())
	{
		return false;
	}

	UFunction* GetValueFunc = RelComp->FindFunction(TEXT("GetDimensionValue"));
	if (!GetValueFunc)
	{
		return false;
	}

	struct FGetDimensionValueParams
	{
		AActor* Target;
		FGameplayTag DimensionTag;
		float ReturnValue;
	};

	FGetDimensionValueParams Params;
	Params.Target = Target;
	Params.DimensionTag = DimensionTag;
	Params.ReturnValue = 0.0f;

	RelComp->ProcessEvent(GetValueFunc, &Params);

	return PerformComparison(Params.ReturnValue);
}

bool UDialogueCondition_CheckRelationship::CheckCurrentState(UActorComponent* RelComp, AActor* Target) const
{
	if (!RelComp || !StateOrTraitTag.IsValid())
	{
		return false;
	}

	UFunction* GetStateFunc = RelComp->FindFunction(TEXT("GetCurrentState"));
	if (!GetStateFunc)
	{
		return false;
	}

	struct FGetCurrentStateParams
	{
		AActor* Target;
		FGameplayTag ReturnValue;
	};

	FGetCurrentStateParams Params;
	Params.Target = Target;

	RelComp->ProcessEvent(GetStateFunc, &Params);

	return Params.ReturnValue.MatchesTagExact(StateOrTraitTag);
}

bool UDialogueCondition_CheckRelationship::CheckHasTrait(UActorComponent* RelComp, AActor* Target) const
{
	if (!RelComp || !StateOrTraitTag.IsValid())
	{
		return false;
	}

	UFunction* HasTraitFunc = RelComp->FindFunction(TEXT("HasTrait"));
	if (!HasTraitFunc)
	{
		return false;
	}

	struct FHasTraitParams
	{
		AActor* Target;
		FGameplayTag TraitTag;
		bool ReturnValue;
	};

	FHasTraitParams Params;
	Params.Target = Target;
	Params.TraitTag = StateOrTraitTag;
	Params.ReturnValue = false;

	RelComp->ProcessEvent(HasTraitFunc, &Params);

	return Params.ReturnValue;
}

bool UDialogueCondition_CheckRelationship::PerformComparison(float ActualValue) const
{
	switch (ComparisonOp)
	{
	case EDialogueRelationshipComparisonOp::Greater:
		return ActualValue > ThresholdValue;

	case EDialogueRelationshipComparisonOp::GreaterOrEqual:
		return ActualValue >= ThresholdValue;

	case EDialogueRelationshipComparisonOp::Less:
		return ActualValue < ThresholdValue;

	case EDialogueRelationshipComparisonOp::LessOrEqual:
		return ActualValue <= ThresholdValue;

	case EDialogueRelationshipComparisonOp::Equal:
		return FMath::Abs(ActualValue - ThresholdValue) <= EqualityTolerance;

	case EDialogueRelationshipComparisonOp::NotEqual:
		return FMath::Abs(ActualValue - ThresholdValue) > EqualityTolerance;

	default:
		return false;
	}
}
