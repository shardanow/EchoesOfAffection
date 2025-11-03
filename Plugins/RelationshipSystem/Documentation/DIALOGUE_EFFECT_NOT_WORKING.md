# DialogueEffect не срабатывает - Диагностика

## ?? Проблема

DialogueEffect_ModifyRelationship в режиме **ExecuteAction** не изменяет Trust при выполнении диалога.

**Симптомы:**
- ? Effect настроен в DialogueNode
- ? Mode = ExecuteAction
- ? ActionTag выбран
- ? Значение Trust не меняется
- ? Виджет не обновляется

---

## ?? Пошаговая диагностика

### **Шаг 1: Проверить логи**

Откройте **Output Log** и запустите диалог. Ищите сообщения:

#### **? Нормальные логи (если всё работает):**

```
LogTemp: UDialogueEffect_ModifyRelationship: Executed action 'Relationship.Action.GiveCompliment'
LogRelationshipSystem: Executed action Relationship.Action.GiveCompliment: BP_Lianne_C -> ThirdPersonCharacter_C
LogRelationshipSystem: Modified dimension 'Relationship.Dimension.Trust' by 5.0
```

#### **? Проблемные логи:**

**1. RelationshipComponent не найден:**
```
LogTemp: Verbose: UDialogueEffect_ModifyRelationship: RelationshipComponent not found on BP_Lianne_C (RelationshipSystem plugin may not be enabled)
```

**Решение:** NPC не имеет `NPCRelationshipComponent`

---

**2. ExecuteAction функция не найдена:**
```
LogTemp: Warning: UDialogueEffect_ModifyRelationship: ExecuteAction function not found
```

**Решение:** Компонент есть, но это не `UNPCRelationshipComponent`

---

**3. Action не может выполниться:**
```
LogRelationshipSystem: Warning: Action not found: Relationship.Action.GiveCompliment
```

**Решение:** ActionTag неправильный или DataAsset не существует

---

**4. Нет логов вообще:**

**Решение:** DialogueEffect вообще не выполняется (см. Шаг 2)

---

### **Шаг 2: Проверить что NPC имеет NPCRelationshipComponent**

#### **В Blueprint BP_Lianne:**

1. Откройте Blueprint NPC
2. Проверьте **Components** панель
3. Должен быть **NPCRelationshipComponent**

```
Components:
  ? NPCRelationshipComponent
    Profile: DA_Profile_Lianne
```

#### **Если компонента нет:**

**Добавить:**
1. Components Panel ? **Add Component**
2. Найти **NPC Relationship Component**
3. Добавить
4. Назначить Profile (опционально)

---

### **Шаг 3: Проверить что Relationship создан**

#### **Relationship должен быть создан ПЕРЕД диалогом:**

```cpp
// Вариант 1: В BeginPlay NPC
void ABP_Lianne::BeginPlay()
{
    Super::BeginPlay();
    
    UNPCRelationshipComponent* RelComp = 
    FindComponentByClass<UNPCRelationshipComponent>();
    
    if (RelComp)
    {
        // Получить Player
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
      AActor* Player = PC ? PC->GetPawn() : nullptr;
        
        if (Player)
        {
   RelComp->CreateRelationshipWith(Player);
      }
    }
}
```

#### **Проверить в Runtime:**

1. Запустить игру
2. Открыть консоль (~)
3. Выполнить команду:

```
RelDebugCount
```

**Ожидается:**
```
LogTemp: Warning: Active relationships: 1
```

**Если 0:**
- Relationship не создан
- Нужно создать при первой встрече

---

### **Шаг 4: Проверить ActionTag**

#### **ActionTag должен быть корректным:**

```
? Правильно: Relationship.Action.GiveCompliment
? Неправильно: Action.GiveCompliment
? Неправильно: Relationship.Action.ComplimentGive
```

#### **Как проверить:**

1. Откройте **DialogueNode** с Effect
2. Проверьте **ActionTag** в Details
3. Сравните с DataAsset:

```
DA_Action_GiveCompliment:
  AssetTag: Relationship.Action.GiveCompliment  ? Должно совпадать
```

#### **Получить список доступных Actions:**

В **RelationshipDatabase** ? **Actions** ? посмотреть все AssetTags

---

### **Шаг 5: Проверить Action DataAsset**

#### **DA_Action_GiveCompliment должен существовать:**

```
Content/Data/Relationships/Actions/DA_Action_GiveCompliment

Настройки:
  AssetTag: Relationship.Action.GiveCompliment
  Effects:
  [0]
      DimensionTag: Relationship.Dimension.Trust
      BaseValue: 5.0  ? Насколько увеличивать
      SuccessProbability: 1.0  ? 100% успех
```

#### **Проверить Effects:**

```cpp
Effects:
  [0]
    DimensionTag: Relationship.Dimension.Trust  ? Правильный Tag?
    BaseValue: 5.0  ? Не 0?
    RandomVariance: 0.0  ? Опционально
    SuccessProbability: 1.0  ? Не 0?
```

---

### **Шаг 6: Проверить CanExecuteAction**

#### **Action может не выполниться если:**

1. **Costs не выполнены:**

```cpp
// В DA_Action_GiveCompliment:
Costs:
  [0]
    DimensionTag: Relationship.Dimension.Friendship
    MinimumValue: 20  ? Требуется Friendship >= 20
```

**Проблема:** Если Friendship < 20, Action НЕ выполнится

**Решение:**
- Убрать Costs
- Или увеличить Friendship

---

2. **Cooldown активен:**

```cpp
Cooldown: 3600  ? 1 час
```

**Проблема:** Action может выполниться только раз в час

**Решение:**
- Уменьшить Cooldown
- Или подождать

---

3. **MaxUses достигнут:**

```cpp
MaxUses: 3  ? Максимум 3 раза
```

**Проблема:** Action уже использовался 3 раза

**Решение:**
- Увеличить MaxUses
- Или использовать другой Action

---

### **Шаг 7: Проверить bApplyToPlayer**

#### **Важно понимать направление:**

```cpp
// В DialogueEffect_ModifyRelationship:

bApplyToPlayer = false (по умолчанию):
  Subject: NPC (Lianne)
  Target: Player
  ? NPC выполняет Action на Player
  ? Изменяется отношение NPC к Player

bApplyToPlayer = true:
  Subject: Player
  Target: NPC (Lianne)
  ? Player выполняет Action на NPC
  ? Изменяется отношение Player к NPC
```

#### **Типичная ошибка:**

```
bApplyToPlayer = false
ActionTag: Relationship.Action.GiveCompliment

Player НЕ имеет NPCRelationshipComponent
? Effect НЕ выполнится ?
```

**Решение:**
- `bApplyToPlayer = false` (NPC ? Player) - рекомендуется
- Или добавить RelationshipComponent на Player

---

### **Шаг 8: Проверить что DialogueEffect вообще выполняется**

#### **Добавить Debug лог:**

В **DialogueNode** ? **Effects** ? перед `Modify Relationship` добавить **Print String**:

```
DialogueNode Effects:
  [0] Print String: "Executing DialogueEffect"
  [1] Modify Relationship (ExecuteAction)
```

**Если "Executing DialogueEffect" НЕ появляется:**
- DialogueNode вообще не выполняется
- Проблема в DialogueSystem, не в RelationshipSystem

---

## ? Правильная настройка (пошагово)

### **1. Создать Action DataAsset**

```
Right Click ? Relationship System ? Relationship Action
Name: DA_Action_GiveCompliment

Settings:
  AssetTag: Relationship.Action.GiveCompliment

  Effects:
    [0]
      DimensionTag: Relationship.Dimension.Trust
      BaseValue: 5.0
      SuccessProbability: 1.0
  
  Costs: (оставить пустым)
  Cooldown: 0
  MaxUses: 0
```

---

### **2. Добавить Action в Database**

```
DA_RelationshipDatabase:
  Actions:
    - DA_Action_GiveCompliment  ? Добавить
```

---

### **3. Добавить NPCRelationshipComponent на NPC**

```
BP_Lianne:
  Components:
    + NPCRelationshipComponent
      Profile: DA_Profile_Lianne
```

---

### **4. Создать Relationship при встрече**

```cpp
// Event BeginPlay (BP_Lianne)

Get Component by Class ? NPCRelationshipComponent
  ?
Get Player Controller ? Get Pawn
  ?
CreateRelationshipWith (Target = Player)
```

---

### **5. Настроить DialogueEffect**

```
DialogueNode: "Ты мне нравишься"
  Effects:
    + Modify Relationship
  Mode: ExecuteAction
    ActionTag: Relationship.Action.GiveCompliment
      bApplyToPlayer: false  ? NPC ? Player
```

---

### **6. Проверить логи**

Запустить диалог, проверить Output Log:

```
? LogTemp: UDialogueEffect_ModifyRelationship: Executed action 'Relationship.Action.GiveCompliment'
? LogRelationshipSystem: Modified dimension 'Relationship.Dimension.Trust' by 5.0
```

---

## ?? Альтернатива: DirectModify (проще)

Если ExecuteAction не работает, используйте **DirectModify**:

```
DialogueEffect:
  Mode: DirectModify  ? Проще!
  DimensionModifiers:
    [0]
      DimensionTag: Relationship.Dimension.Trust
      Delta: 5.0
```

**Преимущества DirectModify:**
- ? Не требует Action DataAsset
- ? Нет Costs/Cooldown/MaxUses проблем
- ? Проще настроить
- ? Всегда работает

**Недостатки:**
- ? Нет случайности (RandomVariance)
- ? Нет Cooldown
- ? Нет MaxUses
- ? Настройки в каждой ноде диалога

---

## ?? Сравнение режимов

| Режим | Сложность | Гибкость | Рекомендуется |
|-------|-----------|----------|---------------|
| **ExecuteAction** | Высокая | Высокая | Для сложных систем |
| **DirectModify** | Низкая | Средняя | Для простых диалогов |

---

## ?? Частые ошибки

### **Ошибка 1: Relationship не создан**

```
? Проблема: CanExecuteAction возвращает false
```

**Решение:**
```cpp
RelComp->CreateRelationshipWith(Player);  // В BeginPlay
```

---

### **Ошибка 2: ActionTag неправильный**

```
? ActionTag: "Relationship.Action.ComplimentGive"
? ActionTag: "Relationship.Action.GiveCompliment"
```

**Решение:** Проверить AssetTag в DataAsset

---

### **Ошибка 3: Action DataAsset не в Database**

```
? DA_Action_GiveCompliment создан, но НЕ в Database.Actions
```

**Решение:** Добавить в `DA_RelationshipDatabase` ? **Actions**

---

### **Ошибка 4: NPCRelationshipComponent отсутствует**

```
? BP_Lianne НЕ имеет NPCRelationshipComponent
```

**Решение:** Добавить компонент в Blueprint

---

### **Ошибка 5: bApplyToPlayer неправильно настроен**

```
? bApplyToPlayer = true, но Player НЕ имеет RelationshipComponent
```

**Решение:** `bApplyToPlayer = false` (NPC ? Player)

---

### **Ошибка 6: Costs блокируют выполнение**

```
Costs:
  Friendship >= 50  ? Требуется
  
Текущий Friendship: 10  ? НЕ хватает
? Action НЕ выполнится ?
```

**Решение:** Убрать Costs или увеличить значения

---

## ?? Чеклист диагностики

### **Проверить настройки:**

- [ ] NPCRelationshipComponent на NPC?
- [ ] Relationship создан при встрече?
- [ ] Action DataAsset существует?
- [ ] ActionTag правильный?
- [ ] Action добавлен в Database?
- [ ] Effects настроены в DataAsset?
- [ ] BaseValue не 0?
- [ ] SuccessProbability не 0?
- [ ] Costs выполнены?
- [ ] Cooldown не активен?
- [ ] MaxUses не достигнут?
- [ ] bApplyToPlayer правильный?
- [ ] DialogueEffect выполняется?

### **Проверить логи:**

- [ ] `RelationshipComponent not found`? ? Добавить компонент
- [ ] `ExecuteAction function not found`? ? Неправильный компонент
- [ ] `Action not found`? ? ActionTag неправильный
- [ ] `Cannot execute action`? ? Costs/Cooldown/MaxUses
- [ ] Нет логов? ? DialogueNode не выполняется

---

## ?? Быстрое решение

### **Если ничего не помогает:**

**Используйте DirectModify:**

```
DialogueEffect:
  Mode: DirectModify
  DimensionModifiers:
    [0]
      DimensionTag: Relationship.Dimension.Trust
  Delta: 5.0
  bApplyToPlayer: false
```

**Это ВСЕГДА работает** ?

---

## ?? Связанные документы

- **[DIALOGUE_INTEGRATION_AND_PROFILES.md](DIALOGUE_INTEGRATION_AND_PROFILES.md)** - Интеграция с диалогами
- **[TROUBLESHOOTING.md](../TROUBLESHOOTING.md)** - Общее решение проблем
- **[README.md](../README.md)** - Основная документация

---

**Создано:** 2 ноября 2025  
**Версия:** 1.0  
**Автор:** Technical Documentation
