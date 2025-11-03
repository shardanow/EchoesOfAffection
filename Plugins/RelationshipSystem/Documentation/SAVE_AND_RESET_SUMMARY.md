# ? Сохранение и сброс - Краткое резюме

## ?? Быстрые ответы

### **Где сохраняются relationships?**

**В двух местах:**
1. ? **Runtime** (в памяти): `URelationshipSubsystem::Relationships` (TMap)
2. ? **SaveGame** (на диске): `{SavedGames}/RelationshipSave.sav`

---

### **Можно ли сбросить?**

? **ДА!** Несколько способов:

```cpp
// Вариант 1: Сброс в Runtime (временно)
Subsystem->ClearAllRelationships();

// Вариант 2: Удалить SaveGame (постоянно)
URelationshipSaveGameHelper::DeleteSave("RelationshipSave");

// Вариант 3: Полный сброс
Subsystem->ClearAllRelationships();
URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
```

---

## ?? Сравнение методов

| Метод | Runtime | SaveGame | Permanent |
|-------|---------|----------|-----------|
| **ClearAllRelationships()** | ? | ? | ? |
| **RemoveRelationship()** | ? (один) | ? | ? |
| **DeleteSave()** | ? | ? | ? |
| **Выход без Save** | ? (авто) | ? | ? |
| **Полный сброс** | ? | ? | ? |

---

## ?? API Reference

### **Сохранение:**

```cpp
// Blueprint
SaveRelationships("RelationshipSave")

// C++
URelationshipSaveGameHelper::SaveRelationships(this);
```

---

### **Загрузка:**

```cpp
// Blueprint
LoadRelationships("RelationshipSave")

// C++
URelationshipSaveGameHelper::LoadRelationships(this);
```

---

### **Сброс:**

```cpp
// Blueprint
ClearAllRelationships()

// C++
Subsystem->ClearAllRelationships();
```

---

### **Удаление SaveGame:**

```cpp
// Blueprint
DeleteSave("RelationshipSave")

// C++
URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
```

---

## ?? Важно

### **Автосохранение НЕ включено:**

```
Relationships ? Память ? БЕЗ автосохранения
```

**Нужно вызывать вручную:**
- При переходах между уровнями
- Периодически (каждые 5-10 мин)
- При выходе из игры

---

### **При выходе данные теряются:**

```cpp
void URelationshipSubsystem::Deinitialize()
{
    ClearAllRelationships();  // ? Автоматически!
}
```

**Вывод:** Обязательно сохранять перед выходом ?

---

## ?? Примеры использования

### **New Game:**

```cpp
void StartNewGame()
{
    // Полный сброс
    Subsystem->ClearAllRelationships();
    URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
    
    // Открыть первый уровень
 OpenLevel("FirstLevel");
}
```

---

### **Continue Game:**

```cpp
void ContinueGame()
{
    // Загрузить сохранение
    if (URelationshipSaveGameHelper::DoesSaveExist("RelationshipSave"))
    {
      URelationshipSaveGameHelper::LoadRelationships(this);
    }
    
    OpenLevel("LastLevel");
}
```

---

### **Автосохранение:**

```cpp
void AMyGameMode::BeginPlay()
{
    // Сохранять каждые 5 минут
    GetWorldTimerManager().SetTimer(
        AutoSaveHandle,
        this,
        &AMyGameMode::AutoSave,
     300.0f,  // 5 минут
        true
    );
}

void AMyGameMode::AutoSave()
{
    URelationshipSaveGameHelper::SaveRelationships(this);
}
```

---

## ?? Debug команды

```cpp
UFUNCTION(Exec)
void RelDebugCount();  // Сколько relationships?

UFUNCTION(Exec)
void RelClearAll();    // Очистить все

UFUNCTION(Exec)
void RelSave(); // Сохранить

UFUNCTION(Exec)
void RelLoad();      // Загрузить

UFUNCTION(Exec)
void RelDeleteSave();  // Удалить SaveGame
```

**В консоли:**
```
RelDebugCount
RelClearAll
RelSave
RelLoad
RelDeleteSave
```

---

## ?? Где находятся файлы?

### **SaveGame location:**

```
Windows:
C:\Users\{User}\AppData\Local\{Project}\Saved\SaveGames\RelationshipSave.sav

Editor (PIE):
{ProjectFolder}\Saved\SaveGames\RelationshipSave.sav
```

**Вручную удалить:** Просто удалите `.sav` файл ?

---

## ?? Полная документация

**[SAVE_AND_RESET_GUIDE.md](SAVE_AND_RESET_GUIDE.md)** - Детальное руководство:
- Runtime vs SaveGame хранилище
- Все методы сохранения/загрузки
- Автосохранение
- Debug / тестирование
- Примеры Blueprint/C++

---

## ? Итого

### **Где данные?**
- **Runtime:** В памяти (`TMap`)
- **SaveGame:** На диске (`.sav`)

### **Автосохранение?**
- ? НЕТ, вызывать вручную

### **Как сбросить?**
- **Временно:** `ClearAllRelationships()`
- **Постоянно:** `DeleteSave()`
- **Полностью:** Оба метода

### **Обязательно:**
- ? Сохранять перед выходом
- ? Загружать при старте
- ? Тестировать сброс

---

**Создано:** 2 ноября 2025  
**См. также:** [SAVE_AND_RESET_GUIDE.md](SAVE_AND_RESET_GUIDE.md)
