# Зацикливание Экшенов - Краткая Инструкция (RU)

## Что добавлено?

В Composite Action теперь можно **зацикливать список дочерних экшенов**!

## Как использовать?

###  1. Откройте ваш Composite Action

В Content Browser найдите Data Asset типа `ScheduleAction_Composite` или создайте новый.

### 2. Включите зацикливание

В свойствах экшена:

```
? bExecuteSequentially = true   (должно быть включено!)
? bLoopChildActions = true      (включаем зацикливание)
   MaxLoops = 0       (0 = бесконечно, или укажите число)
```

### 3. Добавьте дочерние экшены

```
Child Actions:
  [0] MoveToLocation (идти к станку)
  [1] PlayAnimation (работать)
  [2] Wait (30 секунд)
  [3] MoveToLocation (идти на склад)
  [4] Wait (10 секунд)
```

### 4. Готово!

После выполнения последнего экшена (#4), система **автоматически вернется к первому** (#0) и начнет цикл заново!

## Примеры настроек

### Бесконечный цикл (до отмены расписания)
```
bLoopChildActions = true
MaxLoops = 0
```
**Используется для**: рабочие смены, патрули, постоянные действия

### Ограниченное количество повторений
```
bLoopChildActions = true
MaxLoops = 5
```
**Используется для**: "Обойти патруль 5 раз", "Выполнить задачу 3 раза"

### Без зацикливания (как раньше)
```
bLoopChildActions = false
```
**Используется для**: однократные последовательности действий

## Когда цикл останавливается?

1. **Достигнут MaxLoops** (если не 0)
2. **Временное окно расписания закончилось** (Schedule Entry TimeCondition)
3. **Вызвана отмена** (CancelAction, InterruptAction)

## Визуальная схема

```
???????????????????????????????????
?  bLoopChildActions = true       ?
?  MaxLoops = 0 (infinite)     ?
???????????????????????????????????
    ?
         ?
    [Action 1] ????? [Action 2] ????? [Action 3]
         ?   ?
         ?        ?
         ???????????????? LOOP ??????????????
```

## Отладка

### В логах (LogScheduleComposite):
```
Starting composite action with 5 children (Sequential: YES)
Starting first child action
Child action 0 completed, advancing to next
...
Child action 4 completed, advancing to next
Looping child actions (Loop 1/-1)     ? Начался новый цикл!
Restarting first child action
```

### В Visual Logger:
1. Запустите PIE (Play In Editor)
2. Нажмите **Ctrl+L** ? откроется Visual Logger
3. Выберите вашего NPC
4. Смотрите категорию `LogScheduleComposite`

## Частые вопросы

**Q: Почему цикл не работает?**  
A: Проверьте, что `bExecuteSequentially = true`. Зацикливание работает только для последовательного выполнения!

**Q: Как остановить бесконечный цикл?**  
A: Либо дождитесь окончания временного окна в Schedule Entry, либо вызовите `CancelCurrentAction()` на компоненте.

**Q: Можно ли вложить циклы друг в друга?**  
A: Да! Создайте вложенный Composite Action:
```
Composite (Outer, MaxLoops = 0):
  ?? Composite (Inner, MaxLoops = 3)
  ?  ?? [Work Actions...]
  ?? Wait (Break 60 sec)
```

**Q: Влияет ли на производительность?**  
A: Нет, накладные расходы минимальны (один счетчик и простая проверка).

## Полная документация

?? Детальное описание: `Docs/FEATURE_Loop_Child_Actions.md`  
?? Пошаговый пример: `Docs/EXAMPLE_Looping_Actions.md`  
?? Общая документация: `README.md`

---

**Версия**: v1.1  
**Автор**: EchoesOfAffection Team  
**GitHub**: https://github.com/shardanow/EchoesOfAffection
