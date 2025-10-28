# CHANGELOG v1.2 - Critical Bug Fix

## Version 1.2.0 (2025-01-27)

### Critical Bug Fixes

#### ?? FIXED: Access Violation ��� ���������� ������� � Schedule Data Asset

**��������:**
��� ���������� ������� ��� ����������� ��������� � ������ `Entries` � Schedule Data Asset ���������� Fatal Error:
```
Exception thrown at 0x000001D3B4B17770 (UnrealEditor-ScheduleCore.dll) in UnrealEditor.exe: 
0xC0000005: Access violation reading location 0x0000000000000068.
```

**�������:**
� ������� `UScheduleData::GetSortedEntries()` � `UScheduleData::PostEditChangeProperty()` ������-������� ���������� ������������ ������������ ���� ���������� � �� ��������� ��������� �� `nullptr`:

```cpp
// ���������� ���
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B)
{
    return A.Priority > B.Priority; // Access Violation ��� A ��� B == nullptr
});
```

��� ���������� ������ �������� � ������ ����� �������� UE ����� �������� ��������� `nullptr` ��������, ��� ��������� � ������������� �������� ���������.

**�������:**
1. ��������� ��� ���������� ������ �� `const TObjectPtr<UScheduleEntryData>&`
2. ��������� �������� �� `nullptr` � ���������� ���������� null-���������

```cpp
// ������������ ���
Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
{
    // Handle null entries - push them to the end
    if (!A && !B) return false;
    if (!A) return false; // A is null, B comes first
    if (!B) return true;  // B is null, A comes first
    
  return A->Priority > B->Priority;
});
```

**���������� �����:**
- `Plugins/ActorScheduleSystem/Source/ScheduleCore/Private/Data/ScheduleData.cpp`
  - ����� `GetSortedEntries()`
  - ����� `PostEditChangeProperty()`

**������������:**
- ������ �������� `Docs/BUGFIX_Array_Sort_Crash.md` � ��������� ��������� �������� � �������

### �����������

**�� �����������:**
- ? ���������� �������� ����� ����� ������ � Schedule Data Asset
- ? Crash ��� �������������� ������������ Schedule Data � ����������� entries
- ? �������������� �� ���������� �������� Access Violation

**����� �����������:**
- ? ���������� ���������� ������ ���������� �������
- ? ���������� �������������� ���������� �� ����������
- ? ���������� ��������� null-��������� � �������
- ? ���������� ������ ��������� Schedule Data Asset

### ����������� ������

#### ������ ��������� null-����������
```cpp
if (!A && !B) return false;  // ��� null - ��������� �������
if (!A) return false;    // A null - B ����� ���������
if (!B) return true;         // B null - A ����� ���������
// ����� - ��������� �� Priority
```

Null-�������� ������ ���������� � ����� ���������������� �������.

#### ������������ ��� �������������

��� ������ � `TArray<TObjectPtr<T>>` � Unreal Engine:
1. ? ������ ����������� `const TObjectPtr<T>&` � �������, � �� `const T&`
2. ? ���������� ��������� �� `nullptr` ����� ��������������
3. ? ������������� null-�������� ���� � �������� ����������/���������
4. ? �������, ��� UE ����� ��������� ��������� null-�������� ��� �������������� ��������

### �������� �������������

? **������ �������� �������������**
- ��� ��������� � API
- ��� ��������� � data layout
- ������������ Schedule Data Assets �������� ��� ���������
- ��������� ����������� � ����� ���������� ������

### ������������

���������:
- ? ���������� ������ �������� � ������ Schedule Data Asset
- ? ���������� ���������� ��������� ������
- ? ��������� ���������� ������������ ���������
- ? �������� ���������
- ? �������������� ��� ���������� `bAutoSortByPriority`
- ? ������ � null-���������� � �������

### ��������� ���������

��� ������ ��������� � ���� ������.

### ����������

1. �������� ���� `ScheduleData.cpp`
2. ���������������� ������
3. ������������� ��������
4. ���������, ��� ������������ Schedule Data ����������� ���������

### ��������� ����

����� ����� ����������� �������������:
1. ��������� ��� ������������ Schedule Data Assets
2. �������� ������������ �������
3. �������� ������������� ������������ ������� ����������

---

## ������� ������

- **v1.2.0** (2025-01-27): Critical bug fix - Access Violation ��� ���������� entries
- **v1.1.0**: Road Navigation System
- **v1.0.0**: Initial Release

## ������

- �������� CHANGELOG: `CHANGELOG.md`
- ������������ �� ��� �����: `Docs/BUGFIX_Array_Sort_Crash.md`
- Road Navigation CHANGELOG: `CHANGELOG_v1.0_RoadNavigation.md`
- Loop Actions CHANGELOG: `CHANGELOG_v1.1_LoopActions.md`

## ��������

**Project**: Echoes of Affection  
**Repository**: https://github.com/shardanow/EchoesOfAffection  
**Plugin**: Actor Schedule System

---

*���� CHANGELOG ��������� ����������� ����������� ����, ������������� 27 ������ 2025 ����.*
