# ?? Руководство по отладке системы диалогов

## Проблема: Не отображается текст диалога и кнопки выбора

### Шаги для диагностики:

## 1. Компиляция с логированием

**Закройте Unreal Editor** и выполните компиляцию:

```bash
cd "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles"
.\Build.bat EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -WaitMutex
```

Или используйте **Ctrl+Alt+F11** в редакторе для Live Coding.

## 2. Проверка логов

После запуска диалога откройте **Output Log** в Unreal Editor и найдите строки:

### Проверка входа в узел:
```
LogTemp: DialogueWidget::HandleNodeEntered - Node: [NodeId]
LogTemp: DialogueWidget::HandleNodeEntered - Dialogue Text: [Your text here]
```

? **Если видите текст** - данные приходят правильно  
? **Если текст пустой** - проблема в DialogueNode data

### Проверка typewriter:
```
LogTemp: DialogueWidget::StartTypewriter - FullText: '[Your text]', bEnableTypewriter: 1, bIsTyping: 1
LogTemp: DialogueWidget::StartTypewriter - Set DialogueText to: ''
```

? **Если FullText не пустой** - текст получен  
? **Если DialogueText is NULL** - виджет не привязан

### Проверка создания выборов:
```
LogTemp: DialogueWidget::CreateChoiceButtons - Creating X choices
LogTemp: DialogueWidget::CreateChoiceButtons - Creating choice 0: [Choice text]
LogTemp: ChoiceWidget::SetChoiceData - Node: [NodeId], Index: 0
LogTemp: ChoiceWidget::UpdateVisuals - Setting choice text: '[Choice text]'
```

? **Если видите тексты выборов** - данные есть  
? **Если "ChoicesContainer is NULL"** - виджет не привязан  
? **Если "ChoiceWidgetClass is NULL"** - класс не установлен

## 3. Проверка виджета в Blueprint

### DialogueWidget (WBP_DialogueMain):

1. **Откройте виджет** в редакторе
2. **Hierarchy** - проверьте наличие:
   - ? `SpeakerName` (Text Block) - с точным именем!
   - ? `DialogueText` (Rich Text Block) - с точным именем!
   - ? `ChoicesContainer` (Vertical Box) - с точным именем!

3. **Details Panel** ? **Class Settings**:
   - Parent Class: `DialogueWidget` (C++)
   - ? Проверьте что это правильный родительский класс

4. **Details Panel** ? **Default** (ваш виджет):
   - ? **Choice Widget Class** должен быть установлен на `WBP_DialogueChoice`
   - ? **Enable Typewriter** = `true` (если нужен эффект печати)
   - ? **Characters Per Second** > 0

### ChoiceWidget (WBP_DialogueChoice):

1. **Hierarchy** - проверьте:
   - ? `ChoiceButton` (Button) - с точным именем!
   - ? `ChoiceText` (Rich Text Block) - с точным именем!

2. **Parent Class**: `ChoiceWidget` (C++)

## 4. Быстрый тест с отключенным Typewriter

Если текст не появляется, **отключите typewriter** для теста:

В `WBP_DialogueMain` ? **Details**:
- **Enable Typewriter** = `false`

Это покажет текст сразу без анимации.

## 5. Проверка DialogueNode Data Asset

Откройте ваш Dialogue Data Asset и убедитесь:

1. **Dialogue Text** не пустой:
   ```
   DialogueText: "Hello, traveler!"
   ```

2. **Choice Text** не пустой для узлов выбора:
   ```
   ChoiceText: "Tell me more"
   ```

3. **NodeType** правильный:
   - `NpcLine` - для реплик NPC
   - `PlayerChoice` - для выборов игрока

## 6. Проверка привязки виджета

В вашем Blueprint коде (GameMode, PlayerController и т.д.):

```blueprint
// Проверьте что виджет правильно создан
Create Widget ? Class = WBP_DialogueMain

// Проверьте что виджет добавлен на экран
Add to Viewport

// Проверьте что Runner привязан
Get Dialogue Runner ? Widget ? Bind To Runner

// Проверьте что диалог запущен
Runner ? Start Dialogue
```

## 7. Частые ошибки

### ? Виджет не отображается совсем
**Причина**: Не добавлен в Viewport или Z-Order низкий  
**Решение**: `Add to Viewport` и проверьте Z-Order

### ? Текст пустой
**Причина**: 
- DialogueText виджет не привязан (неправильное имя)
- DialogueNode.DialogueText пустой
- Typewriter включен но Characters Per Second = 0

**Решение**: 
1. Проверьте имя виджета: точно `DialogueText`
2. Проверьте Data Asset
3. Проверьте настройки Typewriter

### ? Кнопки выбора не появляются
**Причина**:
- ChoicesContainer не привязан
- ChoiceWidgetClass не установлен
- DialogueRunner не возвращает choices

**Решение**:
1. Проверьте имя: точно `ChoicesContainer`
2. Установите Choice Widget Class
3. Проверьте что DialogueNode имеет connections

### ? Кнопки есть но без текста
**Причина**:
- ChoiceText виджет не привязан в WBP_DialogueChoice
- DialogueNode.ChoiceText пустой

**Решение**:
1. В WBP_DialogueChoice проверьте имя: точно `ChoiceText`
2. В Data Asset заполните ChoiceText

## 8. Проверка с минимальным виджетом

Создайте тестовый виджет с минимальным кодом:

**WBP_TestDialogue** (наследует DialogueWidget):

```
Canvas Panel
??? Vertical Box
    ??? Text Block "SpeakerName"
    ??? Rich Text Block "DialogueText"
    ??? Vertical Box "ChoicesContainer"
```

**WBP_TestChoice** (наследует ChoiceWidget):

```
Button "ChoiceButton"
??? Rich Text Block "ChoiceText"
```

Если с таким виджетом работает - проблема в вашем сложном виджете.

## 9. Команды для быстрой проверки

В **Output Log** вставьте команды:

```
// Показать все активные виджеты
Widget.Debug.Show

// Показать детали конкретного виджета
Widget.Debug.Dump WBP_DialogueMain

// Включить детальное логирование
Log LogTemp Verbose
```

## 10. Что дальше?

Если после всех проверок не работает:

1. ? **Экспортируйте лог** - Output Log ? Save to File
2. ? **Сделайте скриншоты**:
 - Hierarchy виджета
   - Details Panel с настройками
   - Blueprint код запуска диалога
3. ? **Проверьте версию плагина** - убедитесь что все файлы обновлены

## Полезные горячие клавиши

- **~** (тильда) - открыть консоль
- **F1** - скрыть UI (проверить что виджет вообще на экране)
- **Ctrl+Shift+.** - Widget Reflector (отладка UI)
- **Alt+P** - PIE (Play In Editor)

---

**После компиляции с логами запустите игру и пришлите Output Log для анализа!**
