# Quick Reference - Schedule Data Array Sort Fix

## Проблема
Fatal Error (Access Violation) при добавлении entries в Schedule Data Asset

## Решение (v1.2.0)

### До (CRASH)
```cpp
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B) {
    return A.Priority > B.Priority;
});
```

### После (SAFE)
```cpp
Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B) {
    if (!A && !B) return false;
    if (!A) return false;
    if (!B) return true;
    return A->Priority > B->Priority;
});
```

## Файлы
- `ScheduleData.cpp::GetSortedEntries()`
- `ScheduleData.cpp::PostEditChangeProperty()`

## Ссылки
- Полный документ: `Docs/BUGFIX_Array_Sort_Crash.md`
- CHANGELOG: `CHANGELOG_v1.2_CriticalBugFix.md`

## Правило
При сортировке `TArray<TObjectPtr<T>>`:
1. Используй `const TObjectPtr<T>&`, не `const T&`
2. Проверяй на `nullptr` перед разыменованием
3. Null-элементы ? в конец массива
