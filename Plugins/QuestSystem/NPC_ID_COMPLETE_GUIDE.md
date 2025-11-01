# ?? NPC ID - ������ ���� �� ���� ��������������

## ?? �����: ��� � ����� ������������ NPC ID

| ����� | ��� | ���������� | ������ �������� |
|-------|-----|-----------|----------------|
| **GameplayTags** | `FGameplayTag` | ? **�������� ��������** ��� ������� | `NPC.Lianne` |
| **Actor Name** | `FString` | Fallback ��� ������� | `BP_Lianne_C_1` |
| **QuestGiverComponent.NpcId** | `FName` | UI � ������� | `Lianne` |
| **Quest Condition.NpcId** | `FName` | ������ ������� | `Lianne` |
| **DialogueAsset.SpeakerName** | `FText` | UI ������� | `"������"` |

---

## ?? �������� ��������: GameplayTags

### ��� ���������������?

```
BP_Lianne (Actor Blueprint)
 ?? Details Panel
      ?? Tags
  ?? Gameplay Tags: NPC.Lianne
```

### ��� �����������?

```cpp
// DialogueSubsystem.cpp: StartDialogue()
FName NpcId = NAME_None;
if (NPC && NPC->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
{
    IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(NPC);
    if (TagInterface)
    {
        FGameplayTagContainer Tags;
  TagInterface->GetOwnedGameplayTags(Tags);
        
  // ���� ��� ���� "NPC.*"
    FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
      for (const FGameplayTag& Tag : Tags)
        {
            if (Tag.MatchesTag(NPCParent))
     {
     // ��������� "Lianne" �� "NPC.Lianne"
      FString TagString = Tag.ToString();  // "NPC.Lianne"
         int32 LastDotIndex;
     if (TagString.FindLastChar('.', LastDotIndex))
        {
       NpcId = FName(*TagString.RightChop(LastDotIndex + 1));  // "Lianne"
    break;
             }
    }
  }
    }
}
```

### ��� ������������?

```cpp
// ������� ������� �������
UGameEventBusLibrary::EmitDialogueEvent(
 this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NpcId,  // ? "Lianne" (�� ���� NPC.Lianne)
    NAME_None,
    Player,
 NPC
);

// ��������� � GameEventBus:
// EventTag: "GameEvent.Dialogue.Started"
// StringParam: "Lianne"  ? ��� ���� ID ������������ � �������� ������!
```

---

## ?? Fallback: Actor Name

### ����� ������������?

���� � ������ **��� ���� `NPC.*`**, ������������ ���������� ��� ������.

### ��� ��������?

```cpp
// Fallback � DialogueSubsystem.cpp
if (NpcId == NAME_None && NPC)
{
    FString ActorName = NPC->GetName();  // "BP_Lianne_C_1"
  
    // ������� ������� "BP_"
    ActorName.RemoveFromStart(TEXT("BP_"));  // "Lianne_C_1"
    
    // ���� ����� �� ������� '_'
 int32 UnderscoreIndex;
    if (ActorName.FindChar('_', UnderscoreIndex))
    {
        ActorName = ActorName.Left(UnderscoreIndex);  // "Lianne"
    }
    
    NpcId = FName(*ActorName);  // "Lianne"
}
```

### �������:

| Actor Name | ����������� ID |
|-----------|---------------|
| `BP_Lianne_C_1` | `Lianne` |
| `BP_Alice_C_0` | `Alice` |
| `NPC_Merchant_2` | `Merchant` |
| `Lianne` | `Lianne` |

---

## ?? QuestGiverComponent.NpcId

### ����������

**������ ��� UI � �������!** �� ������ �� ������� ��� ������� �������.

### ��� ���������������?

```
BP_Lianne (Actor Blueprint)
 ?? QuestGiverComponent
      ?? NpcId: "Lianne"  ? ������ ���������� (�����������)
      ?? bAutoAssignNpcId: true  ? ����-���������� �� ����� ������
```

### ��� ����-�����������?

```cpp
// QuestGiverComponent.cpp: BeginPlay()
void UQuestGiverComponent::BeginPlay()
{
    Super::BeginPlay();

    // Auto-assign NPC ID from actor name
    if (bAutoAssignNpcId && NpcId.IsNone())
    {
   AActor* Owner = GetOwner();
   if (Owner)
        {
      NpcId = FName(*Owner->GetName());  // "BP_Lianne_C_1"
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Initialized on '%s' with NPC ID '%s'"), 
        *GetOwner()->GetName(), *NpcId.ToString());
}
```

### ��� ������������?

```cpp
// ������ ��� UI ����� � �������!
UE_LOG(LogTemp, Log, TEXT("Quest accepted from NPC: %s"), *NpcId.ToString());

// �� ������������ �:
// ? �������� �������
// ? �������� ��������
// ? ���������� �������
```

---

## ?? Quest Condition.NpcId

### ����������

**������ ��� �������** - � ����� NPC ��������� �������.

### ��� ���������������?

```
DA_Quest_TalkToLianne (QuestAsset)
 ?? Objectives[0]: "Talk to Lianne"
      ?? Conditions[0]
           ?? EventTag: GameEvent.Dialogue.Started
           ?? NpcId: Lianne  ? ������ ��������� � ID �� �������!
```

### ��� �����������?

```cpp
// QuestEventBus.cpp: CheckCondition()
bool UQuestEventBus::CheckCondition(const FQuestCondition& Condition, const FGameEventPayload& Payload)
{
    // �������� EventTag
    if (!Payload.EventTag.MatchesTag(Condition.EventTag))
    {
        return false;  // ��� �� ���������
 }
    
    // �������� NpcId (���� ������)
    if (!Condition.NpcId.IsNone())
    {
    if (Payload.StringParam != Condition.NpcId)
  {
            UE_LOG(LogQuestEventBus, Verbose, TEXT("   ? NPC ID mismatch: Expected '%s', got '%s'"),
  *Condition.NpcId.ToString(),
    *Payload.StringParam.ToString());
return false;  // NPC ID �� ���������
      }
     
        UE_LOG(LogQuestEventBus, Log, TEXT("   ? CONDITION MATCH! StringParam='%s' == NpcId='%s'"),
        *Payload.StringParam.ToString(),
    *Condition.NpcId.ToString());
    }
    
    return true;  // ������� ���������
}
```

### ������ ��������:

```
Event: GameEvent.Dialogue.Started
  ?? StringParam: "Lianne"  ? �� GameplayTags ������

Condition:
  ?? EventTag: GameEvent.Dialogue.Started  ? ���������
  ?? NpcId: Lianne  ? ��������� � StringParam

���������: ������� ���������! Quest �������� +1
```

---

## ?? DialogueAsset.SpeakerName

### ����������

**������ UI �������!** ������������ ��� ���������.

### ��� ���������������?

```
DA_Dialogue_LianneGreeting (DialogueAsset)
 ?? Nodes[0]
      ?? SpeakerName: "������"  ? �������������� ��� ��� UI
```

### ��� ������������?

```cpp
// ������ � UI �������
DialogueWidget->SetSpeakerName(CurrentNode->SpeakerName);  // "������"
```

### ?? �� ������������ �:
- ? ��������
- ? �������� �������
- ? ������������� NPC

---

## ?? ������������� �������

| ������ | GameplayTags | Actor Name | QuestGiver.NpcId | Condition.NpcId | Dialogue.SpeakerName |
|--------|-------------|-----------|-----------------|----------------|---------------------|
| **���������** | ?? �������� | ?? Fallback | ?? UI only | - | - |
| **������������ � ��������** | ? �� | ? Fallback | ? ��� | - | ? ��� |
| **������ �� �������** | ? �� | ? �� | ? ��� | ? �� | ? ��� |
| **�����������** | ������� | ���� | ����/������� | ������� | ������� |
| **�����������** | ? ��� | ? ��� | ? ��� | ? ��� | ? �� |
| **���** | `FGameplayTag` | `FString` | `FName` | `FName` | `FText` |

---

## ? ������������

### 1?? �������� ��������� NPC (�����������!)

```
BP_Lianne (Actor Blueprint)
 ?? Details ? Tags ? Gameplay Tags: NPC.Lianne
```

**��� ���������:**
- ? ���������� ������� ��������
- ? ���������� ������ ������� �������
- ? ������� ������������� NPC

### 2?? Fallback (�����������)

���� �� ������ ������������ GameplayTags, �������� ����� ���������:
```
BP_Lianne_C_1  ?  ���������� "Lianne"
```

### 3?? QuestGiverComponent (���� ����� UI �������)

```
BP_Lianne
 ?? QuestGiverComponent
 ?? bAutoAssignNpcId: true  ? �������� �� ���������
   ?? NpcId: (����-����������)
```

### 4?? ������� ������ (�����!)

```
DA_Quest_TalkToLianne
 ?? Objective: "Talk to Lianne"
      ?? Condition
  ?? EventTag: GameEvent.Dialogue.Started
   ?? NpcId: Lianne  ? ����� ��� � GameplayTag (����� �����)!
```

### 5?? Dialogue Speaker (��� UI)

```
DA_Dialogue_LianneGreeting
 ?? Node
      ?? SpeakerName: "������"  ? �������� ��� ��� ������
```

---

## ?? �������: �������� ���������� ID

### �������� � �����:

```
// 1. DialogueSubsystem ������ �������
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne' | IntParam=0
      ^^^^^^^ ? ����������� ID

// 2. QuestEventBridge �������� �������
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'

// 3. QuestEventBus ��������� �������
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne' (Progress: 0/1, Conditions: 1)
[LogQuestEventBus]?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus]   ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
         ^^^^^^^      ^^^^^^^
        �� �������   �� �������
```

### ? ������: ID �� ���������

```
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
  ^^^^^^^^^^^^^^^^ ? �����������!
[LogQuestEventBus]   ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus]   ? NPC ID mismatch: Expected 'Lianne', got 'lianne_greeting'
```

**�������:** DialogueSubsystem ��������� DialogueId ������ NpcId (��� ����������!)

---

## ?? �������� �����

```
???????????????????????????????????????????????????????
? BP_Lianne (Actor)            ?
? ?
? ???????????????????????   ??????????????????????   ?
? ? GameplayTags        ?   ? QuestGiverComponent?   ?
? ?  ?? NPC.Lianne ??????????  ?? NpcId: auto    ?   ?
? ???????????????????????   ??????????????????????   ?
???????????????????????????????????????????????????????
            ?
   ? ����������� ��� "Lianne"
            ?
?????????????????????????????????????????????????????
? DialogueSubsystem::StartDialogue()    ?
?  ?? UGameEventBusLibrary::EmitDialogueEvent()     ?
?       ?? StringParam = "Lianne" ???????????????   ?
?????????????????????????????????????????????????????
          ?
             ?
?????????????????????????????????????????????????????
? GameEventBus::EmitEvent()   ?
?  EventTag: GameEvent.Dialogue.Started             ?
?  StringParam: "Lianne" ????????????????????????   ?
?????????????????????????????????????????????????????
     ?
         ?
?????????????????????????????????????????????????????
? QuestEventBridge::OnGameEvent()          ?
?  ?? ForwardToQuestSystem() ???????????????????    ?
?????????????????????????????????????????????????????
        ?
               ?
?????????????????????????????????????????????????????
? QuestEventBus::CheckCondition()?
?            ?
?  Payload.StringParam: "Lianne" ??? �� �������    ?
?      ?        ?
?  Condition.NpcId: "Lianne" ???? �� Data Asset    ?
?   ?        ?
?  ? MATCH! ? Quest Progress +1       ?
?????????????????????????????????????????????????????
```

---

## ?? ��. �����

- `NPCID_EXTRACTION_FIXED.md` - ��� �������� ���������� NPC ID
- `QUEST_GIVER_COMPONENT_EXPLAINED.md` - ������ ���������� QuestGiverComponent
- `QUEST_CONDITIONS_COMPLETE_REFERENCE.md` - ��� �������� ������� �������
- `EVENT_TAG_FILTER_FIXED.md` - ��� ����������� �������

---

**TL;DR:** ����������� **GameplayTag `NPC.Lianne`** - ��� �������� � ������� �������� ID! ??
