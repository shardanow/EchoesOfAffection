// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IDialogueService.h"
#include "Engine/StreamableManager.h"
#include "DialogueSubsystem.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueDataAsset;
class UDialogueSessionContext;
class UDialogueSaveGame;
class UDialogueRunnerPool;
struct FDialogueSessionSaveData;

/**
 * Delegate called when async dialogue loading completes
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDialogueLoadedDelegate, UDialogueDataAsset*, LoadedDialogue, bool, bSuccess);

/**
 * Global delegates for any dialogue events in the game
 * Can be used by Player, UI, or any other system to track dialogue state
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAnyDialogueStartedSignature, UDialogueRunner*, Runner, AActor*, Player, AActor*, NPC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnyDialogueEndedSignature, AActor*, Player, AActor*, NPC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAnyDialogueNodeChangedSignature, UDialogueNode*, NewNode, UDialogueNode*, PreviousNode, UDialogueRunner*, Runner);

/**
 * Centralized subsystem for dialogue management
 * 
 * Features:
 * - Object pooling for DialogueRunners
 * - Async asset loading via AssetManager
 * - Automatic save/load functionality
 * - Settings management
 * 
 * Implements IDialogueService for dependency inversion
 */
UCLASS(Config=Game)
class DIALOGUESYSTEMRUNTIME_API UDialogueSubsystem : public UGameInstanceSubsystem, public IDialogueService
{
    GENERATED_BODY()

public:
    //~ Begin Global Events

    /** Called when ANY dialogue starts in the game (globally) */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnAnyDialogueStartedSignature OnAnyDialogueStarted;

    /** Called BEFORE dialogue starts - allows UI setup */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
 FOnAnyDialogueStartedSignature OnDialogueAboutToStart;

    /** Called when ANY dialogue ends in the game (globally) */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
 FOnAnyDialogueEndedSignature OnAnyDialogueEnded;

    /** Called when ANY dialogue node changes in the game (globally) */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnAnyDialogueNodeChangedSignature OnAnyDialogueNodeChanged;

    //~ End Global Events

    //~ Begin USubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

    //~ Begin IDialogueService Interface
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);
    virtual UDialogueRunner* StartDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* ResumeDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC, const FDialogueSessionSaveData& SaveData);
    virtual UDialogueRunner* ResumeDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC, const FDialogueSessionSaveData& SaveData) override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    void EndActiveDialogue();
    virtual void EndActiveDialogue_Implementation() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    bool IsDialogueActive() const;
    virtual bool IsDialogueActive_Implementation() const override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* GetActiveDialogue() const;
    virtual UDialogueRunner* GetActiveDialogue_Implementation() const override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueDataAsset* LoadDialogue(FName DialogueId);
    virtual UDialogueDataAsset* LoadDialogue_Implementation(FName DialogueId) override;
    //~ End IDialogueService Interface

protected:
    //~ Begin Active Dialogues
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> ActiveDialogue;

  /** Current dialogue participants */
  UPROPERTY(Transient)
    TObjectPtr<AActor> CurrentPlayer;

    UPROPERTY(Transient)
    TObjectPtr<AActor> CurrentNPC;

    /** Previous node for tracking changes */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> PreviousNode;

    UPROPERTY(Transient)
    TArray<FName> DialogueHistory;
    //~ End Active Dialogues

    //~ Begin Cache & Asset Registry
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueDataAsset>> LoadedDialogues;
    
    /** Registry mapping DialogueId -> Asset Path for async loading */
    UPROPERTY(Transient)
    TMap<FName, FSoftObjectPath> DialogueAssetRegistry;
    //~ End Cache & Asset Registry

    //~ Begin Async Loading
    /** Info about an async load operation */
    struct FAsyncLoadInfo
    {
        FName DialogueId;
        FSoftObjectPath AssetPath;
        TSharedPtr<FStreamableHandle> StreamableHandle;
        TArray<FOnDialogueLoadedDelegate> Callbacks;
    };
    
    /** Currently pending async loads */
    TMap<FName, FAsyncLoadInfo> PendingAsyncLoads;
    //~ End Async Loading

    //~ Begin Pooling
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunnerPool> RunnerPool;

    UPROPERTY(Config)
    bool bUseObjectPooling = true;

    UPROPERTY(Config)
    int32 InitialPoolSize = 3;

    UPROPERTY(Config)
    int32 MaxPoolSize = 10;
    //~ End Pooling

    //~ Begin Settings
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float TypewriterSpeed = 30.0f;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bAutoAdvanceEnabled = false;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float AutoAdvanceDelay = 3.0f;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bShowHistory = true;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    int32 MaxHistoryLines = 50;

    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bAutoSaveEnabled = true;
    //~ End Settings

    //~ Begin SaveGame
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSaveGame> CurrentSaveGame;
    //~ End SaveGame

public:
    //~ Begin Async Loading API
    /**
     * Asynchronously loads a dialogue asset
     * @param DialogueId - ID of the dialogue to load
     * @param OnLoaded - Callback when loading completes (success or failure)
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
    void LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded);
    
    /**
     * Registers a dialogue asset path for async loading
     * Must be called before attempting to load the dialogue
     * @param DialogueId - ID to register
     * @param AssetPath - Soft reference to the dialogue asset
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
    void RegisterDialogueAsset(FName DialogueId, const FSoftObjectPath& AssetPath);
    
    /**
     * Unregisters a dialogue asset
     * @param DialogueId - ID to unregister
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
    void UnregisterDialogueAsset(FName DialogueId);
    
    /**
     * Cancels a pending async load
     * @param DialogueId - ID of the dialogue load to cancel
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
    void CancelAsyncLoad(FName DialogueId);
    
    /**
     * Checks if a dialogue is currently being loaded
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
    bool IsAsyncLoadPending(FName DialogueId) const;
    
    /**
     * Gets the loading progress of an async load (0.0 to 1.0)
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
    float GetAsyncLoadProgress(FName DialogueId) const;
    //~ End Async Loading API

    //~ Begin Public API (convenience methods)
    /**
     * Preloads multiple dialogues asynchronously (batch loading)
     * More efficient than loading individually
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void PreloadDialogues(const TArray<FName>& DialogueIds);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void ClearCache();

    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    const TArray<FName>& GetDialogueHistory() const { return DialogueHistory; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    FString SaveStateToJson() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    bool LoadStateFromJson(const FString& Json);
    //~ End Public API

    //~ Begin Settings API
    UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
    float GetTypewriterSpeed() const { return TypewriterSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
    void SetTypewriterSpeed(float NewSpeed) { TypewriterSpeed = FMath::Max(1.0f, NewSpeed); }

    UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
    bool IsAutoAdvanceEnabled() const { return bAutoAdvanceEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
    void SetAutoAdvanceEnabled(bool bEnabled) { bAutoAdvanceEnabled = bEnabled; }
    //~ End Settings API

    //~ Begin SaveGame API
    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    bool SaveDialogueState();

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    bool LoadDialogueState();

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool HasActiveSaveState() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    FDialogueSessionSaveData GetActiveSaveState() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void ClearActiveSaveState();

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    bool WasDialogueCompleted(FName DialogueId) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
    void MarkDialogueCompleted(FName DialogueId);

    UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
    int32 GetNodeVisitCount(FName DialogueId, FName NodeId) const;
    //~ End SaveGame API

    //~ Begin Performance API
    UFUNCTION(BlueprintPure, Category = "Dialogue|Performance")
    void GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
    void PreWarmRunnerPool(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
    void TrimRunnerPool();
    //~ End Performance API

protected:
    UDialogueRunner* AcquireRunner();
    void ReleaseRunner(UDialogueRunner* Runner);
    void SaveActiveDialogueState();
    bool LoadSaveGame();
    
    /** Called when an async load completes */
    void OnAsyncLoadCompleted(FName DialogueId);
    
    /** Cancels all pending async loads */
    void CancelPendingAsyncLoads();

    /**
  * Extract NPC ID from actor using QuestActorComponent or fallback methods.
     * Priority:
     * 1. QuestActorComponent (if present)
     * 2. GameplayTags (e.g., "NPC.Lianne" -> "Lianne")
     * 3. Actor name (e.g., "BP_Lianne_C_1" -> "Lianne")
     */
    FName ExtractNpcId(AActor* NPC);

    UFUNCTION()
    void HandleDialogueEnded();

    /** Handles node changes from active runner */
  UFUNCTION()
 void HandleNodeEntered(UDialogueNode* NewNode);

    /** Handles choice selection from active runner */
    UFUNCTION()
    void HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode);
};
