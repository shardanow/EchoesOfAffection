// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DialogueVariant.h"
#include "DialogueState.h"
#include "DialogueParticipants.h"
#include "DialogueVariableStore.h"
#include "DialogueContext.generated.h"

// Forward declarations
class AActor;
class UDialogueNode;
struct FDialogueSessionSaveData;
enum class EDialogueState : uint8;

/**
 * NEW v1.3: Snapshot of dialogue state
 * Used for perfect undo/redo support
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueStateSnapshot
{
    GENERATED_BODY()

    /** Current node ID */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    FName CurrentNodeId;
    
    /** Node history IDs */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    TArray<FName> NodeHistoryIds;
    
    /** Custom variables */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    TMap<FName, FString> CustomVariables;
    
    /** Active tags */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    FGameplayTagContainer ActiveTags;
    
    /** Visited nodes */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
  TArray<FName> VisitedNodes;
    
    /** Conversation history */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    TArray<FConversationEntry> ConversationHistory;
    
 /** Base affinity level */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    float BaseAffinityLevel;
    
    /** Timestamp when snapshot was created */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    FDateTime Timestamp;
    
    /** Dialogue state at snapshot time */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
    uint8 DialogueState;

/** Is this snapshot valid? */
    UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
  bool bIsValid;

    FDialogueStateSnapshot()
        : CurrentNodeId(NAME_None)
        , BaseAffinityLevel(0.0f)
        , Timestamp(FDateTime::UtcNow())
 , DialogueState(0)
        , bIsValid(false)
    {}

    /** Check if snapshot is valid */
bool IsValid() const { return bIsValid; }

    /** Get memory size estimate */
    SIZE_T GetMemorySize() const
    {
        SIZE_T Size = sizeof(FDialogueStateSnapshot);
        Size += NodeHistoryIds.Num() * sizeof(FName);
        Size += CustomVariables.Num() * (sizeof(FName) + sizeof(FString));
     Size += VisitedNodes.Num() * sizeof(FName);
     Size += ConversationHistory.Num() * sizeof(FConversationEntry);
 return Size;
    }
};

/**
 * 
 * v1.3.1: Refactored to use Composition Pattern
 * 
 * Now composed of:
 * - UDialogueState - manages dialogue flow state
 * - UDialogueParticipants - manages actors involved
 * - UDialogueVariableStore - manages data storage
 * 
 * - Player/NPC references
 * - Relationship data
 * - Quest state
 * - Inventory
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueSessionContext : public UObject
{
    GENERATED_BODY()

public:
    //==========================================================================
    // v1.3.1: Component Access (Facade Pattern)
    //==========================================================================

    /** Get state component */
    UFUNCTION(BlueprintPure, Category = "Context|Components")
    UDialogueState* GetState() const { return State; }

    /** Get participants component */
    UFUNCTION(BlueprintPure, Category = "Context|Components")
    UDialogueParticipants* GetParticipants() const { return Participants; }

    /** Get variable store component */
    UFUNCTION(BlueprintPure, Category = "Context|Components")
    UDialogueVariableStore* GetVariables() const { return Variables; }

    /** Initialize components (call this after construction) */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void Initialize();

    /** NEW v1.13: Set owning runner (internal use only) */
  void SetOwningRunner(UObject* Runner) { OwningRunner = Runner; }

    /** NEW v1.13: Get owning runner (internal use only) */
    UObject* GetOwningRunner() const { return OwningRunner.Get(); }

protected:
    //==========================================================================
    // v1.3.1: Component Composition
    //==========================================================================

    /** Dialogue state component */
    UPROPERTY()
    TObjectPtr<UDialogueState> State;

    /** Participants component */
    UPROPERTY()
    TObjectPtr<UDialogueParticipants> Participants;

    /** Variable store component */
    UPROPERTY()
    TObjectPtr<UDialogueVariableStore> Variables;

    /** NEW v1.13: Weak reference to owning DialogueRunner (for sequence management) */
    UPROPERTY(Transient)
    TWeakObjectPtr<UObject> OwningRunner;

    //==========================================================================
    // DEPRECATED v1.3.1: Legacy fields (kept for backward compatibility)
    // These now delegate to components
    //==========================================================================

    /** DEPRECATED: Use GetParticipants()->GetPlayer() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context", meta = (DeprecationMessage = "Use GetParticipants()->GetPlayer()"))
    TObjectPtr<AActor> Player;

    /** DEPRECATED: Use GetParticipants()->GetNPC() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context", meta = (DeprecationMessage = "Use GetParticipants()->GetNPC()"))
    TObjectPtr<AActor> NPC;

    /** DEPRECATED: Use GetParticipants()->GetBaseAffinityLevel() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context", meta = (DeprecationMessage = "Use GetParticipants()"))
    float BaseAffinityLevel = 0.0f;

    /** DEPRECATED: Use GetState()->GetCurrentNode() */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context", meta = (DeprecationMessage = "Use GetState()"))
TWeakObjectPtr<UDialogueNode> CurrentNode;

    /** DEPRECATED: Use GetState()->GetVisitedNodes() */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context", meta = (DeprecationMessage = "Use GetState()"))
    TArray<FName> VisitedNodes;

    /** DEPRECATED: Use GetState()->GetSessionStartTime() */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context", meta = (DeprecationMessage = "Use GetState()"))
    FDateTime SessionStartTime;

    /** DEPRECATED: Use GetVariables()->GetCustomVariable() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context", meta = (DeprecationMessage = "Use GetVariables()"))
    TMap<FName, FString> CustomVariables;

    /** DEPRECATED: Use GetVariables() type-safe API */
    UPROPERTY(SaveGame)
    TMap<FName, FDialogueVariant> TypedVariables;

    /** DEPRECATED: Use GetVariables()->GetActiveTags() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context", meta = (DeprecationMessage = "Use GetVariables()"))
  FGameplayTagContainer ActiveTags;

public:
    //~ Begin UObject Interface
    virtual void BeginDestroy() override;
    //~ End UObject Interface

    //==========================================================================
    // v1.3.1: Facade Delegates to Components (Backward Compatibility)
    //==========================================================================

    // State delegates
    UFUNCTION(BlueprintPure, Category = "Context")
    UDialogueNode* GetCurrentNode() const;

  UFUNCTION(BlueprintCallable, Category = "Context")
    void SetCurrentNode(UDialogueNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Context")
    void MarkNodeVisited(FName NodeId);

    UFUNCTION(BlueprintPure, Category = "Context")
    bool WasNodeVisited(FName NodeId) const;

    UFUNCTION(BlueprintCallable, Category = "Context")
    void AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId);

    // Participant delegates (keep legacy direct access for compatibility)
    UFUNCTION(BlueprintPure, Category = "Context")
    AActor* GetPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Context")
    AActor* GetNPC() const;

    UFUNCTION(BlueprintCallable, Category = "Context")
    void SetPlayer(AActor* InPlayer);

    UFUNCTION(BlueprintCallable, Category = "Context")
    void SetNPC(AActor* InNPC);

    // Variable delegates - Legacy String API
    UFUNCTION(BlueprintPure, Category = "Context")
    FString GetCustomVariable(FName VariableName, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintCallable, Category = "Context")
    void SetCustomVariable(FName VariableName, const FString& Value);

    // Tag delegates
    UFUNCTION(BlueprintCallable, Category = "Context")
    void AddTag(FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "Context")
    void RemoveTag(FGameplayTag Tag);

    UFUNCTION(BlueprintPure, Category = "Context")
    bool HasTag(FGameplayTag Tag) const;

 //==========================================================================
    // v1.3.1: Type-Safe Variable API (Delegates to VariableStore)
    //==========================================================================

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantBool(FName VariableName, bool Value);

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantInt(FName VariableName, int32 Value);

 UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantFloat(FName VariableName, float Value);

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantString(FName VariableName, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantName(FName VariableName, FName Value);

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
  void SetVariantObject(FName VariableName, UObject* Value);

    UFUNCTION(BlueprintCallable, Category = "Context|Variables")
    void SetVariantTag(FName VariableName, FGameplayTag Value);

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    bool GetVariantBool(FName VariableName, bool DefaultValue = false) const;

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    int32 GetVariantInt(FName VariableName, int32 DefaultValue = 0) const;

 UFUNCTION(BlueprintPure, Category = "Context|Variables")
    float GetVariantFloat(FName VariableName, float DefaultValue = 0.0f) const;

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    FString GetVariantString(FName VariableName, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    FName GetVariantName(FName VariableName, FName DefaultValue = NAME_None) const;

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    UObject* GetVariantObject(FName VariableName, UObject* DefaultValue = nullptr) const;

    UFUNCTION(BlueprintPure, Category = "Context|Variables")
    FGameplayTag GetVariantTag(FName VariableName, const FGameplayTag& DefaultValue = FGameplayTag()) const;

    //==========================================================================
    // v1.3: Snapshot API
    //==========================================================================

    UFUNCTION(BlueprintCallable, Category = "Context|Snapshot")
    FDialogueStateSnapshot CreateSnapshot(uint8 DialogueState, const TArray<UDialogueNode*>& NodeHistory) const;

    UFUNCTION(BlueprintCallable, Category = "Context|Snapshot")
  bool RestoreFromSnapshot(const FDialogueStateSnapshot& Snapshot);

    UFUNCTION(BlueprintCallable, Category = "Context")
    void ResetState();

    //==========================================================================
    // Save/Load API
    //==========================================================================

    UFUNCTION(BlueprintCallable, Category = "Context|SaveGame")
    FDialogueSessionSaveData ToSaveData(FName DialogueId) const;

    UFUNCTION(BlueprintCallable, Category = "Context|SaveGame")
    bool FromSaveData(const FDialogueSessionSaveData& SaveData, AActor* InPlayer, AActor* InNPC);

    UFUNCTION(BlueprintCallable, Category = "Context")
    void SaveToJson(FString& OutJson) const;

  UFUNCTION(BlueprintCallable, Category = "Context")
    bool LoadFromJson(const FString& Json);
};
