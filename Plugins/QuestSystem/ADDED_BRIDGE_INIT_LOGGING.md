# ?? ��������� ��������� ����������� ������������� QuestEventBridge

## ?? ��������:

**QuestEventBridge ������ �� �����������������!**

� ����� ���:
- ? `[LogQuestBridge] ?? SubscribeToEventTags`
- ? `[LogQuestBridge] ? Subscribed to hierarchy`

��� ������ ��� `UQuestSubsystem::InitializeGameEventBridge()` ����:
1. �� ����������
2. �� ����� ����� ����� `QuestEventBridge`
3. �� ����� ������� �������
4. �� ����� ������� `Initialize()`

## ? �������:

��������� **��������� �����������** � `QuestSubsystem::InitializeGameEventBridge()`:

```cpp
?? InitializeGameEventBridge: Starting...
? QuestEventBridge class found via FindObject!
?? Creating QuestEventBridge instance...
? GameEventBridge instance created!
?? Calling GameEventBridge::Initialize...
? QuestSubsystem: GameEventBridge initialized successfully!
```

������ ���� ������� �� ����� ������ ���� ���������� ����.

---

## ?? ��� ��������������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������
- � Visual Studio: Build ? Rebuild Solution
- ��� �������� Unreal Editor � �������� `.uproject` ������

### ��� 3: ��������� ���� (PIE)

### ��� 4: ��������� Output Log ��� �������

**������ �������:**

#### �������� �������������:
```
[LogTemp] ?? InitializeGameEventBridge: Starting...
[LogTemp] ? QuestEventBridge class found via FindObject!
[LogTemp] ?? Creating QuestEventBridge instance...
[LogTemp] ? GameEventBridge instance created!
[LogTemp] ?? Calling GameEventBridge::Initialize...
[LogTemp] ? QuestSubsystem: GameEventBridge initialized successfully!
    ??
[LogQuestBridge] QuestEventBridge initialized successfully
[LogQuestBridge] ?? SubscribeToEventTags: Starting subscription...
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Inventory
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Quest
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Dialogue
[LogQuestBridge] ?? SubscribeToEventTags: Complete! Total subscriptions: 6
```

#### ��� ������:
```
[LogTemp] ? QuestEventBridge class NOT FOUND!
```

---

## ?? ��������� ��������:

### 1. ����� �� ������
**�������:** `QuestEventBridge.cpp` �� ������������� � ������  
**�������:** ��������� `QuestSystemRuntime.Build.cs` � ���� ������ ���� �������

### 2. Initialize() �� �������
**�������:** ��������� ������� ������������ (UFUNCTION �� ���������)  
**�������:** ��������� `QuestEventBridge.h` � `Initialize()` ������ ���� `UFUNCTION()`

### 3. Instance is NULL
**�������:** ������ �������� �������  
**�������:** ��������� ����������� `QuestEventBridge`

---

## ?? ��������� ����:

1. ? **������������ ������**
2. ? **��������� ����**
3. ? **�������� Output Log** � ����� ���� `?? InitializeGameEventBridge`

���� ������� **������ �������** ������ QuestEventBridge �� ��������! ??
