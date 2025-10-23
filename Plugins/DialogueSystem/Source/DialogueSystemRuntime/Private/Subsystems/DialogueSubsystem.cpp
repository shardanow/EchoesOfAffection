// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/DialogueSubsystem.h"
#include "Core/DialogueRunner.h"
#include "Data/DialogueDataAsset.h"
#include "Core/DialogueContext.h"
#include "SaveGame/DialogueSaveData.h"
#include "Pool/DialogueObjectPool.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueSubsystem, Log, All);

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize object pool
    if (bUseObjectPooling)
    {
        RunnerPool = NewObject<UDialogueRunnerPool>(this);
        RunnerPool->Initialize(UDialogueRunner::StaticClass(), InitialPoolSize, MaxPoolSize);
        
        UE_LOG(LogDialogueSubsystem, Log, TEXT("DialogueRunner pool initialized: Initial=%d, Max=%d"), 
            InitialPoolSize, MaxPoolSize);
    }
    
    // Load SaveGame on initialization
    LoadSaveGame();
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("DialogueSubsystem Initialized"));
}

void UDialogueSubsystem::Deinitialize()
{
    // Auto-save on shutdown
    if (bAutoSaveEnabled)
    {
        SaveDialogueState();
    }
    
    // End any active dialogue
    if (ActiveDialogue)
    {
        EndActiveDialogue_Implementation();
    }
    
    // Cancel any pending async loads
    CancelPendingAsyncLoads();
    
    // Clear cache
    ClearCache();
    
    // Clean up pool
    if (RunnerPool)
    {
        RunnerPool->Clear();
        RunnerPool = nullptr;
    }
    
    Super::Deinitialize();
}

UDialogueRunner* UDialogueSubsystem::StartDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC)
{
    if (!DialogueAsset || !Player || !NPC)
    {
        UE_LOG(LogDialogueSubsystem, Warning, TEXT("StartDialogue - Invalid parameters"));
        return nullptr;
    }
    
    // End previous dialogue if any
    if (ActiveDialogue)
    {
        EndActiveDialogue_Implementation();
    }
    
    // Acquire runner from pool or create new
    ActiveDialogue = AcquireRunner();
    if (!ActiveDialogue)
    {
        UE_LOG(LogDialogueSubsystem, Error, TEXT("StartDialogue - Failed to acquire DialogueRunner"));
        return nullptr;
    }
    
    // Bind to dialogue ended event
    ActiveDialogue->OnDialogueEnded.AddDynamic(this, &UDialogueSubsystem::HandleDialogueEnded);
    
    // Bind to node entered event for global broadcasting
    ActiveDialogue->OnNodeEntered.AddDynamic(this, &UDialogueSubsystem::HandleNodeEntered);

    // Store participants for event broadcasting
    CurrentPlayer = Player;
    CurrentNPC = NPC;
    PreviousNode = nullptr;

    // Prepare participants array
    TArray<UObject*> Participants;
    Participants.Add(Player);
    Participants.Add(NPC);
    
    // Start dialogue
    ActiveDialogue->StartDialogue(DialogueAsset, Participants);
    
    // Add to history
    DialogueHistory.Add(DialogueAsset->DialogueId);
    
    // Increment visit count in SaveGame
    if (CurrentSaveGame)
    {
        CurrentSaveGame->IncrementNodeVisit(DialogueAsset->DialogueId, DialogueAsset->StartNode);
    }
    
    // Broadcast global event
    OnAnyDialogueStarted.Broadcast(ActiveDialogue, Player, NPC);
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Started dialogue '%s'"), *DialogueAsset->DialogueId.ToString());
    
    return ActiveDialogue;
}

UDialogueRunner* UDialogueSubsystem::ResumeDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC, const FDialogueSessionSaveData& SaveData)
{
    if (!DialogueAsset || !Player || !NPC || !SaveData.IsValid())
    {
        UE_LOG(LogDialogueSubsystem, Warning, TEXT("ResumeDialogue - Invalid parameters"));
        return nullptr;
    }
    
    // End previous dialogue if any
    if (ActiveDialogue)
    {
        EndActiveDialogue_Implementation();
    }
    
    // Acquire runner from pool
    ActiveDialogue = AcquireRunner();
    if (!ActiveDialogue)
    {
        UE_LOG(LogDialogueSubsystem, Error, TEXT("ResumeDialogue - Failed to acquire DialogueRunner"));
        return nullptr;
    }
    
    // Bind to dialogue ended event
    ActiveDialogue->OnDialogueEnded.AddDynamic(this, &UDialogueSubsystem::HandleDialogueEnded);
    
    // Prepare participants array
    TArray<UObject*> Participants;
    Participants.Add(Player);
    Participants.Add(NPC);
    
    // Start dialogue
    ActiveDialogue->StartDialogue(DialogueAsset, Participants);
    
    // Restore context from save data
    if (UDialogueSessionContext* Context = ActiveDialogue->GetContext())
    {
        Context->FromSaveData(SaveData, Player, NPC);
        
        // Resume from saved node
        if (!SaveData.CurrentNodeId.IsNone())
        {
            if (UDialogueNode* SavedNode = DialogueAsset->FindNode(SaveData.CurrentNodeId))
            {
                ActiveDialogue->JumpToNode(SavedNode);
            }
        }
    }
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Resumed dialogue '%s' from save"), *DialogueAsset->DialogueId.ToString());
    
    return ActiveDialogue;
}

void UDialogueSubsystem::EndActiveDialogue_Implementation()
{
    if (!ActiveDialogue)
    {
        return;
    }
    
    // Save state before ending
    if (bAutoSaveEnabled)
    {
        SaveActiveDialogueState();
    }
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Ending active dialogue"));
    
    // End dialogue
    ActiveDialogue->EndDialogue();
    
    // Return to pool instead of destroying
    ReleaseRunner(ActiveDialogue);
    ActiveDialogue = nullptr;
}

bool UDialogueSubsystem::IsDialogueActive_Implementation() const
{
    return ActiveDialogue != nullptr && ActiveDialogue->IsActive();
}

UDialogueRunner* UDialogueSubsystem::GetActiveDialogue_Implementation() const
{
    return ActiveDialogue;
}

UDialogueDataAsset* UDialogueSubsystem::LoadDialogue_Implementation(FName DialogueId)
{
    // Check cache first
    if (TObjectPtr<UDialogueDataAsset>* CachedAsset = LoadedDialogues.Find(DialogueId))
    {
        return *CachedAsset;
    }
    
    // Check if we have a registered asset path for this DialogueId
    if (const FSoftObjectPath* AssetPath = DialogueAssetRegistry.Find(DialogueId))
    {
        // Synchronous load (blocking)
        UDialogueDataAsset* LoadedAsset = Cast<UDialogueDataAsset>(AssetPath->TryLoad());
        
        if (LoadedAsset)
        {
            // Cache for future use
            LoadedDialogues.Add(DialogueId, LoadedAsset);
            UE_LOG(LogDialogueSubsystem, Log, TEXT("Synchronously loaded dialogue '%s'"), *DialogueId.ToString());
            return LoadedAsset;
        }
        else
        {
            UE_LOG(LogDialogueSubsystem, Error, TEXT("Failed to load dialogue asset '%s' from path '%s'"), 
                *DialogueId.ToString(), *AssetPath->ToString());
        }
    }
    else
    {
        UE_LOG(LogDialogueSubsystem, Warning, TEXT("No asset path registered for DialogueId '%s'"), 
            *DialogueId.ToString());
    }
    
    return nullptr;
}

void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // Check cache first
    if (TObjectPtr<UDialogueDataAsset>* CachedAsset = LoadedDialogues.Find(DialogueId))
    {
        // Already loaded, call delegate immediately
        OnLoaded.ExecuteIfBound(*CachedAsset, true);
        return;
    }
    
    // Check if already loading
    if (PendingAsyncLoads.Contains(DialogueId))
    {
        // Add callback to existing load
        PendingAsyncLoads[DialogueId].Callbacks.Add(OnLoaded);
        UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Added callback to existing async load for '%s'"), *DialogueId.ToString());
        return;
    }
    
    // Check if we have a registered asset path
    const FSoftObjectPath* AssetPath = DialogueAssetRegistry.Find(DialogueId);
    if (!AssetPath || !AssetPath->IsValid())
    {
        UE_LOG(LogDialogueSubsystem, Error, TEXT("No valid asset path for DialogueId '%s'"), *DialogueId.ToString());
        OnLoaded.ExecuteIfBound(nullptr, false);
        return;
    }
    
    // Prepare async load info
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
    
    // CRITICAL FIX: Add to map BEFORE starting the async load
    // This prevents race condition where callback fires before we add to map
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
    
    // Get reference to the LoadInfo in the map (important after MoveTemp!)
    FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];
    
    // Start async load with AssetManager
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
    
    // Now it's safe to start the load - even if callback fires immediately,
    // the LoadInfo is already in PendingAsyncLoads
    LoadInfoRef.StreamableHandle = StreamableManager.RequestAsyncLoad(
        *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
        {
            if (UDialogueSubsystem* StrongThis = WeakThis.Get())
            {
                StrongThis->OnAsyncLoadCompleted(DialogueId);
            }
        }),
        FStreamableManager::AsyncLoadHighPriority,
        false, // Don't manage active handle (we'll do it ourselves)
        false, // Don't start stalled
        TEXT("DialogueSubsystem")
    );
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Started async load for dialogue '%s'"), *DialogueId.ToString());
}

void UDialogueSubsystem::PreloadDialogues(const TArray<FName>& DialogueIds)
{
    TArray<FSoftObjectPath> AssetsToLoad;
    
    for (FName DialogueId : DialogueIds)
    {
        // Skip if already loaded
        if (LoadedDialogues.Contains(DialogueId))
        {
            continue;
        }
        
        // Skip if already loading
        if (PendingAsyncLoads.Contains(DialogueId))
        {
            continue;
        }
        
        // Check if registered
        if (const FSoftObjectPath* AssetPath = DialogueAssetRegistry.Find(DialogueId))
        {
            AssetsToLoad.Add(*AssetPath);
        }
    }
    
    if (AssetsToLoad.Num() == 0)
    {
        return;
    }
    
    // Batch load all assets
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
    
    TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
        AssetsToLoad,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueIds]
        {
            if (UDialogueSubsystem* StrongThis = WeakThis.Get())
            {
                for (FName Id : DialogueIds)
                {
                    StrongThis->OnAsyncLoadCompleted(Id);
                }
            }
        }),
        FStreamableManager::AsyncLoadHighPriority,
        false,
        false,
        TEXT("DialogueSubsystemBatch")
    );
    
    // Track batch load
    for (FName DialogueId : DialogueIds)
    {
        if (!LoadedDialogues.Contains(DialogueId) && DialogueAssetRegistry.Contains(DialogueId))
        {
            FAsyncLoadInfo LoadInfo;
            LoadInfo.DialogueId = DialogueId;
            LoadInfo.AssetPath = DialogueAssetRegistry[DialogueId];
            LoadInfo.StreamableHandle = Handle;
            
            PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
        }
    }
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Batch preload started for %d dialogues"), AssetsToLoad.Num());
}

void UDialogueSubsystem::RegisterDialogueAsset(FName DialogueId, const FSoftObjectPath& AssetPath)
{
    if (DialogueId.IsNone() || !AssetPath.IsValid())
    {
        UE_LOG(LogDialogueSubsystem, Warning, TEXT("Cannot register invalid DialogueId or AssetPath"));
        return;
    }
    
    DialogueAssetRegistry.Add(DialogueId, AssetPath);
    UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Registered dialogue '%s' -> '%s'"), 
        *DialogueId.ToString(), *AssetPath.ToString());
}

void UDialogueSubsystem::UnregisterDialogueAsset(FName DialogueId)
{
    DialogueAssetRegistry.Remove(DialogueId);
    
    // Also remove from cache if present
    if (LoadedDialogues.Remove(DialogueId) > 0)
    {
        UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Unregistered and uncached dialogue '%s'"), *DialogueId.ToString());
    }
}

void UDialogueSubsystem::CancelAsyncLoad(FName DialogueId)
{
    if (FAsyncLoadInfo* LoadInfo = PendingAsyncLoads.Find(DialogueId))
    {
        if (LoadInfo->StreamableHandle.IsValid())
        {
            LoadInfo->StreamableHandle->CancelHandle();
        }
        
        // Notify all waiting callbacks
        for (FOnDialogueLoadedDelegate& Callback : LoadInfo->Callbacks)
        {
            Callback.ExecuteIfBound(nullptr, false);
        }
        
        PendingAsyncLoads.Remove(DialogueId);
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Cancelled async load for '%s'"), *DialogueId.ToString());
    }
}

void UDialogueSubsystem::CancelPendingAsyncLoads()
{
    for (auto& Pair : PendingAsyncLoads)
    {
        if (Pair.Value.StreamableHandle.IsValid())
        {
            Pair.Value.StreamableHandle->CancelHandle();
        }
        
        // Notify all waiting callbacks
        for (FOnDialogueLoadedDelegate& Callback : Pair.Value.Callbacks)
        {
            Callback.ExecuteIfBound(nullptr, false);
        }
    }
    
    PendingAsyncLoads.Empty();
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Cancelled all pending async loads"));
}

bool UDialogueSubsystem::IsAsyncLoadPending(FName DialogueId) const
{
    return PendingAsyncLoads.Contains(DialogueId);
}

float UDialogueSubsystem::GetAsyncLoadProgress(FName DialogueId) const
{
    if (const FAsyncLoadInfo* LoadInfo = PendingAsyncLoads.Find(DialogueId))
    {
        if (LoadInfo->StreamableHandle.IsValid())
        {
            return LoadInfo->StreamableHandle->GetProgress();
        }
    }
    
    return 0.0f;
}

void UDialogueSubsystem::OnAsyncLoadCompleted(FName DialogueId)
{
    FAsyncLoadInfo* LoadInfo = PendingAsyncLoads.Find(DialogueId);
    if (!LoadInfo)
    {
        return;
    }
    
    UDialogueDataAsset* LoadedAsset = nullptr;
    bool bSuccess = false;
    
    // Get the loaded asset
    if (LoadInfo->StreamableHandle.IsValid())
    {
        UObject* LoadedObject = LoadInfo->StreamableHandle->GetLoadedAsset();
        LoadedAsset = Cast<UDialogueDataAsset>(LoadedObject);
        
        if (LoadedAsset)
        {
            // Cache the asset
            LoadedDialogues.Add(DialogueId, LoadedAsset);
            bSuccess = true;
            
            UE_LOG(LogDialogueSubsystem, Log, TEXT("Successfully loaded dialogue '%s'"), *DialogueId.ToString());
        }
        else
        {
            UE_LOG(LogDialogueSubsystem, Error, TEXT("Failed to load dialogue '%s' - asset is null or wrong type"), 
                *DialogueId.ToString());
        }
    }
    
    // Notify all callbacks
    for (FOnDialogueLoadedDelegate& Callback : LoadInfo->Callbacks)
    {
        Callback.ExecuteIfBound(LoadedAsset, bSuccess);
    }
    
    // Clean up
    PendingAsyncLoads.Remove(DialogueId);
}

void UDialogueSubsystem::ClearCache()
{
    LoadedDialogues.Empty();
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Cache cleared"));
}

bool UDialogueSubsystem::SaveDialogueState()
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = UDialogueSaveGame::LoadOrCreateSaveGame(this);
    }
    
    if (!CurrentSaveGame)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueSubsystem: Failed to create SaveGame object"));
        return false;
    }
    
    // Save active dialogue if any
    if (ActiveDialogue && ActiveDialogue->IsActive())
    {
        SaveActiveDialogueState();
    }
    
    // Save to slot
    bool bSuccess = UDialogueSaveGame::SaveDialogueSaveGame(this, CurrentSaveGame);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: State saved successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueSubsystem: Failed to save state"));
    }
    
    return bSuccess;
}

bool UDialogueSubsystem::LoadDialogueState()
{
    return LoadSaveGame();
}

void UDialogueSubsystem::SaveActiveDialogueState()
{
    if (!ActiveDialogue || !ActiveDialogue->IsActive())
    {
        return;
    }
    
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = UDialogueSaveGame::LoadOrCreateSaveGame(this);
    }
    
    if (!CurrentSaveGame)
    {
        return;
    }
    
    UDialogueSessionContext* Context = ActiveDialogue->GetContext();
    UDialogueDataAsset* CurrentAsset = ActiveDialogue->GetLoadedDialogue();
    
    if (!Context || !CurrentAsset)
    {
        return;
    }
    
    // Convert context to save data
    FDialogueSessionSaveData SaveData = Context->ToSaveData(CurrentAsset->DialogueId);
    
    // Save to SaveGame object
    CurrentSaveGame->SaveActiveSession(SaveData);
    
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Active dialogue state cached"));
}

bool UDialogueSubsystem::HasActiveSaveState() const
{
    return CurrentSaveGame && CurrentSaveGame->HasActiveSession();
}

FDialogueSessionSaveData UDialogueSubsystem::GetActiveSaveState() const
{
    if (CurrentSaveGame)
    {
        return CurrentSaveGame->GetActiveSession();
    }
    return FDialogueSessionSaveData();
}

void UDialogueSubsystem::ClearActiveSaveState()
{
    if (CurrentSaveGame)
    {
        CurrentSaveGame->ClearActiveSession();
        UDialogueSaveGame::SaveDialogueSaveGame(this, CurrentSaveGame);
    }
}

bool UDialogueSubsystem::WasDialogueCompleted(FName DialogueId) const
{
    if (CurrentSaveGame)
    {
        return CurrentSaveGame->WasDialogueCompleted(DialogueId);
    }
    return false;
}

void UDialogueSubsystem::MarkDialogueCompleted(FName DialogueId)
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = UDialogueSaveGame::LoadOrCreateSaveGame(this);
    }
    
    if (CurrentSaveGame)
    {
        CurrentSaveGame->MarkDialogueCompleted(DialogueId);
        
        if (bAutoSaveEnabled)
        {
            UDialogueSaveGame::SaveDialogueSaveGame(this, CurrentSaveGame);
        }
    }
}

int32 UDialogueSubsystem::GetNodeVisitCount(FName DialogueId, FName NodeId) const
{
    if (CurrentSaveGame)
    {
        return CurrentSaveGame->GetNodeVisitCount(DialogueId, NodeId);
    }
    return 0;
}

FString UDialogueSubsystem::SaveStateToJson() const
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    
    // Save settings
    RootObject->SetNumberField(TEXT("TypewriterSpeed"), TypewriterSpeed);
    RootObject->SetBoolField(TEXT("AutoAdvanceEnabled"), bAutoAdvanceEnabled);
    RootObject->SetNumberField(TEXT("AutoAdvanceDelay"), AutoAdvanceDelay);
    
    // Save history
    TArray<TSharedPtr<FJsonValue>> HistoryArray;
    for (const FName& DialogueId : DialogueHistory)
    {
        HistoryArray.Add(MakeShareable(new FJsonValueString(DialogueId.ToString())));
    }
    RootObject->SetArrayField(TEXT("DialogueHistory"), HistoryArray);
    
    // Save active dialogue state if any
    if (ActiveDialogue && ActiveDialogue->IsActive())
    {
        TSharedPtr<FJsonObject> ActiveDialogueObject = MakeShareable(new FJsonObject());
        
        if (UDialogueSessionContext* Context = ActiveDialogue->GetContext())
        {
            FString ContextJson;
            Context->SaveToJson(ContextJson);
            ActiveDialogueObject->SetStringField(TEXT("Context"), ContextJson);
        }
        
        if (UDialogueNode* CurrentNode = ActiveDialogue->GetCurrentNode())
        {
            ActiveDialogueObject->SetStringField(TEXT("CurrentNodeId"), CurrentNode->NodeId.ToString());
        }
        
        RootObject->SetObjectField(TEXT("ActiveDialogue"), ActiveDialogueObject);
    }
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UDialogueSubsystem::LoadStateFromJson(const FString& Json)
{
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    
    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        return false;
    }
    
    // Load settings
    TypewriterSpeed = RootObject->GetNumberField(TEXT("TypewriterSpeed"));
    bAutoAdvanceEnabled = RootObject->GetBoolField(TEXT("AutoAdvanceEnabled"));
    AutoAdvanceDelay = RootObject->GetNumberField(TEXT("AutoAdvanceDelay"));
    
    // Load history
    DialogueHistory.Empty();
    const TArray<TSharedPtr<FJsonValue>>* HistoryArray;
    if (RootObject->TryGetArrayField(TEXT("DialogueHistory"), HistoryArray))
    {
        for (const TSharedPtr<FJsonValue>& Value : *HistoryArray)
        {
            DialogueHistory.Add(FName(*Value->AsString()));
        }
    }
    
    // Note: Loading active dialogue state would require more complex logic
    // to restore the dialogue runner and its state
    
    return true;
}

bool UDialogueSubsystem::LoadSaveGame()
{
    CurrentSaveGame = UDialogueSaveGame::LoadOrCreateSaveGame(this);
    
    if (CurrentSaveGame)
    {
        UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: SaveGame loaded successfully"));
        return true;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("DialogueSubsystem: Failed to load SaveGame"));
    return false;
}

void UDialogueSubsystem::HandleDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("DialogueSubsystem: Dialogue ended callback"));
    
    // Broadcast global event before cleanup
    OnAnyDialogueEnded.Broadcast(CurrentPlayer, CurrentNPC);

    // Auto-save on dialogue end
    if (bAutoSaveEnabled)
    {
      if (ActiveDialogue)
  {
    UDialogueDataAsset* Asset = ActiveDialogue->GetLoadedDialogue();
   if (Asset)
            {
      MarkDialogueCompleted(Asset->DialogueId);
            }
    }
        
  ClearActiveSaveState();
     SaveDialogueState();
    }
    
    // Cleanup
  ActiveDialogue = nullptr;
    CurrentPlayer = nullptr;
    CurrentNPC = nullptr;
    PreviousNode = nullptr;
}

void UDialogueSubsystem::HandleNodeEntered(UDialogueNode* NewNode)
{
    // Broadcast global node change event
    OnAnyDialogueNodeChanged.Broadcast(NewNode, PreviousNode, ActiveDialogue);
    
    // Update previous node tracker
    PreviousNode = NewNode;
}

UDialogueRunner* UDialogueSubsystem::AcquireRunner()
{
    if (bUseObjectPooling && RunnerPool)
    {
        UDialogueRunner* Runner = Cast<UDialogueRunner>(RunnerPool->Acquire());
        if (Runner)
        {
            UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Acquired DialogueRunner from pool"));
            return Runner;
        }
    }
    
    // Fallback: create new runner
    UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Creating new DialogueRunner (pooling disabled or pool unavailable)"));
    return NewObject<UDialogueRunner>(this);
}

void UDialogueSubsystem::ReleaseRunner(UDialogueRunner* Runner)
{
    if (!Runner)
    {
        return;
    }
    
    // Unbind events
    Runner->OnDialogueEnded.RemoveDynamic(this, &UDialogueSubsystem::HandleDialogueEnded);
    Runner->OnNodeEntered.RemoveDynamic(this, &UDialogueSubsystem::HandleNodeEntered);
 
  if (bUseObjectPooling && RunnerPool)
    {
        RunnerPool->Release(Runner);
        UE_LOG(LogDialogueSubsystem, Verbose, TEXT("Returned DialogueRunner to pool"));
    }
    else
    {
        // Let GC collect it
        Runner->ConditionalBeginDestroy();
    }
}

void UDialogueSubsystem::GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const
{
    if (RunnerPool)
    {
        RunnerPool->GetPoolStats(OutAvailable, OutInUse, OutTotal);
    }
    else
    {
        OutAvailable = OutInUse = OutTotal = 0;
    }
}

void UDialogueSubsystem::PreWarmRunnerPool(int32 Count)
{
    if (RunnerPool)
    {
        RunnerPool->PreWarm(Count);
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Pre-warmed runner pool with %d objects"), Count);
    }
}

void UDialogueSubsystem::TrimRunnerPool()
{
    if (RunnerPool)
    {
        RunnerPool->Trim();
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Trimmed runner pool"));
    }
}
