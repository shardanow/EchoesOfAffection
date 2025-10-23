# ?? Memory Effects: Статус Интеграции с Dialogue System

> **Вопрос:** Можно ли добавлять воспоминания через Data Asset в диалоге?  
> **Ответ:** Частично. Требуется доработка интеграции.  
> **Версия:** v1.3.1  
> **Дата:** 2025-01-23

---

## ? Что УЖЕ Реализовано

### **1. Memory Effects Существуют**

Классы находятся в `DialogueSystemCore`:

```cpp
// Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/MemoryEffects.h

// Базовый класс
UMemoryEffectBase : public UObject

// Конкретные эффекты:
?? UAddMemoryEffect           // Создать воспоминание
?? UModifyMemoryStrengthEffect // Изменить силу
?? URemoveMemoryEffect// Удалить
?? URefreshMemoryEffect        // "Освежить"
```

**Параметры:**

| Effect Class | Параметры | Что делает |
|--------------|-----------|------------|
| `UAddMemoryEffect` | `MemoryTag`, `MemoryStrength (0-1)` | Создает новое воспоминание |
| `UModifyMemoryStrengthEffect` | `MemoryTag`, `StrengthModifier`, `bAdditive` | Изменяет силу существующего |
| `URemoveMemoryEffect` | `MemoryTag` | Удаляет воспоминание |
| `URefreshMemoryEffect` | `MemoryTag` | "Обновляет" timestamp |

---

### **2. Memory Effects Работают Напрямую с NPC**

```cpp
// MemoryEffects.cpp - реализация
void UAddMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
    // Находит NPCMemoryComponent через Reflection
    UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    
    // Вызывает AddMemoryByTag через ProcessEvent (Reflection)
    MemoryComp->ProcessEvent(AddMemoryFunc, &Params);
}
```

**Важно:** Используется **reflection** для вызова методов NPCMemoryComponent, чтобы избежать зависимости `DialogueSystemCore ? DialogueSystemRuntime`.

---

### **3. Dialogue Effects Существуют**

Классы находятся в `DialogueSystemCore`:

```cpp
// Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/DialogueEffectExecutor.h

// Базовый класс
UDialogueEffect : public UObject

// Конкретные эффекты:
?? UDialogueEffect_ModifyAffinity // Изменить симпатию
?? UDialogueEffect_ModifyTrust         // Изменить доверие
?? UDialogueEffect_ModifyInventory     // Дать/забрать предмет
?? UDialogueEffect_SetMemory           // Установить булев флаг
?? UDialogueEffect_SetVariable       // Установить переменную
?? UDialogueEffect_AddWorldStateTag    // Добавить тег
?? UDialogueEffect_StartQuest     // Запустить квест
```

---

### **4. DialogueNode Имеет Effects Array**

```cpp
// DialogueNode.h
UCLASS()
class UDialogueNode : public UObject
{
    GENERATED_BODY()
    
    /** Эффекты ноды */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TArray<TObjectPtr<UDialogueEffect>> Effects;
};
```

**Значит:** В теории можно добавлять эффекты в Data Asset!

---

## ? Что НЕ Работает (Проблемы)

### **Проблема 1: Memory Effects НЕ наследуют UDialogueEffect**

```cpp
// ? ТЕКУЩАЯ РЕАЛИЗАЦИЯ:
UMemoryEffectBase : public UObject
{
virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor);
    //   ? Другая сигнатура!
};

UDialogueEffect : public UObject
{
    virtual void Execute_Implementation(UDialogueSessionContext* Context);
//      ? Другой параметр!
};
```

**Проблема:** Разные сигнатуры `Execute()`:
- `UMemoryEffectBase::Execute(AActor*, AActor*)` - принимает акторов напрямую
- `UDialogueEffect::Execute(UDialogueSessionContext*)` - принимает контекст

**Результат:** Memory Effects **НЕЛЬЗЯ добавить в `DialogueNode::Effects`** напрямую!

---

### **Проблема 2: Нет Адаптера/Wrapper**

```cpp
// Нужно что-то типа:
class UDialogueEffect_Memory : public UDialogueEffect
{
    UMemoryEffectBase* MemoryEffect; // Wrapper!
    
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override
    {
        // Извлечь акторов из Context
        AActor* NPC = Context->GetNPC();
        AActor* Player = Context->GetPlayer();
        
        // Вызвать оригинальный Memory Effect
        MemoryEffect->Execute(NPC, Player);
    }
};
```

**НО:** Такого wrapper'а **НЕТ** в текущей реализации!

---

### **Проблема 3: DialogueNode Not Exposing Memory Effects в UI**

Даже если бы wrapper существовал, в Dialogue Editor нет UI для добавления Memory Effects.

---

## ?? Решения

### **Вариант 1: Создать Wrapper Classes (Рекомендуется)**

Создать промежуточные классы-адаптеры:

```cpp
// NEW: DialogueEffect_AddMemory.h
UCLASS(DisplayName = "Add Memory", meta = (Category = "Memory"))
class UDialogueEffect_AddMemory : public UDialogueEffect
{
    GENERATED_BODY()
    
public:
    /** Memory Tag */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag MemoryTag;
    
    /** Memory Strength (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
  float MemoryStrength = 1.0f;
    
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override
    {
        AActor* NPCActor = Context->GetNPCActor();
   AActor* PlayerActor = Context->GetPlayerActor();
        
   if (!NPCActor)
        {
            return;
        }
   
        // Вызвать оригинальный Memory Effect через reflection
UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
   if (!MemoryCompClass) return;
        
        UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    if (!MemoryComp) return;
        
  UFunction* AddMemoryFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
        if (!AddMemoryFunc) return;
        
        struct FAddMemoryParams
        {
            FGameplayTag Tag;
    float Strength;
        };
        
        FAddMemoryParams Params;
        Params.Tag = MemoryTag;
        Params.Strength = MemoryStrength;
        
        MemoryComp->ProcessEvent(AddMemoryFunc, &Params);
    }
    
    virtual FText GetDisplayText_Implementation() const override
    {
    return FText::Format(
FText::FromString("Add Memory: {0} (Strength: {1})"),
     FText::FromString(MemoryTag.ToString()),
            FText::AsNumber(MemoryStrength)
        );
 }
};
```

**Аналогично для:**
- `UDialogueEffect_ModifyMemoryStrength`
- `UDialogueEffect_RemoveMemory`
- `UDialogueEffect_RefreshMemory`

---

### **Вариант 2: Использовать DSL Effects (Текущий Workaround)**

Пока wrapper'ов нет, можно использовать **строковые DSL effects**:

```cpp
// В DialogueNode::EffectExpression
EffectExpression = "AddMemory(Event.Betrayal, 0.95); ModifyAffinity(Anna, -40)";
```

**НО:** DSL parser (`UDialogueEffectParser`) **НЕ поддерживает Memory Effects** в текущей реализации!

Нужно добавить в `DialogueEffectParser.cpp`:

```cpp
// NEW: Добавить парсинг Memory Effects
UDialogueEffect* UDialogueEffectParser::ParseSingleEffect(const TArray<FString>& Tokens)
{
    FString Command = Tokens[0].ToLower();
    TArray<FString> Args = Tokens;
    Args.RemoveAt(0);
    
    // ... existing parsers ...
    
    // NEW: Memory Effects
    if (Command == TEXT("addmemory"))
  {
        return ParseAddMemory(Args);
  }
    else if (Command == TEXT("modifymemorystrength"))
    {
        return ParseModifyMemoryStrength(Args);
 }
    // ... etc
}

UDialogueEffect* UDialogueEffectParser::ParseAddMemory(const TArray<FString>& Args)
{
    if (Args.Num() < 2)
    {
        UE_LOG(LogDialogueParser, Error, TEXT("AddMemory requires 2 arguments: tag, strength"));
    return nullptr;
 }
    
    // Создать wrapper!
    UDialogueEffect_AddMemory* Effect = NewObject<UDialogueEffect_AddMemory>();
    Effect->MemoryTag = FGameplayTag::RequestGameplayTag(FName(*Args[0]), false);
    Effect->MemoryStrength = FCString::Atof(*Args[1]);
    
    return Effect;
}
```

---

### **Вариант 3: Blueprint Approach (Временное Решение)**

Создать Blueprint-наследник `UDialogueEffect` с вызовом Memory Component:

```
Blueprint: BP_DialogueEffect_AddMemory
Parent: UDialogueEffect

Variables:
- MemoryTag: FGameplayTag
- MemoryStrength: float

Execute (Override):
?? Get NPC Actor (from Context)
?? Get Component by Class: NPCMemoryComponent
?? NPCMemoryComponent ? Add Memory By Tag
   ?? Memory Tag: MemoryTag
   ?? Strength: MemoryStrength
```

**Плюс:** Работает прямо сейчас без изменения кода  
**Минус:** Нужно создавать отдельный Blueprint для каждого типа эффекта  

---

## ?? План Интеграции (v1.4)

### **Фаза 1: Создать Wrapper Classes (1 день)**

```
Новые файлы:
????????????????????????????????????????????????????????????????????
Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/
?? DialogueMemoryEffects.h     (новый)
?? DialogueMemoryEffects.cpp   (новый)

Классы:
?? UDialogueEffect_AddMemory
?? UDialogueEffect_ModifyMemoryStrength
?? UDialogueEffect_RemoveMemory
?? UDialogueEffect_RefreshMemory
```

**Объем:** ~300-400 строк кода

---

### **Фаза 2: Добавить DSL Parser Support (2-3 часа)**

```
Изменения в DialogueEffectParser.cpp:
????????????????????????????????????????????????????????????????????
1. ParseAddMemory()
2. ParseModifyMemoryStrength()
3. ParseRemoveMemory()
4. ParseRefreshMemory()
5. Регистрация в ParseSingleEffect()
```

**Объем:** ~100-150 строк кода

---

### **Фаза 3: Обновить Документацию (1 час)**

```
Добавить в README:
????????????????????????????????????????????????????????????????????
1. Примеры использования Memory Effects в Data Asset
2. DSL синтаксис: AddMemory(tag, strength)
3. Blueprint примеры
```

---

## ?? Текущий Статус: Как Использовать

### **? Что МОЖНО делать СЕЙЧАС:**

#### **Вариант A: Программно из C++**

```cpp
// В любом месте игровой логики
UNPCMemoryComponent* MemComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
MemComp->AddMemoryByTag(
    FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")),
 0.95f
);
```

#### **Вариант B: Через Blueprint Events**

```
Event Graph (Dialogue Blueprint):
????????????????????????????????????????????????????????????????????
Event: On Dialogue Choice Selected
?? Branch: Choice ID == "BetrayChoice"?
?  ?? True:
?     ?? Get NPC Component: NPCMemoryComponent
?   ?? Add Memory By Tag
?  ?? Tag: "Event.Betrayal"
?    ?? Strength: 0.95
```

#### **Вариант C: Через Custom DialogueEffect (Blueprint)**

Создать Blueprint-класс `BP_DialogueEffect_AddMemory` как описано в Варианте 3 выше.

---

### **? Что НЕ РАБОТАЕТ сейчас:**

#### **В Dialogue Data Asset:**

```cpp
// ? НЕ РАБОТАЕТ - Memory Effects не в списке!
DialogueNode.Effects.Add(UAddMemoryEffect);

// ? НЕ РАБОТАЕТ - DSL parser не знает про AddMemory
DialogueNode.EffectExpression = "AddMemory(Event.Betrayal, 0.95)";
```

---

## ?? Итоговая Таблица

| Способ | Статус | Сложность | Когда |
|--------|--------|-----------|-------|
| **Программно (C++)** | ? Работает | ? Легко | Сейчас |
| **Blueprint Events** | ? Работает | ? Легко | Сейчас |
| **Custom BP Effect** | ? Работает | ?? Средне | Сейчас (1 час setup) |
| **Data Asset (Wrapper)** | ? Требует кода | ??? Средне | v1.4 (1 день) |
| **Data Asset (DSL)** | ? Требует кода | ?? Легко | v1.4 (3 часа) |

---

## ?? Рекомендации

### **Для НЕМЕДЛЕННОГО использования (сейчас):**

**Используйте Custom Blueprint Effect:**

1. Создайте Blueprint класс `BP_DialogueEffect_AddMemory`
2. Parent Class: `UDialogueEffect`
3. Добавьте переменные `MemoryTag`, `MemoryStrength`
4. В `Execute` событии вызовите `NPCMemoryComponent->AddMemoryByTag()`
5. Добавьте этот Blueprint в `DialogueNode::Effects` через Dialogue Editor

**Время:** 30-60 минут setup  
**Результат:** Работает прямо сейчас! ?

---

### **Для ДОЛГОСРОЧНОГО решения (v1.4):**

**Реализовать Wrapper Classes:**

1. Создать `UDialogueEffect_AddMemory` и остальные wrapper'ы (C++)
2. Добавить DSL parser support
3. Обновить документацию

**Время:** 1 день разработки  
**Результат:** Полная интеграция в Dialogue System ?

---

## ?? Пример: Быстрое Решение (Blueprint)

### **Шаг 1: Создать Blueprint Effect**

```
Content Browser ? Right Click ? Blueprint Class
Parent: DialogueEffect
Name: BP_DialogueEffect_AddMemory
```

### **Шаг 2: Добавить Variables**

```
Variables:
?? MemoryTag (FGameplayTag)
?  ?? Default: Event.Betrayal
?? MemoryStrength (float)
   ?? Default: 0.95
```

### **Шаг 3: Implement Execute**

```
Event Graph:
????????????????????????????????????????????????????????????????????
Event: Execute (Override)
?? Context ? Get NPC Actor
?? Get Component by Class: NPCMemoryComponent
?? Add Memory By Tag
   ?? Memory Tag: MemoryTag (variable)
   ?? Strength: MemoryStrength (variable)
```

### **Шаг 4: Использовать в Data Asset**

```
Dialogue Data Asset ? DialogueNode
?? Effects:
   ?? Add Element
      ?? Select: BP_DialogueEffect_AddMemory
   ?? Memory Tag: Event.Rescue
         ?? Memory Strength: 0.85
```

**Готово!** Теперь при выборе этой реплики создастся воспоминание! ??

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Complete

**Вывод:** Memory Effects существуют, но не интегрированы напрямую с Dialogue System. **Временное решение:** Custom Blueprint Effects (30 минут). **Полное решение:** Wrapper classes в v1.4 (1 день). ??
