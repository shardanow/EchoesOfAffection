# Quick Reference - Schedule Data Array Sort Fix

## ��������
Fatal Error (Access Violation) ��� ���������� entries � Schedule Data Asset

## ������� (v1.2.0)

### �� (CRASH)
```cpp
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B) {
    return A.Priority > B.Priority;
});
```

### ����� (SAFE)
```cpp
Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B) {
    if (!A && !B) return false;
    if (!A) return false;
    if (!B) return true;
    return A->Priority > B->Priority;
});
```

## �����
- `ScheduleData.cpp::GetSortedEntries()`
- `ScheduleData.cpp::PostEditChangeProperty()`

## ������
- ������ ��������: `Docs/BUGFIX_Array_Sort_Crash.md`
- CHANGELOG: `CHANGELOG_v1.2_CriticalBugFix.md`

## �������
��� ���������� `TArray<TObjectPtr<T>>`:
1. ��������� `const TObjectPtr<T>&`, �� `const T&`
2. �������� �� `nullptr` ����� ��������������
3. Null-�������� ? � ����� �������
