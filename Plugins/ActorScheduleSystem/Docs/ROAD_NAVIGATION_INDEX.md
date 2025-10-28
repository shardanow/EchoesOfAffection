# Road Navigation - Documentation Index

## ?? Выберите подходящее руководство

### Для новичков (Blueprint/Data Assets)

1. **[Быстрый старт (5 минут)](ROAD_QUICK_START_RU.md)** ??
   - Минимальная настройка для начала работы
   - Только самое необходимое
   - Blueprint/Data Asset подход

2. **[Визуальное руководство](ROAD_VISUAL_GUIDE_RU.md)** ??
   - Пошаговые скриншоты
   - Схемы и диаграммы
   - Визуальная отладка

### Для опытных пользователей

3. **[Полное руководство (RU)](ROAD_NAVIGATION_RU.md)** ??
   - Детальное описание всех возможностей
   - Продвинутые настройки
   - Примеры для разных сценариев
   - Troubleshooting

### Для программистов (C++)

4. **[C++ Examples](ROAD_NAVIGATION_CPP.md)** ??
   - Создание NavArea классов
   - Программное создание фильтров
   - Динамическое управление навигацией
   - Unit тесты

---

## ?? Краткое содержание

### Что такое Road Navigation?

Система, позволяющая NPC двигаться **только по дорогам**, игнорируя кратчайший путь через траву/воду.

**Работает через:**
- `FilterClass` в `MoveToLocation` Action
- `NavigationQueryFilter` Data Asset
- `NavArea` разметка в уровне (дорога, трава, вода и т.д.)

---

## ?? Основные компоненты

### 1. NavigationQueryFilter (Фильтр)

**Что:** Data Asset, определяющий стоимость прохода по разным областям NavMesh

**Где создать:**
```
Content Browser ? Right-click ? Miscellaneous ? Data Asset
? NavigationQueryFilter
```

**Настройка:**
```
Areas:
  NavArea_Road: Cost = 0.5 (дешево - AI предпочитает)
NavArea_Grass: Cost = 100.0 (дорого - AI избегает)
```

**Документация:**
- [Быстрый старт - Шаг 1](ROAD_QUICK_START_RU.md#1-создайте-navigation-query-filter)
- [Полное руководство - Шаг 2](ROAD_NAVIGATION_RU.md#шаг-2-создайте-navigation-query-filter)
- [C++ Example](ROAD_NAVIGATION_CPP.md#roa-dnavigationfiltercpp)

---

### 2. Nav Modifier Volume (Разметка дорог)

**Что:** Actor, маркирующий область NavMesh как "дорогу"

**Где найти:**
```
Place Actors Panel ? Volumes ? Nav Modifier Volume
```

**Настройка:**
- Разместить над дорогой
- Масштабировать чтобы покрыть дорогу
- `Area Class`: `NavArea_Road`

**Документация:**
- [Быстрый старт - Шаг 2](ROAD_QUICK_START_RU.md#2-разметьте-дороги-в-уровне)
- [Полное руководство - Шаг 3](ROAD_NAVIGATION_RU.md#шаг-3-разметьте-navmesh-в-уровне)
- [Визуальное руководство - Шаг 2](ROAD_VISUAL_GUIDE_RU.md#шаг-2-разметка-дорог-в-уровне)

---

### 3. FilterClass в MoveToLocation

**Что:** Свойство в `MoveToLocation` Action, применяющее фильтр

**Где настроить:**
```
Schedule Entry ? Action: MoveToLocation ? Filter Class
```

**Значение:** Ваш созданный `NavigationQueryFilter`

**Документация:**
- [Быстрый старт - Шаг 3](ROAD_QUICK_START_RU.md#3-примените-фильтр-к-movetolocation)
- [Полное руководство - Шаг 4](ROAD_NAVIGATION_RU.md#шаг-4-настройте-movetolocation-action)
- [Визуальное руководство - Шаг 3](ROAD_VISUAL_GUIDE_RU.md#шаг-3-настройка-movetolocation-action)

---

## ?? Визуальная проверка

### Нажмите P в PIE

**Цвета NavMesh:**

| Цвет | Значение | NavArea |
|------|----------|---------|
| ?? Желтый | Дорога (низкая стоимость) | `NavArea_Road` |
| ?? Синий | Default (высокая стоимость) | `NavArea_Default` |
| ?? Зеленый | Трава (высокая стоимость) | `NavArea_Grass` |
| ?? Темно-синий | Вода (очень дорого) | `NavArea_Water` |

**Правильная картина:**

```
    ??      ??      ??
  [?? Трава ??]
  
???[?? Дорога ??]??? ? NPC идет здесь
  
  [?? Трава ??]
    ??  ??    ??
```

**Документация:**
- [Визуальное руководство - Шаг 4](ROAD_VISUAL_GUIDE_RU.md#шаг-4-визуальная-проверка)

---

## ?? Быстрый старт (TL;DR)

1. **Создайте фильтр:**
   - Content Browser ? Data Asset ? NavigationQueryFilter
   - Имя: `NQF_RoadOnly`
   - Areas: Road (0.5), Default (100.0)

2. **Разметьте дороги:**
   - Place Actors ? Nav Modifier Volume
   - Над дорогой, Area Class = `NavArea_Road`
 - Build ? Build Paths

3. **Примените к Action:**
   - Schedule Entry ? MoveToLocation
   - Filter Class = `NQF_RoadOnly`

4. **Проверьте:**
   - PIE ? Нажать P
   - Дорога = ?? Желтая
   - NPC идет по дороге ?

**Полный гайд:** [Быстрый старт](ROAD_QUICK_START_RU.md)

---

## ?? Troubleshooting

### Проблема: NPC идет напрямик

**Решения:**

1. Проверьте `FilterClass` в MoveToLocation ? **Самая частая причина!**
2. `bUsePathfinding = true`?
3. Nav Modifier Volume настроен?
4. NavMesh пересобран? (Build ? Build Paths)

**Подробнее:** [Troubleshooting](ROAD_NAVIGATION_RU.md#частые-проблемы)

---

### Проблема: NPC не находит путь

**Решения:**

1. Цель на дороге (желтая зона)?
2. Используйте `Cost` вместо `IsExcluded`
3. Дорога соединяет старт и цель?

**Подробнее:** [Troubleshooting](ROAD_NAVIGATION_RU.md#?-npc-застревает--не-может-найти-путь)

---

## ?? Примеры настроек

### Городская среда

```
NavArea_Road: 0.5 (дороги)
NavArea_Sidewalk: 0.7 (тротуары)
NavArea_Park: 5.0 (парки - можно, но нежелательно)
NavArea_Building: IsExcluded = true (здания - запрещено)
```

**Результат:** NPC предпочитает дороги, может идти по тротуару, избегает парки, не может войти в здания.

---

### Фэнтези мир

```
NavArea_Path: 0.5 (тропинки)
NavArea_Forest: 10.0 (лес - сложно)
NavArea_Water: IsExcluded = true (вода - невозможно)
```

**Результат:** NPC идет по тропинкам, обходит лес, не может плавать.

---

### Sci-Fi станция

```
NavArea_Corridor: 0.5 (коридоры)
NavArea_Room: 1.0 (комнаты)
NavArea_Vent: IsExcluded = true (вентиляция - только для игрока)
```

**Результат:** NPC двигается по коридорам, может заходить в комнаты, не лезет в вентиляцию.

**Больше примеров:** [Полное руководство - Примеры](ROAD_NAVIGATION_RU.md#примеры-использования)

---

## ?? Для программистов

### Создание NavArea

```cpp
UCLASS()
class UNavArea_Road : public UNavArea
{
	GENERATED_BODY()
public:
	UNavArea_Road() {
		DefaultCost = 1.0f;
		DrawColor = FColor::Yellow;
	}
};
```

### Создание фильтра

```cpp
URoadNavigationFilter* Filter = NewObject<URoadNavigationFilter>();

FNavigationFilterArea Area;
Area.AreaClass = UNavArea_Road::StaticClass();
Area.bOverrideTravelCost = true;
Area.TravelCostOverride = 0.5f;
Filter->Areas.Add(Area);
```

### Применение к Action

```cpp
MoveAction->FilterClass = URoadNavigationFilter::StaticClass();
```

**Полные примеры:** [C++ Examples](ROAD_NAVIGATION_CPP.md)

---

## ?? Дополнительные ресурсы

### Официальная документация Unreal Engine

- [Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [NavArea](https://docs.unrealengine.com/5.0/en-US/API/Runtime/AIModule/Navigation/UNavArea/)
- [NavigationQueryFilter](https://docs.unrealengine.com/5.0/en-US/API/Runtime/NavigationSystem/NavFilters/UNavigationQueryFilter/)

### Документация Actor Schedule System

- [README.md](../README.md) - Главная документация
- [QUICKSTART.md](../QUICKSTART.md) - Быстрый старт системы расписания
- [INTEGRATION_RU.md](../INTEGRATION_RU.md) - Интеграция с другими системами

---

## ? Checklist перед запуском

Проверьте перед тем как запустить игру:

- [ ] **NavigationQueryFilter создан** (например, `NQF_RoadOnly`)
- [ ] **Areas настроены** (Road = 0.5, Default = 100.0)
- [ ] **Nav Modifier Volumes размещены** над дорогами
- [ ] **Area Class** = `NavArea_Road` в каждом Volume
- [ ] **NavMesh пересобран** (Build ? Build Paths)
- [ ] **FilterClass установлен** в MoveToLocation Action
- [ ] **bUsePathfinding = true** в MoveToLocation
- [ ] **При нажатии P дорога желтая** ??
- [ ] **NPC идет по дороге** ????????

**Все галочки стоят? Готово! ??**

---

## ?? Получить помощь

### Быстрые ответы

1. **[FAQ](ROAD_NAVIGATION_RU.md#частые-проблемы)** - Частые вопросы
2. **[Troubleshooting](ROAD_NAVIGATION_RU.md#отладка)** - Решение проблем
3. **[Визуальное руководство](ROAD_VISUAL_GUIDE_RU.md#-troubleshooting-визуально)** - Схемы и картинки

### Контакты

- **GitHub Issues:** https://github.com/shardanow/EchoesOfAffection/issues
- **Email:** [support@echoesofaffection.com](mailto:support@echoesofaffection.com)

---

**Версия:** 1.0  
**Обновлено:** 2025-01-XX  
**Автор:** EchoesOfAffection Team

---

## ?? История изменений

### v1.0 - Initial Release (2025-01-XX)

? **Добавлено:**
- Поддержка `FilterClass` в `MoveToLocation`
- Интеграция с `NavigationQueryFilter`
- Автоматическое применение фильтра при движении

? **Документация:**
- Быстрый старт (RU)
- Полное руководство (RU)
- Визуальное руководство (RU)
- C++ примеры

? **Примеры:**
- Городская среда
- Фэнтези мир
- Sci-Fi станция

---

**Выберите подходящее руководство выше и начинайте! ??**
