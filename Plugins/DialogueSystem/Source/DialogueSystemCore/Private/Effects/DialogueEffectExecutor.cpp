// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffectExecutor.h"
#include "Core/DialogueContext.h"

// UDialogueEffect_ModifyAffinity
void UDialogueEffect_ModifyAffinity::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through RelationshipComponent instead
        // float Current = Context->GetAffinity(NPCName);
        // Context->SetAffinity(NPCName, Current + DeltaValue);
    }
}

FText UDialogueEffect_ModifyAffinity::GetDisplayText_Implementation() const
{
    FString Sign = DeltaValue >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Affinity[%s] %s%.1f"), 
        *NPCName.ToString(), *Sign, DeltaValue));
}

// v1.3: Reverse implementation
void UDialogueEffect_ModifyAffinity::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // Reverse is just apply negative delta
        // TODO: Implement through RelationshipComponent
        // float Current = Context->GetAffinity(NPCName);
    // Context->SetAffinity(NPCName, Current - DeltaValue);
    }
}

// UDialogueEffect_ModifyTrust
void UDialogueEffect_ModifyTrust::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through RelationshipComponent invece
        // float Current = Context->GetTrust(NPCName);
        // Context->SetTrust(NPCName, Current + DeltaValue);
    }
}

FText UDialogueEffect_ModifyTrust::GetDisplayText_Implementation() const
{
    FString Sign = DeltaValue >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Trust[%s] %s%.1f"), 
        *NPCName.ToString(), *Sign, DeltaValue));
}

// v1.3: Reverse implementation
void UDialogueEffect_ModifyTrust::Reverse_Implementation(UDialogueSessionContext* Context)
{
 if (Context)
    {
        // Reverse is just apply negative delta
   // TODO: Implement through RelationshipComponent
        // float Current = Context->GetTrust(NPCName);
  // Context->SetTrust(NPCName, Current - DeltaValue);
}
}

// UDialogueEffect_ModifyInventory
void UDialogueEffect_ModifyInventory::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through inventory system
        // if (DeltaCount > 0)
        // {
        //     Context->AddItem(ItemId, DeltaCount);
        // }
        // else if (DeltaCount < 0)
        // {
        //     Context->RemoveItem(ItemId, FMath::Abs(DeltaCount));
        // }
    }
}

FText UDialogueEffect_ModifyInventory::GetDisplayText_Implementation() const
{
    if (DeltaCount > 0)
    {
        return FText::FromString(FString::Printf(TEXT("Receive: %s x%d"), *ItemId.ToString(), DeltaCount));
    }
    else
    {
        return FText::FromString(FString::Printf(TEXT("Lose: %s x%d"), *ItemId.ToString(), FMath::Abs(DeltaCount)));
    }
}

// v1.3: Reverse implementation
void UDialogueEffect_ModifyInventory::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // Reverse inventory change: negate delta
     // TODO: Implement through inventory system
      // int32 ReverseDelta = -DeltaCount;
        // if (ReverseDelta > 0)
        // {
        //  Context->AddItem(ItemId, ReverseDelta);
        // }
        // else if (ReverseDelta < 0)
        // {
        //     Context->RemoveItem(ItemId, FMath::Abs(ReverseDelta));
   // }
    }
}

// UDialogueEffect_ModifyGold
void UDialogueEffect_ModifyGold::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through economy system
        // int32 Current = Context->GetGold();
        // Context->SetGold(Current + DeltaGold);
    }
}

FText UDialogueEffect_ModifyGold::GetDisplayText_Implementation() const
{
    FString Sign = DeltaGold >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Gold %s%d"), *Sign, DeltaGold));
}

// v1.3: Reverse implementation
void UDialogueEffect_ModifyGold::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // Reverse gold change: negate delta
    // TODO: Implement through economy system
        // int32 Current = Context->GetGold();
        // Context->SetGold(Current - DeltaGold);
    }
}

// UDialogueEffect_SetMemory
void UDialogueEffect_SetMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // v1.3: Store old value for reversal
 FString OldValueStr = Context->GetCustomVariable(MemoryKey);
        bHadOldValue = !OldValueStr.IsEmpty();
     if (bHadOldValue)
        {
            OldValue = (OldValueStr == TEXT("true"));
      }
        
        // Set new value
        Context->SetCustomVariable(MemoryKey, Value ? TEXT("true") : TEXT("false"));
    }
}

FText UDialogueEffect_SetMemory::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Set Memory[%s] = %s"), 
        *MemoryKey.ToString(), Value ? TEXT("true") : TEXT("false")));
}

// v1.3: Reverse implementation
void UDialogueEffect_SetMemory::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context && bHadOldValue)
    {
        // Restore old value
    Context->SetCustomVariable(MemoryKey, OldValue ? TEXT("true") : TEXT("false"));
    }
}

// UDialogueEffect_AddWorldStateTag
void UDialogueEffect_AddWorldStateTag::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        Context->AddTag(TagToAdd);
    }
}

FText UDialogueEffect_AddWorldStateTag::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Add Tag: %s"), *TagToAdd.ToString()));
}

// v1.3: Reverse implementation
void UDialogueEffect_AddWorldStateTag::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
  // Reverse of Add is Remove
 Context->RemoveTag(TagToAdd);
    }
}

// UDialogueEffect_RemoveWorldStateTag
void UDialogueEffect_RemoveWorldStateTag::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        Context->RemoveTag(TagToRemove);
    }
}

FText UDialogueEffect_RemoveWorldStateTag::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Remove Tag: %s"), *TagToRemove.ToString()));
}

// v1.3: Reverse implementation
void UDialogueEffect_RemoveWorldStateTag::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // Reverse of Remove is Add
        Context->AddTag(TagToRemove);
    }
}

// UDialogueEffect_StartQuest
void UDialogueEffect_StartQuest::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through quest system
        // Context->StartQuest(QuestId);
    }
}

FText UDialogueEffect_StartQuest::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Start Quest: %s"), *QuestId.ToString()));
}

// UDialogueEffect_CompleteQuest
void UDialogueEffect_CompleteQuest::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through quest system
        // Context->CompleteQuest(QuestId);
    }
}

FText UDialogueEffect_CompleteQuest::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Complete Quest: %s"), *QuestId.ToString()));
}

// UDialogueEffect_SetVariable
void UDialogueEffect_SetVariable::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
    // v1.3: Store old value for reversal
        OldValue = Context->GetCustomVariable(VariableKey);
     
        // Set new value
    Context->SetCustomVariable(VariableKey, Value);
    }
}

FText UDialogueEffect_SetVariable::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Set Variable[%s] = %s"), 
        *VariableKey.ToString(), *Value));
}

// v1.3: Reverse implementation
void UDialogueEffect_SetVariable::Reverse_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
 // Restore old value
  Context->SetCustomVariable(VariableKey, OldValue);
    }
}

// UDialogueEffect_Composite
void UDialogueEffect_Composite::Execute_Implementation(UDialogueSessionContext* Context)
{
    for (const TObjectPtr<UDialogueEffect>& Effect : Effects)
{
        if (Effect)
   {
            Effect->Execute(Context);
     }
    }
}

// v1.3: Constructor
UDialogueEffect_Composite::UDialogueEffect_Composite()
{
    // Composite supports reverse if ALL sub-effects support reverse
    bSupportsReverse = true; // Will be checked dynamically in SupportsReverse()
}

// v1.3: Reverse implementation
void UDialogueEffect_Composite::Reverse_Implementation(UDialogueSessionContext* Context)
{
    // Reverse in REVERSE order
for (int32 i = Effects.Num() - 1; i >= 0; --i)
    {
        if (Effects[i] && Effects[i]->SupportsReverse())
  {
        Effects[i]->Reverse(Context);
   }
 }
}

// v1.3: Check if all sub-effects support reverse
bool UDialogueEffect_Composite::SupportsReverse() const
{
    // Check if all sub-effects support reverse
    for (const TObjectPtr<UDialogueEffect>& Effect : Effects)
    {
     if (Effect && !Effect->SupportsReverse())
        {
  return false;
        }
    }
    return true;
}

FText UDialogueEffect_Composite::GetDisplayText_Implementation() const
{
    FString Result = TEXT("Multiple Effects: ");
    for (int32 i = 0; i < Effects.Num() && i < 3; ++i)
    {
        if (i > 0) Result += TEXT(", ");
        Result += Effects[i] ? Effects[i]->GetDisplayText().ToString() : TEXT("null");
    }
  if (Effects.Num() > 3)
    {
        Result += FString::Printf(TEXT(" (+%d more)"), Effects.Num() - 3);
    }
    return FText::FromString(Result);
}


void UDialogueEffectExecutor::ExecuteEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context)
{
    for (UDialogueEffect* Effect : Effects)
    {
        ExecuteEffect(Effect, Context);
    }
}

void UDialogueEffectExecutor::ExecuteEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context)
{
    if (Effect && Context)
    {
        Effect->Execute(Context);
    }
}

//==============================================================================
// v1.3: Effect Reversal Methods
//==============================================================================

void UDialogueEffectExecutor::ReverseEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context)
{
    if (Effect && Context && Effect->SupportsReverse())
    {
        Effect->Reverse(Context);
    }
}

void UDialogueEffectExecutor::ReverseEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context)
{
    // Reverse in reverse order
    for (int32 i = Effects.Num() - 1; i >= 0; --i)
    {
        ReverseEffect(Effects[i], Context);
    }
}

bool UDialogueEffectExecutor::CanReverseEffect(UDialogueEffect* Effect) const
{
    return Effect && Effect->SupportsReverse();
}
