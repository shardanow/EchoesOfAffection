# Blueprint Interface Error Fix

## Проблема

Если вы видите ошибки:
```
Assertion failed: 0 && "Do not directly call Event functions in Interfaces. 
Call Execute_RegisterListener instead."
```

Это означает, что вы вызываете методы интерфейса напрямую из Blueprint.

## Решение

### Вариант 1: Используйте BP_ префикс методы (Рекомендуется) ?

Все методы Time of Day Subsystem имеют **две версии**:

1. **Версия интерфейса** (с _Implementation суффиксом) - для C++
2. **BP_ версия** - для Blueprint

**В Blueprint всегда используйте BP_ версии:**

#### Замените старые ноды:

| ? Старая нода (вызывает ошибку) | ? Новая нода (работает) |
|----------------------------------|--------------------------|
| `Register Listener` | `Register Listener (BP)` |
| `Unregister Listener` | `Unregister Listener (BP)` |
| `Get Current Time` | `Get Current Time (BP)` |
| `Get Current Phase` | `Get Current Phase (BP)` |
| `Get Normalized Time of Day` | `Get Normalized Time (BP)` |
| `Get Current Season` | `Get Current Season (BP)` |
| `Set Current Time` | `Set Current Time (BP)` |
| `Advance Time` | `Advance Time (BP)` |
| `Set Time Scale` | `Set Time Scale (BP)` |
| `Get Time Scale` | `Get Time Scale (BP)` |
| `Pause Time` | `Pause Time (BP)` |
| `Resume Time` | `Resume Time (BP)` |
| `Is Time Paused` | `Is Time Paused (BP)` |

### Вариант 2: Используйте Blueprint Library

Вместо прямого вызова Subsystem, используйте **UTimeOfDayBlueprintLibrary**:

```
Get Current Phase (World Context = Self)
  ?
  ?
[используем результат - ETimeOfDayPhase]
```

Эти функции автоматически получают Subsystem и вызывают правильные методы.

### Вариант 3: Используйте TimeOfDayListenerComponent

Для получения событий времени:

1. **Добавьте компонент в Actor:**
   - В Blueprint Actor откройте Components
- Add Component ? `Time Of Day Listener Component`

2. **Настройте события:**
   - Отметьте нужные события (Receive Phase Events, etc.)
   - В Event Graph получите ссылку на компонент
   - Bind к его event delegates

3. **Компонент автоматически:**
   - Регистрируется в Subsystem при BeginPlay
   - Unregisters при EndPlay
   - Вызывает ваши Blueprint события

## Примеры исправленного кода

### До (вызывает ошибку):
```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Register Listener [Self]  ? ? ОШИБКА!
```

### После (работает):
```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Register Listener (BP) [Self]  ? ? РАБОТАЕТ!
```

### Или ещё лучше - используйте компонент:
```
[Time Of Day Listener Component уже в Components list]

Event BeginPlay
  ?
  ?
[Ничего не делаем - компонент сам регистрируется!]

// Для получения данных:
Get Current Phase (BP)
  ?
  ?
Print String
```

## Как найти BP_ ноды в Blueprint

1. Откройте Blueprint Event Graph
2. Right-click ? Add Function Call
3. В поиске введите: `BP Get Current` (с префиксом BP)
4. Вы увидите все BP_ версии методов
5. Они будут иметь `(BP)` в названии для отличия

## Дополнительные подсказки

### Pure functions (Get-методы)
BP_ Get-методы помечены как **BlueprintPure**, поэтому не требуют execution pin:

```
???????????????????????????????
? Get Current Phase (BP) ?
???????????????????????????????
? Target: Time Of Day Subsyst ? (no exec pin!)
? Return: ETimeOfDayPhase     ?
???????????????????????????????
```

### Impure functions (Set-методы)
BP_ Set-методы требуют execution pin:

```
     ????????????????????????????????
  ???? Set Time Scale (BP)      ????
     ????????????????????????????????
     ? Target: Time Of Day Subsyst  ?
     ? New Time Scale: 2.0     ?
     ????????????????????????????????
```

## Проверка правильности

После исправления вы **НЕ должны** видеть ошибки:
- ? "Do not directly call Event functions"
- ? "Call Execute_XXX instead"
- ? Всё работает без assertion errors

## Если всё ещё есть ошибки

1. Проверьте, что используете **BP_** версии методов
2. Убедитесь, что скомпилировали Blueprint после изменений
3. Перезапустите редактор
4. Проверьте Output Log на другие ошибки

## Полный пример правильного использования

```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem (World Context = Self)
  ?
  ?
Initialize With Settings [DA_TimeOfDay_Default]
  ?
  ?
Get Current Phase (BP)
  ?
  ?
Switch on ETimeOfDayPhase
  ?? Morning ? Print "Good Morning!"
?? Noon ? Print "Midday!"
  ?? Night ? Print "Good Night!"
  ?? Default ? Print "Unknown phase"
```

Всё! Теперь ваша Time of Day System будет работать корректно в Blueprint! ???

---

**Дополнительная информация:**
- Документация: `README.md`
- Быстрый старт: `QUICKSTART.md`
- Архитектура: `ARCHITECTURE.md`
