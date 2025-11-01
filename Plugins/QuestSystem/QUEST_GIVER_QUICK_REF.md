# ?? QuestGiverComponent - ������� �������

## ��� ���?

**UI ���������** ��� ���������� �������� �� ����������� NPC.

## ��� ���� �����?

| ���������� | �������� |
|-----------|----------|
| ?? **������ ��� �������** | ���������� !, ?, ? ��� NPC � ����������� �� ������� ������� |
| ?? **������ �������** | ���������� ��������� ������ ��� ����� �� NPC |
| ? **���� �������** | UI ��� ������ ������ �� NPC |
| ?? **����� �������** | UI ��� ���������� ������ � NPC |
| ?? **����������** | ��������� ����������� � ������������� ������� |

## ��� ���� �� �����?

| ? �� ��������� | ������ |
|----------------|--------|
| ������� �������� | DialogueSubsystem ��� ��������� NPC ID �� GameplayTags |
| ������� ������� | QuestEventBus ���������� ������� �� GameEventBus |
| �������� ������� | QuestProgressManager ��������� ���������� |
| �������������� ������� | GameEventBridge ��������� ������� � ������ |

## �������� ������

```cpp
// �������� ������ ��� UI
TArray<UQuestAsset*> GetAvailableQuestsForPlayer(APlayerController* Player);
TArray<UQuestAsset*> GetActiveQuestsForPlayer(APlayerController* Player);
TArray<UQuestAsset*> GetCompletableQuestsForPlayer(APlayerController* Player);

// �������� ������ ��� �������
UTexture2D* GetQuestIndicatorIcon(APlayerController* Player);

// ������� �����
bool AcceptQuest(APlayerController* Player, UQuestAsset* QuestAsset);

// ����� �����
bool TurnInQuest(APlayerController* Player, UQuestAsset* QuestAsset);
```

## ��������� � Blueprint

```
BP_Lianne (Actor)
 ?? QuestGiverComponent
      ?? NpcId: "Lianne" (�����������, ����-�����������)
      ?? bAutoAssignNpcId: true
      ?? AvailableQuests: [DA_Quest_TalkToLianne, ...]
      ?? QuestAvailableIcon: Texture_YellowExclamation
    ?? QuestInProgressIcon: Texture_GrayQuestion
      ?? QuestCompleteIcon: Texture_GoldCheck
```

## ?? �����!

**NpcId � QuestGiverComponent ? NpcId � �������� �������!**

- **QuestGiverComponent.NpcId** - ��� UI � �������
- **������� ������ (NpcId)** - ����������� �� GameplayTags ������ NPC

### ���������� ��������� NPC:

```
BP_Lianne (Actor)
 ?? GameplayTags: NPC.Lianne  ? ��� ������� �������!
 ?? QuestGiverComponent
  ?? NpcId: "Lianne"  ? ��� UI (�����������)
```

## �������� ������ �������������

```cpp
// Blueprint: OnPlayerInteract
void BP_Lianne::OnPlayerInteract(APlayerController* Player)
{
    UQuestGiverComponent* QuestGiver = FindComponentByClass<UQuestGiverComponent>();
    
    // 1. �������� ��������� ������
    TArray<UQuestAsset*> Available = QuestGiver->GetAvailableQuestsForPlayer(Player);
    if (Available.Num() > 0)
    {
     ShowQuestListUI(Available);
    }
    
    // 2. �������� ������ ��� �����
 TArray<UQuestAsset*> Completable = QuestGiver->GetCompletableQuestsForPlayer(Player);
    if (Completable.Num() > 0)
    {
        ShowTurnInUI(Completable);
    }
}

// Widget ��� NPC: UpdateQuestIcon
void UpdateIcon()
{
    UTexture2D* Icon = QuestGiver->GetQuestIndicatorIcon(PlayerController);
    QuestIconImage->SetBrushFromTexture(Icon);
    QuestIconImage->SetVisibility(Icon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
```

## ����� �� �����?

- ? ����� ���������� ������������� (����� �������/�������)
- ? ����� ������� ������ �� �������� (��� UI ��������������)
- ? NPC �� ����� ������ ��������

## ��. �����

- `QUEST_GIVER_COMPONENT_EXPLAINED.md` - ������ ����������
- `NPCID_EXTRACTION_FIXED.md` - ��� ����������� NPC ID ��� �������
- `QUEST_CONDITIONS_COMPLETE_REFERENCE.md` - ������� �������

---

**TL;DR:** QuestGiverComponent = UI ������ ��� ������� �� NPC, �� ������� �������! ??
