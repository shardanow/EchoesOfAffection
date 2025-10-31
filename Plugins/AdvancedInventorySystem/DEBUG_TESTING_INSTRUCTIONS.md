# ?? Тестирование с логами отладки

## Что изменилось

Добавлены подробные логи в:
1. `CharacterNeedsComponent::ModifyNeedValue_Implementation()` 
2. `ItemEffectExecutorComponent::ExecuteNeedsModifications()`

Теперь мы увидим **ТОЧНО** где теряется вызов!

---

## Шаги для тестирования

### 1. Перезапустить Unreal Editor

**Live Coding / Hot Reload скомпилирует изменения автоматически.**

### 2. Использовать предмет

- Открыть инвентарь
- Кликнуть ПКМ на предмет (Apple)
- Выбрать "Use"

### 3. Проверить Output Log

Открыть **Window ? Developer Tools ? Output Log**

---

## Ожидаемые логи

### Если интерфейс НЕ РЕАЛИЗОВАН:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Error: Target bp_MyAnimeCharacter_C_0 does NOT implement UNeedsSystemInterface!
```

**Решение:** Добавить интерфейс `NeedsSystemInterface` в Blueprint персонажа.

---

### Если интерфейс реализован НО функции пустые:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Log: Target implements interface! Casting...
LogTemp: Log: Cast successful! Executing 1 needs modifications...
LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
```

**НО:**
- **НЕТ** лога "CharacterNeedsComponent::ModifyNeedValue_Implementation called!"

**Решение:** Функции интерфейса пустые! Нужно добавить ноды в `ModifyNeed` Blueprint функции.

---

### Если ВСЕ РАБОТАЕТ ПРАВИЛЬНО:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Log: Target implements interface! Casting...
LogTemp: Log: Cast successful! Executing 1 needs modifications...
LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
LogTemp: Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: Needs.Hunger, Delta: 30.000000, Owner: bp_MyAnimeCharacter_C_0
LogTemp: Log: Current value: 70.000000, New value will be: 100.000000
LogTemp: Need value changed: Needs.Hunger 70.00 ? 100.00
```

---

## Возможные проблемы

### Проблема 1: Интерфейс не реализован

**Лог:**
```
Error: Target does NOT implement UNeedsSystemInterface!
```

**Решение:**
1. `BP_PlayerCharacter` ? **Class Settings**
2. **Interfaces** ? **Add** ? `NeedsSystemInterface`
3. **Compile** ? **Save**

---

### Проблема 2: Функции интерфейса пустые

**Лог:**
```
Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
(но нет вызова CharacterNeedsComponent::ModifyNeedValue_Implementation)
```

**Решение:**

В `BP_PlayerCharacter` ? **My Blueprint** ? **Interfaces** ? **NeedsSystemInterface** ? **ModifyNeed**:

**Добавить ноды:**
```
Event ModifyNeed
  ?
Get Component by Class (CharacterNeedsComponent)
  ?
Modify Need Value
  - Need Tag: Need Tag (из параметра)
  - Delta: Delta Value (из параметра)
```

---

### Проблема 3: Компонент не найден

**Лог:**
```
Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! ...
Error: Need data NOT FOUND for tag: Needs.Hunger
```

**Решение:**
- Проверить что тег `Needs.Hunger` **точно** совпадает с тегом в настройках
- **Project Settings** ? **Gameplay Tags** ? добавить `Needs.Hunger`
- Убедиться что компонент инициализирован (BeginPlay вызван)

---

### Проблема 4: Effect не настроен

**Лог:**
```
Log: Executing 0 needs modifications...
```

**Решение:**
- Открыть Data Asset эффекта (`DA_Effect_RestoreHunger`)
- **Needs Modifications** ? убедиться что массив НЕ пустой
- Добавить модификацию:
  - **Need Tag**: `Needs.Hunger`
  - **Modification Amount**: `30.0`
  - **bClamp Value**: ?

---

## Скриншот ожидаемых логов

Если все работает, вы увидите:

```
[2025.10.29-17.00.00:000][  0]LogTemp: Context Menu: Used item Apple, Success: true
[2025.10.29-17.00.00:001][  0]LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Target implements interface! Casting...
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Cast successful! Executing 1 needs modifications...
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
[2025.10.29-17.00.00:002][  0]LogTemp: Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: Needs.Hunger, Delta: 30.000000, Owner: bp_MyAnimeCharacter_C_0
[2025.10.29-17.00.00:002][  0]LogTemp: Log: Current value: 70.000000, New value will be: 100.000000
[2025.10.29-17.00.00:003][  0]LogNeedsSystem: Need value changed: Needs.Hunger from 70.00 to 100.00
```

---

## Следующие шаги

1. Перезапустить редактор
2. Использовать предмет
3. Скопировать логи из Output Log
4. Отправить мне логи - я скажу где проблема!

**С новыми логами мы ТОЧНО найдем проблему! ??**
