# ?? QuestGiverComponent - ������ ����������

## ?? ��� ������ QuestGiverComponent?

`QuestGiverComponent` - ��� **UI/UX ���������** ��� ���������� ��������, ������� **����� ���������� NPC**.

### ?? �����: QuestGiverComponent ? ������� �������!

**QuestGiverComponent �� ��������� �:**
- ? ������� ������� ��������
- ? �������� ������� �������
- ? ������������ ��������� �������
- ? ���������� NPC ID ��� �������

**QuestGiverComponent ��������� �:**
- ? ������ ������ ��� ������� NPC (! ? � �.�.)
- ? ������ ��������� ������� ��� ����� �� NPC
- ? UI ��� ����� ������� �� NPC
- ? UI ��� ����� ������� NPC
- ? ���������� ������� �� ������������

---

## ?? ��������� ������

### 1?? NpcId � QuestGiverComponent

```cpp
// � QuestGiverComponent.h
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver")
FName NpcId;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver")
bool bAutoAssignNpcId = true;
```

**���������� ����� NpcId:**
- ?? **UI �������������** - ��� ����������� ����� NPC � UI �������
- ?? **���������� �������** - ����� ������ ���������� �� ����� NPC
- ?? **�������** - ��� ����� � ���������� ���������

**��� �� ��� NpcId, �������:**
- ? ������������ � �������� ������� (`NpcId: Lianne` � DA_Quest)
- ? ��������� � GameEventBus ��� �������
- ? ������ �� ������������ ������� ������

---

### 2?? ��� ������� ����������� NPC ID ��� �������?

#### � DialogueSubsystem.cpp (StartDialogue):

```cpp
// Extract NPC ID: Try to get from tags first (e.g., "NPC.Lianne"), fallback to simplified actor name
FName NpcId = NAME_None;
if (NPC)
{
    // 1. ���������: GameplayTags (NPC.Lianne ? "Lianne")
    if (NPC->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
    {
        IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(NPC);
if (TagInterface)
        {
  FGameplayTagContainer Tags;
    TagInterface->GetOwnedGameplayTags(Tags);
         
    FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
            if (NPCParent.IsValid())
       {
             for (const FGameplayTag& Tag : Tags)
      {
       if (Tag.MatchesTag(NPCParent))
            {
            // Extract "Lianne" from "NPC.Lianne"
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
    
    // 2. Fallback: ���������� ��� ������ (BP_Lianne_C_1 ? "Lianne")
    if (NpcId == NAME_None)
    {
    FString ActorName = NPC->GetName();
        ActorName.RemoveFromStart(TEXT("BP_"));
        int32 UnderscoreIndex;
        if (ActorName.FindChar('_', UnderscoreIndex))
        {
   ActorName = ActorName.Left(UnderscoreIndex);
  }
        NpcId = FName(*ActorName);
    }
}

// ������ ������� � ����������� NpcId
UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NpcId,  // ? ��� ���� ID �������� � ������� ������!
    NAME_None,
    Player,
    NPC
);
```

---

### 3?? ��� ���� ����� QuestGiverComponent?

#### �������� ����������:

```cpp
// �������� ������ �������, ������� NPC ����� ������
TArray<UQuestAsset*> UQuestGiverComponent::GetAvailableQuestsForPlayer(APlayerController* Player) const
{
    TArray<UQuestAsset*> AvailableForPlayer;
  
    UQuestSubsystem* QuestSystem = GetQuestSubsystem();
 UQuestProgressManager* ProgressMgr = QuestSystem->GetProgressManager();
    
    for (UQuestAsset* Quest : LoadedQuests)
    {
        // �������� ������������
        TArray<FText> FailReasons;
        if (ProgressMgr->CanStartQuest(Quest->QuestId, FailReasons))
        {
            // �������� ���������
            EQuestState State = ProgressMgr->GetQuestState(Quest->QuestId);
            if (State == EQuestState::NotStarted || 
       (State == EQuestState::Completed && Quest->Meta.bIsRepeatable))
        {
      AvailableForPlayer.Add(Quest);
        }
        }
 }
    
 return AvailableForPlayer;
}
```

#### �������� �������� �������������:

```cpp
// Blueprint: NPC interaction
void BP_Lianne::OnPlayerInteract(APlayerController* Player)
{
    // 1. �������� ���������
    UQuestGiverComponent* QuestGiver = FindComponentByClass<UQuestGiverComponent>();
    
    // 2. �������� ��������� ������
    TArray<UQuestAsset*> AvailableQuests = QuestGiver->GetAvailableQuestsForPlayer(Player);
    
    // 3. �������� UI � ��������
if (AvailableQuests.Num() > 0)
    {
        ShowQuestDialogUI(AvailableQuests);
    }
    
    // 4. �������� ������ ��� �����
    TArray<UQuestAsset*> CompletableQuests = QuestGiver->GetCompletableQuestsForPlayer(Player);
    
    if (CompletableQuests.Num() > 0)
    {
    ShowTurnInUI(CompletableQuests);
    }
}
```

---

### 4?? ������ ��� ������� NPC

```cpp
// �������� ���������� ������ ��� ����������� ��� NPC
UTexture2D* UQuestGiverComponent::GetQuestIndicatorIcon(APlayerController* Player) const
{
    // ���� �� ����������� ������ ��� �����?
    TArray<UQuestAsset*> CompletableQuests = GetCompletableQuestsForPlayer(Player);
    if (CompletableQuests.Num() > 0)
{
        return QuestCompleteIcon;  // ������� ! ��� ?
    }

    // ���� �� �������� ������ �� ����� NPC?
    TArray<UQuestAsset*> ActiveQuests = GetActiveQuestsForPlayer(Player);
    if (ActiveQuests.Num() > 0)
    {
        return QuestInProgressIcon;  // ����� ?
    }
    
    // ���� �� ��������� ����� ������?
    TArray<UQuestAsset*> AvailableQuests = GetAvailableQuestsForPlayer(Player);
    if (AvailableQuests.Num() > 0)
    {
    return QuestAvailableIcon;  // Ƹ���� !
    }
    
    return nullptr;  // ��� ������
}
```

Blueprint widget ��� NPC:
```cpp
// Tick function � Widget
void UpdateQuestIndicator()
{
    UQuestGiverComponent* QuestGiver = NPC->FindComponentByClass<UQuestGiverComponent>();
  
    UTexture2D* Icon = QuestGiver->GetQuestIndicatorIcon(PlayerController);
    
    if (Icon)
    {
      QuestIconWidget->SetBrushFromTexture(Icon);
        QuestIconWidget->SetVisibility(ESlateVisibility::Visible);
  }
    else
    {
        QuestIconWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}
```

---

## ?? ������ ������� ������

### ��������: ����� �������� � Lianne

#### 1. **������ ��� �������** (QuestGiverComponent ��������)

```
[BP_Lianne] Tick
  ?? QuestGiverComponent->GetQuestIndicatorIcon(Player)
       ?? ���������: ���� �� ������ ��� �����?
    ?? ���������: ���� �� �������� ������?
       ?? ���������: ���� �� ����� ������?
       
���������: ������������ ����� ! (����� ����� ��������)
```

#### 2. **����� ������� �� Lianne** (QuestGiverComponent ��������)

```
[BP_Lianne] OnInteract()
  ?? QuestGiverComponent->GetAvailableQuestsForPlayer(Player)
       ?? ������� ����� "Q_TalkToLianne"
       ?? ��������� �� ������������ (��� ���������)
    
���������: ������������ UI � ������� "Talk to Lianne"
```

#### 3. **����� ��������� �����** (QuestGiverComponent ��������)

```
[QuestDialogUI] OnAcceptButton()
  ?? QuestGiverComponent->AcceptQuest(Player, QuestAsset)
  ?? QuestSubsystem->StartQuestAsync("Q_TalkToLianne")
       
���������: ����� �������� � ������, ������ ��� Lianne ��������
```

#### 4. **����� �������� ������** (QuestGiverComponent �� ���������!)

```
[DialogueSubsystem] StartDialogue("lianne_greeting", Player, BP_Lianne)
  ?? ��������� NPC ID �� GameplayTags: "Lianne"
  ?? EmitDialogueEvent("GameEvent.Dialogue.Started", NpcId="Lianne")
       ?? [QuestEventBridge] OnGameEvent()
      ?? [QuestEventBus] CheckCondition(EventTag, NpcId)
      ?? ����� "Q_TalkToLianne" �������� ��������!
            
���������: Objective "Talk to Lianne" �������� ?
```

---

## ? ������

### QuestGiverComponent ����� ���:
1. **UI �������** - �������� ������ ������� �� NPC
2. **������ ��� �������** - ��������� �������� ������ �������
3. **���������� �������** - ����� ������ ���������� ������
4. **����/����� �������** - UI �������������� � NPC

### QuestGiverComponent �� ����� ���:
1. ? ������� ������� (������������ GameEventBus)
2. ? ������� �������� (DialogueSubsystem ��������� NPC ID)
3. ? ��������� ������� (QuestEventBus + QuestProgressManager)
4. ? ��������������� �������� �������

---

## ?? ������������

### ����� ������������ QuestGiverComponent?

? **�����������**, ����:
- NPC ����� ������ ����� UI
- ����� ������ ��� ������� NPC
- ��������� ������ ������� ��� ��������������
- NPC ��������� ����������� ������

? **�� �����������**, ����:
- ����� ���������� ������������� (����� �������)
- ����� ������� ������ �� �������� (��� �������������� � NPC)
- NPC �� ����� ������ ��������

### ���������� NpcId � QuestGiverComponent

```cpp
// ������� 1: Auto-assign (�� ���������)
bAutoAssignNpcId = true;  // BP_Lianne_C_1 ? "Lianne"

// ������� 2: ������ ����������
NpcId = "Lianne";  // ���� ������ ID ��� UI

// ?? �����: ���� NpcId ������������ ������ ��� UI!
// ��� ������� ������� ������������ ID �� GameplayTags ��� ����� ������!
```

---

## ?? ��������� ���������

- `NPCID_EXTRACTION_FIXED.md` - ��� ����������� NPC ID ��� �������
- `QUEST_CONDITIONS_COMPLETE_REFERENCE.md` - ��� �������� ������� �������
- `EVENT_TAG_FILTER_FIXED.md` - ��� ����������� ������� �� �����

---

**����:** QuestGiverComponent - ��� **UI-������**, � �� ������� �������! ??
