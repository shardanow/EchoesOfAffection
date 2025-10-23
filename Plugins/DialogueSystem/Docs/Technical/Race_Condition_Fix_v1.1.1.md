# Race Condition Fix � DialogueSubsystem (v1.1.1)

## ?? �������� ��������

### �������� ��� (v1.1.0)

```cpp
void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // Prepare load info
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
    
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
  
    // ? ��������: Start async load BEFORE adding to map
    LoadInfo.StreamableHandle = StreamableManager.RequestAsyncLoad(
        *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
        {
            if (UDialogueSubsystem* StrongThis = WeakThis.Get())
            {
     // ? Tries to find DialogueId in PendingAsyncLoads
       StrongThis->OnAsyncLoadCompleted(DialogueId);
   }
  }),
        // ...
    );
    
    // ? ��������: Add to map AFTER starting async load
  PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
}
```

### ��������: Race Condition

**�������� ����:**

1. ���������� `LoadDialogueAsync("Dialogue_Quest1")`
2. `RequestAsyncLoad` �����������
3. **Asset ��� � ������** ? callback ���������� **���������** (���������!)
4. `OnAsyncLoadCompleted("Dialogue_Quest1")` ���� � `PendingAsyncLoads`
5. **�� �������!** (��� �� �������� �� ������ ����)
6. Callbacks �� ���������� ? ������������ ���� �������� **�����**
7. Memory leak: `LoadInfo` �������� � `PendingAsyncLoads` ��������

**������� ���������������:**
- Asset ��� �������� � ������ (��������� �����)
- ������� �������� � SSD
- Asset ����� ���������
- Deterministic � ��������� ������

**�������:** �����, �� �������������� ��������������� ��� ������������ ��������.

---

## ? �������

### ������������ ��� (v1.1.1)

```cpp
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
  return;
    }
    
    // Validate asset path
    const FSoftObjectPath* AssetPath = DialogueAssetRegistry.Find(DialogueId);
    if (!AssetPath || !AssetPath->IsValid())
    {
        OnLoaded.ExecuteIfBound(nullptr, false);
        return;
    }
    
    // Prepare async load info
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
    
    // ? FIX: Add to map BEFORE starting async load
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
    
// ? Get reference to LoadInfo in map (important after MoveTemp!)
    FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];
    
    // Start async load with AssetManager
  UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
 
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
    
  // ? Now safe: even if callback fires immediately, LoadInfo is already in map
    LoadInfoRef.StreamableHandle = StreamableManager.RequestAsyncLoad(
        *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
   {
        if (UDialogueSubsystem* StrongThis = WeakThis.Get())
 {
         // ? Will find DialogueId in PendingAsyncLoads
 StrongThis->OnAsyncLoadCompleted(DialogueId);
            }
        }),
        FStreamableManager::AsyncLoadHighPriority,
    false,
        false,
        TEXT("DialogueSubsystem")
    );
    
    UE_LOG(LogDialogueSubsystem, Log, TEXT("Started async load for dialogue '%s'"), *DialogueId.ToString());
}
```

---

## ?? �������� ���������

### 1. ������� �������� �������

**����:**
```cpp
1. ������� LoadInfo
2. ��������� async load ? callback ����� ��������� ������
3. �������� LoadInfo � map ? ������� ������!
```

**�����:**
```cpp
1. ������� LoadInfo
2. ? �������� LoadInfo � map
3. ? �������� ������ �� LoadInfo � map
4. ��������� async load ? callback ������ ���������
```

### 2. ������������� ������ ����� MoveTemp

**�����:**
```cpp
// ����� MoveTemp ������������ LoadInfo ������!
PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));

// ? �����������: LoadInfo ��� ������
LoadInfo.StreamableHandle = ...;

// ? ���������: �������� ������ �� ����
FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];
LoadInfoRef.StreamableHandle = ...;
```

---

## ?? ������������

### ���� 1: ���������� ��������

```cpp
// � �����
UDialogueSubsystem* Subsystem = GetSubsystem();

// Pre-load asset to memory
UDialogueDataAsset* PreloadedAsset = LoadObject<UDialogueDataAsset>(...);

bool bCallbackExecuted = false;

// Load again (should complete synchronously)
Subsystem->LoadDialogueAsync(
    TEXT("TestDialogue"),
    FOnDialogueLoadedDelegate::CreateLambda([&](UDialogueDataAsset* Asset, bool bSuccess)
    {
        bCallbackExecuted = true;
        TestTrue("Asset loaded", bSuccess);
     TestEqual("Asset valid", Asset, PreloadedAsset);
  })
);

// ? Callback should be executed immediately (or within same frame)
TestTrue("Callback executed", bCallbackExecuted);
```

### ���� 2: ������������� �������

```cpp
int32 CallbackCount = 0;

// Request same asset multiple times
for (int32 i = 0; i < 10; ++i)
{
    Subsystem->LoadDialogueAsync(
  TEXT("TestDialogue"),
    FOnDialogueLoadedDelegate::CreateLambda([&](UDialogueDataAsset*, bool bSuccess)
        {
       CallbackCount++;
         TestTrue("Load success", bSuccess);
})
    );
}

// Wait for completion
// ...

// ? All callbacks should be called
TestEqual("All callbacks executed", CallbackCount, 10);
```

### ���� 3: Race Condition Stress Test

```cpp
// Rapid-fire requests
for (int32 i = 0; i < 1000; ++i)
{
    Subsystem->LoadDialogueAsync(
        FName(*FString::Printf(TEXT("Dialogue_%d"), i % 10)),
FOnDialogueLoadedDelegate::CreateLambda([](UDialogueDataAsset*, bool) {})
    );
}

// Wait for all to complete
// ...

// ? No crashes, no hangs, no memory leaks
TestEqual("Pending loads cleared", Subsystem->GetPendingLoadsCount(), 0);
```

---

## ?? ������������������

### ������� �� ������������������

| ������� | �� | ����� | ��������� |
|---------|-----|-------|-----------|
| **����� ����������** | 100% | 100% | 0% |
| **Memory overhead** | 0 bytes | 0 bytes | 0% |
| **CPU overhead** | 1 map lookup | 2 map lookups | +1 lookup |
| **Thread safety** | ?? Race condition | ? Safe | Fixed |

**�����:** ��������� ����������� ��������� (1 �������������� lookup � TMap), �� ���������� ����������� ���.

---

## ?? Thread Safety Analysis

### ������� ������

```cpp
// UDialogueSubsystem �������� �� Game Thread
// FStreamableManager ����� ������� callback �� ����� �����

LoadDialogueAsync()          [Game Thread]
    ?
RequestAsyncLoad()           [Game Thread]
    ? (async)
Callback fires   [ANY Thread - ����� ���� ����� �� Game Thread!]
    ?
OnAsyncLoadCompleted()    [Game Thread - WeakPtr ��������]
```

**��������:**
���� callback ���������� **���������** �� Game Thread (asset ��� ��������), �� ���� ���� ����������� ���������������, � race condition **�������������**.

**�������:**
���������� � `PendingAsyncLoads` �� ������� callback �����������, ��� ���� ��� ���������� ���������� ������ ��� � ����.

---

## ?? Changelog Entry

```markdown
### v1.1.1 (2025-01-22) - Bug Fix

#### Critical Fixes
- ?? **Fixed race condition in async dialogue loading**
  - Fixed order of operations in `LoadDialogueAsync`
  - Now adds `LoadInfo` to `PendingAsyncLoads` BEFORE starting async load
  - Prevents crash when asset is already in memory and loads synchronously
  - No performance impact (1 additional map lookup)
  
  **Impact:** Rare crash/hang when loading already-cached dialogues
  **Severity:** High (data corruption, user wait forever)
  **Affected:** All async dialogue loading operations
```

---

## ?? Lessons Learned

### 1. **Async !== Always Async**

������ ����������� API � Unreal ����� ������� callback **���������**, ���� �������� ����������� ���������:

```cpp
// FStreamableManager::RequestAsyncLoad �����:
// 1. ������� ���������� ���������� (asset �� � ������)
// 2. ������� callback ������ �� (asset ��� � ������)

// ���������� �������:
1. ����������� ���������
2. ���������������� callback
3. ��������� async �������� ? callback ����� ��������� �����
4. �������� ��������� ������ ? ����� ���� ������� ������!
```

### 2. **MoveTemp � ������**

```cpp
TMap<FName, FStruct> Map;

FStruct Data;
Map.Add(Key, MoveTemp(Data));

// ? Data ������ ������!
Data.Field = Value;

// ? �������� ������ �� ����
FStruct& DataRef = Map[Key];
DataRef.Field = Value;
```

### 3. **Defensive Programming**

������ ������������� ������ ��������:
- Callback ����� ��������� **����������**
- �������� ����� ����������� **���������**
- ������ ����� ���� **��� ������**

---

## ?? ��������� ���������

- [FStreamableManager Documentation](https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/Engine/FStreamableManager/)
- [Async Loading Best Practices](https://docs.unrealengine.com/5.3/en-US/asynchronous-asset-loading-in-unreal-engine/)
- [TMap Thread Safety](https://docs.unrealengine.com/5.3/en-US/API/Runtime/Core/Containers/TMap/)

---

## ? �������� �����������

### ����������� ������

- [x] ��� ������������� ��� ������
- [x] `LoadInfo` ����������� � `PendingAsyncLoads` �� ������� async load
- [x] ������������ ������ �� `LoadInfo` �� ���� (�� ��������� ����������)
- [x] ������ �������� ���� �� ��������
- [x] ������ �������� ��� ������������� �� ��������
- [x] Error handling ��������
- [x] Logging ��������
- [x] Backwards compatible (API �� ���������)

---

**������:** 1.1.1  
**����:** 22.01.2025  
**�����:** DialogueSystem Team  
**������:** ? ���������� � ��������������
