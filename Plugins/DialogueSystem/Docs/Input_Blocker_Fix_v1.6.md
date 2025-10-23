# Dialogue Input Blocker - ����������� ������� (v1.8)

## ����: 2024
## �����: System Architect

---

## ?? CRITICAL FIX v1.8: Enhanced Input Context Restoration

### ��������
**�������:** ����� `UnblockInput()` ���������� ��������� �� �������� ��� ������������� Enhanced Input System.

**�������:** 
```cpp
// ������ ��� v1.7
Subsystem->ClearAllMappings(); // �������� ���������
// ...������...
// ����� ������������� ��������� �� �����������������!
```

**���� ����������:**
```
LogDialogueInput: Input contexts restored - game should re-add its default contexts
```
�� ���� �� ��������������� ��������� �������������!

### ������� v1.8

#### 1. ���������� ���������� ����� �����������
```cpp
void UDialogueInputBlockerComponent::BlockEnhancedInput(const FDialogueInputBlockingSettings& Settings)
{
    // ? �������� ��� �������� ��������� � ������������
    const TArray<TPair<TObjectPtr<const UInputMappingContext>, int32>>& AppliedContexts = 
   Subsystem->GetAppliedInputContexts();
    
    // ? ��������� ��
    for (const TPair<TObjectPtr<const UInputMappingContext>, int32>& ContextPair : AppliedContexts)
    {
if (ContextPair.Key)
        {
            StoredMappingContexts.Add(const_cast<UInputMappingContext*>(ContextPair.Key.Get()));
    StoredMappingPriorities.Add(ContextPair.Value);
         
            UE_LOG(LogDialogueInput, Verbose, TEXT("Saved context: %s with priority %d"), 
     *ContextPair.Key->GetName(), ContextPair.Value);
        }
    }
    
  // ������ ����� ��������� ��������
    Subsystem->ClearAllMappings();
}
```

#### 2. �������������� ���������� ����� �������������
```cpp
void UDialogueInputBlockerComponent::RestoreEnhancedInput()
{
    // ������� ���������� ��������
    if (DialogueInputMappingContext.IsValid())
    {
        Subsystem->RemoveMappingContext(DialogueInputMappingContext.Get());
    }
    
    // ? ��������������� ��� ����������� ���������
    UE_LOG(LogDialogueInput, Log, TEXT("Restoring %d input mapping contexts"), 
        StoredMappingContexts.Num());
    
    for (int32 i = 0; i < StoredMappingContexts.Num(); ++i)
    {
    if (StoredMappingContexts[i])
        {
            int32 Priority = (i < StoredMappingPriorities.Num()) ? 
      StoredMappingPriorities[i] : 0;
            
    Subsystem->AddMappingContext(StoredMappingContexts[i], Priority);
            
            UE_LOG(LogDialogueInput, Verbose, TEXT("Restored context: %s with priority %d"), 
         *StoredMappingContexts[i]->GetName(), Priority);
 }
    }
 
    // ?? �������������� ���� ��������� �� ���� ���������
    if (StoredMappingContexts.Num() == 0)
    {
        UE_LOG(LogDialogueInput, Warning, TEXT("No input contexts were saved!"));
    }
}
```

---

## ����� ���� �������

### �������� (�� v1.8)
1. **������ "�������" �����** - ����������� ������� ������ �� ���� (? ���������� v1.6)
2. **���������� �� �������� (Legacy Input)** - �� ��������� `EnableInput()` (? ���������� v1.6)
3. **���������� �� �������� �� NPC** - ������������ PlayerController (? ���������� v1.7)
4. **���������� �� �������� (Enhanced Input)** - ��������� �� ����������������� (? ���������� v1.8)

---

## Changelog

### v1.8 (������� ������) - Enhanced Input Fix
**����:** 2024

? **����������� �����������:**
- ���������� Input Mapping Contexts ����� �����������
- �������������� Input Mapping Contexts ����� �������������  
- ������������� `GetAppliedInputContexts()` ��� ��������� �������� ����������
- ���������� ����������� ����������
- ��������� ����������� �����������/��������������� ����������
- �������������� ���� ��������� �� ���� ���������

?? **��������� � ����:**
- `BlockEnhancedInput()` - ��������� ��������� � ������������
- `RestoreEnhancedInput()` - ��������������� ��������� � ������������
- ��������� Verbose ���� ��� �������

### v1.7 - NPC PlayerController Fix
**����:** 2024

? **�����������:**
- `BeginPlay()` ������ �� �������� PlayerController
- `BlockInput()` � `UnblockInput()` ������ �������� ������ PC
- ��������� ����������� PlayerController
- ���������� ��������� ������

### v1.6 - Camera Jump & Legacy Input Fix
**����:** 2024

? **�����������:**
- `UnblockInput()` - �������� `FlushPressedKeys()`
- `RestoreLegacyInput()` - �������� `EnableInput(PC)`
- Verbose ����������� ��������� Pawn
- ����� `MoveIgnored` � `LookIgnored` ������

---

## ������������ v1.8

### ����������� ������ Enhanced Input

- [x] **���������� ��������� ���������**
  ```
  LogDialogueInput: Saving 2 input mapping contexts
  LogDialogueInput: Saved context: IMC_Default with priority 0
  LogDialogueInput: Saved context: IMC_Player with priority 1
  ```

- [x] **������������� ��������������� ���������**
  ```
  LogDialogueInput: Restoring 2 input mapping contexts
  LogDialogueInput: Restored context: IMC_Default with priority 0
  LogDialogueInput: Restored context: IMC_Player with priority 1
  ```

- [x] **���������� �������� ����� �������������**
  - WASD ������� ��������� ?
  - ���� ������� ������ ?
  - ��� Input Actions �������� ?

### ���� ��� ��������

#### ? ���������� ���� (v1.8):
```
LogDialogueInput: Saving 2 input mapping contexts
LogDialogueInput: Saved context: IMC_Default with priority 0
LogDialogueInput: Clearing all input mapping contexts for dialogue
LogDialogueInput: Added dialogue input mapping context with priority 10
LogDialogueInput: Input blocked with mode: 1 for PC: PlayerController_0
...������...
LogDialogueInput: Unblocking input for PC: PlayerController_0
LogDialogueInput: Removed dialogue input mapping context
LogDialogueInput: Restoring 2 input mapping contexts
LogDialogueInput: Restored context: IMC_Default with priority 0
LogDialogueInput: Input contexts restored successfully
LogDialogueInput: Input unblocked successfully
```

#### ? ������������ ���� (v1.7):
```
LogDialogueInput: Clearing all input mapping contexts for dialogue
...
LogDialogueInput: Input contexts restored - game should re-add its default contexts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
��������� �� �����������������!
```

---

## �������� � v1.7 �� v1.8

### ��� ����������
1. `BlockEnhancedInput()` - ������ ��������� ���������
2. `RestoreEnhancedInput()` - ������ ��������������� ���������
3. ��������� Verbose ����

### �������� �������������
- ? API �� ���������
- ? Blueprint ������������� ���������
- ? Legacy Input ��-�������� ��������
- ? ����� ������ �����������������

### ���� ��������
1. �������� `.cpp` ����
2. ����������������� ������
3. �������������� �������
4. ��������� ���� (������ ���� "Saved context" � "Restored context")

---
