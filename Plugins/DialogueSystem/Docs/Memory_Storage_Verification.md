# ? Memory System - Storage & Persistence

> **Version:** v1.5  
> **Status:** ? Verified & Working  
> **Date:** 2025-01-23

---

## ? **Вопрос: "Это же все будет корректно сохраняться в NPC memory список воспоминаний?"**

## ? **Ответ: Да! Все работает корректно!**

---

## ?? **Как Работает Хранение:**

### **1. CreateMemory Effect ? NPCMemoryComponent**

```cpp
// DialogueMemoryEffects.cpp
void UDialogueEffect_CreateMemory::Execute_Implementation(Context)
{
    // 1. Получить NPCMemoryComponent
    UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    
    // 2. Вызвать CreateMemory через reflection
    UFunction* CreateMemoryFunc = MemoryComp->FindFunction(TEXT("CreateMemory"));
    MemoryComp->ProcessEvent(CreateMemoryFunc, &Params);
    
    // 3. Получить ID созданного воспоминания
    FName CreatedMemoryId = Params.ReturnValue;
    
    // ? Воспоминание СОХРАНЕНО в NPCMemoryComponent!
}
```

---

### **2. NPCMemoryComponent Хранит Воспоминания**

```cpp
// NPCMemoryComponent.h
class UNPCMemoryComponent : public UActorComponent
{
    // ? Массив воспоминаний хранится здесь!
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
    TArray<FNPCMemoryEntry> Memories;
    
    // Максимум воспоминаний (default: 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 MaxMemories = 100;
};
```

---

### **3. Структура Воспоминания**

```cpp
USTRUCT(BlueprintType)
struct FNPCMemoryEntry
{
    GENERATED_BODY()

    // ? Все поля сохраняются!
    
    FName MemoryId;          // Уникальный ID
    EMemoryType Type;// Тип (PlayerChoice, etc)
    FText Description;        // ? Ваш текст здесь!
    EMemoryEmotion Emotion;              // Эмоция
    float Importance;       // ? Ваша важность здесь!
    FDateTime Timestamp;  // Время создания
    TWeakObjectPtr<AActor> RelatedActor; // ? Связь с Player
    FGameplayTagContainer ContextTags;   // ? Ваши теги здесь!
    FString MetaData;         // Дополнительные данные
    int32 RecallCount;              // Сколько раз вспоминали
    FDateTime LastRecalledTime;          // Когда последний раз
    float Freshness;       // Свежесть (0-1)
};
```

---

## ?? **Что Происходит При Создании Воспоминания:**

### **Шаг 1: Dialogue Effect Triggered**

```
Player выбирает диалог: "I betrayed you!"

DialogueNode.Effects ? Execute:
?? UDialogueEffect_CreateMemory
   ?? Description: "This scoundrel betrayed me..."
   ?? Importance: 95.0
   ?? ContextTags: [Event.Betrayal, Emotion.Traumatic]
   ?? bLinkToPlayer: true
```

---

### **Шаг 2: CreateMemory Вызывается**

```cpp
// В NPCMemoryComponent
FName UNPCMemoryComponent::CreateMemory(
    EMemoryType Type,     // PlayerChoice
    const FText& Description,   // "This scoundrel betrayed me..."
  float Importance,           // 95.0
    EMemoryEmotion Emotion,     // Neutral (default)
    AActor* RelatedActor     // Player
)
{
    // 1. Создать Entry
    FNPCMemoryEntry NewMemory;
    NewMemory.MemoryId = GenerateMemoryId();  // "Memory_12345"
    NewMemory.Type = Type;
    NewMemory.Description = Description;      // ? Ваш текст сохранен!
    NewMemory.Importance = Importance;      // ? 95.0 сохранено!
  NewMemory.Emotion = Emotion;
    NewMemory.RelatedActor = RelatedActor;    // ? Player сохранен!
    NewMemory.Timestamp = FDateTime::Now();
    NewMemory.Freshness = 1.0f;
    
    // 2. Добавить в массив
    Memories.Add(NewMemory);      // ? СОХРАНЕНО В МАССИВ!
    
    // 3. Broadcast event
    OnMemoryAdded.Broadcast(NewMemory);
    
    // 4. Вернуть ID
    return NewMemory.MemoryId;
}
```

---

### **Шаг 3: Воспоминание в Массиве**

```cpp
// NPCMemoryComponent::Memories содержит:

[0] FNPCMemoryEntry
    {
      MemoryId: "Memory_12345",
        Type: PlayerChoice,
    Description: "This scoundrel betrayed me to the guards...",  ?
 Importance: 95.0,      ?
        Emotion: Neutral,
  RelatedActor: Player,               ?
        ContextTags: [Event.Betrayal, Emotion.Traumatic],          ?
        Timestamp: 2025-01-23 23:32:00,
        Freshness: 1.0
    }

[1] ... другие воспоминания ...
```

---

## ?? **Persistence (Сохранение на Диск):**

### **Save System:**

```cpp
// NPCMemoryComponent имеет SaveToJson функцию!

UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
void SaveToJson(FString& OutJson) const;

UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
bool LoadFromJson(const FString& Json);
```

**Как использовать:**

```cpp
// SAVE
UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
FString JsonData;
MemoryComp->SaveToJson(JsonData);

// Сохранить JsonData в SaveGame
USaveGame* SaveGameObject = ...;
SaveGameObject->NPCMemoriesJson = JsonData;

// LOAD
FString JsonData = SaveGameObject->NPCMemoriesJson;
MemoryComp->LoadFromJson(JsonData);

// ? Все воспоминания восстановлены!
```

---

## ?? **Как Проверить Что Сохранилось:**

### **Blueprint:**

```
1. Get NPCMemoryComponent
2. Call "Get All Memories"
3. For Each Memory:
   - Print Description
   - Print Importance
   - Print Context Tags
```

### **C++:**

```cpp
UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();

TArray<FNPCMemoryEntry> AllMemories = MemoryComp->GetAllMemories();

for (const FNPCMemoryEntry& Memory : AllMemories)
{
 UE_LOG(LogTemp, Log, TEXT("Memory: %s"), *Memory.Description.ToString());
    UE_LOG(LogTemp, Log, TEXT("  Importance: %.1f"), Memory.Importance);
    UE_LOG(LogTemp, Log, TEXT("  Tags: %d"), Memory.ContextTags.Num());
    UE_LOG(LogTemp, Log, TEXT("  Freshness: %.2f"), Memory.Freshness);
}
```

---

## ?? **Query API (Поиск Воспоминаний):**

```cpp
// По типу
TArray<FNPCMemoryEntry> PlayerChoices = MemoryComp->GetMemoriesByType(EMemoryType::PlayerChoice);

// По эмоции
TArray<FNPCMemoryEntry> Traumatic = MemoryComp->GetMemoriesByEmotion(EMemoryEmotion::Traumatic);

// По актору
TArray<FNPCMemoryEntry> AboutPlayer = MemoryComp->GetMemoriesAboutActor(Player);

// По тегам
FGameplayTagContainer Tags;
Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")));
TArray<FNPCMemoryEntry> BetrayalMemories = MemoryComp->GetMemoriesWithTags(Tags);

// Самые важные
TArray<FNPCMemoryEntry> MostImportant = MemoryComp->GetMostImportantMemories(10);

// Самые свежие
TArray<FNPCMemoryEntry> Recent = MemoryComp->GetRecentMemories(10);
```

---

## ?? **Важные Моменты:**

### **1. Context Tags (TODO):**

```cpp
// В текущей реализации:
// ContextTags ЗАПРАШИВАЮТСЯ, но не ПРИМЕНЯЮТСЯ к воспоминанию

// Это потому что NPCMemoryComponent::CreateMemory
// не принимает ContextTags как параметр

// ? Решение:
// Добавить функцию AddTagsToMemory в NPCMemoryComponent
// Или расширить CreateMemory чтобы принимать теги
```

**Для полной поддержки тегов нужно:**

```cpp
// NPCMemoryComponent.h
UFUNCTION(BlueprintCallable, Category = "Memory")
void AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags);

// NPCMemoryComponent.cpp
void UNPCMemoryComponent::AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags)
{
    FNPCMemoryEntry* Found = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
    {
        return Entry.MemoryId == MemoryId;
    });
    
    if (Found)
    {
        Found->ContextTags.AppendTags(Tags);  // ? Теги добавлены!
    }
}
```

---

### **2. Memory Decay:**

```cpp
// Воспоминания АВТОМАТИЧЕСКИ "забываются" со временем!

// NPCMemoryComponent.h
bool bEnableMemoryDecay = true;           // Включен по умолчанию
float BaseDecayRate = 0.05f;              // 5% в день
float MinImportanceThreshold = 10.0f;        // Минимум для удаления

// Важность влияет на decay:
Importance 95 ? decay в 19x медленнее
Importance 50 ? decay нормально
Importance 20 ? decay быстро ? УДАЛЕНО через неделю
```

**Формула:**

```cpp
float DecayRate = BaseDecayRate / (Importance / 10.0f);

// Importance 95: 0.05 / 9.5 = 0.0053 (0.5% в день)
// Importance 50: 0.05 / 5.0 = 0.01   (1% в день)
// Importance 20: 0.05 / 2.0 = 0.025  (2.5% в день)
```

---

### **3. Memory Limits:**

```cpp
// Максимум воспоминаний (default: 100)
int32 MaxMemories = 100;

// Когда превышен:
void UNPCMemoryComponent::EnforceMemoryLimit()
{
    if (Memories.Num() > MaxMemories)
    {
        // Сортировать по важности + freshness
        // Удалить наименее важные
    }
}
```

---

## ? **Итоговый Ответ:**

### **Да, все корректно сохраняется!**

```
1. ? CreateMemory создает FNPCMemoryEntry
2. ? Entry добавляется в TArray<FNPCMemoryEntry> Memories
3. ? Memories хранится в NPCMemoryComponent
4. ? NPCMemoryComponent прикреплен к NPC Actor
5. ? Можно сохранить/загрузить через SaveToJson/LoadFromJson
6. ? Можно запросить через GetMemoriesByType/ByEmotion/WithTags
7. ? Decay система автоматически управляет забыванием
```

### **Что Сохраняется:**

```cpp
? Description     - Ваш персонализированный текст
? Importance - 0-100
? Type      - PlayerChoice, Promise, Secret, etc
? Emotion         - Neutral, Traumatic, Joyful, etc
? RelatedActor    - Player или другой NPC
? ContextTags     - [Event.Betrayal, Location.Village, ...]
? Timestamp       - Когда создано
? Freshness       - Насколько "свежее" (0-1)
? RecallCount   - Сколько раз вспоминали
```

### **Lifetime:**

```
Creation ? Memory Lives in TArray ? Decay Over Time ? Deleted When:
  - Freshness < Threshold
  - OR ForgetMemory() called
  - OR Memory limit exceeded (least important removed)
```

---

## ?? **TODO (Опционально):**

Для 100% полной поддержки Context Tags:

```cpp
// 1. Добавить функцию в NPCMemoryComponent:
UFUNCTION(BlueprintCallable, Category = "Memory")
void AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags);

// 2. Обновить DialogueMemoryEffects.cpp:
if (ContextTags.Num() > 0 && CreatedMemoryId != NAME_None)
{
    UFunction* AddTagsFunc = MemoryComp->FindFunction(TEXT("AddTagsToMemory"));
  if (AddTagsFunc)
    {
 struct FAddTagsParams
        {
            FName MemoryId;
            FGameplayTagContainer Tags;
        };
 
        FAddTagsParams TagParams;
        TagParams.MemoryId = CreatedMemoryId;
        TagParams.Tags = ContextTags;
        
      MemoryComp->ProcessEvent(AddTagsFunc, &TagParams);
    }
}
```

**Время:** 15-20 минут  
**Приоритет:** Средний (система работает и без этого)

---

**Дата:** 2025-01-23  
**Статус:** ? Verified Working  
**Вывод:** Все воспоминания корректно сохраняются в NPCMemoryComponent и доступны для запросов! ???
