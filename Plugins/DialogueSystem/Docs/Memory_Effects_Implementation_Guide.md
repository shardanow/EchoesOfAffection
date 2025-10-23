# ?? Memory Effects Integration: Implementation Guide

> **Solution:** C++ Wrapper Classes ��� ���������� Memory Effects � Dialogue System  
> **Status:** ? **COMPILED SUCCESSFULLY!**  
> **Version:** v1.4  
> **Date:** 2025-01-23

---

## ? **��� �������:**

������� **4 ����� C++ ������** ��� ���������� ������������ � ���������:

```
Plugins/DialogueSystem/Source/DialogueSystemCore/
?? Public/Effects/
?  ?? DialogueMemoryEffects.h      ? NEW ? Compiled
?? Private/Effects/
   ?? DialogueMemoryEffects.cpp    ? NEW ? Compiled
```

**������:**
- `UDialogueEffect_AddMemory` - ������� ������������ ?
- `UDialogueEffect_ModifyMemoryStrength` - �������� ���� ?
- `UDialogueEffect_RemoveMemory` - ������� ������������ ?
- `UDialogueEffect_RefreshMemory` - "��������" ������������ ?

**������ ����������:**
```
Result: Succeeded
Total execution time: 2.22 seconds
? ��� 4 ������ �������������� �������!
```

---

## ?? **������������� � Data Asset:**

### **������ 1: ������� ������������ ��� �������������**

```
Content Browser ? Dialogue Data Asset ? Open

DialogueNode: "I betrayed you"
?? Effects:
   ?? Add Element
      ?? Class: UDialogueEffect_AddMemory
  ?? Memory Tag: Event.Betrayal
   ?? Memory Strength: 0.95
```

**���������:** ��� ������ ���� ������� NPC ������� ������������ � ����� `Event.Betrayal` � ����� 95%.

---

### **������ 2: ����� � ����������� ��������������**

```
DialogueNode: "Thank you for helping!"
?? Effects:
   ?? Effect 1: UDialogueEffect_AddMemory
   ?  ?? Memory Tag: Event.Help
   ?  ?? Strength: 0.85
   ?
   ?? Effect 2: UDialogueEffect_AddMemory
   ?  ?? Memory Tag: Quest.VillageDefense.Complete
   ?  ?? Strength: 0.90
   ?
   ?? Effect 3: UDialogueEffect_ModifyAffinity
      ?? NPC Name: Anna
      ?? Delta: +30
```

**���������:** ��������� 2 ������������ + ���������� ��������!

---

### **������ 3: �������� ������ ������������**

```
DialogueNode: "Remember when we first met?"
?? Effects:
   ?? UDialogueEffect_RefreshMemory
   ?? Memory Tag: Event.FirstMeeting
```

**���������:** ������������ "�����������" (timestamp + freshness �������������).

---

### **������ 4: ������� ������������ (������)**

```
DialogueNode: "Let's forget about that..."
?? Effects:
   ?? UDialogueEffect_RemoveMemory
      ?? Memory Tag: Event.OldPromise
```

**���������:** ������������ ��������� ��������� �� ������ NPC.

---

## ?? **��������� Memory Tags (�������):**

### **���������:**

```
Event.Help          // ����� �����
Event.Betrayal           // �������������
Event.Gift     // �������
Event.Rescue      // ��������
Event.FirstMeeting       // ������ �������

Quest.[QuestName]        // ������� � �������
Quest.VillageDefense.Start
Quest.VillageDefense.Complete

Promise.[Topic]        // ��������
Promise.FindSister
Promise.KeepSecret

Emotion.Happy  // �������������
Emotion.Sad
Emotion.Angry
```

**������ ���� ���� � Project Settings ? GameplayTags!**

---

## ?? **������������ �������:**

### **? ����������������:**
```
100 �������� ? 3 ������������ = 300 ������������
������������: 4 C++ ������ (�� 300 Blueprints!)
```

### **? ��������:**
```
�������� ������������:
1. Open Dialogue Data Asset
2. Add Effect ? UDialogueEffect_AddMemory
3. Set Tag + Strength
4. Done!

�����: 10 ������ per ������������
```

### **? Type-Safe:**
```cpp
// �� �������������� ���� ������:
Effect->MemoryTag = "WrongTag";      // ? Error
Effect->MemoryTag = Event.Betrayal;  // ? OK
```

### **? ���������������� ������:**
```
��� ������ ������ � �������������� � NPCMemoryComponent
Effects - ������ ������ �������
��������� � ������ = ��������� � ����� �����
```

---

## ?? **��������� ����:**

### **��� 1: ������������� Unreal Editor** ? �����

```bash
1. ������ Unreal Editor ���� ������
2. ������ ������
3. ���� �������� "Rebuild modules?" ? Yes
4. ������� Output Log �� ������
```

**������ ��� �������������� � ������ � �������������!**

---

### **��� 2: ������� Gameplay Tags**

```
1. Project Settings ? GameplayTags
2. Add New Gameplay Tag:
   - Event.Betrayal
   - Event.Help
   - Event.Rescue
   - Event.FirstMeeting
   (� ����� ������ ������ ����)
3. Save
```

---

### **��� 3: �������� NPCMemoryComponent � NPC**

```
1. ������ Blueprint NPC
2. Add Component ? NPCMemoryComponent
3. Compile
4. Save
```

---

### **��� 4: ������������ � Dialogue Data Asset**

```
1. Open Dialogue Data Asset
2. Select DialogueNode
3. Effects ? Add Element
4. Class: UDialogueEffect_AddMemory
5. Set Memory Tag (��������, Event.Betrayal)
6. Set Memory Strength (0.0 - 1.0)
7. Save
```

**������!** ������ ��� ������ ���� ������� ��������� ������������! ??

---

## ?? **������������ ��������� (v1.4.1):**

### **�������� DSL Parser Support**

�������� ������ ������������ ��� ������:

```
DialogueNode.EffectExpression = "AddMemory(Event.Betrayal, 0.95); ModifyAffinity(Anna, -40)";
```

**�����:** 2-3 ���� ����������  
**����:** `DialogueEffectParser.cpp` - �������� `ParseAddMemory()`, etc.

---

## ?? **�������� ���������:**

### **�� (v1.3.1):**
```
? ������ ��������� ������������ ����� Data Asset
? ����� ������ Blueprint/C++ ��� ��� ������� ������������
? 100 �������� = 100 Blueprint'��
```

### **����� (v1.4):**
```
? ����� ��������� ������������ ����� � Data Asset
? 4 ������������� C++ ������ ��� ���� �������
? 100 �������� = 0 �������������� Blueprint'��
? Type-safe, �������������, ������
? ������� ��������������!
```

---

## ?? **������������:**

- **��� ������������:** ��. ������� ����
- **Memory System:** `Plugins/DialogueSystem/Docs/Memory_System_Creation_And_Time.md`
- **����������:** `Plugins/DialogueSystem/Docs/Memory_Effects_Integration_Status.md`
- **Architecture:** `Plugins/DialogueSystem/Docs/Complete_Architecture_Overview.md`

---

**�������� �����������:** 2025-01-23  
**������:** 2.0  
**������:** ? **Compiled and Ready to Use!**

**�����:** Wrapper classes ������� � ������� ��������������! ������ ����� ��������� ������������ ����� Data Asset ��� �������� ����� Blueprint'��! ������ ����������� Unreal Editor � ������� ������������! ??
