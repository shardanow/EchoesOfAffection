# Relationship System - Краткая справка

## ?? Самые частые проблемы

### 1. Dimension Bar показывает 50% при 0
**Решение:** Измените `MinValue` с `-100` на `0` в DataAssets
?? **Детали:** [QUICKFIX_DIMENSION_BAR.md](../QUICKFIX_DIMENSION_BAR.md)

### 2. Relationship panel пустой
**Решение:** Создайте relationship между Player и NPC
?? **Детали:** [TROUBLESHOOTING.md](../TROUBLESHOOTING.md#problem-no-relationships-visible-in-ui)

### 3. Текст не показывается
**Решение:** Установите `bShowNumericValue = true` и `bShowDimensionName = true`
?? **Детали:** [TROUBLESHOOTING.md](../TROUBLESHOOTING.md)

---

## ?? Структура документации

```
?? README.md         ? Обзор + Quick Start
?? TROUBLESHOOTING.md           ? Все проблемы и решения
?? SETUP_ISSUE.md   ? Настройка системы
?? QUICKFIX_DIMENSION_BAR.md ? Быстрый фикс 50%

?? Documentation/
  ?? INDEX.md     ? Навигация
  ?? DIMENSION_BAR_50_PERCENT_ISSUE.md ? Детальное объяснение
  ?? CHEATSHEET.md        ? ВЫ ЗДЕСЬ
```

---

## ? Быстрые команды

### Blueprint Nodes
```
• GetDimensionValue(Target, DimensionTag) ? float
• ModifyDimensionValue(Target, DimensionTag, Delta)
• GetCurrentState(Target) ? GameplayTag
• HasRelationshipWith(Target) ? bool
• CreateRelationshipWith(Target)
```

### C++ API
```cpp
// Получить значение
float Value = Component->GetDimensionValue(TargetActor, DimensionTag);

// Изменить значение
Component->ModifyDimensionValue(TargetActor, DimensionTag, 10.0f);

// Создать relationship
Component->CreateRelationshipWith(TargetActor);

// Проверить существование
bool bExists = Component->HasRelationshipWith(TargetActor);
```

---

## ?? Debugging

### Enable Verbose Logging
```
LogRelationshipSystem VeryVerbose
LogRelationshipSystemUI VeryVerbose
```

### Key Log Messages
```
? "Created relationship between..." - Relationship создан успешно
?? "No profile assigned..." - Нужно назначить Profile
?? "Relationship does not exist..." - Создайте relationship
```

---

## ?? DataAssets Quick Reference

### Dimension
```
MinValue: 0     ? Минимум (РЕКОМЕНДУЕТСЯ 0)
MaxValue: 100     ? Максимум
DefaultValue: 0          ? Начальное значение
bEnableDecay: false   ? Автоматическое затухание
```

**?? Использование отрицательных значений:**
- ? **НЕ рекомендуется** - см. [NEGATIVE_VALUES_ANALYSIS](NEGATIVE_VALUES_ANALYSIS.md)
- ? **Используйте 0-100** для интуитивности
- ?? Если всё же нужны -100..+100, требуется доработка UI

### Profile
```
InitialDimensions  ? Стартовые значения dimensions
InitialState             ? Начальный State
AssignedTraits           ? Модификаторы поведения
```

---

## ?? UI Setup

### Dimension Bar Widget
```
Required bindings:
• ProgressBar (обязательно)
• ValueText (опционально)
• NameText (опционально)

Settings:
• bShowNumericValue = true
• bShowDimensionName = true
• bNormalizeValue = true
```

### Panel Widget
```
Required:
• DimensionContainer (Vertical/Horizontal Box)
• DimensionBarClass (WBP_RelationshipDimensionBar)
```

---

## ? Checklist первой настройки

- [ ] RelationshipDatabase создан и назначен в Project Settings
- [ ] Dimensions созданы (Trust, Friendship, и т.д.)
- [ ] States созданы (Stranger, Friend, и т.д.)
- [ ] Profile создан и назначен
- [ ] NPCRelationshipComponent добавлен на Player и NPCs
- [ ] Profile назначен на Components
- [ ] Relationship создается при первой встрече
- [ ] Panel подписан на события (GameEventBus или Delegates)
- [ ] Dimension bars правильно настроены в UMG

---

## ?? Частые ошибки

? **Забыли назначить Profile** ? Values всегда 0
? **Не создали relationship** ? Panel пустой
? **MinValue = -100** ? Progress bar показывает 50% при 0
? **Не привязали TextBlock'и** ? Нет подписей
? **Не подписались на события** ? UI не обновляется

---

## ?? Получить помощь

1. **Поиск в документации:** [INDEX.md](INDEX.md)
2. **Troubleshooting:** [TROUBLESHOOTING.md](../TROUBLESHOOTING.md)
3. **GitHub Issues:** https://github.com/shardanow/EchoesOfAffection/issues

---

## ?? Полезные ссылки

- [Полная документация](INDEX.md)
- [Детальное объяснение 50%](DIMENSION_BAR_50_PERCENT_ISSUE.md)
- [Negative Values Analysis](NEGATIVE_VALUES_ANALYSIS.md)
- [Save and Reset Guide](SAVE_AND_RESET_GUIDE.md)
- [Dialogue Integration and Profiles](DIALOGUE_INTEGRATION_AND_PROFILES.md)
- [Quick Start](../README.md#quick-start)
- [Примеры кода](../README.md#blueprint-api)

---

## ?? Быстрые команды для диалогов

### DialogueEffect (изменить в диалоге)
```
DialogueNode ? Effects ? Modify Relationship
  Mode: DirectModify
  Modifiers:
    Trust: +15
    Friendship: +10

// Blueprint
ModifyDimensionValue(Target, DimensionTag, Delta)

// C++
RelComp->ModifyDimensionValue(Target, DimensionTag, Delta);
```

### Profile (начальные значения)
```
DA_Profile_Lianne:
  InitialDimensionValues:
    Trust: 25
    Friendship: 35
    Romance: 10
```

---

## ?? Быстрые команды для сохранения

### Save/Load
```
// Blueprint
SaveRelationships(SlotName)
LoadRelationships(SlotName)
DoesSaveExist(SlotName)
DeleteSave(SlotName)

// C++
URelationshipSaveGameHelper::SaveRelationships(this, "SlotName");
URelationshipSaveGameHelper::LoadRelationships(this, "SlotName");
```

### Reset
```
// Blueprint
ClearAllRelationships()
RemoveRelationship(Subject, Target)

// C++
Subsystem->ClearAllRelationships();
Subsystem->RemoveRelationship(Subject, Target);
```

---

**Версия:** 1.0 | **Обновлено:** 2 ноября 2025
