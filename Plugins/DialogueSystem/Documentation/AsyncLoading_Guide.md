# Dialogue System - Async Loading Guide

## Overview

The Dialogue Subsystem now supports asynchronous dialogue asset loading via Unreal's AssetManager. This allows for non-blocking loading of dialogue assets, improving performance especially when loading large dialogue trees.

## Features

? **Async Loading** - Load dialogue assets without blocking the game thread
? **Asset Registry** - Register dialogue assets with IDs for easy lookup
? **Batch Loading** - Preload multiple dialogues efficiently
? **Progress Tracking** - Monitor loading progress (0.0 to 1.0)
? **Callbacks** - Get notified when loading completes
? **Cancellation** - Cancel pending loads if needed
? **Caching** - Automatically cache loaded assets

## Quick Start

### 1. Register Dialogue Assets

Before you can load dialogues, register them with the subsystem:

```cpp
UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

// Register a single dialogue
FSoftObjectPath AssetPath(TEXT("/Game/Dialogues/Intro_Dialogue.Intro_Dialogue"));
Subsystem->RegisterDialogueAsset(FName("Intro"), AssetPath);

// Or in Blueprints
DialogueSubsystem->RegisterDialogueAsset("Intro", AssetPath);
```

### 2. Load Asynchronously

```cpp
// C++
FOnDialogueLoadedDelegate Callback;
Callback.BindLambda([](UDialogueDataAsset* Asset, bool bSuccess) {
    if (bSuccess && Asset)
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue loaded: %s"), *Asset->GetName());
    }
});

Subsystem->LoadDialogueAsync(FName("Intro"), Callback);
```

```bp
// Blueprint
Subsystem->LoadDialogueAsync("Intro", OnCompleteCallback);

// In OnCompleteCallback:
if (bSuccess && LoadedDialogue != nullptr)
{
    // Use the dialogue
}
```

### 3. Batch Preloading

For loading screens or level transitions:

```cpp
TArray<FName> DialoguesToLoad;
DialoguesToLoad.Add(FName("Intro"));
DialoguesToLoad.Add(FName("Quest1"));
DialoguesToLoad.Add(FName("Quest2"));

Subsystem->PreloadDialogues(DialoguesToLoad);
```

## Advanced Usage

### Check Loading Status

```cpp
// Check if a dialogue is currently loading
if (Subsystem->IsAsyncLoadPending(FName("Intro")))
{
    // Get progress (0.0 to 1.0)
    float Progress = Subsystem->GetAsyncLoadProgress(FName("Intro"));
    UE_LOG(LogTemp, Log, TEXT("Loading: %.0f%%"), Progress * 100.0f);
}
```

### Cancel Loading

```cpp
// Cancel a specific load
Subsystem->CancelAsyncLoad(FName("Intro"));

// Or cancel all pending loads (e.g. during level unload)
Subsystem->CancelPendingAsyncLoads();
```

### Synchronous Loading (Fallback)

The old synchronous `LoadDialogue` still works:

```cpp
UDialogueDataAsset* Dialogue = Subsystem->LoadDialogue(FName("Intro"));
// Warning: This blocks until loaded!
```

## Best Practices

### ? DO:
- Register assets during game initialization
- Use async loading for dialogues during gameplay
- Batch preload dialogues for upcoming levels
- Check `IsAsyncLoadPending()` before starting a new load
- Cache dialogue IDs in your level designers' data

### ? DON'T:
- Don't use synchronous loading in critical paths
- Don't load huge dialogue sets all at once
- Don't forget to unregister when dialogues change
- Don't start dialogue immediately after calling `LoadDialogueAsync`

## Integration Examples

### Loading Screen

```cpp
void AMyGameMode::LoadLevel(FName LevelName)
{
    // Show loading screen
    ShowLoadingScreen();
    
    // Get dialogues for this level
    TArray<FName> LevelDialogues = GetDialoguesForLevel(LevelName);
    
    // Track completion
    int32 LoadedCount = 0;
    int32 TotalCount = LevelDialogues.Num();
    
    // Load each dialogue
    for (FName DialogueId : LevelDialogues)
    {
        FOnDialogueLoadedDelegate Callback;
        Callback.BindLambda([&](UDialogueDataAsset* Asset, bool bSuccess) {
            LoadedCount++;
            
            // Update progress
            float Progress = (float)LoadedCount / TotalCount;
            UpdateLoadingProgress(Progress);
            
            // Done?
            if (LoadedCount >= TotalCount)
            {
                OpenLevel(LevelName);
            }
        });
        
        DialogueSubsystem->LoadDialogueAsync(DialogueId, Callback);
    }
}
```

### Smart NPC Interaction

```cpp
void ANPCCharacter::OnPlayerApproach()
{
    // Start loading dialogue in advance
    FOnDialogueLoadedDelegate Callback;
    Callback.BindUObject(this, &ANPCCharacter::OnDialogueLoaded);
    
    DialogueSubsystem->LoadDialogueAsync(MyDialogueId, Callback);
    
    // Show "Loading..." indicator if needed
    if (DialogueSubsystem->IsAsyncLoadPending(MyDialogueId))
    {
        ShowLoadingIndicator();
    }
}

void ANPCCharacter::OnDialogueLoaded(UDialogueDataAsset* Dialogue, bool bSuccess)
{
    HideLoadingIndicator();
    
    if (bSuccess)
    {
        // Ready to start dialogue
        bCanStartDialogue = true;
    }
}
```

### Level Streaming

```cpp
void ALevelManager::OnLevelStreamed(ULevel* Level)
{
    // Preload dialogues for this level
    TArray<FName> DialogueIds = GetDialogueIDsForLevel(Level);
    
    if (DialogueIds.Num() > 0)
    {
        DialogueSubsystem->PreloadDialogues(DialogueIds);
        UE_LOG(LogTemp, Log, TEXT("Preloading %d dialogues for level %s"), 
            DialogueIds.Num(), *Level->GetName());
    }
}
```

## Performance Considerations

### Object Pooling + Async Loading
The subsystem combines object pooling with async loading:
- **Pooling** reduces runtime allocations (DialogueRunners)
- **Async Loading** prevents frame hitches (Dialogue Assets)

### Memory Management
- Loaded dialogues are cached until `ClearCache()` is called
- Use `UnregisterDialogueAsset()` to remove from cache
- Consider clearing cache during level transitions

### Loading Priority
Async loads use `AsyncLoadHighPriority` by default. For lower priority loads, modify the `StreamableManager.RequestAsyncLoad()` call in `DialogueSubsystem.cpp`.

## Troubleshooting

### Dialogue Not Loading
1. Check if asset is registered: `DialogueAssetRegistry.Contains(DialogueId)`
2. Verify asset path is valid
3. Check logs for error messages
4. Ensure AssetManager is initialized

### Slow Loading
1. Use batch loading instead of individual loads
2. Preload during loading screens
3. Check if assets have heavy dependencies
4. Profile with Unreal Insights

### Memory Leaks
1. Call `CancelPendingAsyncLoads()` on level unload
2. Clear cache with `ClearCache()` when appropriate
3. Monitor with Memory Profiler

## Configuration

Add to `DefaultGame.ini`:

```ini
[/Script/DialogueSystemRuntime.DialogueSubsystem]
; Enable object pooling
bUseObjectPooling=True

; Pool sizes
InitialPoolSize=3
MaxPoolSize=10

; Auto-save settings
bAutoSaveEnabled=True
```

## API Reference

### Async Loading Functions

| Function | Description |
|----------|-------------|
| `LoadDialogueAsync` | Load a single dialogue asynchronously |
| `PreloadDialogues` | Batch load multiple dialogues |
| `RegisterDialogueAsset` | Register a dialogue path |
| `UnregisterDialogueAsset` | Unregister a dialogue |
| `CancelAsyncLoad` | Cancel a specific load |
| `CancelPendingAsyncLoads` | Cancel all loads |
| `IsAsyncLoadPending` | Check if loading |
| `GetAsyncLoadProgress` | Get progress (0-1) |

### Sync Loading Functions

| Function | Description |
|----------|-------------|
| `LoadDialogue` | Synchronous load (blocks) |
| `ClearCache` | Clear all cached assets |

## See Also

- [Performance Guide](Performance_Guide.md)
- [Object Pooling](ObjectPooling_Guide.md)
- [Save System](SaveSystem_Guide.md)
