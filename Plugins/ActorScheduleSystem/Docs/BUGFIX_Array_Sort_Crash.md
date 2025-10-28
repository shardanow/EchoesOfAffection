# BUGFIX: Access Violation ��� ���������� ������� � Schedule Data Asset

## ��������

��� ���������� ������ �������� � ������ `Entries` � ���� ������ ���������� (`UScheduleData`) ��������� ����������� ������:

```
Exception thrown at 0x000001D3B4B17770 (UnrealEditor-ScheduleCore.dll) in UnrealEditor.exe: 
0xC0000005: Access violation reading location 0x0000000000000068.
```

### ���������������
1. ������� Schedule Data Asset � ���������
2. �������� ������ ������� � ������ `Entries`
3. Fatal Error � Access Violation

## �������

� ������ `UScheduleData::PostEditChangeProperty` � `UScheduleData::GetSortedEntries` ������-������� ���������� ����� ��� ����������� ������:

1. **������������ ��� ���������**: ������ ��������� `const UScheduleEntryData&` ������ `const TObjectPtr<UScheduleEntryData>&`
2. **���������� �������� �� nullptr**: ��� ���������� ������ �������� � ������ �� ����� ���� �������� `nullptr`, ��� ��������� � ������������� �������� ���������

### ���������� ���

```cpp
// ����������� - �������� Access Violation
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B)
{
    return A.Priority > B.Priority; // ������������� ��� ��������
});
```

## �������

���������� ��� ������ � �����������:
1. ����������� ���� ��������� (`TObjectPtr<UScheduleEntryData>`)
2. �������� �� `nullptr` � ���������� ����������

### ������������ ���

```cpp
// ��������� - ���������� ����������
SortedEntries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
{
    // Handle null entries - push them to the end
    if (!A && !B) return false;
    if (!A) return false; // A is null, B comes first
    if (!B) return true;  // B is null, A comes first
    
  return A->Priority > B->Priority;
});
```

## ���������� �����

- `Plugins/ActorScheduleSystem/Source/ScheduleCore/Private/Data/ScheduleData.cpp`
  - `UScheduleData::GetSortedEntries()` - ���������� ������ ����������
  - `UScheduleData::PostEditChangeProperty()` - ���������� ������ ����������

## ������������

����� ���������� �����������:
1. ? ���������� ����� ������� � Schedule Data Asset �������� ��� ������
2. ? �������������� ���������� �� ���������� �������� ���������
3. ? ��������� null-��������� � ������� ���������� ���������

## �������������� ���������

������ ��������� null-����������:
- ���� ��� �������� `null` ? ��� ��������� �������
- ���� ������ A null ? B ����� ���������
- ���� ������ B null ? A ����� ���������
- Null-�������� ������ ����������� � ����� ���������������� �������

## ������������

��� ������ � `TArray<TObjectPtr<T>>` � Unreal Engine ������:
1. ����������� `const TObjectPtr<T>&` � �������, � �� `const T&`
2. ���������� ��������� �� `nullptr` ����� ��������������
3. ������������� null-�������� ���� � �������� ����������/���������

## ������

- ���� �����������: 27.01.2025
- ���������� ������: ScheduleCore
- ������ �������: 1.2+
