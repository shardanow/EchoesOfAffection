# Relationship System - Setup Issue & Solution

## ?? **Проблема**

При запуске игры панель отношений (Relationship Panel) **НЕ отображается** во время диалогов, несмотря на то, что:

- ? События `GameEvent.Dialogue.Started` успешно получены
- ? `SetActors()` вызывается корректно
- ? Панель меняет visibility на `SelfHitTestInvisible`

---

## ?? **Логи из игры**

### Проблема 1: Database не настроен
```
[2025.11.02-18.34.15:551][ 29]LogRelationshipSystem: Warning: No Relationship Database configured in Project Settings
[2025.11.02-18.34.15:644][ 29]LogRelationshipSystem: Error: No database available
[2025.11.02-18.34.19:473][454]LogTemp: RelationshipPanel: Dialogue.Started - Player=bp_MyAnimeCharacter_C_0, NPC=BP_Lianne_C_1
[2025.11.02-18.34.19:473][454]LogTemp: RelationshipPanel: SetActors called, visibility changed
```

### Проблема 2: GameplayTags не зарегистрированы
```
Ensure condition failed: false
Requested Gameplay Tag Relationship.DimensionChanged was not found, tags must be loaded from config or registered as a native tag

Ensure condition failed: false
Requested Gameplay Tag Relationship.ActionExecuted was not found, tags must be loaded from config or registered as a native tag
```

---

## ?? **Причины проблем**

### Проблема 1: **Relationship Database НЕ НАСТРОЕН** в Project Settings

#### Что происходит:

1. `RelationshipSubsystem::Initialize()` пытается загрузить Database из Settings
2. `GetDatabase()` возвращает `nullptr` (Database не найден)
3. `CreateRelationship()` **НЕ СОЗДАЁТ** отношение (No database available)
4. `RelationshipPanel::CreateDimensionBars()` **НЕ МОЖЕТ** создать dimension bars (нет Database)
5. **Панель остаётся пустой**, даже если она видима

### Проблема 2: **GameplayTags для событий НЕ ЗАРЕГИСТРИРОВАНЫ**

#### Что происходит:

RelationshipSubsystem использует следующие теги для событий:
- `Relationship.DimensionChanged` - при изменении dimension
- `Relationship.StateTransitioned` - при смене state
- `Relationship.TraitAdded` - при добавлении trait
- `Relationship.TraitRemoved` - при удалении trait
- `Relationship.ActionExecuted` - при выполнении action

Если эти теги не зарегистрированы, Unreal Engine выдаёт **Ensure** и система событий не работает.

---

## ?? **Файловая структура**

### Database Asset существует:
```
D:\Game Dev\Projects\UEProjects\EchoesOfAffection\
??? Content\
    ??? Game\
??? Core\
  ??? Subsystems\
      ??? RelationshipSystem\
  ??? DA_ReleationshipDatabase.uasset  ?? (опечатка в названии)
```

**Примечание:** В названии asset'а опечатка: `Releationship` вместо `Relationship`

---

## ? **Решение 1: Настройка Database**

### **Способ 1: Через Project Settings (РЕКОМЕНДУЕТСЯ)**

1. Откройте **Edit ? Project Settings**
2. В левом меню найдите **"Relationship System"**
3. В секции **"Database"** выберите:
   ```
   /Game/Game/Core/Subsystems/RelationshipSystem/DA_ReleationshipDatabase
   ```
4. Настройте дополнительные параметры:
   - **Enable Automatic Decay:** `True` (опционально)
   - **Decay Processing Interval:** `60.0` секунд
   - **Game Hours Per Real Second:** `1.0`
   - **Enable Debug Logging:** `True` (для диагностики)
5. **Сохраните** настройки (Ctrl+S)
6. **Перезапустите** редактор Unreal Engine

---

### **Способ 2: Через Config файл**

Добавьте в `Config/DefaultGame.ini`:

```ini
[/Script/RelationshipSystemCore.RelationshipSystemSettings]
Database=/Game/Game/Core/Subsystems/RelationshipSystem/DA_ReleationshipDatabase.DA_ReleationshipDatabase
bEnableAutomaticDecay=True
DecayProcessingInterval=60.0
GameHoursPerRealSecond=1.0
bEnableDebugLogging=True
bShowDebugInfo=False
```

**После добавления:**
1. Сохраните файл
2. Перезапустите редактор

---

### **Способ 3: Через Blueprint (временное решение)**

В вашем **GameMode** или **GameInstance Blueprint**:

1. В **Event BeginPlay** добавьте:
   ```
   Get Relationship Subsystem
   ? Set Database Override
      ? Database: /Game/Game/.../DA_ReleationshipDatabase
   ```

Или в **C++** (`GameMode::BeginPlay`):

```cpp
if (UWorld* World = GetWorld())
{
    if (URelationshipSubsystem* RelSys = World->GetSubsystem<URelationshipSubsystem>())
    {
        URelationshipDatabase* DB = LoadObject<URelationshipDatabase>(
            nullptr, 
        TEXT("/Game/Game/Core/Subsystems/RelationshipSystem/DA_ReleationshipDatabase.DA_ReleationshipDatabase")
        );
        
        if (DB)
        {
   RelSys->SetDatabaseOverride(DB);
            UE_LOG(LogTemp, Log, TEXT("? RelationshipDatabase override set successfully!"));
        }
    }
}
```

---

## ? **Решение 2: Регистрация GameplayTags**

### **Способ 1: Через Project Settings (РЕКОМЕНДУЕТСЯ)**

1. Откройте **Edit ? Project Settings**
2. В левом меню найдите **"Project - GameplayTags"**
3. В секции **"Gameplay Tag Table List"** нажмите **"+"** для добавления нового источника тегов
4. Создайте новый **Data Table** типа `GameplayTagTableRow`:
   - Правый клик в Content Browser ? **Miscellaneous ? Data Table**
 - Выберите **GameplayTagTableRow** как Row Structure
   - Назовите: `DT_RelationshipSystemTags`
5. Откройте `DT_RelationshipSystemTags` и добавьте следующие строки:

| Tag | DevComment |
|-----|------------|
| `Relationship.DimensionChanged` | Fired when a relationship dimension value changes |
| `Relationship.StateTransitioned` | Fired when relationship state transitions to another |
| `Relationship.TraitAdded` | Fired when a trait is added to a relationship |
| `Relationship.TraitRemoved` | Fired when a trait is removed from a relationship |
| `Relationship.ActionExecuted` | Fired when a relationship action is executed |

6. **Сохраните** Data Table
7. В **Project Settings ? GameplayTags ? Gameplay Tag Table List** добавьте ссылку на `DT_RelationshipSystemTags`
8. **Перезапустите** редактор

---

### **Способ 2: Через DefaultGameplayTags.ini**

Создайте или отредактируйте файл `Config/DefaultGameplayTags.ini`:

```ini
[/Script/GameplayTags.GameplayTagsSettings]
ImportTagsFromConfig=True

+GameplayTagList=(Tag="Relationship.DimensionChanged",DevComment="Fired when a relationship dimension value changes")
+GameplayTagList=(Tag="Relationship.StateTransitioned",DevComment="Fired when relationship state transitions to another")
+GameplayTagList=(Tag="Relationship.TraitAdded",DevComment="Fired when a trait is added to a relationship")
+GameplayTagList=(Tag="Relationship.TraitRemoved",DevComment="Fired when a trait is removed from a relationship")
+GameplayTagList=(Tag="Relationship.ActionExecuted",DevComment="Fired when a relationship action is executed")
```

**После добавления:**
1. Сохраните файл
2. Перезапустите редактор

---

### **Способ 3: Через Native Tags (для разработчиков плагина)**

Добавьте в `RelationshipSystemCore.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] { 
    "Core", 
    "CoreUObject", 
    "Engine",
    "GameplayTags" // Добавьте эту зависимость
});
```

Создайте файл `RelationshipSystemTags.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace RelationshipSystemTags
{
    // Event tags
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relationship_DimensionChanged);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relationship_StateTransitioned);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relationship_TraitAdded);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relationship_TraitRemoved);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relationship_ActionExecuted);
}
```

И `RelationshipSystemTags.cpp`:

```cpp
#include "RelationshipSystemTags.h"

namespace RelationshipSystemTags
{
    UE_DEFINE_GAMEPLAY_TAG(Relationship_DimensionChanged, "Relationship.DimensionChanged");
    UE_DEFINE_GAMEPLAY_TAG(Relationship_StateTransitioned, "Relationship.StateTransitioned");
    UE_DEFINE_GAMEPLAY_TAG(Relationship_TraitAdded, "Relationship.TraitAdded");
    UE_DEFINE_GAMEPLAY_TAG(Relationship_TraitRemoved, "Relationship.TraitRemoved");
 UE_DEFINE_GAMEPLAY_TAG(Relationship_ActionExecuted, "Relationship.ActionExecuted");
}
```

---

## ?? **Ожидаемый результат после настройки**

После правильной настройки Database **И** регистрации GameplayTags логи должны показать:

```
LogRelationshipSystem: RelationshipSubsystem initialized
LogRelationshipSystem: Building Relationship Database indices...
LogRelationshipSystem: Database indexed: 2 dimensions, 2 states, 1 traits, 0 rules, 1 actions, 1 profiles
LogRelationshipSystem: Created relationship: BP_Lianne_C_1 -> bp_MyAnimeCharacter_C_0 (State: Relationship.State.Stranger)
LogTemp: RelationshipPanel: Dialogue.Started - Player=bp_MyAnimeCharacter_C_0, NPC=BP_Lianne_C_1
LogTemp: RelationshipPanel: SetActors called, visibility changed
```

**БЕЗ ошибок** о missing GameplayTags!

**Панель отношений появится** с dimension bars (Trust, Friendship, и т.д.)!

---

## ?? **Проверка настройки Database Asset**

Откройте `DA_ReleationshipDatabase` в редакторе и убедитесь, что настроены:

### **1. Dimensions** (минимум 2):
- **Trust**
  - Tag: `Relationship.Dimension.Trust`
  - Min Value: `-100`
  - Max Value: `100`
  - Default Value: `0`
  - Decay Rate: `0.5` (опционально)

- **Friendship**
- Tag: `Relationship.Dimension.Friendship`
  - Min Value: `0`
  - Max Value: `100`
  - Default Value: `10`
  - Decay Rate: `0.3` (опционально)

### **2. States** (минимум 2):
- **Stranger**
  - Tag: `Relationship.State.Stranger`
  - Is Default State: `?`
  - Display Name: "Stranger"

- **Acquaintance**
  - Tag: `Relationship.State.Acquaintance`
  - Display Name: "Acquaintance"
  - Transitions to: Friend (при Trust > 30)

### **3. Default Profile**:
- **Create:** `DA_RelationshipProfile_Default`
- **Initial State:** `Relationship.State.Stranger`
- **Initial Dimensions:**
  - Trust: `0`
  - Friendship: `10`

---

## ?? **Проверка регистрации GameplayTags**

### Через редактор:

1. **Window ? Developer Tools ? Gameplay Tags**
2. В поиске введите: `Relationship.`
3. Убедитесь, что видите все 5 тегов:
   - ? `Relationship.DimensionChanged`
   - ? `Relationship.StateTransitioned`
   - ? `Relationship.TraitAdded`
   - ? `Relationship.TraitRemoved`
   - ? `Relationship.ActionExecuted`

### Через логи:

После запуска игры **НЕ ДОЛЖНО** быть:
```
? Requested Gameplay Tag Relationship.DimensionChanged was not found
? Requested Gameplay Tag Relationship.ActionExecuted was not found
```

---

## ?? **Дополнительная диагностика**

### Если панель всё ещё не появляется:

1. **Проверьте Widget Blueprint:**
   - `WBP_RelationshipPanel` содержит `VerticalBox` с именем `DimensionContainer`
   - `DimensionBarClass` установлен на `WBP_RelationshipDimensionBar`
   - `bShowAllDimensions` = `True` (для показа всех dimensions)

2. **Проверьте NPCRelationshipComponent:**
   - На `BP_Lianne_C` добавлен `NPCRelationshipComponent`
   - `Relationship Profile` установлен (или используется Default)

3. **Проверьте логи:**
   ```
   LogRelationshipSystem: Created relationship: ...
   LogTemp: RelationshipPanel: SetActors called...
   LogTemp: RelationshipPanel: CreateDimensionBars - создаёт bars...
   ```

4. **Проверьте GameplayTags:**
   - Откройте **Window ? Developer Tools ? Gameplay Tags**
   - Убедитесь, что все теги `Relationship.*` зарегистрированы

---

## ?? **Контрольный список**

### Database Setup:
- [ ] Database asset создан (`DA_ReleationshipDatabase`)
- [ ] Database настроен в Project Settings
- [ ] Database содержит минимум 2 Dimensions
- [ ] Database содержит минимум 2 States
- [ ] Default Profile настроен

### GameplayTags Setup:
- [ ] Теги `Relationship.*` зарегистрированы в Project Settings
- [ ] Теги видны в **Window ? Developer Tools ? Gameplay Tags**
- [ ] Нет ошибок "Tag was not found" в логах

### Component Setup:
- [ ] NPCRelationshipComponent добавлен на NPC
- [ ] Widget содержит DimensionContainer
- [ ] DimensionBarClass установлен
- [ ] Редактор перезапущен после настройки

---

## ?? **Следующие шаги**

После успешной настройки Database **И** GameplayTags:

1. **Запустите игру** (PIE)
2. **Начните диалог** с Lianne
3. **Проверьте**, что панель отображается с dimension bars
4. **Проверьте логи** - не должно быть ошибок о missing tags
5. **Тестируйте** изменение значений dimensions через actions
6. **Проверьте**, что события `Relationship.*` эмитятся корректно

---

## ?? **Список всех необходимых GameplayTags**

### Для RelationshipSystem:

#### События системы:
```
Relationship.DimensionChanged
Relationship.StateTransitioned
Relationship.TraitAdded
Relationship.TraitRemoved
Relationship.ActionExecuted
```

#### Dimensions (примеры):
```
Relationship.Dimension.Trust
Relationship.Dimension.Friendship
Relationship.Dimension.Respect
Relationship.Dimension.Fear
Relationship.Dimension.Romance
```

#### States (примеры):
```
Relationship.State.Stranger
Relationship.State.Acquaintance
Relationship.State.Friend
Relationship.State.BestFriend
Relationship.State.Enemy
Relationship.State.Lover
```

#### Traits (примеры):
```
Relationship.Trait.FirstImpression
Relationship.Trait.SharedSecret
Relationship.Trait.Betrayed
Relationship.Trait.Grateful
```

#### Actions (примеры):
```
Relationship.Action.Greet
Relationship.Action.Compliment
Relationship.Action.Insult
Relationship.Action.Gift
Relationship.Action.Help
```

---

## ?? **Дополнительная помощь**

Если проблема сохраняется:

1. Включите **Debug Logging** в Settings
2. Проверьте **Output Log** на наличие ошибок:
   - Ошибки о missing Database
   - Ошибки о missing GameplayTags
   - Ошибки о missing components
3. Убедитесь, что все **Gameplay Tags** зарегистрированы через **Window ? Developer Tools ? Gameplay Tags**
4. Проверьте **TROUBLESHOOTING.md** для дополнительных решений
5. Проверьте, что плагин **GameplayTags** включён в Project Settings

---

## ??? **Частые ошибки**

### 1. "Requested Gameplay Tag X was not found"
**Решение:** Зарегистрируйте теги через Project Settings ? GameplayTags

### 2. "No Relationship Database configured"
**Решение:** Установите Database в Project Settings ? Relationship System

### 3. Панель видима, но пустая
**Решение:** Проверьте, что Database содержит Dimensions и создано отношение

### 4. Dimension bars не обновляются
**Решение:** Проверьте, что GameplayTags для событий зарегистрированы

### 5. NPCRelationshipComponent не создаёт отношение
**Решение:** Убедитесь, что Database и Profile настроены

---

**Дата создания:** 2025-01-02  
**Автор:** GitHub Copilot  
**Версия:** 2.0  
**Последнее обновление:** 2025-01-02 (добавлена секция GameplayTags)
