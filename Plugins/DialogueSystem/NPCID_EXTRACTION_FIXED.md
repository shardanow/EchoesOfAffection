# ? DIALOGUE EVENTS: NPCID EXTRACTION FIXED!

## ?? ��������:

**������� �������������**, �� **�� �������� ������ NpcId**!

### ���� ����������:

```
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
[LogTemp] ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
```

**StringParam = 'lianne_greeting'** (DialogueId) ? **NpcId = 'Lianne'**

? **������ �� ������ ? ����� �� ������� ��������!**

---

## ? �������:

### ��������� NpcId �� NPC actor:

**��������� ����������:**
1. **GameplayTags** � ���� ��� ���� `NPC.Lianne` ? ��������� `"Lianne"`
2. **Fallback** � �������� ��� ������: `BP_Lianne_C_1` ? `"Lianne"`

### ��� � StartDialogue():

```cpp
// Extract NPC ID: Try to get from tags first (e.g., "NPC.Lianne"), fallback to simplified actor name
FName NpcId = NAME_None;
if (NPC)
{
    // Try IGameplayTagAssetInterface first
    if (NPC->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
    {
        IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(NPC);
  if (TagInterface)
        {
            FGameplayTagContainer Tags;
      TagInterface->GetOwnedGameplayTags(Tags);

    // Look for NPC.* tag
            FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
            if (NPCParent.IsValid())
{
        for (const FGameplayTag& Tag : Tags)
       {
       if (Tag.MatchesTag(NPCParent))
         {
       // Extract last part of tag (e.g., "NPC.Lianne" -> "Lianne")
        FString TagString = Tag.ToString();
      int32 LastDotIndex;
  if (TagString.FindLastChar('.', LastDotIndex))
             {
                  NpcId = FName(*TagString.RightChop(LastDotIndex + 1));
   break;
 }
                 }
    }
 }
        }
    }
    
    // Fallback: use simplified actor name if no tag found
    if (NpcId == NAME_None)
    {
 FString ActorName = NPC->GetName();
        // Remove Blueprint suffix (e.g., "BP_Lianne_C_1" -> "Lianne")
  ActorName.RemoveFromStart(TEXT("BP_"));
        int32 UnderscoreIndex;
        if (ActorName.FindChar('_', UnderscoreIndex))
        {
            ActorName = ActorName.Left(UnderscoreIndex);
    }
        NpcId = FName(*ActorName);
    }
}

UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
  NpcId, // NpcId extracted from tags or actor name
    NAME_None,
    Player,
    NPC
);
```

---

## ?? ��������� �����:

### 1. DialogueSubsystem.cpp

- ? ��������� ������ ���������� NpcId �� GameplayTags
- ? Fallback �� ���������� ��� ������
- ? ������� NpcId ������ DialogueId � ������ ���������

### 2. DialogueSubsystem.cpp (includes)

- ? �������� `#include "GameplayTagAssetInterface.h"`

---

## ?? ������ �������� � ����� ����������:

### ������� 1: NPC � GameplayTags ?

**NPC ����� ���:** `NPC.Lianne`

**���������:**
```
NpcId = "Lianne" ? �������� �� ����!
```

### ������� 2: NPC ��� ����� (fallback) ?

**NPC ��� ������:** `BP_Lianne_C_1`

**���������:**
```
ActorName.RemoveFromStart("BP_") ? "Lianne_C_1"
ActorName.Left(UnderscoreIndex) ? "Lianne" ? ��������!
```

---

## ?? ��������� ���� ����� �����������:

### ��� ������ �������:

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne' | IntParam=0
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne' (Progress: 0/1, Conditions: 1)
[LogQuestEventBus]   ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

---

## ?? ����� ��� �������� �������:

### � Data Asset ������:

```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
NpcId: Lianne  ? ������ ��������� � ����������� ID!
    Count: 1
```

### � NPC:

**������� � (�������������):** �������� GameplayTag
```
Gameplay Tags: NPC.Lianne  ? ID = "Lianne"
```

**������� � (fallback):** ������� ����� ���������
```
Actor Name: BP_Lianne_C_1  ? ID = "Lianne" (��������)
```

---

## ?? ��� ������ ������:

1. **������� Unreal Editor**
2. **��������� ����** � ���������� � Lianne
3. **��������� ����** � ������ ���� ������� � ���������� NpcId!
4. **����� ������ �������� ��������!** ?

---

**NPCID EXTRACTION FIXED!** ?????
