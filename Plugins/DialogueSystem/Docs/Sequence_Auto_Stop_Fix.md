# Sequence Auto-Stop on Dialogue End (v1.13)

## Проблема

Раніше, якщо діалог завершувався **під час відтворення секвенсу**, виникали баги:

1. **Камера проваливалася в гравця** — секвенс продовжував контролювати камеру після завершення діалогу
2. **Актори починали бігти під час секвенсу** — вони поверталися до свого розкладу (schedule), коли секвенс ще грав
3. **Секвенс продовжував грати після діалогу** — немає автоматичної зупинки

## Рішення

Додана автоматична **реєстрація та зупинка секвенсу** при завершенні діалогу.

### Архітектура

```
DialogueRunner (власник)
    ??> DialogueSessionContext (зберігає weak ref на Runner)
  ??> DialogueEffect_PlaySequence (реєструється при Execute)
```

### Життєвий цикл

1. **Старт секвенсу** (`DialogueEffect_PlaySequence::Execute`)
   - Секвенс отримує `Context->GetOwningRunner()`
   - Реєструється через `Runner->RegisterActiveSequence(this)`
   - Зберігається в `DialogueRunner::ActiveSequenceEffect`

2. **Завершення діалогу** (`DialogueRunner::EndDialogue`)
   - Перевіряє `ActiveSequenceEffect->IsPlaying()`
   - Викликає `ActiveSequenceEffect->StopSequence()`
   - **ДО** очищення контексту та інших ресурсів

3. **Природне завершення секвенсу** (`OnSequenceFinished`)
   - Викликає `CleanupSequence()`
   - Відміняє реєстрацію через `Runner->ClearActiveSequence()`
   - Відновлює трансформи акторів
   - Відновлює камеру

### Зміни в коді

#### DialogueRunner.h
```cpp
// Нове поле
UPROPERTY(Transient)
TObjectPtr<UDialogueEffect_PlaySequence> ActiveSequenceEffect;

// Нові методи
void RegisterActiveSequence(UDialogueEffect_PlaySequence* SequenceEffect);
void ClearActiveSequence();
```

#### DialogueRunner.cpp
```cpp
void UDialogueRunner::EndDialogue()
{
    // CRITICAL: Stop any active sequence before cleanup
if (ActiveSequenceEffect && ActiveSequenceEffect->IsPlaying())
  {
      ActiveSequenceEffect->StopSequence();
    }
    ActiveSequenceEffect = nullptr;
    
    // ... existing cleanup code ...
}
```

#### DialogueEffect_PlaySequence.h
```cpp
// Нові методи
UFUNCTION(BlueprintCallable, Category = "Effect")
void StopSequence();

UFUNCTION(BlueprintPure, Category = "Effect")
bool IsPlaying() const;

// Внутрішня логіка
void CleanupSequence();
```

#### DialogueEffect_PlaySequence.cpp
```cpp
void Execute_Implementation(UDialogueSessionContext* Context)
{
    // Register with DialogueRunner
    if (UDialogueRunner* Runner = Cast<UDialogueRunner>(Context->GetOwningRunner()))
    {
        Runner->RegisterActiveSequence(this);
    }
    
    // ... start sequence playback ...
}

void StopSequence()
{
    if (SequencePlayer)
    {
    SequencePlayer->Stop();
        SequencePlayer->OnFinished.RemoveAll(this);
    }
    
    CleanupSequence();
}

void CleanupSequence()
{
    // Unregister from DialogueRunner
    if (UDialogueRunner* Runner = Cast<UDialogueRunner>(Context->GetOwningRunner()))
    {
      Runner->ClearActiveSequence();
    }
    
    // Restore transforms, camera, destroy actor
}
```

#### DialogueContext.h
```cpp
// Додано weak reference на owning runner
UPROPERTY(Transient)
TWeakObjectPtr<UObject> OwningRunner;

void SetOwningRunner(UObject* Runner) { OwningRunner = Runner; }
UObject* GetOwningRunner() const { return OwningRunner.Get(); }
```

### Переваги

? **Немає провалювання камери** — секвенс зупиняється і камера коректно відновлюється  
? **Актори не бігають під час секвенсу** — schedule не відновлюється передчасно  
? **Автоматична очистка** — секвенс зупиняється навіть якщо діалог закривається раптово  
? **Безпечно для GC** — використовується `TWeakObjectPtr` для уникнення циклічних посилань  
? **Сумісність з Blueprint** — всі методи доступні через Blueprint  

### Приклади використання

#### C++
```cpp
// Автоматично працює! Нічого не потрібно змінювати
UDialogueRunner* Runner = GetDialogueRunner();
Runner->StartDialogue(DialogueAsset, Participants);

// Якщо діалог завершиться під час секвенсу - секвенс автоматично зупиниться
Runner->EndDialogue();
```

#### Blueprint
Працює автоматично! Якщо ви використовуєте `PlaySequence` effect в діалозі, він буде зупинений при завершенні діалогу.

### Тестування

1. Створіть діалог з `PlaySequence` effect
2. Запустіть діалог
3. Під час відтворення секвенсу натисніть **ESC** або закрийте діалог
4. **Результат:** 
   - Секвенс миттєво зупиняється
   - Камера відновлюється на гравця
   - Трансформи акторів відновлюються
   - Schedule відновлюється коректно

### Відомі обмеження

- **Один активний секвенс** — підтримується тільки один секвенс за раз (достатньо для більшості випадків)
- **Потребує Context** — секвенс повинен мати доступ до `DialogueSessionContext` для реєстрації

### Версія

**v1.13** — Sequence Auto-Stop on Dialogue End

### Автор

Implemented by GitHub Copilot Assistant

### Дата

2025-11-05
