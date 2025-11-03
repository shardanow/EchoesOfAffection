# DialogueEffect не работает - Быстрая помощь

## ?? Проблема

DialogueEffect_ModifyRelationship (ExecuteAction) НЕ изменяет Trust.

---

## ? Быстрая проверка (5 минут)

### **1. Проверьте Output Log**

Запустите диалог и проверьте лог:

```
? Хорошо:
LogTemp: UDialogueEffect_ModifyRelationship: Executed action 'Relationship.Action.GiveCompliment'
LogRelationshipSystem: Modified dimension 'Relationship.Dimension.Trust' by 5.0

? Проблема:
LogTemp: RelationshipComponent not found on BP_Lianne_C
```

---

### **2. Есть ли NPCRelationshipComponent?**

Откройте Blueprint NPC ? Components панель:

```
? Должно быть:
Components:
  NPCRelationshipComponent
    Profile: DA_Profile_Lianne

? Если нет:
  Add Component ? NPCRelationshipComponent
```

---

### **3. Создан ли Relationship?**

Консоль (~) ? введите:

```
RelDebugCount
```

**Ожидается:**
```
Active relationships: 1
```

**Если 0:**
```cpp
// Добавьте в BeginPlay NPC:

Get Component ? NPCRelationshipComponent
  ?
Get Player Controller ? Get Pawn
  ?
CreateRelationshipWith (Player)
```

---

### **4. Правильный ли ActionTag?**

Проверьте в DialogueNode:

```
? Правильно:
ActionTag: Relationship.Action.GiveCompliment

? Неправильно:
ActionTag: Action.GiveCompliment
ActionTag: Relationship.Action.ComplimentGive
```

---

### **5. Существует ли Action DataAsset?**

```
Content/Data/Relationships/Actions/DA_Action_GiveCompliment

? Должен быть:
  AssetTag: Relationship.Action.GiveCompliment
  Effects:
    [0] DimensionTag: Relationship.Dimension.Trust
        BaseValue: 5.0
```

---

## ?? Быстрое решение

**Если ничего не помогает, используйте DirectModify:**

### **Вместо ExecuteAction:**

```
? Mode: ExecuteAction
  ActionTag: Relationship.Action.GiveCompliment
```

### **Используйте DirectModify:**

```
? Mode: DirectModify
  DimensionModifiers:
    [0]
 DimensionTag: Relationship.Dimension.Trust
      Delta: 5.0
```

**Это ВСЕГДА работает** ?

---

## ?? Чеклист (30 секунд)

- [ ] NPCRelationshipComponent на NPC?
- [ ] Relationship создан? (`RelDebugCount`)
- [ ] ActionTag правильный?
- [ ] Action DataAsset существует?
- [ ] Логи показывают выполнение?

**Если хоть один НЕТ** ? см. полную документацию

---

## ?? Полная документация

**[DIALOGUE_EFFECT_NOT_WORKING.md](DIALOGUE_EFFECT_NOT_WORKING.md)** - Детальная диагностика

---

**Версия:** 1.0  
**Дата:** 2 ноября 2025
