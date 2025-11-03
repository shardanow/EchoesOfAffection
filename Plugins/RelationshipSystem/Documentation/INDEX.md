# Relationship System - Документация

## ?? Быстрый доступ к решениям

### ? Критические проблемы

1. **[Dimension Bar показывает 50% вместо 0%](QUICKFIX_DIMENSION_BAR.md)** ? СТАРТ ЗДЕСЬ
   - Самая частая проблема при первой настройке
   - Решение за 1 минуту

2. **[No relationships visible in UI](TROUBLESHOOTING.md#problem-no-relationships-visible-in-ui)**
   - Relationship panel пустой
   - Нет dimensions/state в UI

3. **[Setup Issues](SETUP_ISSUE.md)**
 - Проблемы с первой настройкой
   - Интеграция с другими системами

---

## ?? Полная документация

### Основные документы

- **[README.md](README.md)** - Обзор, установка, Quick Start
- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Решение всех проблем
- **[SETUP_ISSUE.md](SETUP_ISSUE.md)** - Настройка и интеграция

### Детальные руководства

#### UI & Визуализация
- **[DIMENSION_BAR_50_PERCENT_ISSUE.md](Documentation/DIMENSION_BAR_50_PERCENT_ISSUE.md)**
  - Полное объяснение проблемы с 50%
  - 3 варианта решения
  - Математика нормализации
  - Сравнение подходов

#### Дизайн системы
- **[NEGATIVE_VALUES_ANALYSIS.md](Documentation/NEGATIVE_VALUES_ANALYSIS.md)**
- Нужны ли отрицательные значения?
  - Анализ всего кода системы
  - Сравнение 0-100 vs -100..+100
  - Концептуальные сценарии
  - Технические рекомендации

#### Сохранение и управление данными
- **[SAVE_AND_RESET_GUIDE.md](Documentation/SAVE_AND_RESET_GUIDE.md)**
  - Где хранятся relationships?
  - Runtime vs SaveGame
  - Как сохранять и загружать
  - Как сбросить данные
  - Автосохранение
  - Debug команды

#### Интеграция с диалогами
- **[DIALOGUE_INTEGRATION_AND_PROFILES.md](Documentation/DIALOGUE_INTEGRATION_AND_PROFILES.md)**
  - Как изменить Trust в диалоге?
  - DialogueEffect_ModifyRelationship
  - Автообновление виджета
  - Для чего InitialDimensionValues?
  - Примеры создания уникальных NPC
  - Профили персонажей

#### Решение проблем
- **[DIALOGUE_EFFECT_NOT_WORKING.md](Documentation/DIALOGUE_EFFECT_NOT_WORKING.md)**
  - DialogueEffect не срабатывает
  - Пошаговая диагностика
  - Проверка логов
  - Частые ошибки
  - Быстрое решение (DirectModify)

#### Компоненты (TODO)
- Component Setup Guide
- Profile Configuration
- Event Integration

#### DataAssets (TODO)
- Dimension Configuration
- State Transitions
- Traits & Modifiers
- Profiles & Initial Values

#### Системная архитектура (TODO)
- Event-Driven Design
- Performance Optimization
- Save/Load System
- Integration Patterns

---

## ?? Начало работы

### Для новичков

1. Прочитайте **[README.md](README.md)** - Quick Start секцию
2. Если возникли проблемы ? **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)**
3. Если UI показывает 50% ? **[QUICKFIX_DIMENSION_BAR.md](QUICKFIX_DIMENSION_BAR.md)**

### Для продвинутых

1. **[DIMENSION_BAR_50_PERCENT_ISSUE.md](Documentation/DIMENSION_BAR_50_PERCENT_ISSUE.md)** - Детальное объяснение
2. Изучите код в:
   - `Source/RelationshipSystemCore/` - Логика
   - `Source/RelationshipSystemUI/` - UI компоненты
   - `Source/RelationshipSystemEditor/` - Редакторские инструменты

---

## ?? Поиск по симптомам

| Симптом | Документ | Решение |
|---------|----------|---------|
| **ProgressBar заполнен на 50%** | [QUICKFIX](QUICKFIX_DIMENSION_BAR.md) | Измените MinValue на 0 |
| **Panel пустой** | [TROUBLESHOOTING](TROUBLESHOOTING.md) | Создайте relationship |
| **Нет текста в dimension bars** | [TROUBLESHOOTING](TROUBLESHOOTING.md) | Привяжите TextBlock'и в UMG |
| **Values всегда 0** | [TROUBLESHOOTING](TROUBLESHOOTING.md) | Проверьте Profile assignment |
| **Events не срабатыют** | [SETUP_ISSUE](SETUP_ISSUE.md) | Проверьте GameEventBus |
| **Нужны ли отрицательные значения?** | [NEGATIVE_VALUES_ANALYSIS](Documentation/NEGATIVE_VALUES_ANALYSIS.md) | НЕТ, используйте 0-100 |
| **Где сохраняются relationships?** | [SAVE_AND_RESET_GUIDE](Documentation/SAVE_AND_RESET_GUIDE.md) | Runtime + SaveGame |
| **Как сбросить данные?** | [SAVE_AND_RESET_GUIDE](Documentation/SAVE_AND_RESET_GUIDE.md) | ClearAllRelationships() + DeleteSave() |
| **Как изменить Trust в диалоге?** | [DIALOGUE_INTEGRATION_AND_PROFILES](Documentation/DIALOGUE_INTEGRATION_AND_PROFILES.md) | DialogueEffect_ModifyRelationship |
| **Для чего InitialDimensionValues?** | [DIALOGUE_INTEGRATION_AND_PROFILES](Documentation/DIALOGUE_INTEGRATION_AND_PROFILES.md) | Начальные значения для разных NPC |
| **DialogueEffect не работает?** | [DIALOGUE_EFFECT_NOT_WORKING](Documentation/DIALOGUE_EFFECT_NOT_WORKING.md) | Диагностика ExecuteAction |
| **Виджет не обновляется?** | [DIALOGUE_EFFECT_NOT_WORKING](Documentation/DIALOGUE_EFFECT_NOT_WORKING.md) | Проверить логи и настройки |

---

## ?? Структура файлов

```
RelationshipSystem/
??? README.md # Обзор + Quick Start
??? TROUBLESHOOTING.md   # Решение всех проблем
??? SETUP_ISSUE.md        # Настройка системы
??? QUICKFIX_DIMENSION_BAR.md     # Быстрое решение 50%
?
??? Documentation/
?   ??? INDEX.md    # ? ВЫ ЗДЕСЬ
?   ??? DIMENSION_BAR_50_PERCENT_ISSUE.md  # Детальное объяснение
?   ??? NEGATIVE_VALUES_ANALYSIS.md        # Анализ отрицательных значений
?   ??? SAVE_AND_RESET_GUIDE.md
?   ??? (будущие документы...)
?
??? Source/
?   ??? RelationshipSystemCore/            # Основная логика
?   ??? RelationshipSystemUI/ # UI компоненты
?   ??? RelationshipSystemEditor/  # Редакторские инструменты
?
??? Content/
    ??? DataAssets/        # Примеры DataAsset'ов
```

---

## ?? Внешние ссылки

- **GitHub Repository**: https://github.com/shardanow/EchoesOfAffection
- **Issue Tracker**: https://github.com/shardanow/EchoesOfAffection/issues

---

## ?? Статистика

| Категория | Файлов |
|-----------|--------|
| Основные документы | 4 |
| Детальные руководства | 4 |
| Примеры кода | 0 |
| TODO | 7 |

---

## ?? Версия документации

- **Создано:** 2 ноября 2025
- **Последнее обновление:** 2 ноября 2025
- **Версия плагина:** 1.0
- **Версия UE:** 5.6

---

## ?? Советы

### Если документ не найден
1. Проверьте правильность пути
2. Посмотрите в `Documentation/` подпапку
3. Используйте поиск в IDE (Ctrl+Shift+F)

### Если проблема не описана
1. Проверьте Output Log в Unreal Editor
2. Включите verbose logging:
   ```
   LogRelationshipSystem VeryVerbose
   LogRelationshipSystemUI VeryVerbose
   ```
3. Создайте Issue на GitHub с логами

### Быстрая навигация
- `Ctrl+F` ? поиск по документу
- `Ctrl+Shift+F` ? поиск по всем файлам
- Используйте ссылки в начале каждого документа
