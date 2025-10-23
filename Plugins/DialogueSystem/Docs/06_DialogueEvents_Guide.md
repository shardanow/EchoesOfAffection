# Руководство по событиям диалогов (Dialogue Events)

## Обзор

`DialogueComponent` предоставляет три основных события (делегата), которые можно прослушивать из Blueprint или C++ для реагирования на изменения состояния диалога.

## Доступные события

### 1. OnDialogueStarted
**Тип:** `FOnDialogueStartedSignature`  
**Параметры:** `UDialogueRunner* Runner`

Вызывается когда диалог начинается.

**Пример использования в Blueprint:**

```
????????????????????????
?  Dialogue Component  ?
????????????????????????
? OnDialogueStarted ??????? [Custom Logic]
????????????????????????    ?
     ?
    Print String: "Dialogue Started!"
           Disable Player Input
      Show Dialogue UI
```

**Пример использования в C++:**

```cpp
// В вашем Actor/Component
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
  
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::HandleDialogueStarted);
  }
}

void AMyNPC::HandleDialogueStarted(UDialogueRunner* Runner)
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue started with runner: %s"), *Runner->GetName());
 // Ваша логика здесь
}
```

---

### 2. OnDialogueEnded
**Тип:** `FOnDialogueEndedSignature`  
**Параметры:** Нет

Вызывается когда диалог завершается.

**Пример использования в Blueprint:**

```
????????????????????????
?  Dialogue Component  ?
????????????????????????
? OnDialogueEnded ????????? [Custom Logic]
????????????????????????       ?
         ?
         Print String: "Dialogue Ended!"
        Enable Player Input
              Hide Dialogue UI
       Give Quest Reward
```

**Пример использования в C++:**

```cpp
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::HandleDialogueEnded);
    }
}

void AMyNPC::HandleDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
    // Вернуть управление игроку
    // Обновить квесты
    // Сохранить состояние
}
```

---

### 3. OnDialogueNodeChanged
**Тип:** `FOnDialogueNodeChangedSignature`  
**Параметры:** 
- `UDialogueNode* NewNode` - новая реплика/нода
- `UDialogueNode* PreviousNode` - предыдущая нода (может быть nullptr при первой реплике)

Вызывается каждый раз когда диалог переходит к новой реплике.

**Пример использования в Blueprint:**

```
????????????????????????????
?   Dialogue Component  ?
????????????????????????????
? OnDialogueNodeChanged ??????? [Branch: Is Valid?]
????????????????????????????       ?
          ??? True  ??? Play Voice Line
   ?Update Subtitles
  ?            Trigger Animation
        ?
        ??? False ??? (First line)
```

**Пример использования в C++:**

```cpp
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
     DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::HandleNodeChanged);
    }
}

void AMyNPC::HandleNodeChanged(UDialogueNode* NewNode, UDialogueNode* PreviousNode)
{
    if (NewNode)
    {
 UE_LOG(LogTemp, Log, TEXT("New dialogue line: %s"), *NewNode->GetDialogueText().ToString());
        
        // Воспроизвести озвучку
 if (NewNode->VoiceClip)
        {
     PlayVoiceClip(NewNode->VoiceClip);
    }
    
     // Изменить анимацию
        TriggerEmotionAnimation(NewNode->EmotionTag);
    }
}
```

---

## Практические примеры

### Пример 1: Система камер для диалогов

```cpp
// В вашем PlayerController или специальном компоненте
void ADialogueCameraManager::SetupDialogueListeners()
{
    // Найти все DialogueComponent в мире
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        if (UDialogueComponent* DialogueComp = Actor->FindComponentByClass<UDialogueComponent>())
        {
            DialogueComp->OnDialogueStarted.AddDynamic(this, &ADialogueCameraManager::OnAnyDialogueStarted);
            DialogueComp->OnDialogueEnded.AddDynamic(this, &ADialogueCameraManager::OnAnyDialogueEnded);
   }
    }
}

void ADialogueCameraManager::OnAnyDialogueStarted(UDialogueRunner* Runner)
{
    // Переключить на диалоговую камеру
    SetDialogueCameraActive(true);
}

void ADialogueCameraManager::OnAnyDialogueEnded()
{
    // Вернуть обычную камеру
    SetDialogueCameraActive(false);
}
```

### Пример 2: Система озвучки и субтитров

```cpp
UCLASS()
class UDialogueAudioManager : public UActorComponent
{
    GENERATED_BODY()
    
public:
 UFUNCTION()
    void HandleNodeChanged(UDialogueNode* NewNode, UDialogueNode* PreviousNode)
    {
      // Остановить предыдущую озвучку
        if (CurrentAudioComponent)
  {
            CurrentAudioComponent->Stop();
  }
        
    if (NewNode && NewNode->VoiceClip)
        {
        // Воспроизвести новую озвучку
 CurrentAudioComponent = UGameplayStatics::SpawnSound2D(
  GetWorld(), 
                NewNode->VoiceClip
            );
   
       // Показать субтитры
            ShowSubtitles(NewNode->GetDialogueText(), NewNode->VoiceClip->Duration);
        }
    }
    
private:
    UPROPERTY(Transient)
    UAudioComponent* CurrentAudioComponent;
};
```

### Пример 3: Система достижений

```cpp
void UAchievementManager::SetupDialogueTracking()
{
    // Подписаться на все завершения диалогов
    for (UDialogueComponent* DialogueComp : AllDialogueComponents)
    {
        DialogueComp->OnDialogueEnded.AddDynamic(this, &UAchievementManager::OnDialogueCompleted);
    }
}

void UAchievementManager::OnDialogueCompleted()
{
    TotalDialoguesCompleted++;
    
    // Проверить достижения
    if (TotalDialoguesCompleted >= 10)
    {
        UnlockAchievement("Chatty");
    }
    
    if (TotalDialoguesCompleted >= 100)
    {
        UnlockAchievement("Conversationalist");
 }
}
```

---

## Blueprint: Пошаговое руководство

### Шаг 1: Получить Dialogue Component

1. В вашем Blueprint (например, NPC Character)
2. Добавьте `Dialogue Component` через компоненты панель
3. Или получите его через `Get Component by Class`

### Шаг 2: Подписаться на события

1. Выберите Dialogue Component
2. В Details панели найдите секцию **Events**
3. Нажмите `+` рядом с нужным событием:
   - `On Dialogue Started`
   - `On Dialogue Ended`
   - `On Dialogue Node Changed`

### Шаг 3: Реализовать логику

Пример Blueprint Event Graph:

```
Event BeginPlay
    ?
    ?
Get Component by Class (DialogueComponent)
    ?
    ?
Bind Event to OnDialogueStarted ??? Custom Event: MyDialogueStartedHandler
    ?
    ?
Bind Event to OnDialogueEnded ??? Custom Event: MyDialogueEndedHandler
    ?
    ?
Bind Event to OnDialogueNodeChanged ??? Custom Event: MyNodeChangedHandler


Custom Event: MyNodeChangedHandler (NewNode, PreviousNode)
    ?
    ?
Is Valid? (NewNode)
    ??? True
    ?   ?
    ?   ?
    ?   Get Dialogue Text
    ?   ?
    ?   ?
    ?   Print String
    ?   ?
    ?   ?
    ?   Play Sound 2D (Voice Clip)
    ?
    ??? False
  ?
        ?
  (Do nothing - first line)
```

---

## Важные замечания

### Порядок вызова событий

При начале диалога:
1. `OnDialogueStarted` (компонент)
2. `OnNodeEntered` ? `OnDialogueNodeChanged` (первая реплика)

При смене реплики:
1. `OnNodeEntered` ? `OnDialogueNodeChanged`

При завершении диалога:
1. `OnDialogueEnded` (компонент)

### Thread Safety

Все события вызываются в **Game Thread**, поэтому безопасно:
- Обращаться к UObject
- Вызывать Blueprint ноды
- Взаимодействовать с UI

### Memory Management

- Не храните прямые ссылки на `UDialogueRunner` дольше чем необходимо
- `PreviousNode` может быть `nullptr` при первой реплике
- Всегда проверяйте `IsValid()` перед использованием узлов

### Performance

События вызываются синхронно, поэтому:
- Избегайте тяжёлых операций в обработчиках
- Используйте Async Tasks для длительных операций
- Кешируйте данные где возможно

---

## Часто задаваемые вопросы

**Q: Могу ли я подписаться на события нескольких NPC одновременно?**  
A: Да! Просто подпишитесь на события каждого `DialogueComponent` отдельно.

**Q: Как отписаться от событий?**  
A: Используйте `Remove` или `RemoveDynamic`:
```cpp
DialogueComp->OnDialogueStarted.RemoveDynamic(this, &AMyClass::MyHandler);
```

**Q: Можно ли изменять диалог внутри обработчика события?**  
A: Да, но будьте осторожны с рекурсией. Лучше использовать `JumpToNode` через задержку.

**Q: События срабатывают в мультиплеере?**  
A: События локальные для каждого клиента. Для синхронизации используйте RPC.

---

## См. также

- [DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)
- [Complete Examples](05_Complete_Examples.md)
- [DialogueComponent API Reference](../Source/DialogueSystemRuntime/Public/Components/DialogueComponent.h)
