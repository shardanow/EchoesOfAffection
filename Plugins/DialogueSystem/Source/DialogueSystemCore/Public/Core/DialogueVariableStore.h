// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "DialogueVariant.h"
#include "DialogueVariableStore.generated.h"

/**
 * Dialogue Variable Storage
 * 
 * Handles:
 * - Type-safe variables (FDialogueVariant)
 * - Legacy string variables (backward compatibility)
 * - Gameplay tags
 * 
 * v1.3.1: Extracted from DialogueContext (SRP principle)
 * 
 * Benefits:
 * - Single Responsibility: Only data storage
 * - Type safety through FDialogueVariant
 * - Backward compatible with string-based storage
 * - Easy to serialize/deserialize
 * - Reusable in different contexts
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueVariableStore : public UObject
{
    GENERATED_BODY()

public:
  //==========================================================================
    // Type-Safe Variable API (v1.3.1)
    //==========================================================================

    /** Set variable (type-safe) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantBool(FName Key, bool Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantInt(FName Key, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantFloat(FName Key, float Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantString(FName Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantName(FName Key, FName Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantObject(FName Key, UObject* Value);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetVariantTag(FName Key, FGameplayTag Value);

    /** Get variable (type-safe with defaults) */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    bool GetVariantBool(FName Key, bool DefaultValue = false) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    int32 GetVariantInt(FName Key, int32 DefaultValue = 0) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    float GetVariantFloat(FName Key, float DefaultValue = 0.0f) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    FString GetVariantString(FName Key, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    FName GetVariantName(FName Key, FName DefaultValue = NAME_None) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    UObject* GetVariantObject(FName Key, UObject* DefaultValue = nullptr) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    FGameplayTag GetVariantTag(FName Key, const FGameplayTag& DefaultValue = FGameplayTag()) const;

    /** Check if variable exists */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    bool HasVariable(FName Key) const;

    /** Remove variable */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void RemoveVariable(FName Key);

    /** Get all variable keys */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    TArray<FName> GetAllVariableKeys() const;

/** Clear all variables */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void ClearVariables();

    //==========================================================================
    // Legacy String-Based API (Backward Compatibility)
    //==========================================================================

    /** Get variable as string (legacy) */
UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    FString GetCustomVariable(FName Key, const FString& DefaultValue = TEXT("")) const;

    /** Set variable as string (legacy) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void SetCustomVariable(FName Key, const FString& Value);

    //==========================================================================
    // Gameplay Tags Management
    //==========================================================================

    /** Add gameplay tag */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void AddTag(FGameplayTag Tag);

    /** Remove gameplay tag */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void RemoveTag(FGameplayTag Tag);

 /** Check if tag exists */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    bool HasTag(FGameplayTag Tag) const;

    /** Get all active tags */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    const FGameplayTagContainer& GetActiveTags() const { return ActiveTags; }

    /** Clear all tags */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void ClearTags();

    //==========================================================================
    // State Management
    //==========================================================================

    /** Reset all data */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Variables")
    void Reset();

    /** Check if store is empty */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    bool IsEmpty() const;

    /** Get memory usage estimate in bytes */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Variables")
    int32 GetMemoryUsage() const;

protected:
    /** Type-safe variable storage (v1.3.1) */
    UPROPERTY(SaveGame)
    TMap<FName, FDialogueVariant> TypedVariables;

    /** Legacy string-based storage (backward compatibility) */
    UPROPERTY(SaveGame)
 TMap<FName, FString> CustomVariables;

    /** Active gameplay tags */
    UPROPERTY(SaveGame)
    FGameplayTagContainer ActiveTags;

    /** Helper: Sync variant to legacy for backward compatibility */
    void SyncVariantToLegacy(FName Key, const FDialogueVariant& Value);

    friend class UDialogueSessionContext;
};
