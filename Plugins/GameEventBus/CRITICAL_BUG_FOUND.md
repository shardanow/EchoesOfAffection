# ?? ������� ����������� ��������!

## ? ��������: QuestSubsystem::EmitQuestEvent() �� ����������!

### ��� �������:

```cpp
// QuestEventBridge.cpp ������� �������:
QuestSubsystem->EmitQuestEvent(QuestPayload);  // ? ����������

// ��! � QuestSubsystem.cpp ����� ������ ���!
```

### ������ ������ �� �����������:

```
Player ��������� ������
  ?
InventoryComponent::AddItem("Apple", 1)
  ?
GameEventBus::EmitEvent(Inventory.Event.ItemAcquired) ? ��������
  ?
QuestEventBridge::OnGameEvent(Payload) ? ��������
  ?
QuestSubsystem->EmitQuestEvent(QuestPayload) ? ����� �� ����������!
  ?
? ������ �� ���������� ?
```

---

## ?? ��� ����� �������:

### 1. �������� ����� � QuestSubsystem.cpp

```cpp
void UQuestSubsystem::EmitQuestEvent(const FQuestEventPayload& Payload)
{
    if (!EventBus)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestSubsystem::EmitQuestEvent: EventBus is null!"));
return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::EmitQuestEvent: %s"), *Payload.EventTag.ToString());
    
    // �������� � QuestEventBus
    EventBus->BroadcastEvent(Payload);
}
```

### 2. ��������� ��� QuestEventBus::BroadcastEvent() ������������ �������

����� �������� ������ ���������� ������� ��� ��������� �������!

---

## ?? ������ �����������

### ��� ��������:
- ? GameEventBusSubsystem ��������� �������
- ? InventoryComponentGameEventHelper ��������� �������
- ? QuestEventBridge ������������� � �������� �������
- ? QuestEventBridge ����������� � QuestSubsystem

### ��� �������:
- ? QuestSubsystem::EmitQuestEvent() �� ����������
- ? QuestEventBus::BroadcastEvent() �� ��������� ������
- ? ��� ����� ����� ��������� � UpdateObjectiveProgress()

---

## ?? ���� �����������

### ���� 1: QuestSubsystem.cpp

�������� ���������� `EmitQuestEvent`:

```cpp
void UQuestSubsystem::EmitQuestEvent(const FQuestEventPayload& Payload)
{
    if (!EventBus)
    {
        UE_LOG(LogTemp, Error, TEXT("EmitQuestEvent: EventBus is null!"));
        return;
    }
    
    // ����������� ��� �������
  UE_LOG(LogTemp, Log, TEXT("EmitQuestEvent: Tag=%s, String=%s, Int=%d"), 
        *Payload.EventTag.ToString(), 
        *Payload.StringParam.ToString(), 
        Payload.IntParam);
    
    // �������� ������� � EventBus
    EventBus->BroadcastEvent(Payload);
}
```

### ���� 2: QuestEventBus.cpp

�������� ������ ���������� �������:

```cpp
void UQuestEventBus::BroadcastEvent(const FQuestEventPayload& Payload)
{
    UE_LOG(LogTemp, Log, TEXT("QuestEventBus::BroadcastEvent: %s"), *Payload.EventTag.ToString());
    
    // �������� ProgressManager
    UQuestProgressManager* ProgressMgr = GetProgressManager();
    if (!ProgressMgr)
    {
      UE_LOG(LogTemp, Error, TEXT("BroadcastEvent: ProgressManager is null!"));
   return;
    }
    
    // �������� ��� �������� ������
    TArray<FQuestSaveData> ActiveQuests = ProgressMgr->GetActiveQuests();
    
    UE_LOG(LogTemp, Log, TEXT("BroadcastEvent: Checking %d active quests"), ActiveQuests.Num());
  
    // ��� ������� ��������� ������
    for (const FQuestSaveData& QuestData : ActiveQuests)
    {
    // ��������� objectives
    // ���� objective ������� ��� ������� ? �������� ��������
        HandleQuestEvent(QuestData.QuestId, Payload);
    }
}

void UQuestEventBus::HandleQuestEvent(FName QuestId, const FQuestEventPayload& Payload)
{
    // ��������� �������: ������� ������ ��� ItemAcquired
    
    if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        FName ItemID = Payload.StringParam;
 int32 Count = Payload.IntParam;
     
        // ���������� �������� ��������
// ObjectiveID ����� ���� ����� - ��������� � ItemID
 FName ObjectiveID = FName(*FString::Printf(TEXT("Collect_%s"), *ItemID.ToString()));
        
        UE_LOG(LogTemp, Log, TEXT("Trying to update: Quest=%s, Objective=%s, Amount=%d"),
            *QuestId.ToString(), *ObjectiveID.ToString(), Count);
        
        GetProgressManager()->UpdateObjectiveProgress(QuestId, ObjectiveID, Count);
    }
}
```

---

## ?? ��������� ������� (������� ����)

���� ����� ������ ��������������, ������ ������ ����� � QuestEventBridge:

```cpp
void UQuestEventBridge::ForwardToQuestSystem(const FGameEventPayload& GamePayload)
{
    UQuestSubsystem* QuestSubsystem = Cast<UQuestSubsystem>(QuestSubsystemRef.Get());
    if (!QuestSubsystem)
    {
        return;
    }
  
    // ��������� ����: ������ ���������� ��� ItemAcquired
    if (GamePayload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
      FName ItemID = GamePayload.StringParam;
        int32 Count = GamePayload.IntParam;
        
        UE_LOG(LogQuestBridge, Log, TEXT("TEMP FIX: ItemAcquired - %s x%d"), *ItemID.ToString(), Count);
        
        // �������� ��� �������� ������
        TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
        
  for (const FQuestSaveData& QuestData : ActiveQuests)
   {
            // ������� � ������� ���������� ObjectiveID
        TArray<FName> PossibleObjectiveIDs = {
   FName(*FString::Printf(TEXT("Collect_%s"), *ItemID.ToString())),
  FName(*FString::Printf(TEXT("ItemAcquired_%s"), *ItemID.ToString())),
   FName(*FString::Printf(TEXT("%s_Objective"), *QuestData.QuestId.ToString()))
            };

       for (FName ObjectiveID : PossibleObjectiveIDs)
        {
      if (QuestSubsystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
                {
            UE_LOG(LogQuestBridge, Log, TEXT("SUCCESS: Updated %s -> %s (+%d)"),
  *QuestData.QuestId.ToString(), *ObjectiveID.ToString(), Count);
         break; // ���� ������� - �������
      }
            }
     }
    }
    
    // ������������ ��� (���� ����������� �����):
    // FQuestEventPayload QuestPayload;
    // QuestPayload.EventTag = GamePayload.EventTag;
    // QuestPayload.StringParam = GamePayload.StringParam;
    // QuestPayload.IntParam = GamePayload.IntParam;
    // QuestPayload.FloatParam = GamePayload.FloatParam;
    // QuestPayload.TagsParam = GamePayload.AdditionalTags;
  // QuestPayload.InstigatorActor = GamePayload.InstigatorActor;
    // QuestPayload.TargetActor = GamePayload.TargetActor;
    // QuestSubsystem->EmitQuestEvent(QuestPayload);
}
```

---

## ? ����� ����������� �������:

### 1. ������� ����

### 2. ������ ����:
```
GameEventBus.EnableHistory 1
GameEventBus.SetVerbose 1
```

### 3. ������ ������

### 4. ������� ���:
```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple
LogQuestBridge: TEMP FIX: ItemAcquired - Apple x1
LogQuestBridge: SUCCESS: Updated CollectApples -> CollectApples_Objective (+1)
LogQuest: Objective updated: CollectApples (1/5)
```

---

## ?? ����

**��������:**  
`QuestSubsystem::EmitQuestEvent()` �� ����������!

**�������:**  
1. �������� ���������� ������
2. �������� ������ ���������� � QuestEventBus
3. ��� ������������ ��������� ���� � QuestEventBridge

**����� �����������:**
������ ����� ����������� ��� ����� ���������! ?
