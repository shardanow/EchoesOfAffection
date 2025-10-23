// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueVariableStore.h"

//==============================================================================
// Type-Safe Variable API
//==============================================================================

void UDialogueVariableStore::SetVariantBool(FName Key, bool Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeBool(Value);
    TypedVariables.Add(Key, Variant);
    SyncVariantToLegacy(Key, Variant);
}

void UDialogueVariableStore::SetVariantInt(FName Key, int32 Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeInt(Value);
    TypedVariables.Add(Key, Variant);
    SyncVariantToLegacy(Key, Variant);
}

void UDialogueVariableStore::SetVariantFloat(FName Key, float Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeFloat(Value);
    TypedVariables.Add(Key, Variant);
    SyncVariantToLegacy(Key, Variant);
}

void UDialogueVariableStore::SetVariantString(FName Key, const FString& Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeString(Value);
    TypedVariables.Add(Key, Variant);
    CustomVariables.Add(Key, Value); // String doesn't need prefix
}

void UDialogueVariableStore::SetVariantName(FName Key, FName Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeName(Value);
    TypedVariables.Add(Key, Variant);
 SyncVariantToLegacy(Key, Variant);
}

void UDialogueVariableStore::SetVariantObject(FName Key, UObject* Value)
{
  FDialogueVariant Variant = FDialogueVariant::MakeObject(Value);
    TypedVariables.Add(Key, Variant);
    SyncVariantToLegacy(Key, Variant);
}

void UDialogueVariableStore::SetVariantTag(FName Key, FGameplayTag Value)
{
    FDialogueVariant Variant = FDialogueVariant::MakeTag(Value);
    TypedVariables.Add(Key, Variant);
    SyncVariantToLegacy(Key, Variant);
}

bool UDialogueVariableStore::GetVariantBool(FName Key, bool DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetBool(DefaultValue);
    }
    
    // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
 {
        return LegacyValue->ToBool();
    }
    
    return DefaultValue;
}

int32 UDialogueVariableStore::GetVariantInt(FName Key, int32 DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetInt(DefaultValue);
    }
  
    // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
    {
        return FCString::Atoi(**LegacyValue);
    }
    
    return DefaultValue;
}

float UDialogueVariableStore::GetVariantFloat(FName Key, float DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetFloat(DefaultValue);
    }
    
    // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
    {
        return FCString::Atof(**LegacyValue);
    }
    
    return DefaultValue;
}

FString UDialogueVariableStore::GetVariantString(FName Key, const FString& DefaultValue) const
{
 if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetString(DefaultValue);
    }
    
  // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
    {
 return *LegacyValue;
    }
    
    return DefaultValue;
}

FName UDialogueVariableStore::GetVariantName(FName Key, FName DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetName(DefaultValue);
    }
    
 // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
    {
        return FName(**LegacyValue);
    }
    
    return DefaultValue;
}

UObject* UDialogueVariableStore::GetVariantObject(FName Key, UObject* DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
      return Variant->GetObject(DefaultValue);
    }
    
  return DefaultValue;
}

FGameplayTag UDialogueVariableStore::GetVariantTag(FName Key, const FGameplayTag& DefaultValue) const
{
    if (const FDialogueVariant* Variant = TypedVariables.Find(Key))
    {
        return Variant->GetTag(DefaultValue);
    }
    
    // Fallback to legacy
    if (const FString* LegacyValue = CustomVariables.Find(Key))
    {
      return FGameplayTag::RequestGameplayTag(FName(**LegacyValue), false);
    }
    
return DefaultValue;
}

bool UDialogueVariableStore::HasVariable(FName Key) const
{
    return TypedVariables.Contains(Key) || CustomVariables.Contains(Key);
}

void UDialogueVariableStore::RemoveVariable(FName Key)
{
    TypedVariables.Remove(Key);
    CustomVariables.Remove(Key);
}

TArray<FName> UDialogueVariableStore::GetAllVariableKeys() const
{
    TArray<FName> Keys;
    TypedVariables.GetKeys(Keys);
    
    // Add legacy keys that aren't in typed
    TArray<FName> LegacyKeys;
    CustomVariables.GetKeys(LegacyKeys);
    for (FName Key : LegacyKeys)
    {
        Keys.AddUnique(Key);
    }
    
    return Keys;
}

void UDialogueVariableStore::ClearVariables()
{
    TypedVariables.Empty();
    CustomVariables.Empty();
}

//==============================================================================
// Legacy String-Based API
//==============================================================================

FString UDialogueVariableStore::GetCustomVariable(FName Key, const FString& DefaultValue) const
{
    return GetVariantString(Key, DefaultValue);
}

void UDialogueVariableStore::SetCustomVariable(FName Key, const FString& Value)
{
    SetVariantString(Key, Value);
}

//==============================================================================
// Gameplay Tags Management
//==============================================================================

void UDialogueVariableStore::AddTag(FGameplayTag Tag)
{
    ActiveTags.AddTag(Tag);
}

void UDialogueVariableStore::RemoveTag(FGameplayTag Tag)
{
    ActiveTags.RemoveTag(Tag);
}

bool UDialogueVariableStore::HasTag(FGameplayTag Tag) const
{
    return ActiveTags.HasTag(Tag);
}

void UDialogueVariableStore::ClearTags()
{
    ActiveTags.Reset();
}

//==============================================================================
// State Management
//==============================================================================

void UDialogueVariableStore::Reset()
{
    TypedVariables.Empty();
    CustomVariables.Empty();
    ActiveTags.Reset();
}

bool UDialogueVariableStore::IsEmpty() const
{
    return TypedVariables.Num() == 0 && 
           CustomVariables.Num() == 0 && 
           ActiveTags.Num() == 0;
}

int32 UDialogueVariableStore::GetMemoryUsage() const
{
    int32 Size = 0;
    
    // TypedVariables
    Size += TypedVariables.Num() * (sizeof(FName) + sizeof(FDialogueVariant));
    
    // CustomVariables
  Size += CustomVariables.Num() * sizeof(FName);
    for (const auto& Pair : CustomVariables)
 {
        Size += Pair.Value.Len() * sizeof(TCHAR);
    }
    
    // ActiveTags
    Size += ActiveTags.Num() * sizeof(FGameplayTag);
    
    return Size;
}

//==============================================================================
// Helper Methods
//==============================================================================

void UDialogueVariableStore::SyncVariantToLegacy(FName Key, const FDialogueVariant& Value)
{
    // Keep CustomVariables in sync for backward compatibility
    CustomVariables.Add(Key, Value.ToString());
}
