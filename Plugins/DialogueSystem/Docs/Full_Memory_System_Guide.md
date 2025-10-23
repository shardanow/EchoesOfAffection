# ?? Full Memory System - Complete Guide

> **Version:** v1.5 - Full Memory Support  
> **Status:** ? Compiled Successfully  
> **Date:** 2025-01-23

---

## ? **��� �����������:**

������ � ������� **5 �������** ��� ������ � ��������������:

```
Plugins/DialogueSystem/Source/DialogueSystemCore/Effects/

? UDialogueEffect_AddMemory             // ������� (������ ���)
? UDialogueEffect_CreateMemory    // ?? ������ (����� + ������)
? UDialogueEffect_ModifyMemoryStrength
? UDialogueEffect_RemoveMemory
? UDialogueEffect_RefreshMemory
```

---

## ?? **��� ������� � �������������:**

### **1. ������� (`AddMemory`) - ��� ������ � ������� �������**

```cpp
UDialogueEffect_AddMemory
?? MemoryTag: FGameplayTag    // "Event.Help"
?? MemoryStrength: float       // 0.85
```

**����� ������������:**
- ? ����� ������� ("Player met Anna")
- ? �������� ����� ("Village saved")
- ? ������� ���������
- ? ������� ������� ��������

**������:**
```
DialogueNode: "Nice to meet you!"
?? Effect: AddMemory
   ?? Tag: Character.Anna.Met
   ?? Strength: 1.0
```

---

### **2. ������ (`CreateMemory`) - ��� �������� ��������** ??

```cpp
UDialogueEffect_CreateMemory
?? Description: FText          // "The traveler saved my life..."
?? Importance: float     // 90.0 (0-100)
?? ContextTags: TagContainer   // [Event.Rescue, Location.Village]
?? bLinkToPlayer: bool         // true
```

**����� ������������:**
- ? ������ �������� �������
- ? ������������� ������� NPC
- ? ������������������� ������������
- ? ������� ���������/���������
- ? ������ � �������������

**������:**
```
DialogueNode: "You betrayed me!"
?? Effect: CreateMemory
   ?? Description: "This scoundrel revealed my secret to the guards. I trusted them, and they stabbed me in the back. I will never forgive this."
   ?? Importance: 95.0
   ?? Context Tags:
   ?  ?? Event.Betrayal
   ?  ?? Location.Village
   ?  ?? NPC.Guard
   ?? Link To Player: ? Yes
```

---

## ?? **������������ �������:**

### **������ 1: ������������� (������ �����������)**

```
DialogueNode: "I told the guards about your secret"
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "The traveler I trusted betrayed me to the guards. They revealed the location of my hideout. Because of them, my friends were arrested. I will never trust outsiders again."
   ?  ?? Importance: 95.0
   ?  ?? Context Tags:
   ?  ?  ?? Event.Betrayal
   ?  ?  ?? Location.Hideout
   ?  ?  ?? NPC.Guard
   ?  ?  ?? Emotion.Traumatic
   ?  ?? Link To Player: ? Yes
   ?
 ?? ModifyAffinity
      ?? NPC: CurrentNPC
      ?? Delta: -60
```

**���������:**
```cpp
FNPCMemoryEntry
{
Description: "The traveler I trusted betrayed me...",
    Importance: 95.0,
    ContextTags: [Event.Betrayal, Location.Hideout, NPC.Guard, Emotion.Traumatic],
    RelatedActor: Player,
    Timestamp: 2025-01-23 23:25:00,
    Freshness: 100%
}
```

**Decay:**
- Importance 95 ? decay �� 98% ���������
- Emotion.Traumatic ? decay ��� �� 50% ���������
- **����:** NPC ����� ������� 10+ ��� �������� �������!

---

### **������ 2: �������� ������� (����������� ��������)**

```
DialogueNode: "Thank you for saving us!"
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "This brave hero defended our village when bandits attacked. They risked their life for people they barely knew. We owe them everything."
   ?  ?? Importance: 90.0
   ?  ?? Context Tags:
   ?  ?  ?? Event.Rescue
   ?  ?  ?? Event.Combat
   ?  ?  ?? Location.Village
   ?  ?  ?? Enemy.Bandits
   ?  ?  ?? Emotion.Grateful
   ?  ?? Link To Player: ? Yes
   ?
   ?? AddMemory (Simple)
   ?  ?? Tag: Quest.VillageDefense.Complete
   ?  ?? Strength: 1.0
   ?
   ?? ModifyAffinity
      ?? Delta: +50
```

**��� ������������ �������:**

1. **������ ������������** (CreateMemory):
   - ������������������� �����
   - ������� ��������
   - ��������� ����� ��� �������
   - ������� � �������

2. **������� ������������** (AddMemory):
   - ���� ������ ��� �������
   - ������� �������� � ��������

---

### **������ 3: ������ NPC - ������ ������������**

#### **Anna (������ NPC):**

```
DialogueNode: "You helped me!"
?? Effect: CreateMemory
   ?? Description: "This kind stranger helped me find my lost daughter. I don't know what I would have done without them. They have a good heart."
   ?? Importance: 85.0
   ?? Tags: [Event.Help, Quest.FindDaughter, Emotion.Grateful]
   ?? Link To Player: ? Yes
```

#### **Guard (����������� NPC):**

```
DialogueNode: "Job done"
?? Effect: CreateMemory
   ?? Description: "Mercenary completed the patrol mission. Acceptable performance. Nothing special."
?? Importance: 40.0
   ?? Tags: [Event.Help, Job.Patrol]
   ?? Link To Player: ? Yes
```

#### **Bandit Leader (���������� NPC):**

```
DialogueNode: "You killed my men!"
?? Effect: CreateMemory
   ?? Description: "This dog slaughtered my best fighters. They will pay for this. I will hunt them down and make them suffer."
   ?? Importance: 90.0
   ?? Tags: [Event.Combat, Enemy.Player, Emotion.Vengeful]
   ?? Link To Player: ? Yes
```

**���� ������� - ��� ���������� ������ ������������!**

---

### **������ 4: �������� (Promise System)**

```
DialogueNode: "I promise I'll find your sister"
?? Effects:
   ?? CreateMemory (Full)
?  ?? Description: "The traveler promised to find my sister Sarah who went missing in the forest. They looked me in the eyes and gave their word. I choose to believe them."
   ?  ?? Importance: 80.0
   ?  ?? Tags:
   ?  ?  ?? Promise.FindSister
 ?  ?  ?? Character.Sarah
   ?  ?  ?? Location.Forest
   ?  ?  ?? Emotion.Hopeful
   ?  ?? Link To Player: ? Yes
   ?
   ?? StartQuest
      ?? QuestID: FindSarahQuest
```

**�����, ���� ����� ��������:**

```
DialogueNode: "You lied to me..."
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "They promised to find Sarah, but they never did. Either they forgot, or they lied from the start. My sister is still missing because of their broken promise."
   ?  ?? Importance: 85.0
   ?  ?? Tags:
   ?  ??? Promise.Broken
   ?  ?  ?? Character.Sarah
   ?  ?  ?? Emotion.Betrayed
   ?  ?? Link To Player: ? Yes
   ?
   ?? ModifyTrust
      ?? Delta: -70
```

---

## ?? **��������� ��������:**

| ������ | AddMemory (Simple) | CreateMemory (Full) |
|--------|-------------------|---------------------|
| **��������** | ? ��� (������ ���) | ? ��������� ����� |
| **��������** | ?? ����� strength | ? ������ �������� (0-100) |
| **����** | ?? 1 ��� | ? ��������� ����� |
| **����� � �������** | ? ��� | ? Player/������ NPC |
| **��������������** | ? ��������� | ? ��������� ��� NPC |
| **UI/Debug** | ? ������ ID | ? �������� ����� |
| **�������� setup** | ??? ������ | ?? ������ |
| **����� ���** | �����, ������� | �����, ������ |

---

## ?? **Guidelines: ����� ��� ������������**

### **��������� `AddMemory` (Simple):**

```
? ����� ������:     "Player met Anna"
? ��������� �����:  "Quest started/completed"
? �������:    "Player visited village"
? ������� �������:  "if HasMemoryWithTag(Character.Anna.Met)"
? ����������������: ������� �������� ����
```

### **��������� `CreateMemory` (Full):**

```
? ������ ������:      �������������, ������, ��������
? ������������� �������: ���������, ���������, ����������
? ��������: ������/���������� ��������
? ������ � �������������: �����/������ ������ �� ���������
? �������������� NPC: ������ NPC ����� ������� ��-�������
? ������� ���������:  ��������� ���� �������� ������
```

---

## ?? **��������� CreateMemory (��������):**

### **Description (FText):**

```
? �����:  "Player helped"
? ������: "This brave stranger defended our village when bandits attacked at dawn. They fought alongside the guards and saved countless lives. We will remember this day forever."
```

**������:**
- ���� �� ���� NPC (������ ����)
- �������� ������ (���, �����, ���)
- ������� ������ NPC
- ����� ���������� ��� ������� NPC

---

### **Importance (0-100):**

```
0-30:   ������
 - Casual conversation
        - Small favors
     - Routine interactions

31-60:  �������� �������
   - Quest help
        - Gifts
        - Minor conflicts

61-90:  ������ �������
        - Betrayal/rescue
        - Major quest outcomes
        - Character development

91-100: �����������������
      - Traumatic events
        - Life-saving actions
        - Romance milestones
```

**������� Importance:**
- Decay rate (��� ���� - ��� ��������� ����������)
- ��������� � AI (����� ������ ������ �������)
- ����������� ���������� � ��������

---

### **Context Tags:**

```
��������������� ���������:

Event.*     - ��� ���������
?? Event.Help
?? Event.Betrayal
?? Event.Combat
?? Event.Gift
?? Event.Rescue

Quest.*     - ����� � ��������
?? Quest.[QuestID].Start
?? Quest.[QuestID].Complete
?? Quest.[QuestID].Failed

Promise.*   - ��������
?? Promise.KeepSecret
?? Promise.FindSister
?? Promise.Revenge
?? Promise.Broken

Emotion.*   - ������������� ��������
?? Emotion.Joyful
?? Emotion.Traumatic
?? Emotion.Grateful
?? Emotion.Vengeful
?? Emotion.Hopeful

Location.*  - ��� ���������
?? Location.Village
?? Location.Forest
?? Location.Cave

Character.* - ��� ����������
?? Character.[Name].Met
?? Character.[Name].Friend
?? Character.[Name].Enemy
```

**��������� 3-5 ����� �� ������������ ��� ������ ����������!**

---

### **Link To Player:**

```
? TRUE:  �������� ������, ��������, ���������
? FALSE: ����������, ����� NPC, ���������
```

**�������:**
- ? Link: "Player saved me"
- ? Link: "Player betrayed my trust"
- ? No Link: "I saw a beautiful sunset"
- ? No Link: "The village was attacked"

---

## ?? **������������� � Dialogue Conditions:**

### **� Simple Memory:**

```cpp
Condition: HasMemoryWithTag("Event.Betrayal")

// � �������:
if (HasMemoryWithTag(Event.Betrayal))
{
    ShowDialogue("You... how dare you come back here!");
}
```

### **� Full Memory:**

```cpp
// �������� ��������
Condition: HasMemoryWithImportance("Event.Betrayal", 80.0f)

// �������� ��������� �����
Condition: HasMemoryWithAllTags([Event.Help, Location.Village])

// �������� ����� � �������
Condition: HasMemoryAboutPlayer("Event.Rescue")
```

---

## ?? **Workflow: �������� ������������**

### **1. �������� ���:**

```
��� ���� �������? ? AddMemory
��� ������ �������� ������? ? CreateMemory
```

### **2. ������ �������� (��� CreateMemory):**

```
������� ��� ������:
- ��� NPC ����� ��� �������?
- ����� ������ ���������� NPC?
- ����� ������ ���������� NPC?
- ��� ��� ������� NPC?
```

### **3. �������� Importance:**

```
������ ����:
- ������� �� NPC ��� ����� ������? ? 20-40
- �������� �� ����� �����? ? 50-70
- ������� �� �������? ? 80-100
```

### **4. ������ ����:**

```
Minimum 3 ����:
1. Event.* (��� ���������)
2. Location.* ��� Character.* (��������)
3. Emotion.* (��� NPC ��� ���������)
```

### **5. ��������:**

```
1. ������ ������������ � �������
2. ������� � debug UI (���� ����)
3. ������� ������� ��������
4. �������� decay �� ��������
```

---

## ?? **��������� ����:**

### **1. ������ Gameplay Tags:**

```
Project Settings ? GameplayTags

Event.*
?? Event.Help
?? Event.Betrayal
?? Event.Gift
?? Event.Rescue
?? Event.Combat

Quest.*
?? Quest.VillageDefense.Complete
?? ...

Promise.*
?? Promise.FindSister
?? ...

Emotion.*
?? Emotion.Joyful
?? Emotion.Traumatic
?? ...
```

### **2. ������ Dialogue Data Asset:**

```
Content Browser ? Right Click
? Dialogue ? Dialogue Data Asset

Add DialogueNode
? Effects ? Add Element
? Choose: CreateMemory ��� AddMemory
```

### **3. ������� ���������:**

```
CreateMemory:
?? Description: "������������������� �����"
?? Importance: 70.0
?? Context Tags: [Event.Help, Location.Village]
?? Link To Player: ?
```

---

## ?? **����:**

### **������ � ���� ����:**

```
? 5 ������� ��� ������ � ��������������
? ������� ������ (AddMemory) ��� ������
? ������ ������ (CreateMemory) ��� ������
? ������������������� �������� ��� ������� NPC
? ������� �������� � decay
? ������������� ���� ��� ����������
? ����� ������������ � ��������
```

### **���������:**

- **AddMemory** - ��� 70% ������� (�����, �������)
- **CreateMemory** - ��� 30% ������� (������ �������)

**���������� ��� ������� ��� ������������ ��������!**

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Complete & Compiled

**�����:** ������ ������� ������������ � �������, ��������� � ������ ����������� � ������ � �������������! ????
