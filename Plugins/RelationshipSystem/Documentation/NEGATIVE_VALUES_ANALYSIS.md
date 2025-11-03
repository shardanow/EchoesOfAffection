# Анализ: Нужны ли отрицательные значения в Relationship System?

## ?? Краткий ответ

**НЕТ**, система **НЕ завязана** на отрицательных значениях.

Использование диапазона **0-100** вместо **-100 до +100**:
- ? **Рекомендуется** для большинства случаев
- ? **Безопасно** - никакой функционал не сломается
- ? **Интуитивно** - проще для понимания
- ? **Проще** - не требует изменений в коде

---

## ?? Детальный анализ системы

### 1. Где используются значения Dimensions?

#### ? **Функции, работающие с любым диапазоном:**

```cpp
// ClampValue - просто ограничивает значение между Min и Max
float URelationshipDimension::ClampValue(float Value) const
{
    return FMath::Clamp(Value, MinValue, MaxValue);
}
```
**Вывод:** Работает с **любыми** MinValue/MaxValue.

---

```cpp
// NormalizeValue - приводит к диапазону 0-1
float URelationshipDimension::NormalizeValue(float Value) const
{
 const float Range = GetRange(); // MaxValue - MinValue
    if (Range > KINDA_SMALL_NUMBER)
    {
        return (Value - MinValue) / Range;
    }
    return 0.5f;
}
```
**Примеры:**
- **0-100**: `(0 - 0) / 100 = 0.0` ?
- **-100 до +100**: `(0 - (-100)) / 200 = 0.5` ?

**Вывод:** Работает **корректно** с любым диапазоном, но визуально **0-100 интуитивнее**.

---

```cpp
// GetRange - просто вычитание
float URelationshipDimension::GetRange() const
{
    return MaxValue - MinValue;
}
```
**Вывод:** Работает с **любыми** значениями.

---

#### ? **Decay (затухание значений):**

```cpp
float URelationshipDimension::CalculateDecay(float CurrentValue, float DeltaHours) const
{
    if (!bEnableDecay || DeltaHours <= 0.0f)
    {
     return 0.0f;
    }

    // Направление к DecayTarget
    const float Direction = FMath::Sign(DecayTarget - CurrentValue);
    float DecayAmount = DecayRatePerHour * DeltaHours * Direction;

  // Применение кривой (опционально)
    if (DecayCurve.GetRichCurveConst() && DecayCurve.GetRichCurveConst()->GetNumKeys() > 0)
    {
        const float CurveMultiplier = DecayCurve.GetRichCurveConst()->Eval(CurrentValue);
        DecayAmount *= CurveMultiplier;
    }

    // Не превышать DecayTarget
    const float DistanceToTarget = FMath::Abs(DecayTarget - CurrentValue);
    if (FMath::Abs(DecayAmount) > DistanceToTarget)
    {
        DecayAmount = DistanceToTarget * Direction;
    }

    return DecayAmount;
}
```

**Примеры:**
- **0-100**, DecayTarget=50: Работает ?
- **-100 до +100**, DecayTarget=0: Работает ?

**Вывод:** Decay **не зависит** от знака значений, работает **в любом диапазоне**.

---

### 2. Где создаются начальные значения?

#### ? **RelationshipProfile - InitialDimensions:**

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profile")
TMap<FGameplayTag, float> InitialDimensions;
```

**Как используется:**
```cpp
// При создании relationship
for (const auto& InitialDim : Profile->InitialDimensions)
{
    FRelationshipDimensionValue DimValue(InitialDim.Key, InitialDim.Value);
    RelData->Dimensions.Add(InitialDim.Key, DimValue);
}
```

**Вывод:** Вы можете указать **любые** начальные значения:
- **0-100**: InitialDimensions["Trust"] = 0
- **-100 до +100**: InitialDimensions["Trust"] = 0

Система **не предполагает** отрицательные значения.

---

### 3. Где изменяются значения?

#### ? **ModifyDimensionValue:**

```cpp
bool URelationshipSubsystem::ModifyDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float Delta)
{
 FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
    if (!RelData)
    {
    return false;
    }

    float ModifiedDelta = ApplyDimensionChangeWithModifiers(RelData, DimensionTag, Delta);
    float CurrentValue = RelData->GetDimensionValue(DimensionTag);
    
    return SetDimensionValue(Subject, Target, DimensionTag, CurrentValue + ModifiedDelta);
}
```

**Примеры:**
- **0-100**: CurrentValue=50, Delta=+10 ? 60 ?
- **-100 до +100**: CurrentValue=0, Delta=+10 ? 10 ?

**Вывод:** Работает **одинаково** для любого диапазона.

---

#### ? **SetDimensionValue с Clamping:**

```cpp
bool URelationshipSubsystem::SetDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue)
{
    // ...
    URelationshipDimension* Dimension = Database->FindDimension(DimensionTag);
    if (!Dimension)
    {
        return false;
    }

// Автоматическое ограничение значения
    DimValue->Value = Dimension->ClampValue(NewValue);
    // ...
}
```

**Вывод:** Значения **автоматически** ограничиваются MinValue/MaxValue.

---

### 4. Где используются для логики?

#### ? **RelationshipState - условия перехода:**

```cpp
// State Transitions основаны на сравнении значений
struct FStateTransitionCondition
{
    FGameplayTag DimensionTag;
    float MinValue;  // Может быть любым
    float MaxValue;// Может быть любым
};
```

**Примеры:**
- **0-100**: "Переход в Friend при Trust >= 60"
- **-100 до +100**: "Переход в Friend при Trust >= 0"

**Вывод:** Логика работает **с любыми** значениями, не требует отрицательных.

---

#### ? **RelationshipRule - формулы:**

```cpp
// Формулы используют текущие значения
// Пример: "Trust * 0.5 + Friendship * 0.3"
```

**Вывод:** Формулы **нейтральны** к диапазону, работают с любыми числами.

---

#### ? **RelationshipAction - эффекты:**

```cpp
// Действия применяют Delta к текущему значению
// Пример: "Увеличить Trust на +15"
```

**Вывод:** Actions **не зависят** от абсолютных значений.

---

### 5. UI компоненты

#### ?? **RelationshipDimensionBar - ЕДИНСТВЕННАЯ проблема:**

```cpp
void URelationshipDimensionBar::NativeRefreshDisplay()
{
    float NormalizedValue = GetNormalizedValue(); // 0-1
    ProgressBar->SetPercent(NormalizedValue);
}
```

**Проблема:**
- **0-100**: 0 ? нормализовано в 0.0 ? **0%** ? Ожидаемо
- **-100 до +100**: 0 ? нормализовано в 0.5 ? **50%** ? Неинтуитивно

**Решение:**
1. Изменить MinValue на 0 (рекомендуется)
2. Изменить код UI (уже сделано, но требует компиляции)

---

## ?? Сравнение диапазонов

| Аспект | 0-100 | -100 до +100 |
|--------|-------|--------------|
| **Математика** | ? Работает | ? Работает |
| **Decay** | ? Работает | ? Работает |
| **State Transitions** | ? Работает | ? Работает |
| **Actions** | ? Работает | ? Работает |
| **Formulas** | ? Работает | ? Работает |
| **UI отображение** | ? 0=0%, 100=100% | ?? 0=50%, требует доработки |
| **Интуитивность** | ? Очень понятно | ?? Может запутать |
| **Семантика** | ? "Нет ? Максимум" | ?? "Враг ? Друг" |

---

## ?? Концептуальные сценарии использования

### Сценарий A: "Trust" (Доверие)

#### Вариант 1: **0-100** (Рекомендуется)
```
0   = Нет доверия
50  = Умеренное доверие
100 = Полное доверие
```
**Интуитивность:** ? Высокая

---

#### Вариант 2: **-100 до +100**
```
-100 = Полное недоверие / Подозрительность
0    = Нейтральность
+100 = Полное доверие
```
**Интуитивность:** ?? Средняя (нужна документация)

**Проблема:** При нормализации 0 ? 50%, что выглядит как "средний уровень доверия", хотя по логике это "нейтральность".

---

### Сценарий B: "Friendship" (Дружба)

#### Вариант 1: **0-100** (Рекомендуется)
```
0   = Незнакомцы
50  = Знакомые
100 = Лучшие друзья
```
**Интуитивность:** ? Высокая

---

#### Вариант 2: **-100 до +100**
```
-100 = Враги
0    = Незнакомцы
+100 = Лучшие друзья
```
**Интуитивность:** ?? Средняя

**Проблема:** "Враг" vs "Незнакомец" - семантически разные концепции. Лучше иметь отдельный dimension "Hostility" (Враждебность).

---

### Сценарий C: Несколько dimensions для полной картины

#### **Рекомендуемый подход (все 0-100):**

```cpp
// Dimensions:
Trust:      0-100  // Насколько можно доверять
Friendship: 0-100  // Насколько близки
Respect:    0-100  // Насколько уважают
Hostility:  0-100  // Насколько враждебны
Fear:       0-100  // Насколько боятся
```

**Преимущества:**
- ? Каждый dimension **независим**
- ? Нет **"противоположных"** значений в одном dimension
- ? Логика понятна: **0 = минимум, 100 = максимум**
- ? UI отображение **интуитивно**

**Пример:**
```
Незнакомец:
  Trust: 0, Friendship: 0, Respect: 0, Hostility: 0, Fear: 0

Друг:
  Trust: 80, Friendship: 90, Respect: 70, Hostility: 0, Fear: 0

Враг:
  Trust: 10, Friendship: 0, Respect: 20, Hostility: 90, Fear: 30

Боится но уважает:
  Trust: 40, Friendship: 20, Respect: 85, Hostility: 10, Fear: 80
```

---

## ?? Технический анализ кода

### Поиск зависимостей от отрицательных значений:

#### ? **Проверено:**

1. **URelationshipDimension:**
   - `ClampValue()` - работает с любыми Min/Max ?
   - `NormalizeValue()` - работает с любыми Min/Max ?
   - `CalculateDecay()` - не зависит от знака ?
   - `IsAtMinimum()`, `IsAtMaximum()` - сравнивают с Min/Max ?

2. **URelationshipSubsystem:**
   - `GetDimensionValue()` - просто возвращает значение ?
   - `SetDimensionValue()` - применяет Clamp ?
   - `ModifyDimensionValue()` - добавляет Delta ?

3. **FRelationshipData:**
   - Хранит `TMap<FGameplayTag, FRelationshipDimensionValue>` ?
   - Никаких предположений о диапазоне ?

4. **State Transitions:**
   - Условия сравнивают с пороговыми значениями ?
   - Пороги могут быть **любыми** ?

5. **RelationshipProfile:**
   - `InitialDimensions` - просто Map значений ?
   - Никаких ограничений на знак ?

6. **RelationshipAction:**
   - `DimensionChanges` - применяет Delta ?
   - Не зависит от абсолютных значений ?

7. **RelationshipRule:**
   - Формулы работают с текущими значениями ?
   - Нейтральны к диапазону ?

#### ? **НЕ найдено:**
- ? Hardcoded проверок на отрицательность
- ? Специальной логики для отрицательных значений
- ? Предположений о диапазоне -100 до +100

---

## ?? Рекомендации

### ? **Используйте 0-100 для:**
- Trust (Доверие)
- Friendship (Дружба)
- Respect (Уважение)
- Loyalty (Верность)
- Familiarity (Знакомство)

**Причины:**
- Интуитивно понятно
- UI отображение корректное (0% = пустой bar)
- Проще для дизайнеров
- Стандарт для большинства игр

---

### ?? **Используйте -100 до +100 только если:**
1. Вам **действительно нужна** семантика "отрицательное vs положительное"
2. Вы готовы **доработать UI** (уже сделано в коде, требует компиляции)
3. Вы **задокументируете** это для команды

**Примеры редких случаев:**
- **Reputation** (-100 = злодей, 0 = нейтрал, +100 = герой)
- **Alignment** (-100 = хаос, 0 = нейтральность, +100 = порядок)

**НО:** Даже эти случаи лучше решать через **отдельные dimensions**:
- Вместо Reputation: `Infamy (0-100)` + `Fame (0-100)`
- Вместо Alignment: `Chaos (0-100)` + `Order (0-100)`

---

## ?? Финальный вывод

### **Система НЕ завязана на отрицательных значениях:**

| Компонент | Требует отрицательные? | Комментарий |
|-----------|------------------------|-------------|
| **Core Logic** | ? НЕТ | Работает с любым диапазоном |
| **Decay** | ? НЕТ | Нейтрален к знаку |
| **State Transitions** | ? НЕТ | Использует пороги |
| **Actions** | ? НЕТ | Применяет Delta |
| **Formulas** | ? НЕТ | Работает с любыми числами |
| **Save/Load** | ? НЕТ | Сохраняет как float |
| **UI** | ?? ТРЕБУЕТ ДОРАБОТКИ | Для -100..+100 |

---

### **Рекомендация:**

? **Используйте диапазон 0-100** для всех dimensions:

```
MinValue: 0
MaxValue: 100
DefaultValue: 0
```

**Преимущества:**
- ? Работает "из коробки"
- ? Интуитивно понятно
- ? UI корректен
- ? Нет путаницы с нормализацией
- ? Стандартный подход

**Недостатки:**
- Нет

---

### **Если всё же нужны отрицательные:**

1. Измените UI код (уже готов в `RelationshipDimensionBar.cpp`)
2. Скомпилируйте проект
3. **Задокументируйте** семантику для команды
4. Убедитесь, что дизайнеры понимают концепцию

**НО:** В 99% случаев **0-100 лучше** ?

---

## ?? Источники анализа

### Проанализированные файлы:
1. `RelationshipDimension.h` / `.cpp` - Математика dimensions
2. `RelationshipSubsystem.cpp` - Операции с значениями
3. `RelationshipProfile.h` - Начальные значения
4. `RelationshipDimensionBar.cpp` - UI отображение
5. `RelationshipAction.h` - Применение эффектов
6. `RelationshipRule.h` - Формулы и условия
7. `RelationshipState.h` - State transitions

### Выводы:
- ? **Нет hardcoded зависимостей** от отрицательных значений
- ? **Вся математика нейтральна** к диапазону
- ? **Единственная проблема** - визуальное отображение UI
- ? **Решение простое** - изменить MinValue на 0

---

## ? Чеклист для принятия решения

- [ ] Нужна ли мне семантика "противоположностей" в одном dimension?
  - **НЕТ** ? Используйте **0-100** ?
  - **ДА** ? Рассмотрите отдельные dimensions

- [ ] Будут ли дизайнеры путаться с нормализацией 0=50%?
- **ДА** ? Используйте **0-100** ?
  - **НЕТ** ? Можете использовать -100..+100

- [ ] Готов ли я потратить время на компиляцию для UI фикса?
  - **НЕТ** ? Используйте **0-100** ?
  - **ДА** ? Можете использовать -100..+100

- [ ] Хочу ли я стандартное, понятное решение?
  - **ДА** ? Используйте **0-100** ???

---

**Итого: Используйте 0-100. Система не требует отрицательных значений.** ?

---

**Версия:** 1.0 | **Дата:** 2 ноября 2025 | **Автор:** Technical Analysis
