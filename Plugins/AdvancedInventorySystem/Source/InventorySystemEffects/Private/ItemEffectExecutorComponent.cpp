// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemEffectExecutorComponent.h"
#include "ItemEffectDataAsset.h"
#include "ItemDataAsset.h"
#include "InventoryItem.h"
#include "InventorySystemInterfaces.h"
#include "TimerManager.h"
#include "Engine/World.h"

UItemEffectExecutorComponent::UItemEffectExecutorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemEffectExecutorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UItemEffectExecutorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopAllPersistentEffects();
	Super::EndPlay(EndPlayReason);
}

bool UItemEffectExecutorComponent::ExecuteItemEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType, AActor* Target)
{
	if (!Item || !Item->ItemData)
	{
		return false;
	}

	TArray<UItemEffectDataAsset*> Effects = Item->ItemData->GetEffectsByTrigger(TriggerType);
	if (Effects.Num() == 0)
	{
		return false;
	}

	bool bAnyExecuted = false;

	for (UItemEffectDataAsset* Effect : Effects)
	{
		if (ExecuteSingleEffect(Effect, Item, Target))
		{
			bAnyExecuted = true;
		}
	}

	return bAnyExecuted;
}

bool UItemEffectExecutorComponent::ExecuteSingleEffect(UItemEffectDataAsset* Effect, UInventoryItem* Item, AActor* Target)
{
	if (!Effect || !Item)
	{
		return false;
	}

	// Get targets for this effect
	TArray<AActor*> Targets = GetEffectTargets(Effect, Target);
	if (Targets.Num() == 0)
	{
		return false;
	}

	bool bAnyExecuted = false;

	for (AActor* EffectTarget : Targets)
	{
		if (!EffectTarget)
		{
			continue;
		}

		// Check conditions
		if (!Effect->CanExecute(EffectTarget))
		{
			continue;
		}

		// Handle execution delay
		if (Effect->ExecutionDelay > 0.0f)
		{
			// Execute after delay
			FTimerHandle DelayTimerHandle;
			FTimerDelegate DelayDelegate;
			DelayDelegate.BindLambda([this, Effect, EffectTarget, Item]()
			{
				ExecuteNeedsModifications(Effect, EffectTarget);
				ExecuteDialogueTriggers(Effect, EffectTarget);
				ExecuteTagModifications(Effect, EffectTarget);
				ExecuteCustomEffect(Effect, EffectTarget, Item);
			});

			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, DelayDelegate, Effect->ExecutionDelay, false);
			bAnyExecuted = true;
		}
		else
		{
			// Execute immediately
			ExecuteNeedsModifications(Effect, EffectTarget);
			ExecuteDialogueTriggers(Effect, EffectTarget);
			ExecuteTagModifications(Effect, EffectTarget);
			ExecuteCustomEffect(Effect, EffectTarget, Item);
			bAnyExecuted = true;
		}
	}

	return bAnyExecuted;
}

void UItemEffectExecutorComponent::StartPersistentEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType)
{
	if (!Item || !Item->ItemData)
	{
		return;
	}

	if (TriggerType != EItemEffectTrigger::WhileOwned && TriggerType != EItemEffectTrigger::WhileEquipped)
	{
		return;
	}

	TArray<UItemEffectDataAsset*> Effects = Item->ItemData->GetEffectsByTrigger(TriggerType);

	for (UItemEffectDataAsset* Effect : Effects)
	{
		if (!Effect || !Effect->IsRepeatingEffect())
		{
			continue;
		}

		// Create active effect
		FActiveItemEffect& ActiveEffect = ActiveEffects.AddDefaulted_GetRef();
		ActiveEffect.EffectData = Effect;
		ActiveEffect.Target = GetOwner(); // Default to owner
		ActiveEffect.SourceItemId = Item->ItemInstanceId;
		ActiveEffect.RemainingExecutions = Effect->ExecutionCount;
		ActiveEffect.NextExecutionTime = GetWorld()->GetTimeSeconds();

		// Setup timer for repeating effect
		int32 EffectIndex = ActiveEffects.Num() - 1;
		
		if (Effect->ExecutionInterval > 0.0f)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUObject(this, &UItemEffectExecutorComponent::ExecuteRepeatingEffect, EffectIndex);
			
			GetWorld()->GetTimerManager().SetTimer(
				ActiveEffect.TimerHandle,
				TimerDelegate,
				Effect->ExecutionInterval,
				true,
				0.0f // Start immediately
			);
		}
	}
}

void UItemEffectExecutorComponent::StopPersistentEffects(FInventoryItemId ItemId)
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		if (ActiveEffects[i].SourceItemId == ItemId)
		{
			if (ActiveEffects[i].TimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ActiveEffects[i].TimerHandle);
			}
			ActiveEffects.RemoveAt(i);
		}
	}
}

void UItemEffectExecutorComponent::StopAllPersistentEffects()
{
	for (FActiveItemEffect& ActiveEffect : ActiveEffects)
	{
		if (ActiveEffect.TimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ActiveEffect.TimerHandle);
		}
	}

	ActiveEffects.Empty();
}

void UItemEffectExecutorComponent::ExecuteNeedsModifications(UItemEffectDataAsset* Effect, AActor* Target)
{
	if (!Effect || !Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteNeedsModifications: Effect or Target is null!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ExecuteNeedsModifications: Target=%s, Effect=%s"), 
		*Target->GetName(), *Effect->GetName());

	// Check if target implements needs system interface
	if (!Target->Implements<UNeedsSystemInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("  -> Target %s does NOT implement UNeedsSystemInterface!"), *Target->GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("  -> Target implements interface! Executing %d needs modifications..."), Effect->NeedsModifications.Num());

	// Execute all needs modifications
	// Use Execute_ function directly on UObject (works with Blueprint implementations!)
	for (const FNeedsModification& NeedMod : Effect->NeedsModifications)
	{
		if (NeedMod.NeedTag.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("  -> Calling ModifyNeed: Tag=%s, Amount=%f"), 
				*NeedMod.NeedTag.ToString(), NeedMod.ModificationAmount);
			
			// Call via Execute_ function which works with Blueprint interfaces
			INeedsSystemInterface::Execute_ModifyNeed(Target, NeedMod.NeedTag, NeedMod.ModificationAmount, NeedMod.bClampValue);
		}
	}
}

void UItemEffectExecutorComponent::ExecuteDialogueTriggers(UItemEffectDataAsset* Effect, AActor* Target)
{
	if (!Effect || !Target)
	{
		return;
	}

	// Check if target implements dialogue system interface
	if (!Target->Implements<UDialogueSystemInterface>())
	{
		return;
	}

	IDialogueSystemInterface* DialogueInterface = Cast<IDialogueSystemInterface>(Target);
	if (!DialogueInterface)
	{
		return;
	}

	// Execute all dialogue triggers
	for (const FDialogueTrigger& DialogueTrigger : Effect->DialogueTriggers)
	{
		if (DialogueTrigger.DialogueTag.IsValid())
		{
			// Check conditions if needed
			if (DialogueTrigger.bCheckConditions)
			{
				if (!DialogueInterface->Execute_CheckDialogueCondition(Target, DialogueTrigger.DialogueTag))
				{
					continue;
				}
			}

			DialogueInterface->Execute_TriggerDialogue(Target, DialogueTrigger.DialogueTag, nullptr);
		}
	}
}

void UItemEffectExecutorComponent::ExecuteTagModifications(UItemEffectDataAsset* Effect, AActor* Target)
{
	if (!Effect || !Target)
	{
		return;
	}

	// Tag modifications would require a tag component or interface
	// This is a placeholder for tag system integration
	// Implementation depends on the specific tag system being used
}

void UItemEffectExecutorComponent::ExecuteCustomEffect(UItemEffectDataAsset* Effect, AActor* Target, UInventoryItem* SourceItem)
{
	if (!Effect || !Target || !Effect->CustomEffectClass)
	{
		return;
	}

	// Custom effect execution via Blueprint-implementable interface or event
	// This allows for maximum flexibility in effect implementation
	// Implementation is left to project-specific needs
}

TArray<AActor*> UItemEffectExecutorComponent::GetEffectTargets(UItemEffectDataAsset* Effect, AActor* DefaultTarget)
{
	TArray<AActor*> Targets;

	if (!Effect)
	{
		return Targets;
	}

	switch (Effect->TargetType)
	{
	case EItemEffectTarget::Self:
		if (GetOwner())
		{
			Targets.Add(GetOwner());
		}
		break;

	case EItemEffectTarget::NPC:
		if (DefaultTarget)
		{
			Targets.Add(DefaultTarget);
		}
		break;

	case EItemEffectTarget::CustomSelector:
		if (Effect->CustomTargetSelector && GetOwner())
		{
			UObject* SelectorInstance = NewObject<UObject>(this, Effect->CustomTargetSelector);
			if (SelectorInstance && SelectorInstance->Implements<UItemEffectTargetSelector>())
			{
				IItemEffectTargetSelector* Selector = Cast<IItemEffectTargetSelector>(SelectorInstance);
				if (Selector)
				{
					Selector->Execute_SelectTargets(SelectorInstance, nullptr, GetOwner(), Targets);
				}
			}
		}
		break;
	}

	return Targets;
}

void UItemEffectExecutorComponent::ExecuteRepeatingEffect(int32 EffectIndex)
{
	if (!ActiveEffects.IsValidIndex(EffectIndex))
	{
		return;
	}

	FActiveItemEffect& ActiveEffect = ActiveEffects[EffectIndex];

	if (!ActiveEffect.EffectData || !ActiveEffect.Target)
	{
		return;
	}

	// Check if target still meets conditions
	if (!ActiveEffect.EffectData->CanExecute(ActiveEffect.Target))
	{
		return;
	}

	// Execute the effect
	ExecuteNeedsModifications(ActiveEffect.EffectData, ActiveEffect.Target);
	ExecuteDialogueTriggers(ActiveEffect.EffectData, ActiveEffect.Target);
	ExecuteTagModifications(ActiveEffect.EffectData, ActiveEffect.Target);
	ExecuteCustomEffect(ActiveEffect.EffectData, ActiveEffect.Target, nullptr);

	// Update remaining executions
	if (ActiveEffect.RemainingExecutions > 0)
	{
		ActiveEffect.RemainingExecutions--;

		// Stop if no more executions
		if (ActiveEffect.RemainingExecutions <= 0 && ActiveEffect.EffectData->ExecutionCount > 0)
		{
			if (ActiveEffect.TimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ActiveEffect.TimerHandle);
			}
			ActiveEffects.RemoveAt(EffectIndex);
		}
	}
}
