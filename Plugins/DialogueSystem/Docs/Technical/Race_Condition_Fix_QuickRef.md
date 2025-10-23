# Race Condition Fix - Quick Reference

> **Version:** 1.1.1  
> **Date:** 22.01.2025  
> **File:** `DialogueSubsystem.cpp::LoadDialogueAsync()`

---

## ?? TL;DR (Too Long; Didn't Read)

**��������:** Callback async �������� ��� ��������� **��** ����, ��� `LoadInfo` ��������� � `PendingAsyncLoads`.

**�������:** ������� ������� �������� � ������ ��������� � ���� **�����** �������� ��������.

**���������:** ��������� ������, �� ��������� crash ��� ��������� �������� ��������.

---

## ? ���� (v1.1.0)

```cpp
void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // ...��������...
    
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
  
    // ? ��������: ������ �������� ����� ����������� � ����
    LoadInfo.StreamableHandle = StreamableManager.RequestAsyncLoad(
        *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
   {
// ? ���� callback ��������� ������, LoadInfo ��� �� � ����!
    if (UDialogueSubsystem* StrongThis = WeakThis.Get())
  {
         StrongThis->OnAsyncLoadCompleted(DialogueId); // <-- �� ������!
            }
        }),
        // ...
    );
    
    // ? ���������� � ���� ����� �������
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
}
```

### ?? ��� ����� ����� �� ���:

1. Asset ��� � ������ ? callback ���������� **���������**
2. `OnAsyncLoadCompleted()` ���� � `PendingAsyncLoads`
3. **�� �������** ? callback �� ����������
4. ������������ ���� **�����**

---

## ? ����� (v1.1.1)

```cpp
void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // ...��������...
    
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
    
    // ? FIX: ��������� � ���� ����� �������� ��������
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
 
    // ? �������� ������ �� LoadInfo � ���� (����� MoveTemp!)
    FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];

    // Start async load
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
    
    // ? ������ ���������: ���� ���� callback ��������� ���������, LoadInfo ��� � ����
    LoadInfoRef.StreamableHandle = StreamableManager.RequestAsyncLoad(
  *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
        {
  if (UDialogueSubsystem* StrongThis = WeakThis.Get())
 {
 // ? ������ ������ DialogueId � PendingAsyncLoads
     StrongThis->OnAsyncLoadCompleted(DialogueId);
  }
        }),
        // ...
    );
}
```

---

## ?? �������� ���������

### 1. ������� ��������

**����:**
```
1. ������� LoadInfo
2. ��������� async load      ? callback ����� ��������� �����
3. �������� LoadInfo � map   ? ������� ������!
```

**�����:**
```
1. ������� LoadInfo
2. ? �������� LoadInfo � map
3. ? �������� ������ FAsyncLoadInfo&
4. ��������� async load      ? callback ������ ���������
```

### 2. ������������� ������

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

## ?? ������������������

| ������� | �� | ����� | ��������� |
|---------|-----|-------|-----------|
| CPU overhead | 1 map lookup | 2 map lookups | **+1 lookup** |
| Memory | 0 bytes | 0 bytes | **0** |
| Thread safety | ?? Race condition | ? Safe | **Fixed** |

**�����:** ��������� ����������� **���������**, �� ���������� ����������� ���.

---

## ?? ��� ��������������

### ���� 1: ���������� ��������
```cpp
// Pre-load asset
UDialogueDataAsset* PreloadedAsset = LoadObject<UDialogueDataAsset>(...);

bool bCallbackExecuted = false;

// Load again (should complete synchronously)
Subsystem->LoadDialogueAsync(
    TEXT("TestDialogue"),
    FOnDialogueLoadedDelegate::CreateLambda([&](UDialogueDataAsset*, bool bSuccess)
    {
        bCallbackExecuted = true;
    })
);

// ? Callback ������ ���� ������ ����������
check(bCallbackExecuted);
```

### ���� 2: ������������� �������
```cpp
int32 CallbackCount = 0;

// Request same asset 10 times
for (int32 i = 0; i < 10; ++i)
{
    Subsystem->LoadDialogueAsync(
        TEXT("TestDialogue"),
   FOnDialogueLoadedDelegate::CreateLambda([&](UDialogueDataAsset*, bool)
      {
            CallbackCount++;
        })
    );
}

// Wait...

// ? ��� 10 callbacks ������ ���� �������
check(CallbackCount == 10);
```

---

## ?? Lesson Learned

### Async !== Always Async

������ async API � Unreal ����� ������� callback **���������**:

```cpp
// FStreamableManager::RequestAsyncLoad �����:
// 1. ������� ���������� ���������� (asset �� � ������)
// 2. ������� callback ������ �� (asset ��� � ������) ? �����!
```

### ���������� �������:
```cpp
1. ����������� ���������
2. ���������������� callback   ? ����� �������� async ��������!
3. ��������� async ��������
```

---

## ?? ��������� �����

- `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Subsystems/DialogueSubsystem.cpp`
- `Plugins/DialogueSystem/Docs/Technical/Race_Condition_Fix_v1.1.1.md` (�����������)
- `Plugins/DialogueSystem/CHANGELOG.md` (������� ���������)

---

## ? Checklist

- [x] ��� ������������� ��� ������
- [x] LoadInfo ����������� �� ������� async load
- [x] ������������ ������ �� ���� (�� ��������� ����������)
- [x] Backwards compatible (API �� ���������)
- [x] ������������ ���������
- [x] CHANGELOG ��������

---

**Status:** ? Fixed  
**Approved By:** DialogueSystem Team  
**Date:** 22.01.2025
