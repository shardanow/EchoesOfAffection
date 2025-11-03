# ? Вопросы о сохранении данных - РЕШЕНО

## ?? Вопросы пользователя

### **1. "А эти значения все отношений где-то сохраняются вообще?"**

**Ответ:** ? **ДА**, в двух местах:

#### **Runtime (в памяти):**
```cpp
// URelationshipSubsystem
TMap<FRelationshipKey, FRelationshipData> Relationships;
```
- Хранится пока работает игра
- Очищается при выходе
- **НЕ** сохраняется автоматически

#### **SaveGame (на диске):**
```cpp
// URelationshipSaveGame
{SavedGames}/RelationshipSave.sav
```
- Сохраняется при вызове `SaveRelationships()`
- Загружается при вызове `LoadRelationships()`
- **Требует** ручного вызова

---

### **2. "Их можно сбросить если да то где?"**

**Ответ:** ? **ДА**, несколько способов:

#### **Способ 1: Сброс в Runtime**
```cpp
// Blueprint
GetSubsystem(RelationshipSubsystem)->ClearAllRelationships()

// C++
Subsystem->ClearAllRelationships();
```
**Результат:** Удаляет ВСЕ relationships из памяти ?

---

#### **Способ 2: Удалить один relationship**
```cpp
// Blueprint
RemoveRelationship(Subject, Target)

// C++
Subsystem->RemoveRelationship(Subject, Target);
```
**Результат:** Удаляет ОДИН relationship ?

---

#### **Способ 3: Удалить SaveGame файл**
```cpp
// Blueprint
DeleteSave("RelationshipSave")

// C++
URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
```
**Результат:** Удаляет `.sav` файл с диска ?

---

#### **Способ 4: Полный сброс**
```cpp
void ResetAllRelationshipData()
{
    // Runtime
    Subsystem->ClearAllRelationships();
    
    // SaveGame
    URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
}
```
**Результат:** Полная очистка данных ??

---

## ?? Созданные документы

### **1. SAVE_AND_RESET_GUIDE.md** (~20KB)

**Полное руководство:**
- ? Runtime vs SaveGame хранилище
- ? Детальный анализ `FRelationshipData`
- ? Формат сериализации `FSerializedRelationship`
- ? API для Blueprint и C++
- ? 4 способа сброса данных
- ? Автосохранение (примеры)
- ? Debug console команды
- ? Расположение файлов на диске

**Разделы:**
1. Где используются значения
2. Runtime хранилище
3. SaveGame система
4. Как сохранять/загружать
5. Как сбросить
6. Автоматическое сохранение
7. Debug и тестирование
8. Примеры Blueprint/C++

---

### **2. SAVE_AND_RESET_SUMMARY.md** (~3KB)

**Краткое резюме:**
- ? Быстрые ответы
- ? Таблица сравнения методов
- ? API reference
- ? Важные замечания
- ? Примеры использования
- ? Debug команды

---

## ?? Ключевые находки

### **Хранилище данных:**

| Тип | Местоположение | Lifetime | Автосохранение |
|-----|----------------|----------|----------------|
| **Runtime** | `URelationshipSubsystem::Relationships` (TMap) | Пока игра работает | ? НЕТ |
| **SaveGame** | `{SavedGames}/RelationshipSave.sav` | Постоянно | ? НЕТ |

---

### **Методы сброса:**

| Метод | Что очищает | Постоянно? | Use Case |
|-------|-------------|------------|----------|
| `ClearAllRelationships()` | Runtime | ? | Быстрый сброс в игре |
| `RemoveRelationship()` | Runtime (один) | ? | Точечное удаление |
| `DeleteSave()` | SaveGame | ? | Удаление сохранений |
| Выход без сохранения | Runtime (авто) | ? | Откат изменений |
| Полный сброс | Runtime + SaveGame | ? | New Game |

---

## ?? API Cheatsheet

### **Save/Load:**

```cpp
// Сохранить
URelationshipSaveGameHelper::SaveRelationships(this);

// Загрузить
URelationshipSaveGameHelper::LoadRelationships(this);

// Проверить существование
bool bExists = URelationshipSaveGameHelper::DoesSaveExist("RelationshipSave");

// Удалить
URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
```

---

### **Reset:**

```cpp
// Все relationships
Subsystem->ClearAllRelationships();

// Один relationship
Subsystem->RemoveRelationship(Subject, Target);

// Проверить количество
int32 Count = Subsystem->GetRelationshipCount();
```

---

## ?? Важные замечания

### **1. Автосохранение НЕ включено**

```
Relationships создаются ? Память ? БЕЗ автосохранения
```

**Нужно вызывать вручную:**
- При переходах между уровнями
- Периодически (каждые 5-10 минут)
- При выходе из игры

---

### **2. При Deinitialize данные очищаются**

```cpp
void URelationshipSubsystem::Deinitialize()
{
    ClearAllRelationships();  // ? Автоматически!
}
```

**Вывод:** Обязательно сохранить перед выходом ?

---

### **3. Actor names для идентификации**

```cpp
// Сохранение
SerRel.SubjectActorName = Subject->GetName();  // "BP_Lianne_C_1"

// Загрузка
AActor* Subject = FindActorByName(World, "BP_Lianne_C_1");
```

**Проблема:** Если имя меняется ? relationship не восстановится ?

**Решение:** Использовать **стабильные** имена:
```cpp
Actor->SetActorLabel(TEXT("Lianne"));
```

---

## ?? Рекомендации

### **Когда сохранять:**
- ? При переходе между уровнями
- ? Периодически (Timer, каждые 5 мин)
- ? При выходе из игры
- ? После важных событий

### **Когда загружать:**
- ? При старте игры (`GameInstance::Init()`)
- ? После загрузки уровня

### **Когда сбрасывать:**
- ? New Game ? Полный сброс (Runtime + SaveGame)
- ? Death/Fail ? Откат к Save (LoadRelationships)
- ? Debugging ? ClearAllRelationships()

---

## ?? Примеры использования

### **New Game:**

```cpp
void UMyGameInstance::StartNewGame()
{
    URelationshipSubsystem* Subsystem = GetSubsystem<URelationshipSubsystem>();
    
    // Полный сброс
  Subsystem->ClearAllRelationships();
    URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
    
    // Открыть первый уровень
  UGameplayStatics::OpenLevel(this, "FirstLevel");
}
```

---

### **Continue Game:**

```cpp
void UMyGameInstance::Init()
{
    Super::Init();
    
    // Автозагрузка если существует
    if (URelationshipSaveGameHelper::DoesSaveExist("RelationshipSave"))
    {
        URelationshipSaveGameHelper::LoadRelationships(this);
        UE_LOG(LogTemp, Log, TEXT("Relationships loaded!"));
    }
}
```

---

### **Автосохранение:**

```cpp
void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
  // Сохранение каждые 5 минут
  GetWorldTimerManager().SetTimer(
        AutoSaveTimer,
        this,
    &AMyPlayerController::AutoSave,
        300.0f,  // 5 минут
        true     // Повторять
    );
}

void AMyPlayerController::AutoSave()
{
    URelationshipSaveGameHelper::SaveRelationships(this);
    UE_LOG(LogTemp, Log, TEXT("Relationships auto-saved"));
}
```

---

## ?? Файлы на диске

### **SaveGame расположение:**

```
Windows:
C:\Users\{UserName}\AppData\Local\{ProjectName}\Saved\SaveGames\RelationshipSave.sav

Editor (PIE):
{ProjectFolder}\Saved\SaveGames\RelationshipSave.sav

Linux:
~/.config/Epic/{ProjectName}/Saved/SaveGames/RelationshipSave.sav

Mac:
~/Library/Application Support/Epic/{ProjectName}/Saved/SaveGames/RelationshipSave.sav
```

**Вручную удалить:** Просто удалите `.sav` файл ?

---

## ?? Debug Console Commands

### **Добавить в GameMode:**

```cpp
UFUNCTION(Exec)
void RelDebugCount()
{
    URelationshipSubsystem* Subsystem = GetSubsystem<URelationshipSubsystem>();
    int32 Count = Subsystem->GetRelationshipCount();
    UE_LOG(LogTemp, Warning, TEXT("Active relationships: %d"), Count);
}

UFUNCTION(Exec)
void RelClearAll()
{
    GetSubsystem<URelationshipSubsystem>()->ClearAllRelationships();
    UE_LOG(LogTemp, Warning, TEXT("All relationships cleared!"));
}

UFUNCTION(Exec)
void RelSave()
{
    bool bSuccess = URelationshipSaveGameHelper::SaveRelationships(this);
    UE_LOG(LogTemp, Warning, TEXT("Save %s"), bSuccess ? TEXT("OK") : TEXT("FAIL"));
}

UFUNCTION(Exec)
void RelLoad()
{
    bool bSuccess = URelationshipSaveGameHelper::LoadRelationships(this);
    UE_LOG(LogTemp, Warning, TEXT("Load %s"), bSuccess ? TEXT("OK") : TEXT("FAIL"));
}

UFUNCTION(Exec)
void RelDeleteSave()
{
    bool bSuccess = URelationshipSaveGameHelper::DeleteSave("RelationshipSave");
    UE_LOG(LogTemp, Warning, TEXT("Delete %s"), bSuccess ? TEXT("OK") : TEXT("FAIL"));
}
```

### **Использование в консоли:**

```
` (тильда) ? открыть консоль

RelDebugCount     ? Сколько relationships?
RelClearAll    ? Очистить все
RelSave    ? Сохранить
RelLoad     ? Загрузить
RelDeleteSave     ? Удалить SaveGame
```

---

## ?? Обновленные документы

### **Индекс:**
- ? `Documentation/INDEX.md` - добавлена секция "Сохранение и управление"
- ? Таблица поиска обновлена (2 новых вопроса)

### **README:**
- ? Добавлена ссылка на Save and Reset Guide

### **CHEATSHEET:**
- ? Добавлены быстрые команды Save/Load/Reset
- ? Обновлены полезные ссылки

---

## ? Итоговая схема работы

```
[Старт игры]
      ?
LoadRelationships() ? Если существует SaveGame
      ?
[Игра идет] ? Relationships в памяти (Runtime)
      ?
ModifyDimensionValue(), AddTrait(), etc.
?
SaveRelationships() ? Периодически сохранять
   ?
[Выход из игры]
      ?
Deinitialize() ? ClearAllRelationships()
      ?
SaveGame файл остается на диске (если сохраняли)
```

---

## ?? Все созданные документы

### **Основные:**
1. **SAVE_AND_RESET_GUIDE.md** - Полное руководство (~20KB)
2. **SAVE_AND_RESET_SUMMARY.md** - Краткое резюме (~3KB)

### **Обновленные:**
3. **INDEX.md** - Добавлена навигация
4. **README.md** - Добавлена ссылка
5. **CHEATSHEET.md** - Добавлены команды

---

## ?? Финальный ответ

### **Где сохраняются?**
1. **Runtime:** `URelationshipSubsystem::Relationships` (TMap в памяти)
2. **SaveGame:** `{SavedGames}/RelationshipSave.sav` (файл на диске)

### **Можно сбросить?**
? **ДА!** 4 способа:
1. `ClearAllRelationships()` - Runtime
2. `RemoveRelationship()` - Runtime (один)
3. `DeleteSave()` - SaveGame
4. Полный сброс - Runtime + SaveGame

### **Автосохранение?**
? **НЕТ**, нужно вызывать:
```cpp
URelationshipSaveGameHelper::SaveRelationships(this);
```

### **Где файлы?**
```
Windows: C:\Users\{User}\AppData\Local\{Project}\Saved\SaveGames\
Editor:  {Project}\Saved\SaveGames\
```

---

**Все ответы готовы! Документация полная.** ???

---

**Создано:** 2 ноября 2025  
**Версия:** 1.0  
**Автор:** Technical Documentation  
**GitHub:** https://github.com/shardanow/EchoesOfAffection
