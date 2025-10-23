# ? Full Memory System - �����������!

> **����:** 2025-01-23  
> **������:** ? Compiled Successfully  
> **������:** v1.5

---

## ?? **��� �����������:**

### **5 Memory Effect Classes:**

```
? UDialogueEffect_AddMemory       // ������� (��� + ����)
? UDialogueEffect_CreateMemory       // ?? ������ (����� + ������)
? UDialogueEffect_ModifyMemoryStrength
? UDialogueEffect_RemoveMemory
? UDialogueEffect_RefreshMemory
```

### **����������:**

```
Result: Succeeded
Total execution time: 4.07 seconds

[1/6] Compile DialogueMemoryEffects.cpp  ?
[2/6] Compile Module.DialogueSystemCore   ?
[3/6] Link UnrealEditor-DialogueSystemCore.lib  ?
[4/6] Link UnrealEditor-DialogueSystemCore.dll  ?
```

---

## ?? **��� �������:**

### **1. Simple (`AddMemory`):**

```
���: �����, �������, ������� �������

DialogueNode ? AddMemory
?? Memory Tag: Event.Help
?? Strength: 0.85
```

**���������:** ������� ������������ � �����

---

### **2. Full (`CreateMemory`):** ??

```
���: �����, ������, ��������������

DialogueNode ? CreateMemory
?? Description: "The traveler saved my life when bandits attacked..."
?? Importance: 90.0
?? Context Tags: [Event.Rescue, Location.Village, Emotion.Grateful]
?? Link To Player: ? Yes
```

**���������:** ������ ������������ � ������� � ����������

---

## ?? **������� �������������:**

### **�������������:**

```
CreateMemory:
?? Description: "This scoundrel betrayed me to the guards..."
?? Importance: 95.0
?? Tags: [Event.Betrayal, Location.Village, Emotion.Traumatic]
?? Link: Player
```

### **��������:**

```
CreateMemory:
?? Description: "This hero defended our village from bandits..."
?? Importance: 90.0
?? Tags: [Event.Rescue, Event.Combat, Enemy.Bandits]
?? Link: Player
```

### **������� ����:**

```
AddMemory:
?? Tag: Character.Anna.Met
?? Strength: 1.0
```

---

## ?? **������������:**

### **��������������:**

```
���� ������� - ������ ������������:

Anna (������):
"This kind stranger helped me find my daughter..."

Guard (�����������):
"Mercenary completed the mission. Acceptable."

Bandit (����������):
"This dog killed my men. They will pay!"
```

### **Decay System:**

```
Importance 95 + Emotion.Traumatic:
? Decay � 10-20 ��� ���������
? NPC ������ ������
```

### **Filtering:**

```
// ������� ����� �� �����
HasMemoryWithTag("Event.Betrayal")
HasMemoryWithAllTags([Event.Help, Location.Village])
HasMemoryAboutPlayer("Event.Rescue")
```

---

## ?? **������������:**

1. **������ �����������:**  
   `Plugins/DialogueSystem/Docs/Full_Memory_System_Guide.md`

2. **������� ����������:**  
   `Plugins/DialogueSystem/Docs/Memory_Effects_Implementation_Guide.md`

3. **������� ������:**  
   `Plugins/DialogueSystem/Docs/Memory_System_Creation_And_Time.md`

---

## ?? **Quick Start:**

### **1. ������ Tags:**

```
Project Settings ? GameplayTags

Event.Help
Event.Betrayal
Event.Rescue
Emotion.Grateful
Emotion.Traumatic
Location.Village
```

### **2. ������ Dialogue:**

```
Dialogue Data Asset
? Add Node
? Effects ? CreateMemory
? Fill Description, Importance, Tags
```

### **3. Test:**

```
Play ? Trigger Dialogue ? Check NPC Memory
```

---

## ?? **����:**

```
? ������ ������� ������������
? ������������������� ��������
? ������� �������� � decay
? ������������� ����
? ����� � ��������
? ������� + ������ ������
? �������������� � ������!
```

**���������:**
- `AddMemory` - ��� 70% ������� (�����)
- `CreateMemory` - ��� 30% ������� (�����)

---

**����:** 2025-01-23  
**������:** ? Production Ready
**����� ����������:** ~2 ����

???? **����������� ������� ������ NPC ������!**
