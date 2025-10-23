# Quick Start: Показать курсор в диалоге

## ?? Самый быстрый способ

### В Blueprint NPC:

```
1. Откройте BP_YourNPC
2. Найдите DialogueInputBlockerComponent
3. В Details ? Default Blocking Settings:
   ?? Show Mouse Cursor = ? TRUE

ГОТОВО! Курсор будет показываться и скрываться автоматически.
```

## ?? Для DialogueWidget

Если у вас есть кнопки выбора в UI:

```
1. Откройте DialogueInputBlockerComponent
2. Установите:
   ?? Show Mouse Cursor = ? TRUE
 ?? Enable Click Events = ? TRUE
   ?? Enable Mouse Over Events = ? TRUE (для hover)
```

## ?? Программно в Blueprint

```blueprint
Event: Start Dialogue
    ?
[Get Component: DialogueInputBlocker]
    ?
[Get Current Blocking Settings]
    ?
[Set bShowMouseCursor] = True
  ?
[Block Input] (с обновлёнными настройками)
```

## ? Результат (v1.11)

? Курсор видим во время диалога  
? Можно кликать по UI элементам  
? Работает hover на кнопках  
? Автоматически скрывается после диалога  
? **НОВОЕ:** Фокус ввода возвращается в игру мгновенно (не нужен клик!)  

## ?? Что изменилось в v1.11?

### ? Старая проблема (v1.10):
```
Закончили диалог ? Курсор исчез
НО: Нужно кликнуть на viewport чтобы управлять персонажем ?
```

### ? Исправлено (v1.11):
```
Закончили диалог ? Курсор исчез
Управление работает СРАЗУ ? (фокус вернулся автоматически)
```

---

**Смотрите:**  
- `Mouse_Cursor_Support_v1.10.md` - Основная документация  
- `Input_Focus_Fix_v1.11.md` - Исправление фокуса

**Версия:** 1.11
