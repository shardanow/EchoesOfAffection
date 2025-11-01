// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/QuestAssetFactory.h"
#include "QuestAsset.h"
#include "Misc/Guid.h"

UQuestAssetFactory::UQuestAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UQuestAsset::StaticClass();
}

UObject* UQuestAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UQuestAsset* NewQuestAsset = NewObject<UQuestAsset>(InParent, Class, Name, Flags);

	if (NewQuestAsset)
	{
		// Generate default Quest ID
		FGuid NewGuid = FGuid::NewGuid();
		FString IdString = FString::Printf(TEXT("Q_%s"), *NewGuid.ToString(EGuidFormats::Short));
		NewQuestAsset->QuestId = FName(*IdString);

		// Set default title
		NewQuestAsset->Title = FText::FromString(TEXT("New Quest"));
		NewQuestAsset->Description = FText::FromString(TEXT("Quest description goes here"));

		// Add default phase
		FQuestPhase DefaultPhase;
		DefaultPhase.PhaseId = FName(TEXT("Phase_Start"));
		DefaultPhase.Title = FText::FromString(TEXT("Starting Phase"));
		DefaultPhase.bRequireAllObjectives = true;

		// Add default objective
		FQuestObjective DefaultObjective;
		DefaultObjective.ObjectiveId = FName(TEXT("Objective_1"));
		DefaultObjective.Title = FText::FromString(TEXT("Complete the objective"));
		DefaultObjective.Style = EObjectiveStyle::Custom;
		DefaultObjective.Logic = EObjectiveLogic::And;
		DefaultObjective.Counters.TargetCount = 1;

		// Add default condition
		FObjectiveCondition DefaultCondition;
		DefaultCondition.EventType = EConditionEventType::Custom;
		DefaultCondition.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Custom"));
		DefaultCondition.Count = 1;

		DefaultObjective.Conditions.Add(DefaultCondition);
		DefaultPhase.Objectives.Add(DefaultObjective);
		NewQuestAsset->Phases.Add(DefaultPhase);

		// Set default meta
		NewQuestAsset->Meta.DifficultyLevel = 1;
		NewQuestAsset->Meta.Priority = 0;
		NewQuestAsset->Meta.bIsRepeatable = false;
		NewQuestAsset->Meta.bIsHidden = false;
		NewQuestAsset->Meta.bIsMainQuest = false;

		// Set default start policy
		NewQuestAsset->StartPolicy.PolicyType = EQuestStartPolicy::Manual;

		// Set default failure policy
		NewQuestAsset->FailurePolicy.FailureType = EQuestFailureType::None;
	}

	return NewQuestAsset;
}
