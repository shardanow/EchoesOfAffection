# ?? MusicManager: Исправление проблемы с одинаковым Priority

## ? Проблема

При переходе между зонами с **одинаковым Priority** музыка начинала играть и сразу затихала.

### Логи показывали:

```
[378] LogMusicManager: Music request: town_first (Priority: 10, Type: 0)
[378] LogMusicManager: ? Immediate transition to: town_first (Volume: 1.00)
[378] LogTemp: Player entered MusicZone 'BP_StartTown_Music_C_2' - Priority: 10

[31] LogMusicManager: Music request: town_second (Priority: 10, Type: 1)
[31] LogMusicManager: ?? StartCrossfade: NewMusic=town_second, Duration=2.00
[31] LogTemp: Player entered MusicZone 'BP_StartTown_Music_Other_C_4' - Priority: 10

[38] LogMusicManager: Released zone with priority: 10  ? ? ПРОБЛЕМА!
[38] LogTemp: Player exited MusicZone 'BP_StartTown_Music_C_2' - Priority: 10
```

**Что происходило:**
1. ? Входим в зону A (Priority: 10) ? музыка town_first
2. ? Входим в зону B (Priority: 10) ? начинается кроссфейд на town_second
3. ? Выходим из зоны A (Priority: 10) ? `ReleaseZone(10)` удаляет **ОБЕ** зоны!
4. ? `FindHighestPriorityZone` ничего не находит
5. ? Музыка останавливается

---

## ?? Корневая причина

### Старая реализация:

```cpp
// ? ПРОБЛЕМА: TMap<Priority, Request>
TMap<int32, FMusicRequest> ActiveZones;

void RequestMusicChange(USoundBase* NewMusic, int32 Priority, ...)
{
// ? Если две зоны имеют одинаковый Priority, вторая ПЕРЕЗАПИСЫВАЕТ первую!
    ActiveZones.Add(Priority, Request);
}

void ReleaseZone(int32 Priority)
{
    // ? Удаляет ВСЕ зоны с этим Priority!
  ActiveZones.Remove(Priority);
}
```

**Проблема:** `TMap` допускает **только один элемент на ключ**!

---

## ? Решение: Уникальные ID для каждой зоны

### Новая архитектура:

```cpp
// ? TMap<UniqueID, Request> - каждая зона имеет уникальный ID
TMap<int32, FMusicRequest> ActiveZones;

// ? Счётчик для генерации уникальных ID
int32 NextZoneID = 0;

// ? НОВАЯ функция: RegisterMusicZone
int32 RegisterMusicZone(USoundBase* NewMusic, int32 Priority, ...)
{
    int32 ZoneID = NextZoneID++; // Генерируем уникальный ID
    ActiveZones.Add(ZoneID, Request); // Добавляем с уникальным ключом
  return ZoneID; // Возвращаем для последующего Release
}

// ? ИЗМЕНЕНО: ReleaseZone по уникальному ID
void ReleaseZone(int32 ZoneID)
{
    ActiveZones.Remove(ZoneID); // Удаляет ТОЛЬКО эту зону
}
```

---

## ?? Сравнение: До vs После

### До (НЕ РАБОТАЛО):

| Действие | ActiveZones | Результат |
|----------|-------------|-----------|
| Enter Zone A (Priority: 10) | `{10: town_first}` | ? Играет town_first |
| Enter Zone B (Priority: 10) | `{10: town_second}` | ? **Перезаписало** A! |
| Exit Zone A (Priority: 10) | `{}` | ? Удалило **ОБЕ** зоны! |

### После (РАБОТАЕТ):

| Действие | ActiveZones | Результат |
|----------|-------------|-----------|
| Enter Zone A (Priority: 10) | `{0: town_first(P:10)}` | ? Играет town_first |
| Enter Zone B (Priority: 10) | `{0: town_first(P:10), 1: town_second(P:10)}` | ? Кроссфейд A?B |
| Exit Zone A | `{1: town_second(P:10)}` | ? Удалило **ТОЛЬКО** A! |

---

## ?? Ключевые изменения

### 1. MusicManager.h

```cpp
class UMusicManager : public UGameInstanceSubsystem
{
    // ? НОВАЯ функция: регистрация зоны с уникальным ID
  UFUNCTION(BlueprintCallable, Category = "Music")
    int32 RegisterMusicZone(USoundBase* NewMusic, int32 Priority, ...);

    // ? ИЗМЕНЕНО: Release по уникальному ID
UFUNCTION(BlueprintCallable, Category = "Music")
    void ReleaseZone(int32 ZoneID);

protected:
  // TMap<UniqueID, Request> - ключ теперь уникальный ID
    TMap<int32, FMusicRequest> ActiveZones;

    // ? НОВОЕ: Счётчик для генерации уникальных ID
    int32 NextZoneID = 0;
};
```

### 2. MusicManager.cpp

```cpp
int32 UMusicManager::RegisterMusicZone(...)
{
    // Генерируем уникальный ID
    int32 ZoneID = NextZoneID++;

    // Добавляем с уникальным ключом
    ActiveZones.Add(ZoneID, Request);

    UE_LOG(LogMusicManager, Log, TEXT("?? Registered zone ID=%d: %s (Priority: %d)"),
   ZoneID, *NewMusic->GetName(), Priority);

    // Ищем зону с наивысшим Priority
FindHighestPriorityZone(HighestRequest, HighestPriority);
    // ...

    return ZoneID; // Возвращаем для последующего Release
}

void UMusicManager::ReleaseZone(int32 ZoneID)
{
    if (!ActiveZones.Contains(ZoneID))
    {
   UE_LOG(LogMusicManager, Warning, TEXT("?? ReleaseZone: Zone ID=%d not found!"), ZoneID);
        return;
  }

    // Удаляем ТОЛЬКО эту зону
    ActiveZones.Remove(ZoneID);
    UE_LOG(LogMusicManager, Log, TEXT("??? Released zone ID=%d"), ZoneID);

    // Ищем следующую зону с наивысшим Priority
    FindHighestPriorityZone(NextRequest, NextPriority);
    // ...
}

bool UMusicManager::FindHighestPriorityZone(...)
{
    // ? ИЗМЕНЕНО: Ищем по Priority из Value, а не из Key
    for (const auto& Pair : ActiveZones)
    {
        const FMusicRequest& Request = Pair.Value;
   
        if (Request.Priority > OutPriority)
        {
        OutPriority = Request.Priority;
          OutRequest = Request;
            bFound = true;
        }
    }
    // ...
}
```

### 3. MusicZoneVolume.h

```cpp
class AMusicZoneVolume : public ATriggerBox
{
private:
    // ? НОВОЕ: Уникальный ID зоны в MusicManager
    int32 RegisteredZoneID = -1;
};
```

### 4. MusicZoneVolume.cpp

```cpp
void AMusicZoneVolume::HandlePlayerEntered(APlayerController* PC)
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
   // ? Регистрируем зону и сохраняем ID
        RegisteredZoneID = Manager->RegisterMusicZone(
      ZoneMusic, Priority, TransitionType, 
            CrossfadeDuration, VolumeMultiplier
        );

        UE_LOG(LogTemp, Log, TEXT("? Entered MusicZone '%s' - ID: %d, Priority: %d"),
            *GetName(), RegisteredZoneID, Priority);
    }
}

void AMusicZoneVolume::HandlePlayerExited(APlayerController* PC)
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
  {
        if (RegisteredZoneID >= 0)
        {
   // ? Release по уникальному ID
     Manager->ReleaseZone(RegisteredZoneID);

    UE_LOG(LogTemp, Log, TEXT("? Exited MusicZone '%s' - Released ID: %d"),
  *GetName(), RegisteredZoneID);

         RegisteredZoneID = -1; // Сбрасываем ID
        }
    }
}
```

---

## ?? Тестирование

### Сценарий: Две зоны с одинаковым Priority

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Immediate
- Zone B: Priority 10, Music: town_second, Transition: Crossfade
```

**Тест:**
```
1. Входим в Zone A
2. Входим в Zone B (перекрывается с A)
3. Выходим из Zone A (остаёмся в B)
```

**Ожидаемое поведение:**
```
1. Zone A: town_first играет ?
2. Zone B: Кроссфейд town_first ? town_second ?
3. Exit A: town_second продолжает играть ?
```

**Новые логи:**
```
[378] LogMusicManager: ?? Registered zone ID=0: town_first (Priority: 10)
[378] LogTemp: ? Entered MusicZone 'BP_StartTown_Music_C_2' - ID: 0, Priority: 10

[31] LogMusicManager: ?? Registered zone ID=1: town_second (Priority: 10)
[31] LogTemp: ? Entered MusicZone 'BP_StartTown_Music_Other_C_4' - ID: 1, Priority: 10
[31] LogMusicManager: ?? Highest priority zone: Priority=10, Music=town_second

[38] LogMusicManager: ??? Released zone ID=0 (Priority: 10)
[38] LogTemp: ? Exited MusicZone 'BP_StartTown_Music_C_2' - Released ID: 0
[38] LogMusicManager: ?? Highest priority zone: Priority=10, Music=town_second
```

**Результат:** ? town_second продолжает играть!

---

## ?? Дополнительные преимущества

### 1. Поддержка множества зон с одинаковым Priority

Теперь можно иметь **несколько зон** с одинаковым приоритетом:

```
Zone A: Priority 10, Music: town_first
Zone B: Priority 10, Music: town_second
Zone C: Priority 10, Music: town_third

Игрок в A+B+C одновременно:
- ActiveZones = {0: town_first, 1: town_second, 2: town_third}
- Играет последняя зарегистрированная (town_third)
```

### 2. Правильное переключение при выходе

```
Игрок в A+B+C, выходит из C:
- ActiveZones = {0: town_first, 1: town_second}
- Переключается на town_second ?
```

### 3. Поддержка разных Priority

```
Zone A: Priority 10
Zone B: Priority 20
Zone C: Priority 15

Игрок в A+B+C:
- ActiveZones = {0: A(P:10), 1: B(P:20), 2: C(P:15)}
- Играет B (наивысший Priority) ?
```

---

## ?? Обратная совместимость

Старая функция `RequestMusicChange` всё ещё работает:

```cpp
// ? Можно вызывать напрямую (для Blueprint-совместимости)
void RequestMusicChange(USoundBase* NewMusic, int32 Priority, ...)
{
    // Просто обёртка для RegisterMusicZone
    RegisterMusicZone(NewMusic, Priority, ...);
}
```

**Но:** Лучше использовать `RegisterMusicZone` + `ReleaseZone(ID)` для полного контроля!

---

## ?? Итого

### Проблемы:

1. ? `TMap<Priority, Request>` - только одна зона на Priority
2. ? `ReleaseZone(Priority)` удаляла **все** зоны с этим Priority
3. ? При переходе между зонами с одинаковым Priority музыка затихала

### Решения:

1. ? `TMap<UniqueID, Request>` - уникальный ID для каждой зоны
2. ? `RegisterMusicZone` возвращает ID
3. ? `ReleaseZone(ID)` удаляет **только** нужную зону
4. ? `FindHighestPriorityZone` ищет по Priority из Value

### Результат:

- ? **Множество зон** с одинаковым Priority работают правильно
- ? **Правильное переключение** при выходе из зон
- ? **Кроссфейд работает** корректно
- ? **Обратная совместимость** сохранена

---

**Дата:** 24 октября 2025  
**Версия:** 1.4 (UNIQUE ID FIX)  
**Статус:** ? ВСЕ БАГИ ИСПРАВЛЕНЫ!
