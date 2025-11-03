# Интеграция с диалогами и ProfileDataAssets

## ?? Два вопроса - один документ

### **Вопрос 1:** Как увеличить Trust в диалоге и увидеть это в виджете?
### **Вопрос 2:** Для чего в Profile есть InitialDimensionValues?

---

## ?? Часть 1: Изменение Trust в диалоге

### **Короткий ответ:**

Используйте **DialogueEffect_ModifyRelationship** в ноде диалога:

```
DialogueNode ? Effects ? Add Effect ? Modify Relationship
  Mode: Direct Modify Dimensions
  Dimension Modifiers:
    - DimensionTag: Relationship.Dimension.Trust
    - Delta: +15
```

**Виджет обновится автоматически** если:
1. ? RelationshipPanel подписан на события
2. ? GameEventBus работает

---

## ?? Детальная инструкция

### **Шаг 1: Создать DialogueEffect в ноде диалога**

#### **В Blueprint Dialogue Graph:**

1. Откройте **DialogueNode** (любую ноду диалога)
2. В секции **Effects** нажмите **+ Add**
3. Выберите **DialogueEffect_ModifyRelationship**

---

### **Шаг 2: Настроить эффект**

#### **Настройки DialogueEffect_ModifyRelationship:**

```
???????????????????????????????????????????????
? Modify Relationship Effect     ?
???????????????????????????????????????????????
? Mode: Direct Modify Dimensions  ? ? Выбрать режим
?  ?
? Dimension Modifiers:   ?
?   [0]          ?
?     DimensionTag: Relationship.Dimension.Trust  ? ? Выбрать Trust
? Delta: +15.0   ? ? Насколько увеличить
?           ?
? bApplyToPlayer: false   ? ? false = NPC ? Player
?      ? ? true = Player ? NPC
???????????????????????????????????????????????
```

#### **Пример: Увеличить Trust на +15**

```
Mode: DirectModify
DimensionModifiers:
  - DimensionTag: "Relationship.Dimension.Trust"
    Delta: +15.0  (положительное = увеличение)
```

#### **Пример: Уменьшить Trust на -10**

```
Mode: DirectModify
DimensionModifiers:
  - DimensionTag: "Relationship.Dimension.Trust"
    Delta: -10.0  (отрицательное = уменьшение)
```

#### **Пример: Изменить несколько dimensions сразу**

```
Mode: DirectModify
DimensionModifiers:
  - DimensionTag: "Relationship.Dimension.Trust"
    Delta: +15.0
  - DimensionTag: "Relationship.Dimension.Friendship"
    Delta: +20.0
  - DimensionTag: "Relationship.Dimension.Respect"
    Delta: +5.0
```

---

### **Шаг 3: Понять направление изменения**

#### **bApplyToPlayer = false** (по умолчанию):

```
Изменяется: NPC ? Player
Subject: NPC
Target: Player

Пример:
  Lianne (NPC) увеличивает Trust к Player на +15
  
В виджете игрока покажет:
  "Lianne's Trust: 65 ? 80"
```

#### **bApplyToPlayer = true:**

```
Изменяется: Player ? NPC
Subject: Player
Target: NPC

Пример:
  Player увеличивает Trust к Lianne на +15
  
В виджете покажет:
  "Your Trust to Lianne: 40 ? 55"
```

---

### **Шаг 4: Виджет обновится автоматически**

#### **Как происходит обновление:**

```
1. DialogueEffect выполняется
  ?
2. ModifyDimensionValue() вызывается на NPCRelationshipComponent
     ?
3. RelationshipSubsystem изменяет значение
     ?
4. BroadcastDimensionChanged() отправляет событие
     ?
5. GameEventBus получает "Relationship.DimensionChanged"
     ?
6. RelationshipPanel получает событие
     ?
7. RelationshipDimensionBar обновляет ProgressBar
     ?
8. Игрок видит изменение в UI
```

---

### **Шаг 5: Проверить что виджет подписан на события**

#### **В RelationshipPanel (UMG Widget):**

```cpp
// Проверьте настройки в Details панели:

bListenToDialogueEvents = true  // Показывать при диалогах
bAutoHideWhenNoDialogue = true  // Скрывать когда нет диалога
bAutoRefresh = true             // Автообновление
RefreshInterval = 0.1        // Интервал обновления
```

#### **Как работает подписка:**

```cpp
// RelationshipPanel автоматически подписывается на:

1. "GameEvent.Dialogue.Started"
   ? Показывает виджет, устанавливает Player/NPC

2. "Relationship.DimensionChanged"
   ? Обновляет конкретный DimensionBar

3. "GameEvent.Dialogue.Completed"
   ? Скрывает виджет (если bAutoHideWhenNoDialogue=true)
```

---

## ?? Примеры использования в диалогах

### **Пример 1: Положительный ответ (Trust +15)**

```
DialogueNode: "Я помогу тебе с этим."
  Effects:
    - Modify Relationship
      Mode: DirectModify
Modifiers:
        Trust: +15
        Friendship: +10
```

**Результат:** Lianne увеличивает Trust к игроку на +15, Friendship на +10

---

### **Пример 2: Грубый ответ (Trust -10, Hostility +5)**

```
DialogueNode: "Отстань от меня!"
  Effects:
    - Modify Relationship
      Mode: DirectModify
   Modifiers:
        Trust: -10
        Hostility: +5
```

**Результат:** Отношения ухудшаются

---

### **Пример 3: Флирт (Friendship +20, Romance +10)**

```
DialogueNode: "*Улыбается* Ты выглядишь прекрасно сегодня."
  Effects:
    - Modify Relationship
      Mode: DirectModify
      Modifiers:
        Friendship: +20
        Romance: +10
        Respect: +5
```

**Результат:** Романтические отношения развиваются

---

### **Пример 4: Использование Action (рекомендуемый способ)**

```
DialogueNode: "Вот, возьми этот подарок."
  Effects:
    - Modify Relationship
      Mode: ExecuteAction
      ActionTag: "Relationship.Action.GiveGift"
```

**Преимущества:**
- ? Все эффекты настроены в DataAsset
- ? Cooldown автоматически
- ? MaxUses автоматически
- ? Probability учитывается
- ? Легко балансировать

---

## ?? Отладка: Почему виджет не обновляется?

### **Проблема 1: Виджет не появляется**

**Причина:** Panel не подписан на "Dialogue.Started"

**Решение:**
```cpp
// В UMG Widget настройки:
bListenToDialogueEvents = true
```

**Лог:**
```
RelationshipPanel: GameEventBus not available
```

**Фикс:** Проверьте что GameEventBus плагин включен

---

### **Проблема 2: Значения не меняются**

**Причина:** DialogueEffect неправильно настроен

**Проверить:**
```
1. Mode = DirectModify?
2. DimensionTag корректный? (Relationship.Dimension.Trust)
3. Delta не 0?
4. bApplyToPlayer правильный?
```

**Лог:**
```
UDialogueEffect_ModifyRelationship: Modified dimension 'Relationship.Dimension.Trust' by 15.0
```

---

### **Проблема 3: Виджет не обновляется после изменения**

**Причина:** Panel не подписан на "Relationship.DimensionChanged"

**Решение:**
```cpp
// Проверьте в RelationshipPanel.cpp:

void URelationshipPanel::SubscribeToEvents()
{
    // Должна быть подписка:
    FGameplayTag RelationshipChangedTag = 
        FGameplayTag::RequestGameplayTag(
     FName("Relationship.DimensionChanged"), false
     );
}
```

**Лог:**
```
? RelationshipPanel: Subscribed to GameEventBus events
```

---

### **Проблема 4: Relationship не создан**

**Причина:** Relationship между Player и NPC не существует

**Решение:**
```cpp
// Создайте relationship при первой встрече:

void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    // Получить Player
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    AActor* Player = PC->GetPawn();
  
    // Получить RelationshipComponent
    UNPCRelationshipComponent* RelComp = 
 FindComponentByClass<UNPCRelationshipComponent>();
    
    // Создать relationship
    if (RelComp && Player)
    {
      RelComp->CreateRelationshipWith(Player);
    }
}
```

---

## ?? Все режимы DialogueEffect_ModifyRelationship

### **1. DirectModify (рекомендуется для диалогов)**

```cpp
Mode: DirectModify
DimensionModifiers:
  - DimensionTag: "Relationship.Dimension.Trust"
  Delta: +15.0
```

**Когда использовать:**
- ? Простые изменения в диалогах
- ? Прямое управление значениями
- ? Быстрая настройка

---

### **2. ExecuteAction (рекомендуется для сложных действий)**

```cpp
Mode: ExecuteAction
ActionTag: "Relationship.Action.GiveGift"
```

**Когда использовать:**
- ? Комплексные эффекты
- ? Cooldown нужен
- ? MaxUses ограничен
- ? Probability случайности

**Пример Action DataAsset:**
```
DA_Action_GiveGift:
  Effects:
    - Trust: +20
 - Friendship: +15
  Cooldown: 3600 (1 час)
  MaxUses: 5 (максимум 5 раз)
  Probability: 0.8 (80% шанс)
```

---

### **3. AddTrait**

```cpp
Mode: AddTrait
TraitTag: "Relationship.Trait.Romantic"
```

**Когда использовать:**
- ? Разблокировать романтические диалоги
- ? Дать особые бонусы
- ? Изменить поведение NPC

---

### **4. RemoveTrait**

```cpp
Mode: RemoveTrait
TraitTag: "Relationship.Trait.Distrustful"
```

**Когда использовать:**
- ? Убрать негативные эффекты
- ? Прогресс в отношениях

---

### **5. ForceStateTransition**

```cpp
Mode: ForceStateTransition
TargetStateTag: "Relationship.State.Friend"
```

**Когда использовать:**
- ? Скриптованные переходы
- ? Квестовые моменты
- ? Принудительное изменение статуса

---

## ?? Часть 2: InitialDimensionValues в Profile

### **Короткий ответ:**

**InitialDimensionValues** - это **начальные значения** dimensions при создании relationship.

**Для чего:**
- ? Разные NPC начинают с разных значений
- ? Не все отношения начинаются с 0
- ? Уникальные характеры персонажей

---

## ?? Назначение InitialDimensionValues

### **Проблема без InitialDimensionValues:**

```cpp
// Все relationships начинаются одинаково:

Player ? Lianne:
  Trust: 0 (DefaultValue из DA_Dimension_Trust)
  Friendship: 0
  Respect: 0

Player ? Guard:
  Trust: 0 (одинаково!)
  Friendship: 0
  Respect: 0

Player ? Merchant:
  Trust: 0 (снова 0!)
  Friendship: 0
  Respect: 0
```

**Проблема:** Все NPC одинаковые ?

---

### **Решение с InitialDimensionValues:**

```cpp
// Каждый Profile задает свои начальные значения:

DA_Profile_Lianne:
  InitialDimensionValues:
    Trust: 20      ? Lianne доверчивая
  Friendship: 30 ? Дружелюбная
    Respect: 10

DA_Profile_Guard:
  InitialDimensionValues:
    Trust: 5    ? Осторожный
    Friendship: 0  ? Профессиональный
  Respect: 50    ? Уважает авторитет

DA_Profile_Merchant:
  InitialDimensionValues:
    Trust: 15      ? Умеренное доверие
    Friendship: 10 ? Заинтересован в клиентах
    Greed: 60      ? Жадный
```

**Результат:** Уникальные персонажи ?

---

## ?? Как работает InitialDimensionValues

### **Код создания relationship:**

```cpp
// RelationshipProfile.cpp

TMap<FGameplayTag, FRelationshipDimensionValue> 
URelationshipProfile::CreateInitialDimensions(
    const URelationshipDatabase* Database, 
    float CurrentGameTime
) const
{
    TMap<FGameplayTag, FRelationshipDimensionValue> Result;

    // Проходим по всем dimensions в Database
    for (URelationshipDimension* Dimension : Database->GetAllDimensions())
    {
        FGameplayTag DimTag = Dimension->GetAssetTag();
        
     // ШАГ 1: Проверяем есть ли override в Profile
float InitialValue = GetInitialDimensionValue(DimTag, Database);

        // ШАГ 2: Clamp к MinValue/MaxValue
InitialValue = Dimension->ClampValue(InitialValue);
        
        // ШАГ 3: Создаем dimension value
        FRelationshipDimensionValue DimValue(DimTag, InitialValue);
        DimValue.LastModifiedTime = CurrentGameTime;
        
      Result.Add(DimTag, DimValue);
    }
    
 return Result;
}
```

---

### **Логика GetInitialDimensionValue:**

```cpp
float URelationshipProfile::GetInitialDimensionValue(
    FGameplayTag DimensionTag, 
    const URelationshipDatabase* Database
) const
{
    // ПРИОРИТЕТ 1: InitialDimensionValues из Profile
    if (const float* OverrideValue = InitialDimensionValues.Find(DimensionTag))
    {
        return *OverrideValue;  // ? Используем override
    }

    // ПРИОРИТЕТ 2: DefaultValue из Dimension DataAsset
    if (Database)
    {
        if (const URelationshipDimension* Dimension = Database->FindDimension(DimensionTag))
    {
            return Dimension->DefaultValue;  // ? Fallback
        }
    }

    // ПРИОРИТЕТ 3: 0 (если ничего не найдено)
    return 0.0f;
}
```

---

## ?? Примеры использования

### **Пример 1: Дружелюбный NPC**

```
DA_Profile_FriendlyVillager:
  InitialDimensionValues:
    Trust: 30        ? Сразу доверяет
    Friendship: 40   ? Дружелюбный
    Respect: 20    ? Уважает незнакомцев
    Hostility: 0     ? Не агрессивный
```

**Результат:** При первой встрече уже есть базовое доверие

---

### **Пример 2: Подозрительный бандит**

```
DA_Profile_Bandit:
  InitialDimensionValues:
    Trust: 0 ? Не доверяет
    Friendship: 0    ? Не интересуется дружбой
    Respect: 0       ? Не уважает
    Hostility: 60    ? Сразу враждебный
    Fear: 20         ? Немного боится (игрок сильный)
```

**Результат:** Агрессивное поведение с самого начала

---

### **Пример 3: Романтический интерес**

```
DA_Profile_LoveInterest:
  InitialDimensionValues:
    Trust: 25        ? Умеренное доверие
    Friendship: 35   ? Заинтересована в общении
 Respect: 30 ? Уважает
    Romance: 10      ? Небольшая симпатия
    Attraction: 15   ? Привлекательность
```

**Результат:** Есть потенциал для романтики

---

### **Пример 4: Нейтральный торговец**

```
DA_Profile_Merchant:
  InitialDimensionValues:
    Trust: 15        ? Доверяет покупателям
  Friendship: 5    ? Профессиональная дистанция
    Respect: 10      ? Уважает клиентов
    Greed: 70        ? Основная мотивация - деньги
```

**Результат:** Бизнес-ориентированный NPC

---

## ?? Сравнение с DefaultValue

| Аспект | DefaultValue (Dimension) | InitialDimensionValues (Profile) |
|--------|-------------------------|----------------------------------|
| **Где настраивается** | DA_Dimension_Trust | DA_Profile_Lianne |
| **Для кого** | Все relationships | Конкретный Profile |
| **Приоритет** | Низкий (fallback) | Высокий (override) |
| **Когда использовать** | Глобальная настройка | Индивидуальные NPC |

---

### **Пример приоритета:**

```
DA_Dimension_Trust:
  DefaultValue: 0  ? Глобальное значение

DA_Profile_Lianne:
  InitialDimensionValues:
    Trust: 20  ? Override для Lianne

DA_Profile_Guard:
  (нет Trust в InitialDimensionValues)
  
Результат при создании:

Player ? Lianne:
  Trust: 20  ? (используется Profile override)

Player ? Guard:
  Trust: 0   ? (используется Dimension DefaultValue)
```

---

## ?? Когда использовать InitialDimensionValues

### **? Используйте когда:**

1. **Разные NPC - разные характеры**
   ```
   Дружелюбный NPC: Trust 30, Friendship 40
   Подозрительный NPC: Trust 5, Hostility 20
   ```

2. **Квестовые NPC с предысторией**
   ```
   Старый друг: Friendship 80, Trust 70
   Бывший враг: Hostility 50, Trust 10
   ```

3. **Уникальные архетипы**
 ```
   Романтический интерес: Romance 10, Attraction 15
Наставник: Respect 60, Trust 40
   Соперник: Rivalry 30, Respect 20
   ```

4. **Профессиональные роли**
   ```
   Торговец: Greed 70, Trust 15
   Стража: Duty 80, Respect 50
   Шпион: Deception 60, Trust 0
   ```

---

### **? НЕ используйте когда:**

1. **Все NPC одинаковые**
   ? Используйте DefaultValue в Dimension

2. **Значения меняются динамически**
   ? Используйте State transitions или Actions

3. **Временные эффекты**
   ? Используйте Traits или Modifiers

---

## ?? Практический пример: Создание NPC

### **Задача:** Создать Lianne - дружелюбную романтическую NPC

#### **Шаг 1: Создать Profile DataAsset**

```
Content/Data/Relationships/Profiles/DA_Profile_Lianne
```

#### **Шаг 2: Настроить InitialDimensionValues**

```
DA_Profile_Lianne:
  DisplayName: "Lianne"
  Description: "Friendly and romantic merchant's daughter"
  
  InitialState: "Relationship.State.Stranger"
  
  InitialDimensionValues:
    Relationship.Dimension.Trust: 25  ? Доверяет незнакомцам
    Relationship.Dimension.Friendship: 35    ? Дружелюбная
    Relationship.Dimension.Respect: 20       ? Уважает других
    Relationship.Dimension.Romance: 10       ? Открыта к романтике
    Relationship.Dimension.Attraction: 15    ? Находит игрока привлекательным
    Relationship.Dimension.Hostility: 0      ? Не агрессивная
    Relationship.Dimension.Fear: 0           ? Не боится
  
  InitialTraits:
    - Relationship.Trait.Romantic
    - Relationship.Trait.Optimistic
  
  bCanBeSubject: true
  bCanBeTarget: true
```

---

#### **Шаг 3: Назначить Profile на NPC**

```cpp
// BP_Lianne (Blueprint)

Components:
  NPCRelationshipComponent
    Profile: DA_Profile_Lianne  ? Назначить
```

---

#### **Шаг 4: Создать relationship при встрече**

```cpp
// При первой встрече с игроком:

void ABP_Lianne::OnFirstMeeting(AActor* Player)
{
    UNPCRelationshipComponent* RelComp = 
        FindComponentByClass<UNPCRelationshipComponent>();
    
    if (RelComp)
    {
  // Создается relationship с InitialDimensionValues:
        RelComp->CreateRelationshipWith(Player);
        
        // Результат:
        // Trust: 25
        // Friendship: 35
        // Romance: 10
        // ...
    }
}
```

---

#### **Шаг 5: В диалоге увеличить Romance**

```
DialogueNode: "Ты мне нравишься..."
  Effects:
    - Modify Relationship
      Mode: DirectModify
      Modifiers:
        Romance: +20  (10 ? 30)
        Attraction: +10 (15 ? 25)
     Trust: +10 (25 ? 35)
```

**Виджет покажет:**
```
Lianne:
  Romance: [??????????] 30%
  Trust: [??????????] 35%
  Friendship: [??????????] 35%
```

---

## ? Итоговая схема работы

```
1. Создание NPC с Profile
       ?
2. Profile ? InitialDimensionValues
       ?
3. Relationship создается с начальными значениями
       ?
4. В диалоге: DialogueEffect ? ModifyDimensionValue
?
5. RelationshipSubsystem ? BroadcastDimensionChanged
       ?
6. GameEventBus ? "Relationship.DimensionChanged"
       ?
7. RelationshipPanel ? RefreshDisplay
    ?
8. RelationshipDimensionBar ? SetPercent
       ?
9. Игрок видит изменение в UI
```

---

## ?? Связанные документы

- **[README.md](../README.md)** - Основная документация
- **[TROUBLESHOOTING.md](../TROUBLESHOOTING.md)** - Решение проблем
- **[CHEATSHEET.md](CHEATSHEET.md)** - Быстрая справка
- **[SETUP_ISSUE.md](../SETUP_ISSUE.md)** - Настройка интеграции

---

## ?? Чеклист интеграции с диалогами

### **Настройка DialogueEffect:**

- [ ] Создан DialogueEffect_ModifyRelationship в ноде
- [ ] Mode выбран (DirectModify/ExecuteAction/etc.)
- [ ] DimensionTag корректный
- [ ] Delta правильный (+ или -)
- [ ] bApplyToPlayer настроен правильно

### **Настройка UI:**

- [ ] RelationshipPanel создан в UMG
- [ ] DimensionContainer привязан
- [ ] DimensionBarClass назначен
- [ ] bListenToDialogueEvents = true
- [ ] bAutoRefresh = true

### **Настройка Profile:**

- [ ] Profile DataAsset создан
- [ ] InitialDimensionValues настроены
- [ ] InitialState назначен
- [ ] InitialTraits добавлены
- [ ] Profile назначен на NPCRelationshipComponent

### **Проверка работы:**

- [ ] Relationship создается при встрече
- [ ] DialogueEffect выполняется (лог)
- [ ] Значения меняются в subsystem
- [ ] GameEventBus работает
- [ ] UI обновляется
- [ ] Визуально всё корректно

---

## ?? Финальные ответы

### **Вопрос 1: Как увеличить Trust в диалоге?**

**Ответ:**
```
DialogueNode ? Effects ? Modify Relationship
  Mode: DirectModify
  DimensionModifiers:
    Trust: +15
```

**Виджет обновится автоматически** ?

---

### **Вопрос 2: Для чего InitialDimensionValues?**

**Ответ:**

**Чтобы разные NPC начинали с разных значений:**
- Lianne: Trust 25, Friendship 35 (дружелюбная)
- Guard: Trust 5, Respect 50 (осторожный)
- Bandit: Hostility 60 (враждебный)

**Вместо всех с 0** ?

---

**Создано:** 2 ноября 2025  
**Версия:** 1.0  
**Автор:** Technical Documentation
