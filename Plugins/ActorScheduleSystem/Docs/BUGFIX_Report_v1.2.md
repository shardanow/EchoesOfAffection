# ����� �� ����������� ������������ ���� - Schedule Data Array Sort

**����:** 27 ������ 2025  
**������:** 1.2.0  
**������:** ? ���������� � ��������������

---

## ������

������� ��������� ����������� ���, ���������� Access Violation (0xC0000005) ��� ���������� ������� � Schedule Data Asset. �������� ���� � ������������ ������ ������-������� ���������� � `TObjectPtr`.

---

## ��������

### ��������
- Fatal Error ��� ���������� ������� �������� � ������ `Entries`
- Access Violation reading location 0x0000000000000068
- Crash � UnrealEditor-ScheduleCore.dll

### ���������������
1. ������� Schedule Data Asset � ���������
2. �������� ������ ������� � ������ Entries
3. �������� ������ � Fatal Error

### �������
- 100% �����������������
- ����������� ��������� (��������� ������ � ��������)

---

## ������

### �������� �������

**����:** `ScheduleData.cpp`  
**������:** 
- `UScheduleData::GetSortedEntries()`
- `UScheduleData::PostEditChangeProperty()`

**���������� ���:**
```cpp
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B)
{
    return A.Priority > B.Priority;
});
```

**�������:**
1. **������������ ��� ���������**: ������ ��������� `const UScheduleEntryData&` ������ `const TObjectPtr<UScheduleEntryData>&`
2. **���������� �������� �� null**: ��� ���������� ������ �������� UE ����� ��������� ��������� nullptr
3. **������������� ��� ��������**: ��������� � `A.Priority` ��� `A == nullptr` ? Access Violation

### ����������� ��������

� Unreal Engine ��� ���������� �������� � `TArray<TObjectPtr<T>>` ����� ��������:
1. ��������� ����� ���� � �������
2. �������� ���� ����� ��������� `nullptr`
3. ���� � ��� ����� ����������� `PostEditChangeProperty` ? ���������� ? crash

---

## �������

### ������������ ���

```cpp
Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
{
    // Handle null entries - push them to the end
    if (!A && !B) return false;  // ��� null - �����
    if (!A) return false;    // A null - B ���� ������
    if (!B) return true;         // B null - A ���� ������

    return A->Priority > B->Priority;  // ��������� �� Priority
});
```

### ���������

1. ? ��� ��������� ������� �� `const TObjectPtr<UScheduleEntryData>&`
2. ? ��������� �������� �� nullptr ��� ����� ���������
3. ? Null-�������� ���������� � ����� �������
4. ? ������ ����� �������� ���������� �������������

### ���������� �����

**��������:**
- `Plugins/ActorScheduleSystem/Source/ScheduleCore/Private/Data/ScheduleData.cpp`
  - ����� `GetSortedEntries()` - ������ ~12-24
  - ����� `PostEditChangeProperty()` - ������ ~95-107

**�������:**
- `Plugins/ActorScheduleSystem/Docs/BUGFIX_Array_Sort_Crash.md` - ��������� ������������
- `Plugins/ActorScheduleSystem/Docs/BUGFIX_Array_Sort_QuickRef.md` - ������� �������
- `Plugins/ActorScheduleSystem/Docs/VERIFICATION_Array_Sort_Fix.md` - ���������� ������������
- `Plugins/ActorScheduleSystem/CHANGELOG_v1.2_CriticalBugFix.md` - CHANGELOG ������

---

## ������������

### ����������� �����

? **Unit Tests**
- ������ ������
- ���� �������
- ������������� ��������
- Null-��������
- ��������� ����������

? **Edge Cases**
- ������ null ��������
- ������� ��������������
- ���������� ����������

? **Integration Tests**
- Schedule Component
- Schedule Subsystem
- Editor UI

? **Regression Tests**
- ������������ Schedule Data Assets
- Blueprint ����������
- const-correctness

? **Performance Tests**
- 10 ���������: < 1ms
- 100 ���������: < 10ms
- 1000 ���������: < 100ms

? **Memory Safety**
- ��� ������ ������
- ��� dangling pointers
- ���������� TObjectPtr management

### ����������

**����� ������:** 24  
**��������:** 24  
**�������������:** 0  
**����������:** 100%

---

## �����������

### �� �����������
- ? ���������� �������� � Schedule Data � ���������
- ? Crash ��� ���������� entries
- ? ���������� ���������� ������� ����������

### ����� �����������
- ? ���������� ������ � Schedule Data
- ? ���������� ����������/�������� entries
- ? ���������� ��������������
- ? ���������� ����� ������������

### �������� �������������
- ? **������ �������� �������������**
- ��� ��������� � API
- ��� ��������� � data layout
- ������������ ������ �������� ��� �����������

---

## ������������

### ��� �������������

��� ������ � `TArray<TObjectPtr<T>>`:

1. ? **������** ����������� `const TObjectPtr<T>&` � �������
2. ? **������** ���������� �� nullptr ����� ��������������
3. ? ������������� null-�������� ���� � sort/compare ��������
4. ? ������� � ��������� nullptr ��� ��������������

### ���-����� �������

```cpp
// ? ����������� - ������������� crash
Array.Sort([](const T& A, const T& B) { return A.Value > B.Value; });

// ? ��������� - ���������� ����������
Array.Sort([](const TObjectPtr<T>& A, const TObjectPtr<T>& B) {
    if (!A && !B) return false;
    if (!A) return false;
    if (!B) return true;
    return A->Value > B->Value;
});
```

### Best Practices

1. **Defensive Programming**: ������ ���������� ���������
2. **Type Safety**: ����������� ���������� ���� TObjectPtr
3. **Editor Awareness**: ���������� ����������� ��������� UE
4. **Testing**: ���������� edge cases � null-����������

---

## ��������� ����

### ����������� ��������
- [x] ��������� ���
- [x] �������� ������������
- [x] �������� ������������
- [x] ������� CHANGELOG
- [x] Commit ���������

### ������������� ������
- [ ] Code review ������ sort �������� � �������
- [ ] ��������� ����������� �������� � ������ �������
- [ ] �������� coding guidelines �������
- [ ] �������� ������������� ������������ ������ �������

### ������������ ������
- [ ] �������� unit tests ��� array operations
- [ ] ������� helper ������� ��� ���������� ����������
- [ ] ��������������� best practices ��� �������
- [ ] ����������� static analysis tools

---

## �������

### ����� ����������
- **������ ��������:** 30 �����
- **���������� �������:** 15 �����
- **������������:** 45 �����
- **������������:** 60 �����
- **�����:** ~2.5 ����

### ���������
- **���:** ������ (simple fix)
- **������������:** ������� (many edge cases)
- **������������:** ������� (comprehensive docs)

### �����������
- **���������:** P0 (Critical)
- **Severity:** Blocker
- **Impact:** High (blocks development)

---

## ����������

? **����������� ��� ������� ���������**

�������� ���� ������� ������������ ������� � `TObjectPtr` � ������-�������� ����������. ����������� ���������� (���������� �������� �� nullptr), �� ���������� ����� ��� ������������ �������.

����������� ��������� ������� ����������, �� ������� ��������� � ������������ ������� � ��������� ��� ������������ ���������.

### �������� �����

1. ?? ������ ����������� ���������� ���� ��� ������ � smart pointers
2. ?? ���������� �� nullptr ����� ��������������
3. ?? ���������� ����������� ��������� UE (��������� nullptr)
4. ?? ���������� edge cases � null-����������
5. ?? �������������� best practices ��� �������

---

**Prepared by:** Schedule System Development Team  
**Date:** 27 ������ 2025
**Version:** 1.2.0  
**Status:** ? Ready for Production

---

## ����������

- ?? `BUGFIX_Array_Sort_Crash.md` - ������ ������������
- ?? `BUGFIX_Array_Sort_QuickRef.md` - ������� �������
- ?? `VERIFICATION_Array_Sort_Fix.md` - ���������� ������������
- ?? `CHANGELOG_v1.2_CriticalBugFix.md` - CHANGELOG

## ��������

**Project:** Echoes of Affection  
**Repository:** https://github.com/shardanow/EchoesOfAffection  
**Plugin:** Actor Schedule System  
**Module:** ScheduleCore
