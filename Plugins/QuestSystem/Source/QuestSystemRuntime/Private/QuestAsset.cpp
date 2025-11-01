// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestAsset.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UQuestAsset::UQuestAsset()
{
	// Set default values
}

FPrimaryAssetId UQuestAsset::GetPrimaryAssetId() const
{
	// Use QuestId as the primary asset name
	if (!QuestId.IsNone())
	{
		return FPrimaryAssetId(FPrimaryAssetType("Quest"), QuestId);
	}
	return FPrimaryAssetId();
}

FQuestPhase UQuestAsset::GetPhase(FName PhaseId) const
{
	// ?? DIAGNOSTIC: Log all phases in asset
	UE_LOG(LogTemp, Warning, TEXT("?? UQuestAsset::GetPhase called for PhaseId='%s' in Quest='%s'"), 
		*PhaseId.ToString(), *QuestId.ToString());
	UE_LOG(LogTemp, Warning, TEXT("   Total phases in asset: %d"), Phases.Num());
	
	for (int32 i = 0; i < Phases.Num(); i++)
	{
		const FQuestPhase& Phase = Phases[i];
		
		UE_LOG(LogTemp, Warning, TEXT("   Phase[%d]: PhaseId='%s', Title='%s'"), 
			i, *Phase.PhaseId.ToString(), *Phase.Title.ToString());
		UE_LOG(LogTemp, Warning, TEXT("      - Objectives: %d"), Phase.Objectives.Num());
		UE_LOG(LogTemp, Warning, TEXT(" - PhaseRewards.Rewards: %d"), Phase.PhaseRewards.Rewards.Num());
		UE_LOG(LogTemp, Warning, TEXT("   - PhaseRewards.GrantedTags: %d"), Phase.PhaseRewards.GrantedTags.Num());
		
		// Log individual rewards
		for (int32 j = 0; j < Phase.PhaseRewards.Rewards.Num(); j++)
		{
			const FQuestReward& Reward = Phase.PhaseRewards.Rewards[j];
			UE_LOG(LogTemp, Warning, TEXT("     Reward[%d]: Type=%d, Amount=%d, ItemId='%s'"), 
				j, (int32)Reward.RewardType, Reward.Amount, *Reward.ItemId.ToString());
		}
		
		if (Phase.PhaseId == PhaseId)
		{
			UE_LOG(LogTemp, Warning, TEXT("   ? MATCH FOUND! Returning phase '%s'"), *PhaseId.ToString());
			return Phase;
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("   ? Phase '%s' NOT FOUND! Returning empty phase."), *PhaseId.ToString());
	return FQuestPhase();
}

bool UQuestAsset::HasPhase(FName PhaseId) const
{
	for (const FQuestPhase& Phase : Phases)
	{
		if (Phase.PhaseId == PhaseId)
		{
			return true;
		}
	}
	return false;
}

FQuestPhase UQuestAsset::GetFirstPhase() const
{
	if (Phases.Num() > 0)
	{
		return Phases[0];
	}
	return FQuestPhase();
}

TArray<FName> UQuestAsset::GetAllObjectiveIds() const
{
	TArray<FName> ObjectiveIds;
	for (const FQuestPhase& Phase : Phases)
	{
		for (const FQuestObjective& Objective : Phase.Objectives)
		{
			ObjectiveIds.Add(Objective.ObjectiveId);
		}
	}
	return ObjectiveIds;
}

bool UQuestAsset::ValidateQuestData(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Check quest ID
	if (QuestId.IsNone())
	{
		OutErrors.Add(FText::FromString(TEXT("Quest ID cannot be empty")));
		bIsValid = false;
	}

	// Check title
	if (Title.IsEmpty())
	{
		OutErrors.Add(FText::FromString(TEXT("Quest Title cannot be empty")));
		bIsValid = false;
	}

	// Check phases
	if (Phases.Num() == 0)
	{
		OutErrors.Add(FText::FromString(TEXT("Quest must have at least one phase")));
		bIsValid = false;
	}

	// Validate phase IDs are unique
	TSet<FName> PhaseIds;
	for (const FQuestPhase& Phase : Phases)
	{
		if (Phase.PhaseId.IsNone())
		{
			OutErrors.Add(FText::FromString(TEXT("Phase ID cannot be empty")));
			bIsValid = false;
		}
		else if (PhaseIds.Contains(Phase.PhaseId))
		{
			OutErrors.Add(FText::Format(
				FText::FromString(TEXT("Duplicate Phase ID: {0}")),
				FText::FromName(Phase.PhaseId)
			));
			bIsValid = false;
		}
		else
		{
			PhaseIds.Add(Phase.PhaseId);
		}

		// Check objectives
		if (Phase.Objectives.Num() == 0)
		{
			OutErrors.Add(FText::Format(
				FText::FromString(TEXT("Phase '{0}' must have at least one objective")),
				FText::FromName(Phase.PhaseId)
			));
			bIsValid = false;
		}

		// Validate objective IDs are unique within phase
		TSet<FName> ObjectiveIds;
		for (const FQuestObjective& Objective : Phase.Objectives)
		{
			if (Objective.ObjectiveId.IsNone())
			{
				OutErrors.Add(FText::Format(
					FText::FromString(TEXT("Objective ID in phase '{0}' cannot be empty")),
					FText::FromName(Phase.PhaseId)
				));
				bIsValid = false;
			}
			else if (ObjectiveIds.Contains(Objective.ObjectiveId))
			{
				OutErrors.Add(FText::Format(
					FText::FromString(TEXT("Duplicate Objective ID '{0}' in phase '{1}'")),
					FText::FromName(Objective.ObjectiveId),
					FText::FromName(Phase.PhaseId)
				));
				bIsValid = false;
			}
			else
			{
				ObjectiveIds.Add(Objective.ObjectiveId);
			}

			// Check conditions
			if (Objective.Conditions.Num() == 0)
			{
				OutErrors.Add(FText::Format(
					FText::FromString(TEXT("Objective '{0}' must have at least one condition")),
					FText::FromName(Objective.ObjectiveId)
				));
				bIsValid = false;
			}
		}

		// Validate phase transitions
		if (!Phase.Transition.NextPhaseId.IsNone())
		{
			if (!HasPhase(Phase.Transition.NextPhaseId))
			{
				OutErrors.Add(FText::Format(
					FText::FromString(TEXT("Phase '{0}' transitions to non-existent phase '{1}'")),
					FText::FromName(Phase.PhaseId),
					FText::FromName(Phase.Transition.NextPhaseId)
				));
				bIsValid = false;
			}
		}
	}

	// Check for circular phase dependencies (basic check)
	// More sophisticated cycle detection could be added

	return bIsValid;
}

#if WITH_EDITOR
void UQuestAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Auto-validate on property change
	TArray<FText> Errors;
	if (!ValidateQuestData(Errors))
	{
		for (const FText& Error : Errors)
		{
			UE_LOG(LogTemp, Warning, TEXT("Quest Validation: %s"), *Error.ToString());
		}
	}
}

EDataValidationResult UQuestAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TArray<FText> Errors;
	if (!ValidateQuestData(Errors))
	{
		for (const FText& Error : Errors)
		{
			Context.AddError(Error);
		}
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
