// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemEffectDataAsset.h"
#include "ItemDataAsset.h"

bool FItemEffectCondition::EvaluateConditions(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// Level checks
	if (MinimumLevel > 0 || MaximumLevel > 0)
	{
		// Would need a level interface to check this properly
		// For now, assume it passes
	}

	// Tag checks would require proper tag system integration
	// This is a placeholder for the actual implementation

	// Custom condition check could be implemented via Blueprint
	// using a dynamic delegate or interface call

	return true;
}

UItemEffectDataAsset::UItemEffectDataAsset()
{
	EffectName = FText::FromString("New Effect");
	EffectDescription = FText::FromString("Effect description");
	TriggerType = EItemEffectTrigger::OnUse;
	TargetType = EItemEffectTarget::Self;
	ExecutionDelay = 0.0f;
	ExecutionCount = 1;
	ExecutionInterval = 0.0f;
}

bool UItemEffectDataAsset::CanExecute(AActor* Target) const
{
	return ExecutionConditions.EvaluateConditions(Target);
}

bool UItemEffectDataAsset::IsPersistentEffect() const
{
	return TriggerType == EItemEffectTrigger::WhileOwned || 
	       TriggerType == EItemEffectTrigger::WhileEquipped;
}

bool UItemEffectDataAsset::IsRepeatingEffect() const
{
	return ExecutionCount != 1 || ExecutionInterval > 0.0f;
}
