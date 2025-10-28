# Движение только по дорогам - Полное руководство

## Обзор

В системе `ScheduleAction_MoveToLocation` **полностью поддерживается движение только по дорогам** через механизм **Navigation Query Filter** (фильтр навигационных запросов).

Это позволяет NPC двигаться только по определенным областям NavMesh (например, дороги, тротуары) и избегать других (трава, вода, бездорожье).

---

## Как это работает?

### Архитектура

```
Actor Schedule System
      ?
MoveToLocation Action
      ?
FilterClass (NavigationQueryFilter)
      ?
NavMesh + NavArea Costs
      ?
AI выбирает путь по дорогам
```

### Ключевые компоненты:

1. **NavMesh** - сетка навигации в уровне
2. **NavArea** - области NavMesh с разными свойствами (дорога, трава, вода)
3. **NavigationQueryFilter** - фильтр, определяющий "стоимость" прохода по каждой области
4. **FilterClass** в `MoveToLocation` - применяет фильтр к движению

---

## Пошаговая настройка

### Шаг 1: Создайте NavArea классы

#### 1.1. Создайте NavArea для дороги

**File ? New C++ Class ? Navigation Area ? Name: `NavArea_Road`**

```cpp
// NavArea_Road.h
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Road.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Road : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Road();
};
```

```cpp
// NavArea_Road.cpp
#include "NavArea_Road.h"

UNavArea_Road::UNavArea_Road()
{
	// Низкая стоимость - AI предпочитает дороги
	DefaultCost = 1.0f;
	FixedAreaEnteringCost = 0.0f;
	DrawColor = FColor::Yellow; // Для отладки
	bIsMetaArea = false;
}
```

#### 1.2. Создайте NavArea для травы/бездорожья

```cpp
// NavArea_Grass.h
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Grass.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Grass : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Grass();
};
```

```cpp
// NavArea_Grass.cpp
#include "NavArea_Grass.h"

UNavArea_Grass::UNavArea_Grass()
{
	// Высокая стоимость - AI избегает траву
	DefaultCost = 10.0f;  // В 10 раз дороже!
	FixedAreaEnteringCost = 0.0f;
	DrawColor = FColor::Green;
	bIsMetaArea = false;
}
```

### Шаг 2: Создайте Navigation Query Filter

**Content Browser ? Right-click ? Miscellaneous ? Data Asset ? NavigationQueryFilter**

Назовите: `NQF_RoadOnly` (Navigation Query Filter - Road Only)

#### Настройки фильтра:

1. **Откройте** `NQF_RoadOnly`
2. **В секции Areas добавьте:**

| Area Class | Travel Cost Override | Entering Cost Override | Is Excluded |
|------------|---------------------|------------------------|-------------|
| `NavArea_Road` | ? 0.5 | - | ? |
| `NavArea_Grass` | ? 100.0 | - | ? |
| `NavArea_Default` | ? 50.0 | - | ? |

**Объяснение стоимостей:**
- `0.5` - Дорога (очень дешево, AI выберет в первую очередь)
- `100.0` - Трава (очень дорого, AI будет избегать)
- `50.0` - Default (дорого, но не настолько как трава)

**Альтернативно - полное исключение:**

Если хотите **полностью запретить** движение вне дорог:

| Area Class | Is Excluded |
|------------|-------------|
| `NavArea_Grass` | ? true |
| `NavArea_Default` | ? true |

### Шаг 3: Разметьте NavMesh в уровне

#### 3.1. Добавьте Nav Modifier Volume

1. **Place Actors Panel ? Volumes ? Nav Modifier Volume**
2. **Разместите** над дорогой
3. **Масштабируйте** чтобы покрыть дорогу

#### 3.2. Настройте Area Class

В свойствах **Nav Modifier Volume**:
- `Area Class`: `NavArea_Road`

Повторите для всех дорог в уровне.

#### 3.3. Пересоберите NavMesh

1. **Нажмите P** в редакторе ? увидите NavMesh
2. **Build ? Build Paths** (или просто двигайте Nav Modifier Volume)
3. **Проверьте**: Дороги должны быть желтыми (NavArea_Road), остальное зеленым (NavArea_Grass)

### Шаг 4: Настройте MoveToLocation Action

#### В вашем Schedule Entry:

1. **Откройте** Schedule Entry Data Asset
2. **В Action ? MoveToLocation:**
   - `Filter Class`: ? Выберите `NQF_RoadOnly`
   - `Acceptance Radius`: 100
   - `bUsePathfinding`: ? true

#### Пример в Blueprint/Data Asset:

```
Schedule Entry: "Идти на работу"
?? Time Condition: 08:00-09:00
?? Action: MoveToLocation
   ?? Filter Class: NQF_RoadOnly  ? ВОТ ОНО!
   ?? Location Selector: ActorTag (Workplace)
   ?? Acceptance Radius: 100
   ?? bUsePathfinding: true
```

---

## Практический пример

### Сценарий: NPC идет на работу только по дорогам

#### 1. Создайте Filter

**Content/Navigation/NQF_RoadOnly**:
```
Areas:
  [0] NavArea_Road:
      TravelCostOverride = 0.5
      bOverrideTravelCost = true
  
  [1] NavArea_Grass:
  TravelCostOverride = 1000.0  // Почти запрещено
      bOverrideTravelCost = true
```

#### 2. Создайте Schedule Entry

**Content/Schedules/Entries/DA_Entry_GoToWork**:
```
Entry Name: "Идти на работу"
Time Condition:
  Time Type: TimeRange
  Start Hour: 8
  Start Minute: 0
  End Hour: 9
  End Minute: 0

Action: MoveToLocation
  Filter Class: NQF_RoadOnly
  Location Selector: ActorTag
    Required Tags: Location.Workplace
    Selection Mode: Closest
  Acceptance Radius: 100
  bUsePathfinding: true
```

#### 3. Разместите маркеры

```
Уровень:
  ?? LocationMarker "Дом" (Tag: Location.Home)
  ?? LocationMarker "Работа" (Tag: Location.Workplace)
  ?? Nav Modifier Volume (над дорогой, AreaClass: NavArea_Road)
  ?? NPC с ScheduleComponent
```

### Результат:

- ? NPC идет **только по желтой зоне** (дорога)
- ? NPC **НЕ срезает** через траву
- ? Если дорога длиннее, NPC все равно выберет ее

---

## Отладка

### Визуализация NavMesh

1. **Нажмите P** в PIE (Play In Editor)
2. **Цвета областей**:
   - ?? Желтый = `NavArea_Road` (дорога)
   - ?? Зеленый = `NavArea_Grass` (трава)
   - ?? Синий = `NavArea_Default` (обычная зона)

### Проверка пути

1. **Включите AI Debug**:
   - Console: `showdebug AI`
   - Или: `ai.debug.nav.show 1`

2. **Проверьте путь**:
   - Линия должна идти **только по дорогам**
   - Если линия идет напрямик ? фильтр не работает

### Логирование

В `ScheduleAction_MoveToLocation.cpp` уже есть логи:

```cpp
if (FilterClass)
{
    MoveRequest.SetNavigationFilter(FilterClass);
    UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Using FilterClass: %s"), *FilterClass->GetName());
}
```

Проверьте Output Log на наличие этой строки.

---

## Продвинутые настройки

### Разные фильтры для разных NPC

#### Пример: Торговец vs Охранник

**NQF_MerchantFilter** (только дороги):
```
NavArea_Road: Cost = 0.5
NavArea_Grass: Excluded = true
```

**NQF_GuardFilter** (может идти через траву в патруле):
```
NavArea_Road: Cost = 1.0
NavArea_Grass: Cost = 5.0  // Можно, но нежелательно
```

### Динамическая смена фильтра

```cpp
// В Blueprint или C++
void ASomeActor::SwitchToOffRoadMode()
{
UScheduleAction_MoveToLocation* MoveAction = GetMoveAction();
    MoveAction->FilterClass = NQF_OffRoad;
}
```

### Комбинированные фильтры

**NQF_RoadAndSidewalk**:
```cpp
Areas:
  NavArea_Road: Cost = 0.5
  NavArea_Sidewalk: Cost = 0.6
  NavArea_Grass: Cost = 100.0
```

NPC будет предпочитать дорогу, но может использовать тротуар.

---

## Частые проблемы

### ? NPC все равно идет напрямик

**Причины:**
1. `FilterClass` не установлен ? Проверьте Data Asset
2. NavMesh не размечен ? Нажмите P, проверьте цвета
3. `bUsePathfinding = false` ? Включите!

**Решение:**
```
MoveToLocation:
  ? FilterClass: NQF_RoadOnly
  ? bUsePathfinding: true
  ? Nav Modifier Volumes настроены
```

### ? NPC застревает / не может найти путь

**Причины:**
1. Цель находится **вне** дороги
2. Фильтр **слишком строгий** (все области excluded)

**Решение:**
- Используйте `TravelCostOverride` вместо `IsExcluded`
- Или: разместите цель на дороге

### ? NavMesh не отображается цветом

**Решение:**
1. **Пересоберите NavMesh**: Build ? Build Paths
2. **Проверьте Nav Modifier Volume**: Area Class установлен?
3. **Restart Editor** (иногда помогает)

---

## Производительность

### Стоимость фильтрации

- ? **Минимальная** - фильтр применяется только при запросе пути
- ? **Кэшируется** движком
- ? **Не влияет на FPS** при разумном количестве NPC

### Оптимизация

1. **Используйте меньше Nav Modifier Volumes** - объединяйте зоны
2. **Не создавайте сложные фильтры** - 3-5 областей достаточно
3. **Переиспользуйте фильтры** между NPC

---

## Примеры использования

### 1. Городская среда

```
NavArea_Road (0.5)        - Дороги
NavArea_Sidewalk (0.7)    - Тротуары
NavArea_Park (5.0) - Парки (можно, но нежелательно)
NavArea_Building (excluded) - Здания (запрещено)
```

### 2. Фэнтези мир

```
NavArea_Path (0.5)        - Тропинки
NavArea_Forest (10.0)     - Лес (сложно идти)
NavArea_Water (excluded)  - Вода (невозможно пройти)
NavArea_Cliff (excluded)  - Обрыв (смерть)
```

### 3. Sci-Fi станция

```
NavArea_Corridor (0.5)    - Коридоры
NavArea_Room (1.0)        - Комнаты
NavArea_Vent (excluded)   - Вентиляция (только для игрока)
NavArea_Airlock (excluded) - Шлюз (опасно)
```

---

## API Reference

### UScheduleAction_MoveToLocation

```cpp
/** Filter class for navigation (e.g., for road-only navigation) */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
TSubclassOf<class UNavigationQueryFilter> FilterClass;
```

**Использование:**
- Установите в Data Asset или Blueprint
- Фильтр применяется автоматически при вызове `MoveTo`

### NavigationQueryFilter

**Основные свойства:**

| Свойство | Тип | Описание |
|----------|-----|----------|
| `Areas` | Array | Список переопределений для NavArea |
| `TravelCostOverride` | float | Стоимость прохода |
| `EnteringCostOverride` | float | Стоимость входа в область |
| `bIsExcluded` | bool | Полностью запретить область |

**C++ Example:**

```cpp
void AMyActor::SetupRoadFilter()
{
    UNavigationQueryFilter* Filter = NewObject<UNavigationQueryFilter>();
    
    // Дорога - дешево
 FNavigationFilterArea RoadArea;
    RoadArea.AreaClass = UNavArea_Road::StaticClass();
    RoadArea.bOverrideTravelCost = true;
    RoadArea.TravelCostOverride = 0.5f;
    Filter->Areas.Add(RoadArea);
  
    // Трава - дорого
    FNavigationFilterArea GrassArea;
    GrassArea.AreaClass = UNavArea_Grass::StaticClass();
    GrassArea.bOverrideTravelCost = true;
    GrassArea.TravelCostOverride = 100.0f;
    Filter->Areas.Add(GrassArea);
    
    MoveAction->FilterClass = Filter->GetClass();
}
```

---

## См. также

- [Official UE Documentation: Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [NavArea Documentation](https://docs.unrealengine.com/5.0/en-US/API/Runtime/AIModule/Navigation/UNavArea/)
- [README.md](../README.md) - Общая документация системы расписания
- [QUICKSTART.md](../QUICKSTART.md) - Быстрый старт

---

## Changelog

### v1.0 - Initial Release
- ? Поддержка `FilterClass` в `MoveToLocation`
- ? Интеграция с Navigation Query Filter
- ? Автоматическое применение фильтра при движении

### v1.1 - Documentation
- ? Добавлена полная документация
- ? Примеры настройки NavArea
- ? Руководство по отладке

---

**Автор**: EchoesOfAffection Team  
**Дата**: 2025-01-XX  
**Версия**: 1.0
