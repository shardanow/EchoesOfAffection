# Race Condition Fix - Quick Reference

> **Version:** 1.1.1  
> **Date:** 22.01.2025  
> **File:** `DialogueSubsystem.cpp::LoadDialogueAsync()`

---

## ?? TL;DR (Too Long; Didn't Read)

**Проблема:** Callback async загрузки мог вызваться **ДО** того, как `LoadInfo` добавился в `PendingAsyncLoads`.

**Решение:** Изменен порядок операций — теперь добавляем в мапу **ПЕРЕД** запуском загрузки.

**Результат:** Исправлен редкий, но критичный crash при повторной загрузке диалогов.

---

## ? Было (v1.1.0)

```cpp
void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // ...проверки...
    
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
  
    // ? ПРОБЛЕМА: Запуск загрузки ПЕРЕД добавлением в мапу
    LoadInfo.StreamableHandle = StreamableManager.RequestAsyncLoad(
        *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
   {
// ? Если callback вызовется СЕЙЧАС, LoadInfo еще не в мапе!
    if (UDialogueSubsystem* StrongThis = WeakThis.Get())
  {
         StrongThis->OnAsyncLoadCompleted(DialogueId); // <-- не найдет!
            }
        }),
        // ...
    );
    
    // ? Добавление в мапу ПОСЛЕ запуска
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
}
```

### ?? Что могло пойти не так:

1. Asset уже в памяти ? callback вызывается **синхронно**
2. `OnAsyncLoadCompleted()` ищет в `PendingAsyncLoads`
3. **Не находит** ? callback не вызывается
4. Пользователь ждет **вечно**

---

## ? Стало (v1.1.1)

```cpp
void UDialogueSubsystem::LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded)
{
    // ...проверки...
    
    FAsyncLoadInfo LoadInfo;
    LoadInfo.DialogueId = DialogueId;
    LoadInfo.AssetPath = *AssetPath;
    LoadInfo.Callbacks.Add(OnLoaded);
    
    // ? FIX: Добавляем в мапу ПЕРЕД запуском загрузки
    PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));
 
    // ? Получаем ссылку на LoadInfo в мапе (после MoveTemp!)
    FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];

    // Start async load
    UAssetManager& AssetManager = UAssetManager::Get();
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    
    TWeakObjectPtr<UDialogueSubsystem> WeakThis(this);
    
    // ? Теперь безопасно: даже если callback вызовется синхронно, LoadInfo уже в мапе
    LoadInfoRef.StreamableHandle = StreamableManager.RequestAsyncLoad(
  *AssetPath,
        FStreamableDelegate::CreateLambda([WeakThis, DialogueId]()
        {
  if (UDialogueSubsystem* StrongThis = WeakThis.Get())
 {
 // ? Теперь найдет DialogueId в PendingAsyncLoads
     StrongThis->OnAsyncLoadCompleted(DialogueId);
  }
        }),
        // ...
    );
}
```

---

## ?? Ключевые изменения

### 1. Порядок операций

**Было:**
```
1. Создать LoadInfo
2. Запустить async load      ? callback может вызваться ЗДЕСЬ
3. Добавить LoadInfo в map   ? СЛИШКОМ ПОЗДНО!
```

**Стало:**
```
1. Создать LoadInfo
2. ? Добавить LoadInfo в map
3. ? Получить ссылку FAsyncLoadInfo&
4. Запустить async load      ? callback теперь безопасен
```

### 2. Использование ссылки

**Важно:**
```cpp
// После MoveTemp оригинальный LoadInfo пустой!
PendingAsyncLoads.Add(DialogueId, MoveTemp(LoadInfo));

// ? НЕПРАВИЛЬНО: LoadInfo уже пустой
LoadInfo.StreamableHandle = ...;

// ? ПРАВИЛЬНО: Получить ссылку из мапы
FAsyncLoadInfo& LoadInfoRef = PendingAsyncLoads[DialogueId];
LoadInfoRef.StreamableHandle = ...;
```

---

## ?? Производительность

| Метрика | До | После | Изменение |
|---------|-----|-------|-----------|
| CPU overhead | 1 map lookup | 2 map lookups | **+1 lookup** |
| Memory | 0 bytes | 0 bytes | **0** |
| Thread safety | ?? Race condition | ? Safe | **Fixed** |

**Вывод:** Изменение практически **бесплатно**, но исправляет критический баг.

---

## ?? Как протестировать

### Тест 1: Синхронная загрузка
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

// ? Callback должен быть вызван немедленно
check(bCallbackExecuted);
```

### Тест 2: Множественные запросы
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

// ? Все 10 callbacks должны быть вызваны
check(CallbackCount == 10);
```

---

## ?? Lesson Learned

### Async !== Always Async

Многие async API в Unreal могут вызвать callback **синхронно**:

```cpp
// FStreamableManager::RequestAsyncLoad может:
// 1. Вернуть управление немедленно (asset не в памяти)
// 2. Вызвать callback СЕЙЧАС ЖЕ (asset уже в памяти) ? ВАЖНО!
```

### Безопасный паттерн:
```cpp
1. Подготовить состояние
2. Зарегистрировать callback   ? ПЕРЕД запуском async операции!
3. Запустить async операцию
```

---

## ?? Связанные файлы

- `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Subsystems/DialogueSubsystem.cpp`
- `Plugins/DialogueSystem/Docs/Technical/Race_Condition_Fix_v1.1.1.md` (подробности)
- `Plugins/DialogueSystem/CHANGELOG.md` (история изменений)

---

## ? Checklist

- [x] Код скомпилирован без ошибок
- [x] LoadInfo добавляется ДО запуска async load
- [x] Используется ссылка из мапы (не локальная переменная)
- [x] Backwards compatible (API не изменился)
- [x] Документация обновлена
- [x] CHANGELOG обновлен

---

**Status:** ? Fixed  
**Approved By:** DialogueSystem Team  
**Date:** 22.01.2025
