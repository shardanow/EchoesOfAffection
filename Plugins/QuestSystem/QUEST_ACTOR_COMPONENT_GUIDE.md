# ?? Quest Actor Component - Complete Guide

## ?? ��� ���?

**QuestActorComponent** � ��� ������������� ��������� ��� ���������� �������, ������� ��������� � ��������� �������.

### ?? ������� ����

������ ������������ ������ ���������� ID �� �����/���, ������ **���� ���������** ��������� ���� ��������� �������������� ������.

---

## ? �������� ������������

| ���� | ����� |
|------|-------|
| ID ����� � GameplayTags ��� ����� | ����� ���� `QuestActorId` |
| ������ ���������� ���������� �� ���� | �������������� � ���������� |
| ������ ��� NPC | ��� **�����** ������� (NPC, ��������, �������) |
| ��� ���������� | `TMap<FName, FString>` ��� ��������� ������ |
| ���������, ������ ID | ����� �������� � ������ |

---

## ??? �����������

```
QuestActorComponent
?? IDENTITY
?  ?? QuestActorId (FName) - ����� ID
?  ?? bAutoAssignId (bool) - �������������� �� �����/�����
?  ?? QuestActorTags (FGameplayTagContainer) - �������������
?
?? DISPLAY
?  ?? DisplayName (FText) - ��� UI
?  ?? Description (FText) - ��������
?  ?? Icon (UTexture2D*) - ������
?
?? ROLES
?  ?? bCanGiveQuests - ����� �������� ������?
?  ?? bCanBeQuestTarget - ����� ���� �����?
?  ?? bCanEmitQuestEvents - ����� ������� �������?
?  ?? bCanBeTracked - ����� �����������?
?
?? METADATA
   ?? TMap<FName, FString> - ��������� ������
```

---

## ?? ������� �����

### ��� 1: �������� ���������

**� Blueprint:**
```
BP_Lianne:
  ?? Add Component ? Quest Actor Component
```

**� C++:**
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
UQuestActorComponent* QuestActor;

AMyNPC::AMyNPC()
{
    QuestActor = CreateDefaultSubobject<UQuestActorComponent>(TEXT("QuestActor"));
}
```

### ��� 2: ��������� ID

**������� �: ������������� (�������������)**
```
Quest Actor Component:
  ?? bAutoAssignId: ? TRUE
  ?? (QuestActorId ������ - ���������� �������������)

NPC Actor:
  ?? Gameplay Tags: NPC.Lianne  ? ���������� "Lianne"
```

**������� �: �������**
```
Quest Actor Component:
  ?? QuestActorId: "Lianne"  ? ���� ������
  ?? bAutoAssignId: ? FALSE
```

### ��� 3: ��������� ����

```
Quest Actor Component:
  ?? bCanGiveQuests: ? TRUE  ? ���� NPC ����� ������
  ?? bCanBeQuestTarget: ? TRUE  ? ���� ����� �����������������
  ?? bCanEmitQuestEvents: ? TRUE  ? ���� ������ �������
  ?? bCanBeTracked: ? TRUE  ? ���� ���������� � �������
```

### ��� 4: (�����������) �������� ����������

```
Quest Actor Component:
  ?? Metadata:
       ?? "Faction" ? "TownGuard"
       ?? "QuestType" ? "MainQuest"
       ?? "RequiredLevel" ? "5"
```

---

## ?? �������������

### C++ API

#### �������� Quest Actor ID

```cpp
// �� ����������
UQuestActorComponent* QuestActor = NPC->FindComponentByClass<UQuestActorComponent>();
if (QuestActor)
{
  FName NpcId = QuestActor->GetQuestActorId();
    UE_LOG(LogTemp, Log, TEXT("NPC ID: %s"), *NpcId.ToString());
}
```

#### ��������� ����

```cpp
if (QuestActor->HasQuestTag(FGameplayTag::RequestGameplayTag("Quest.NPC.Friendly")))
{
    // ����������� NPC
}

if (QuestActor->HasQuestTagMatching(FGameplayTag::RequestGameplayTag("Quest.NPC")))
{
    // ����� ��� ������������ � "Quest.NPC"
}
```

#### ������ � �����������

```cpp
// ��������
bool bFound;
FString Faction = QuestActor->GetMetadata("Faction", bFound);
if (bFound)
{
    UE_LOG(LogTemp, Log, TEXT("Faction: %s"), *Faction);
}

// ����������
QuestActor->SetMetadata("LastInteraction", "2024-01-15");

// ��������� �������
if (QuestActor->HasMetadata("QuestGiverType"))
{
    // ...
}
```

#### ������� ������� �� ������

```cpp
QuestActor->EmitQuestEvent(
    FGameplayTag::RequestGameplayTag("GameEvent.Quest.CustomEvent"),
    1, // IntParam
    PlayerActor // Target
);
```

### Blueprint API

#### �������� ID

```
Get Quest Actor Id (Target = Quest Actor Component) ? Name
```

#### ��������� ����

```
Branch (Condition = Quest Actor Component ? Can Give Quests)
  ? True: Show Quest Icon
  ? False: Hide Quest Icon
```

#### ����������

```
Get Metadata (Quest Actor Component, Key = "Faction")
  ? String (Output)
  ? Found (Bool)
```

---

## ?? ������� �������������

### 1?? NPC Quest Giver

```
BP_Lianne:
  ?? QuestActorComponent:
  ?  ?? QuestActorId: "Lianne"
  ?  ?? bAutoAssignId: true
  ?  ?? QuestActorTags: Quest.NPC, Quest.Friendly, Quest.QuestGiver
  ?  ?? DisplayName: "������"
  ?  ?? Description: "����������� ���������� �������"
  ?  ?? bCanGiveQuests: TRUE
  ?  ?? bCanBeQuestTarget: TRUE
  ?  ?? bCanEmitQuestEvents: TRUE
  ?
  ?? QuestGiverComponent:
  ?? AvailableQuests: [DA_Quest_TalkToLianne]
```

### 2?? Collectable Item

```
BP_ItemApple:
  ?? QuestActorComponent:
     ?? QuestActorId: "RedApple"
       ?? QuestActorTags: Quest.Item, Quest.Collectable, Quest.Food
       ?? DisplayName: "������� ������"
       ?? Icon: T_Icon_Apple
     ?? bCanGiveQuests: FALSE
       ?? bCanBeQuestTarget: TRUE
       ?? Metadata:
            ?? "Rarity" ? "Common"
     ?? "StackSize" ? "99"
```

### 3?? Quest Location

```
BP_LocationMarker_Library:
  ?? QuestActorComponent:
       ?? QuestActorId: "Library"
     ?? QuestActorTags: Quest.Location, Quest.Town, Quest.Indoor
    ?? DisplayName: "����������"
       ?? bCanBeQuestTarget: TRUE
       ?? bCanBeTracked: TRUE
       ?? Metadata:
            ?? "Zone" ? "Town"
```

### 4?? Quest Object (Interactable)

```
BP_MagicCrystal:
  ?? QuestActorComponent:
       ?? QuestActorId: "MagicCrystal"
       ?? QuestActorTags: Quest.QuestObject, Quest.Unique, Quest.Magical
       ?? DisplayName: "���������� ��������"
       ?? bCanBeQuestTarget: TRUE
       ?? bCanEmitQuestEvents: TRUE
 ?? Metadata:
  ?? "QuestRequired" ? "Q_FindCrystal"
          ?? "OneTimeUse" ? "true"
```

---

## ?? ���������� � ���������

### DialogueSubsystem

**������������� ���������� QuestActorComponent:**

```cpp
// DialogueSubsystem.cpp - StartDialogue_Implementation()
FName NpcId = ExtractNpcId(NPC);

FName UDialogueSubsystem::ExtractNpcId(AActor* NPC)
{
    // 1?? PRIORITY: QuestActorComponent
    UQuestActorComponent* QuestActor = NPC->FindComponentByClass<UQuestActorComponent>();
    if (QuestActor)
    {
     return QuestActor->GetQuestActorId();
  }
    
    // 2?? FALLBACK: GameplayTags
    // 3?? FALLBACK: Actor Name
}
```

### QuestEventBus

**��������� QuestActorComponent ��� ���������:**

```cpp
// QuestEventBus.cpp - BroadcastEvent()
if (Payload.InstigatorActor)
{
    UQuestActorComponent* QuestActor = Payload.InstigatorActor->FindComponentByClass<UQuestActorComponent>();
  if (QuestActor && !QuestActor->bCanEmitQuestEvents)
    {
UE_LOG(LogTemp, Warning, TEXT("Actor cannot emit events!"));
        return; // Reject event
    }
}

// DoesConditionMatchEvent() - ��������� NPC/Item ID
FName ActualNpcId = Payload.StringParam;
if (ActualNpcId.IsNone() && Payload.TargetActor)
{
 UQuestActorComponent* QuestActor = Payload.TargetActor->FindComponentByClass<UQuestActorComponent>();
    if (QuestActor)
    {
        ActualNpcId = QuestActor->GetQuestActorId();
    }
}
```

---

## ?? Auto-Assign ID Logic

### ��������� ����������

```
1?? QuestActorId (���� ����� �������)
   ?
2?? GameplayTags (e.g., "NPC.Lianne" ? "Lianne")
   ?
3?? Actor Name (e.g., "BP_Lianne_C_1" ? "Lianne")
   ?
4?? Actor Name as-is (last resort)
```

### ������ �����

```
[QuestActorComponent] Auto-assigned QuestActorId from GameplayTags: 'BP_Lianne_C_1' -> 'Lianne'
[QuestActorComponent] QuestActorComponent initialized on 'BP_Lianne_C_1' with ID 'Lianne' (auto-assigned)
[QuestActorComponent]   Roles: QuestGiver, Target, EventEmitter, Trackable
[QuestActorComponent]   Quest Tags: Quest.NPC.Quest.Friendly
```

---

## ?? ��������� � ������

### �������������� �������� � BeginPlay()

```
? �������� QuestActorId (�����?)
? �������� ����� (���� �� ���� ��������?)
? �������� QuestGiverComponent (���� bCanGiveQuests=true)
? �������� DisplayName (�����?)
```

### �������� ������

| ������ | ��� | ������� |
|--------|-----|---------|
| ID ������ | `QuestActorId is None` | �������� `bAutoAssignId` ��� ������ ������� |
| ��� ����� | `No quest roles enabled` | �������� ���� �� ���� ���� |
| ��� QuestGiver | `bCanGiveQuests=true but no QuestGiverComponent` | �������� ��������� |
| DisplayName ����� | `DisplayName is empty` | ��������� ��� UI |

---

## ?? ������ ��������

### ? DO

1. **������ ���������� ���������** �� ������, ����������� � �������
2. **����������� QuestActorTags** ��� �������������
3. **��������� ������ ������ ����** (�� ��� �����)
4. **���������� DisplayName** ��� UI
5. **����������� ����������** ��� ��������� ������

### ? DON'T

1. **�� ���������� ��� ���� = false** (��������� ����������)
2. **�� ���������� ������** (��������� ��� �� ������)
3. **�� ����������� ����** (��������� ��������� ������)
4. **�� ��������� ��� fallback** (������ ������ ��������)

---

## ?? �������� �� ������ �������

### ������� �: ����������� ��������

```
1. �������� QuestActorComponent �� ����� ������
2. ������ ������ ���������� �������� (fallback)
3. ���������� ��������� ��������� �� ��� ������
4. ������� fallback ������ (�����������)
```

### ������� �: �������������� ��������

```cpp
// Editor Utility Widget ��� Commandlet
void MigrateActorsToQuestActorComponent()
{
    TArray<AActor*> AllActors;
    // ����� ���� NPC � �������
  
    for (AActor* Actor : AllActors)
    {
        if (!Actor->FindComponentByClass<UQuestActorComponent>())
        {
UQuestActorComponent* QuestActor = NewObject<UQuestActorComponent>(Actor);
 QuestActor->bAutoAssignId = true;
     QuestActor->RegisterComponent();
            
            UE_LOG(LogTemp, Log, TEXT("Added QuestActorComponent to %s"), *Actor->GetName());
        }
    }
}
```

---

## ?? ��������� � ��������������

| ������ | ����� | ������ |
|--------|-------|--------|
| **GameplayTags only** | ����� | ID �����, ��� ���������� |
| **Actor Name only** | ��� ��������� | ��������, ������� �� ��� |
| **Hardcoded IDs** | ������ | �� �����������, ��� UI |
| **QuestActorComponent** ? | ����, �����, ���������� | ����� ��������� ��������� |

---

## ?? ����� ������������

### ? ����������� QuestActorComponent ���:

- **NPC** - �����������, ���� ��������
- **Items** - �������� ��� �����/�������������
- **Locations** - ������� �������, ��������
- **Quest Objects** - ������������� ������� �������
- **Enemies** - ���� ����� ����� ������������ �����

### ? �� ����������� ���:

- **Player** - ������ �� ��������� ��� "quest actor"
- **UI Widgets** - ��������� ������ ��� AActor
- **Temporary Actors** - ���� ����� <1 �������

---

## ?? Checklist

����� ����������� QuestActorComponent:

```
? ����� ��������� � ��������� �������?
? ����� ���������� ID ��� ������� �������?
? ����� ����� ������� ������� ��� ���� �����?
? ����� ���������� ��� ��������� ������?
? ������ ����� ��������� � Editor?

���� ���� �� 3 "��" ? ���������� ���������!
```

---

## ?? ��������� ����

1. **������� �������** � �������
2. **�������� ���������** �� ���� ������
3. **��������� ����** � ����������
4. **��������������** � ��������
5. **���������** ���������� � ������� ���������

---

## ?? ��. �����

- **QUICK_START.md** - ������� ����� ��������� �������
- **NPC_ID_COMPLETE_GUIDE.md** - ������ ������� NPC ID (fallback)
- **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** - ��� ������������ ID � ��������
- **README_FINAL.md** - ������ ������������ ��������� �������

---

**������:** 1.0  
**����:** 2024-01-15  
**�����:** Quest System Team  
**������:** ? Production Ready
