# Быстрый старт: Движение только по дорогам

## ? За 5 минут

### 1. Создайте Navigation Query Filter

**Content Browser ? Right-click ? Miscellaneous ? Data Asset ? NavigationQueryFilter**

Назовите: `NQF_RoadOnly`

**Настройки:**
```
Areas:
  [0] NavArea_Road:
      Override Travel Cost: ?
   Travel Cost Override: 0.5
  
  [1] NavArea_Default:
      Override Travel Cost: ?
      Travel Cost Override: 100.0
```

### 2. Разметьте дороги в уровне

1. **Place Actors ? Volumes ? Nav Modifier Volume**
2. Разместите над дорогой
3. **Details Panel:**
   - `Area Class`: `NavArea_Road`
4. **Build ? Build Paths**

### 3. Примените фильтр к MoveToLocation

**В вашем Schedule Entry:**
```
Action: MoveToLocation
  ?? Filter Class: NQF_RoadOnly  ? Выберите созданный фильтр
  ?? Location Selector: ActorTag
  ?? bUsePathfinding: ? true
  ?? Acceptance Radius: 100
```

### 4. Проверьте

1. **Play In Editor (PIE)**
2. **Нажмите P** ? увидите NavMesh
3. **Дорога должна быть желтой** (NavArea_Road)
4. **NPC должен идти только по дороге** ??

---

## ?? Визуальная проверка

### Цвета NavMesh (нажмите P):

- ?? **Желтый** = Дорога (NavArea_Road) - **NPC идет здесь**
- ?? **Синий** = Default (NavArea_Default) - **NPC избегает**
- ?? **Зеленый** = Трава (NavArea_Grass) - **NPC избегает**

### Правильный путь:

```
NPC ? ???????????? ? Цель  ? (по дороге)
NPC ? ???????? ? Цель      ? (напрямик)
```

---

## ?? Частые проблемы

### ? NPC идет напрямик (не по дороге)

**Проверьте:**
1. `Filter Class` установлен в MoveToLocation? ? **Самая частая причина!**
2. `bUsePathfinding = true`?
3. Nav Modifier Volume настроен?
4. NavMesh пересобран? (Build ? Build Paths)

### ? NPC не может найти путь

**Решение:**
- Цель должна быть **на дороге** (желтая зона)
- Или: используйте `Cost` вместо `IsExcluded`

---

## ?? Настройки стоимости

| Цель | Travel Cost | Результат |
|------|-------------|-----------|
| **Предпочитать** дорогу | 0.5 | NPC выберет дорогу если возможно |
| **Избегать** траву | 10.0 - 100.0 | NPC пойдет в обход |
| **Запретить** воду | IsExcluded = true | NPC не может идти через воду |

**Рекомендуемые значения:**
```
Дорога:    0.5  (очень дешево)
Тротуар:   0.7  (дешево)
Трава:     10.0 (дорого, но можно)
Вода: 100.0 (почти запрещено)
Здания: IsExcluded = true (полный запрет)
```

---

## ?? Полная документация

Для детального руководства смотрите: [ROAD_NAVIGATION_RU.md](ROAD_NAVIGATION_RU.md)

Включает:
- ? Создание собственных NavArea классов (C++)
- ? Продвинутые настройки фильтров
- ? Отладка и визуализация
- ? Примеры для разных сценариев (город, фэнтези, sci-fi)
- ? API Reference

---

## ?? Примеры использования

### Городской NPC (строго по дорогам):
```
NQF_StrictRoad:
  NavArea_Road: 0.5
  NavArea_Sidewalk: 0.6
  NavArea_Grass: IsExcluded = true
```

### Патрульный охранник (может срезать):
```
NQF_GuardPatrol:
  NavArea_Road: 0.5
  NavArea_Grass: 5.0  ? Можно, но неохотно
```

### Монстр вне дорог:
```
NQF_Monster:
  NavArea_Forest: 1.0
  NavArea_Road: 10.0  ? Избегает дорог!
```

---

**Готово!** Ваш NPC теперь ходит только по дорогам ????????

**Помощь:** Если что-то не работает, смотрите [Troubleshooting](ROAD_NAVIGATION_RU.md#отладка)
