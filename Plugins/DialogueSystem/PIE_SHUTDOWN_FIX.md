# PIE SHUTDOWN FIX - Blueprint Errors During Dialogue Exit

## PROBLEM ?

When ending PIE (Play In Editor) while a dialogue is active, Blueprint errors were occurring:

```
LogDialogueInput: Error: UnblockInput: No valid PlayerController found
LogScript: Warning: Accessed None trying to read (real) property CallFunc_GetPlayerState_ReturnValue_2
PIE: Error: Blueprint Runtime Error: "Accessed None trying to read (real) property CallFunc_GetPlayerState_ReturnValue_2"
```

### Root Cause

During PIE shutdown, the cleanup order causes issues:

1. **World starts tearing down** ? `World->bIsTearingDown` becomes `true`
2. **PlayerController is destroyed**
3. **DialogueSubsystem::Deinitialize()** is called
4. **EndDialogue()** triggers `HandleDialogueEnded` callback
5. **Blueprint event tries to access destroyed PlayerController** ? ? **"Accessed None" errors**

The problem occurred in three locations:
- `DialogueInputBlockerComponent::UnblockInput()` - tried to access PlayerController
- `DialogueCameraComponent::DeactivateDialogueCamera()` - tried to restore view target
- `DialogueSubsystem::HandleDialogueEnded()` - triggered Blueprint events accessing destroyed objects

---

## SOLUTION ?

Added **world teardown checks** to all cleanup methods to prevent accessing destroyed objects during PIE shutdown.

### Files Modified

1. **`DialogueCameraComponent.cpp`** (2 changes)
2. **`DialogueInputBlockerComponent.cpp`** (2 changes)
3. **`DialogueSubsystem.cpp`** (1 change)

---

## DETAILED CHANGES

### 1. DialogueCameraComponent::EndPlay()

**BEFORE:**
```cpp
void UDialogueCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bCameraActive && EndPlayReason != EEndPlayReason::EndPlayInEditor && EndPlayReason != EEndPlayReason::Quit)
 {
        DeactivateDialogueCamera();
    }
    Super::EndPlay(EndPlayReason);
}
```

**AFTER:**
```cpp
void UDialogueCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UWorld* World = GetWorld();
    bool bIsTearingDown = !World || World->bIsTearingDown;
    
    if (bCameraActive && !bIsTearingDown)
    {
  DeactivateDialogueCamera();
    }
    else if (bCameraActive && bIsTearingDown)
 {
        // During teardown, cleanup without accessing potentially destroyed actors
     UE_LOG(LogDialogueCamera, Log, TEXT("EndPlay during world teardown - skipping full deactivation"));
      
        // Just cleanup local state
        bIsBlending = false;
      SetComponentTickEnabled(false);
    
 if (TrackingTimerHandle.IsValid())
        {
     GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
        }
        
     if (DialogueCameraActor && !DialogueCameraActor->IsActorBeingDestroyed())
        {
            DialogueCameraActor->Destroy();
   DialogueCameraActor = nullptr;
        }
      
        bCameraActive = false;
    }
    
 Super::EndPlay(EndPlayReason);
}
```

---

### 2. DialogueCameraComponent::DeactivateDialogueCamera()

**BEFORE:**
```cpp
void UDialogueCameraComponent::DeactivateDialogueCamera()
{
    if (!bCameraActive)
        return;
    
    // ... stop timers ...
    
    APlayerController* PC = CachedPlayerController.Get();
AActor* OriginalTarget = OriginalViewTarget.Get();
    
    if (PC && bRestoreOriginalCamera)
    {
        if (OriginalTarget)
        {
      PC->SetViewTargetWithBlend(OriginalTarget, RestoreBlendTime, VTBlend_EaseInOut);
        }
        // ...
    }
}
```

**AFTER:**
```cpp
void UDialogueCameraComponent::DeactivateDialogueCamera()
{
    if (!bCameraActive)
        return;
    
    // ... stop timers ...
    
    UWorld* World = GetWorld();
    bool bIsTearingDown = !World || World->bIsTearingDown;
    
    APlayerController* PC = CachedPlayerController.Get();
    AActor* OriginalTarget = OriginalViewTarget.Get();
    
    // Only restore camera if not tearing down
    if (!bIsTearingDown && PC && bRestoreOriginalCamera)
    {
        if (OriginalTarget && !OriginalTarget->IsActorBeingDestroyed())
  {
     PC->SetViewTargetWithBlend(OriginalTarget, RestoreBlendTime, VTBlend_EaseInOut);
        }
        // ...
    }
  else if (bIsTearingDown)
 {
        UE_LOG(LogDialogueCamera, Log, TEXT("Skipping camera restoration during world teardown"));
    }
}
```

---

### 3. DialogueInputBlockerComponent::EndPlay()

**BEFORE:**
```cpp
void UDialogueInputBlockerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bInputBlocked && EndPlayReason != EEndPlayReason::EndPlayInEditor && EndPlayReason != EEndPlayReason::Quit)
    {
        UnblockInput();
    }
    Super::EndPlay(EndPlayReason);
}
```

**AFTER:**
```cpp
void UDialogueInputBlockerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bInputBlocked && 
     EndPlayReason != EEndPlayReason::EndPlayInEditor && 
        EndPlayReason != EEndPlayReason::Quit)
    {
    UnblockInput();
    }
    else if (bInputBlocked)
    {
      // During PIE end/quit, just mark as unblocked without restoration
      UE_LOG(LogDialogueInput, Log, TEXT("EndPlay during PIE shutdown - skipping input restoration"));
 bInputBlocked = false;
  }
    
  Super::EndPlay(EndPlayReason);
}
```

---

### 4. DialogueInputBlockerComponent::UnblockInput()

**BEFORE:**
```cpp
void UDialogueInputBlockerComponent::UnblockInput()
{
    if (!bInputBlocked)
        return;
    
    APlayerController* PC = FindPlayerController();
    if (!PC)
    {
    UE_LOG(LogDialogueInput, Error, TEXT("UnblockInput: No valid PlayerController found"));
        return;
    }
    
    // ... restore input ...
}
```

**AFTER:**
```cpp
void UDialogueInputBlockerComponent::UnblockInput()
{
    if (!bInputBlocked)
        return;
    
    // Check if world is tearing down
    UWorld* World = GetWorld();
    bool bIsTearingDown = !World || World->bIsTearingDown;
    
    if (bIsTearingDown)
    {
   UE_LOG(LogDialogueInput, Log, TEXT("UnblockInput: Skipping restoration during world teardown"));
      bInputBlocked = false;
   return;
    }
    
    APlayerController* PC = FindPlayerController();
    if (!PC)
    {
        UE_LOG(LogDialogueInput, Warning, TEXT("UnblockInput: No valid PlayerController found - possibly during shutdown"));
     bInputBlocked = false;
        return;
    }
    
    // ... restore input ...
}
```

---

### 5. DialogueSubsystem::Deinitialize() ? **CRITICAL**

**BEFORE:**
```cpp
void UDialogueSubsystem::Deinitialize()
{
    if (bAutoSaveEnabled)
    {
        SaveDialogueState();
    }
 
    // End any active dialogue
  if (ActiveDialogue)
    {
     EndActiveDialogue_Implementation(); // ? Triggers HandleDialogueEnded callback!
    }
    
    CancelPendingAsyncLoads();
    ClearCache();
    
    Super::Deinitialize();
}
```

**AFTER:**
```cpp
void UDialogueSubsystem::Deinitialize()
{
    UWorld* World = GetWorld();
    bool bIsTearingDown = !World || World->bIsTearingDown;
    
    if (bIsTearingDown)
    {
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Deinitialize during world teardown - skipping dialogue end callbacks"));
        
        // During teardown, cleanup without triggering events
        if (ActiveDialogue)
        {
 // Unbind events to prevent callbacks
   ActiveDialogue->OnDialogueEnded.RemoveDynamic(this, &UDialogueSubsystem::HandleDialogueEnded);
            ActiveDialogue->OnNodeEntered.RemoveDynamic(this, &UDialogueSubsystem::HandleNodeEntered);
            ActiveDialogue->OnChoiceSelected.RemoveDynamic(this, &UDialogueSubsystem::HandleChoiceSelected);
  
            // Just clear reference
            ActiveDialogue = nullptr;
     }
        
        CurrentPlayer = nullptr;
        CurrentNPC = nullptr;
        PreviousNode = nullptr;
    }
    else
    {
        // Normal shutdown - do auto-save and proper cleanup
        if (bAutoSaveEnabled)
        {
   SaveDialogueState();
        }
        
        if (ActiveDialogue)
     {
         EndActiveDialogue_Implementation();
        }
  }
    
    CancelPendingAsyncLoads();
    ClearCache();
    
    Super::Deinitialize();
}
```

**KEY CHANGE:** When `bIsTearingDown` is true, we:
1. **Unbind delegates** to prevent `HandleDialogueEnded` from firing
2. **Clear references** directly without calling `EndDialogue()`
3. **Skip Blueprint event broadcasts** that would access destroyed PlayerController

---

## EXPECTED BEHAVIOR AFTER FIX

### ? Normal Dialogue End (F key)

- Full cleanup runs as before
- Input restored
- Camera restored
- Events broadcast
- Save game updated
- **NO CHANGES**

### ? PIE End During Dialogue (ESC)

**BEFORE:**
```
LogDialogueInput: Error: UnblockInput: No valid PlayerController found
LogScript: Warning: Accessed None trying to read (real) property CallFunc_GetPlayerState_ReturnValue_2
PIE: Error: Blueprint Runtime Error: "Accessed None..."
```

**AFTER:**
```
LogDialogueSubsystem: Deinitialize during world teardown - skipping dialogue end callbacks
LogDialogueCamera: EndPlay during world teardown - skipping full deactivation
LogDialogueInput: EndPlay during PIE shutdown - skipping input restoration
```

**NO Blueprint errors!** ?

---

## TESTING

1. **Start PIE**
2. **Begin dialogue with Lianne**
3. **Press ESC to end PIE while dialogue is active**
4. **Check Output Log** - should see teardown messages, no Blueprint errors

---

## WHY THIS FIX WORKS

The key insight is that during PIE shutdown:
- The **PlayerController** is destroyed **before** subsystems deinitialize
- **Blueprint events** try to access `GetPlayerState()` ? crashes
- **Camera restoration** tries to `SetViewTarget()` ? crashes
- **Input restoration** tries to access `PlayerController->PlayerInput` ? crashes

By checking `World->bIsTearingDown`, we:
1. **Skip all actor access** during shutdown
2. **Clean up only local state** (timers, flags, references)
3. **Unbind delegates** to prevent callbacks
4. **Let UE's teardown system** handle actor cleanup

This is the **correct pattern** for handling PIE shutdown in subsystems and components.

---

## COMPATIBILITY

? **No breaking changes**
? **Normal dialogue flow unchanged**
? **Only affects PIE shutdown behavior**
? **Follows UE best practices for cleanup order**

---

## FILES CHANGED

1. `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Components/DialogueCameraComponent.cpp`
2. `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Components/DialogueInputBlockerComponent.cpp`
3. `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Subsystems/DialogueSubsystem.cpp`

---

## BUILD STATUS

? **Compiled successfully** - No errors
? **Ready for testing**

---

**FIXED:** 2025-01-11
**ISSUE:** Blueprint errors when ending PIE during active dialogue
**SOLUTION:** Added world teardown checks to all cleanup methods
