# Шпаргалка по событиям DialogueComponent

## ?? Три главных события

| Событие | Когда срабатывает | Параметры | Пример использования |
|---------|------------------|-----------|---------------------|
| **OnDialogueStarted** | При начале диалога | `UDialogueRunner* Runner` | Отключить управление, показать UI |
| **OnDialogueEnded** | При завершении диалога | Нет | Включить управление, скрыть UI |
| **OnDialogueNodeChanged** | При смене реплики | `UDialogueNode* NewNode`<br>`UDialogueNode* PreviousNode` | Озвучка, анимация, субтитры |

---

## ? Быстрый старт (Blueprint)

### Вариант 1: Через Details панель
1. Выбрать **Dialogue Component**
2. В Details ? Events ? нажать **+** рядом с событием
3. Добавить логику в созданный Event Node

### Вариант 2: Вручную
```
Event BeginPlay
    ?
Get Dialogue Component
?
Bind Event to On Dialogue Started
```

---

## ?? Код-шаблоны

### C++ - Базовая подписка

```cpp
// В BeginPlay или конструкторе
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::OnDialogueStart);
        DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::OnDialogueEnd);
DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::OnNodeChange);
    }
}

// Обработчики
UFUNCTION()
void AMyNPC::OnDialogueStart(UDialogueRunner* Runner) { /* логика */ }

UFUNCTION()
void AMyNPC::OnDialogueEnd() { /* логика */ }

UFUNCTION()
void AMyNPC::OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode) { /* логика */ }
```

### C++ - Отписка (важно!)

```cpp
void AMyNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>())
    {
        DialogueComp->OnDialogueStarted.RemoveDynamic(this, &AMyNPC::OnDialogueStart);
        DialogueComp->OnDialogueEnded.RemoveDynamic(this, &AMyNPC::OnDialogueEnd);
        DialogueComp->OnDialogueNodeChanged.RemoveDynamic(this, &AMyNPC::OnNodeChange);
    }
  
    Super::EndPlay(EndPlayReason);
}
```

---

## ?? Готовые паттерны (Blueprint)

### Паттерн 1: Управление вводом
```
On Dialogue Started ? Get Player Controller ? Disable Input
On Dialogue Ended ? Get Player Controller ? Enable Input
```

### Паттерн 2: UI диалога
```
On Dialogue Started ? Create Widget ? Add to Viewport ? Store Ref
On Dialogue Ended ? Get Ref ? Is Valid? ? Remove from Parent
```

### Паттерн 3: Озвучка
```
On Dialogue Node Changed ? Is Valid (New Node)? 
  ? Get Voice Clip ? Play Sound 2D
```

### Паттерн 4: Анимация эмоций
```
On Dialogue Node Changed ? Get Emotion Tag ? Switch on Name
  ?? Happy ? Play Animation (Smile)
  ?? Sad ? Play Animation (Frown)
  ?? Angry ? Play Animation (Angry)
```

### Паттерн 5: Достижения
```
On Dialogue Ended ? Increment Counter ? Branch (Counter >= 10)
  ? True ? Unlock Achievement
```

---

## ?? Порядок вызова

```
1. StartDialogue()
     ?
2. OnDialogueStarted (Dialogue Component)
     ?
3. OnNodeEntered (Runner) ? OnDialogueNodeChanged (первая реплика)
     ?
4. [Игрок выбирает вариант]
     ?
5. OnNodeEntered (Runner) ? OnDialogueNodeChanged (следующая реплика)
     ?
6. [Повторение шагов 4-5]
  ?
7. OnDialogueEnded (Dialogue Component)
```

---

## ?? Частые ошибки

| Ошибка | Почему | Решение |
|--------|--------|---------|
| **Previous Node = null** | Первая реплика | Используйте `Is Valid?` |
| **Событие не срабатывает** | Не подписались | Проверьте подписку в BeginPlay |
| **Crash при завершении** | Не отписались | Добавьте RemoveDynamic в EndPlay |
| **UI не появляется** | Забыли Add to Viewport | Проверьте цепочку Create?Add |
| **Озвучка не играет** | Voice Clip = null | Проверьте `Is Valid (Voice Clip)` |

---

## ?? Отладка

### Базовая отладка (Blueprint)
```
On Dialogue Started ? Print String "Started!" (Green, 2s)
On Dialogue Ended ? Print String "Ended!" (Red, 2s)
On Node Changed ? Print String (Get Dialogue Text) (Yellow, 3s)
```

### Расширенная отладка (C++)
```cpp
UE_LOG(LogTemp, Log, TEXT("Dialogue started with %s"), *Runner->GetName());
UE_LOG(LogTemp, Log, TEXT("Node changed: %s"), *NewNode->GetDialogueText().ToString());
UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
```

### Включить логи движка
```ini
; DefaultEngine.ini
[Core.Log]
LogDialogueComponent=Verbose
LogDialogueRunner=Verbose
```

---

## ?? Полезные функции

### Через Dialogue Component
- `GetActiveRunner()` - текущий раннер
- `IsInDialogue()` - идёт ли диалог
- `GetRelationshipComponent()` - компонент отношений
- `GetMemoryComponent()` - компонент памяти

### Через Dialogue Runner
- `GetCurrentNode()` - текущая нода
- `GetContext()` - контекст диалога
- `GetAvailableChoices()` - доступные варианты
- `IsActive()` - активен ли раннер

### Через Dialogue Node
- `GetDialogueText()` - текст реплики
- `GetSpeakerName()` - имя говорящего
- `VoiceClip` - озвучка (может быть null)
- `EmotionTag` - тег эмоции

---

## ?? Примеры из реальных проектов

### Пример 1: The Witcher-style диалоги
```cpp
// Таймер для автоматического пропуска
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->VoiceClip)
    {
        float Duration = NewNode->VoiceClip->Duration;
  GetWorld()->GetTimerManager().SetTimer(
        AutoSkipTimer, 
            this, 
         &ADialogueManager::AutoSkipNode,
         Duration + 1.0f
        );
    }
}
```

### Пример 2: Mass Effect-style прерывания
```cpp
// Interrupt actions
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->Tags.Contains("Renegade_Interrupt"))
    {
     ShowInterruptPrompt(EInterruptType::Renegade);
    }
}
```

### Пример 3: Disco Elysium-style мысли
```cpp
// Internal thoughts
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->Speaker == "InternalVoice")
    {
        UIManager->ShowThoughtBubble(NewNode->GetDialogueText());
    }
}
```

---

## ?? Ссылки на документацию

- **Полное руководство:** [06_DialogueEvents_Guide.md](06_DialogueEvents_Guide.md)
- **Быстрый старт:** [06_DialogueEvents_QuickStart.md](06_DialogueEvents_QuickStart.md)
- **Примеры:** [05_Complete_Examples.md](05_Complete_Examples.md)
- **API Reference:** [DialogueComponent.h](../Source/DialogueSystemRuntime/Public/Components/DialogueComponent.h)

---

## ?? Мобильная версия (сокращённая)

```
?? ШПАРГАЛКА: DialogueComponent Events

? СОБЫТИЯ:
  • OnDialogueStarted(Runner) - диалог начался
  • OnDialogueEnded() - диалог закончился  
  • OnDialogueNodeChanged(New, Prev) - сменилась реплика

? ПОДПИСКА (C++):
  Component->OnDialogueStarted.AddDynamic(this, &Class::Handler);

?? ПОДПИСКА (BP):
  Dialogue Component ? Details ? Events ? +

?? НЕ ЗАБУДЬТЕ:
  • Is Valid? для Previous Node
  • RemoveDynamic в EndPlay
  • Is Valid? для Voice Clips

?? ОТЛАДКА:
  Print String + Is Valid? = ??
```

---

**Версия:** 1.1.0  
**Дата:** 22.01.2025  
**Автор:** DialogueSystem Team
