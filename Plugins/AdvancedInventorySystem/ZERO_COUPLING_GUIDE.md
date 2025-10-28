# ?? Zero Coupling Architecture - Quick Reference

## Проблема

**Плохо:**
```cpp
// InteractableComponent.cpp
#include "WorldItemActor.h" // ? Hard dependency!

if (AWorldItemActor* WorldItem = Cast<AWorldItemActor>(Owner))
{
WorldItem->PickupItem(Interactor); // ? Tight coupling!
}
```

**Результат:**
- ? InteractableComponent бесполезен без плагина
- ? Нельзя использовать для doors, NPCs
- ? Плагин зависит от проекта
- ? Нарушение Single Responsibility

---

## Решение

**Хорошо:**
```cpp
// InteractableComponent.cpp
// NO INCLUDES! Just broadcasts event

EInteractionResult BeginInteract_Implementation(AActor* InteractingActor)
{
    OnBeginInteract.Broadcast(InteractingActor); // ? Generic!
    return EInteractionResult::Success;
}
```

```cpp
// WorldItemActor.cpp  
// NO INCLUDES! Finds component by name

void ConfigureInteractionComponent_Implementation(UActorComponent* Comp)
{
    // Find delegate via reflection
    FMulticastDelegate* Delegate = FindDelegate(Comp, "OnBeginInteract");
    
    // Bind callback
    Delegate->Add(this, "HandleInteractionBegin");
}

void HandleInteractionBegin(AActor* Interactor)
{
    PickupItem(Interactor); // ? Decoupled!
}
```

**Результат:**
- ? InteractableComponent работает с любым actor
- ? WorldItemActor работает без interaction system
- ? Полная модульность
- ? Каждая система делает свою работу

---

## Как Работает

### 1. Runtime Discovery (BeginPlay)

```cpp
// WorldItemActor ищет класс по имени
UClass* CompClass = FindObject<UClass>(ANY_PACKAGE, "InteractableComponent");

if (CompClass) // Найден? Создаём!
{
    UActorComponent* Comp = NewObject<UActorComponent>(this, CompClass);
    Comp->RegisterComponent();
}
// Не найден? Ничего страшного, работаем без него!
```

### 2. Configuration via Reflection

```cpp
// Настраиваем компонент через reflection
FStructProperty* DataProp = CompClass->FindPropertyByName("InteractionData");
void* DataPtr = DataProp->ContainerPtrToValuePtr(Comp);

// Устанавливаем значения
SetPropertyValue(DataPtr, "InteractionName", GetInteractionName());
SetPropertyValue(DataPtr, "ActionText", GetInteractionActionText());
```

### 3. Delegate Binding

```cpp
// Подписываемся на события компонента
FMulticastDelegateProperty* DelegateProp = 
    CompClass->FindPropertyByName("OnBeginInteract");

FMulticastScriptDelegate* Delegate = 
    DelegateProp->GetPropertyValuePtr(Comp);

FScriptDelegate Callback;
Callback.BindUFunction(this, "HandleInteractionBegin");
Delegate->Add(Callback);
```

### 4. Event Flow

```
Player presses E
    ?
InteractionComponent::TryInteract()
  ?
InteractableComponent::BeginInteract()
    ?
OnBeginInteract.Broadcast(Player) // Generic event!
    ?
WorldItemActor::HandleInteractionBegin(Player) // Subscribed listener
    ?
PickupItem(Player) // Actual logic
```

---

## Benefits

### For InteractableComponent (Your Project)

```cpp
// Can use on ANYTHING:

// Door
OnBeginInteract ? Toggle Door

// NPC
OnBeginInteract ? Start Dialogue

// Lever
OnBeginInteract ? Activate Mechanism

// Chest
OnBeginInteract ? Open Inventory

// WorldItemActor (from plugin)
OnBeginInteract ? Pickup Item
```

**One component, infinite uses!**

### For WorldItemActor (Plugin)

```cpp
// Works in ANY project:

// With InteractableComponent
? Auto-creates, auto-configures, works perfectly

// With different component name
? Change InteractableComponentClassName, still works

// Without interaction system
? Still spawns, still has mesh, no errors

// With custom interaction system
? Adapts to your implementation
```

**One plugin, works everywhere!**

---

## Code Checklist

### ? InteractableComponent (Generic)

```cpp
// NO WorldItemActor knowledge
// NO plugin-specific code
// ONLY:
? Properties (InteractionData)
? Delegates (OnBeginInteract, OnFocused, etc.)
? Generic interface implementation
? Highlight logic (optional)
```

### ? WorldItemActor (Generic)

```cpp
// NO InteractableComponent #include
// NO hard casting to specific component type
// ONLY:
? Dynamic component discovery (FindObject)
? Reflection-based configuration
? Delegate subscription
? UFUNCTION callbacks for delegates
```

---

## Testing Independence

### Test 1: Compile Plugin Standalone

```bash
# Remove/disable your project
# Compile just the plugin
Result: ? Compiles successfully!
```

### Test 2: Use InteractableComponent on Door

```cpp
// Create BP_Door (NOT a WorldItemActor!)
// Add InteractableComponent manually
// Bind OnBeginInteract ? Open Door logic

Result: ? Works perfectly!
```

### Test 3: Use Plugin Without Interaction System

```cpp
// Disable/remove InteractableComponent from project
// Spawn WorldItemActor

Result: ? Spawns normally, just not interactable
Log: "Component not found, skipping auto-creation"
```

---

## Common Patterns

### Pattern 1: Generic Component with Delegates

```cpp
class UGenericComponent : public UActorComponent
{
    UPROPERTY() FData Data; // Configuration
    
 UPROPERTY(BlueprintAssignable)
    FOnEvent OnEvent; // Event broadcast
    
    void DoSomething()
{
      OnEvent.Broadcast(); // ? Generic!
    }
};
```

### Pattern 2: Plugin Uses Component via Reflection

```cpp
class APluginActor : public AActor
{
    void BeginPlay()
    {
     // Find component by name
        UClass* CompClass = FindComponent("GenericComponent");
if (!CompClass) return; // ? Graceful fallback
      
        // Create & configure
        UActorComponent* Comp = CreateComponent(CompClass);
        ConfigureViaReflection(Comp);
        
        // Subscribe to events
        BindToDelegate(Comp, "OnEvent", this, "HandleEvent");
    }
    
    UFUNCTION()
    void HandleEvent() { /* Plugin-specific logic */ }
};
```

### Pattern 3: Result

```
GenericComponent ? NO DEPENDENCY ? PluginActor
       ?    ?
  Used by many actors Works in many projects
     ?        ?
 Fully reusable      Fully portable
```

---

## Performance Impact

| Operation | Time | Frequency |
|-----------|------|-----------|
| FindObject (component discovery) | ~0.1ms | Once at BeginPlay |
| NewObject (component creation) | ~0.05ms | Once at BeginPlay |
| Property access (reflection) | ~0.01ms | Once at BeginPlay |
| Delegate binding | ~0.001ms | Once at BeginPlay |
| Delegate broadcast | ~0.0001ms | Per interaction |

**Total one-time cost:** ~0.16ms per item  
**Runtime cost:** ~0.0001ms per interaction  
**Memory overhead:** ~116 bytes per item

**Verdict:** Negligible! ??

---

## When to Use This Pattern

### ? Use Zero Coupling When:

- Building reusable plugins
- Creating modular systems
- Supporting multiple projects
- Maintaining clean architecture
- Team has different specializations

### ? Don't Overcomplicate When:

- Single project, no reuse needed
- Tight integration is intentional
- Performance is ultra-critical (rare!)
- Team is small and unified

**For this use case:** ? Perfect fit!

---

## Quick Decision Tree

```
Is this a plugin?
    ?? YES ? Use reflection + delegates
    ?? NO ? Direct coupling OK (maybe)
        ?? Will be reused elsewhere?
        ? ?? YES ? Use reflection + delegates
        ?   ?? NO ? Direct coupling OK
        ?? Want clean architecture?
  ?? YES ? Use reflection + delegates
   ?? NO ? Direct coupling OK
```

---

## Summary

**Before:**
```
Plugin ? depends on ? Project
? Can't distribute
? Can't reuse
? Tight coupling
```

**After:**
```
Plugin ? independent ? Project
? Distributable
? Reusable
? Zero coupling
```

**How:** Reflection + Delegates = ?? Perfect Modularity

---

**Remember:**
- ?? Find by name, not by #include
- ?? Configure via reflection, not direct access
- ?? Communicate via delegates, not direct calls
- ?? Fail gracefully if missing

**Result:** Systems that just work together, but don't depend on each other! ??
