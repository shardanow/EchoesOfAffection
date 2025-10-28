# ����������� ������� ����������

## ��� ���������, ��� ������� �������� ���������

### 1. �������� ����� ��� ������� (? �����������)

��� ������� ���� �� ������ ������ ��������� ��������� � �����:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components
```

#### ��� ��������:
- ? ���������� ���������� ��������
- ? ��������� ����� ����������� (�� ��������� 08:00, �����������)
- ? ������� ������ � ����������� �����������

---

### 2. �������� ����������� �����������

����� ����� �� `ScheduleComponent` ���������� � ����, �� ������ ������:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

#### ��� ��������:
- ? NPC ��������������� � ������� ����������
- ? ��� ���� ����� ����������� ����������
- ? ����� ����� ���������� ������������������ �������

#### ? ���� ����� �� �����:
1. ���������, ��� � ������ ���� `ScheduleComponent`
2. ���������, ��� `ScheduleData` ���������� � ����������
3. ���������, ��� ����� � ���� (�� ������ � Blueprint)

---

### 3. �������� ���������� �������

#### ������� A: �������������� ���������� (�������������)

�������� `UTimeOfDayScheduleIntegration` � ��� GameState:

```cpp
// � GameState Blueprint ��� C++
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Systems")
UTimeOfDayScheduleIntegration* ScheduleIntegration;
```

**��������� ����:**
```
? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

#### ������� B: ������ ����������

���� ����������� `ScheduleTimeIntegrationComponent`:

```
Schedule Time Integration: Subscribed to time events (implement binding in your project)
```

**?? �����:** ���� ��������� ������� ������ �������� �������!

---

### 4. �������� ��������� �������

����� ����� �������� (������ ���), �� ������ ������:

```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 3 registered components
   ? Evaluated 3 schedule components
```

#### ��� ��������:
- ? ������� ������� �������� ����� ����� � ������� ����������
- ? ��� ������������������ ���������� ���� ���������
- ? ���������� ������� � ��������� (���� ���� ����������)

#### ? ���� ����� �� �����:
1. **���������� ������� �� ���������!**
2. ����������� `UTimeOfDayScheduleIntegration` (��. ������ 3)
3. ��� ������� ��������� `NotifyTimeChanged` �� ����� ������� �������

---

### 5. �������� ���������� �����

����� ������� ���������� �����, �������� Verbose �����������:

```cpp
// � DefaultEngine.ini ��� ����� ������� �������
LogTemp=Verbose

// ��� ����� ������� � ����:
Log LogTemp Verbose
```

**��������� ���� ��� ���������� ������:**
```
?? Schedule: Actor 'BP_NPC_Shopkeeper_C_1' - Starting action 'Open Shop' (09:00-18:00)
?? Schedule: Moving to location (1200, 300, 100)
?? Schedule: Playing animation 'OpenShop_Anim'
```

*(������ ���� ������� �� ����� ���������� `ScheduleAction`)*

---

## ������� �����������: ���-����

### ? ���������� ������� �������

���������, ��� ������:

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDayScheduleIntegration: Successfully connected!`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

���� **��� 4 ������** ���� � ����� ? **������� ��������!**

---

### ? ���-�� �� ��������?

| �������� | ������� |
|----------|---------|
| ��� ������������� ���������� | ���������, ��� ������ ������� � `.uproject` |
| ��� ����������� ����������� | �������� `ScheduleComponent` � ������, ���������� `ScheduleData` |
| ��� ��������� ������� | **�������� `UTimeOfDayScheduleIntegration` � GameState!** |
| ��� ���������� ����� | ��������� ���������� � `ScheduleData`, �������� Verbose ���� |

---

## ��������� ��������� ����������

### ��� 1: �������� ��������� ���������� � GameState

1. �������� ��� GameState Blueprint (��� �������� �����)
2. �������� ��������� `TimeOfDayScheduleIntegration`
3. ��������� Blueprint

**������!** ���������� ��������� �������������.

### ��� 2: ��������� �����������

��������� ���� � ������� � �����:
```
? TimeOfDayScheduleIntegration: Successfully connected!
```

### ��� 3: �������� NPC � �����������

1. �������� ����� (��������, NPC)
2. �������� `ScheduleComponent`
3. �������� `ScheduleData` asset
4. ��������� ��� � `ScheduleComponent -> ScheduleData`
5. ��������� ���������� � Data Asset

### ��� 4: ��������� �����������

��������� ����, ������� � �����:
```
?? Registered schedule component for actor: BP_YourNPC_C_1 (Total registered: 1)
```

### ��� 5: ��������� ��������� �������

����� ������� ����� ��������� (������ ���), �� �������:
```
? Schedule System: Time changed to 10:00 (Day 1)
   - Evaluating 1 registered components
```

---

## ������� ������� ��� �������

```cpp
// �������� ��������� ����
Log LogTemp Verbose

// ��������� ��������� ���� (������ ������ ���������)
Log LogTemp Log

// �������� ��� ����
Log LogTemp VeryVerbose
```

---

## ������� �������� ����������

### ��� ������ ����������� ������ ������� ���:

1. ? TimeOfDay ? ���������� ������� OnHourChanged
2. ?? Integration ? ������������� �������
3. ?? Integration ? �������� `ScheduleSubsystem->NotifyTimeChanged()`
4. ?? ScheduleSubsystem ? ��������� ��� ������������������ ����������
5. ?? ScheduleComponent ? ��������� ���� ����������
6. ? ScheduleAction ? �����������, ���� ����� ���������

**��� ��� ������������� ����������!**

---

## �������������� ��������

### ��������� ���������� ������������������ �������

```cpp
// � Blueprint ��� C++
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
int32 TotalActors, ActiveActions, PausedActions;
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

UE_LOG(LogTemp, Log, TEXT("?? Schedule Stats: Total=%d, Active=%d, Paused=%d"), 
    TotalActors, ActiveActions, PausedActions);
```

### ����� ���� NPC � ������������� ������

```cpp
FGameplayTagContainer SearchTags;
SearchTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Schedule.NPC.Shopkeeper")));

TArray<AActor*> Shopkeepers = Subsystem->GetActorsWithScheduleTags(SearchTags, false);

UE_LOG(LogTemp, Log, TEXT("Found %d shopkeepers with schedules"), Shopkeepers.Num());
```

---

## Troubleshooting: ������ ��������

### 1. "Schedule Subsystem Initialized" ����, �� ��� ����������� �����������

**�������:** NPC �� ���������� � ���� ��� ��������� �� �������

**�������:**
- ���������, ��� ����� �������� � ������ (�� ������ � Content Browser)
- ��������� `bAutoActivate = true` � ����������
- ���������, ��� � ������ �� �������� `BeginPlay`

### 2. ��� ����� "Time changed"

**�������:** ���������� ������� �� ���������

**�������:**
- �������� `UTimeOfDayScheduleIntegration` � GameState
- ��� ������� ��������� `NotifyTimeChanged` �� ����� ������� �������

### 3. ����� ��������, �� ������ �� �����������

**�������:** ���������� �� ��������� ��� ����� �� ���������

**�������:**
- ��������� `ScheduleData` ? ���� �� ��� ������?
- ��������� ����� ������ ������ (������ ��������� � ������� ��������)
- �������� `Log LogTemp Verbose` ��� ��������� �����

### 4. ������ ����������� �����������

**�������:** ��� �������� �� ��� �������� ������

**�������:**
- ��� ��������� ��������� ��� ���������� �����
- ����������� `InterruptCurrentAction()` ���� ����� ��������

---

## �������� ��������: "��� ��������!"

? **����������� ����� ����� ��� �������:**

```
[LogTemp] ? Schedule Subsystem Initialized
[LogTemp]    - Initial time: 08:00 (Day 1)
[LogTemp] ? TimeOfDayScheduleIntegration: Successfully connected!
[LogTemp] ?? Registered schedule component for actor: BP_NPC_1 (Total registered: 1)
```

? **��� ��������� ������� (������ ���):**

```
[LogTemp] ? Schedule System: Time changed to 09:00 (Day 1)
[LogTemp]    - Evaluating 1 registered components
```

? **���� ��� ��� ���� ? ������� �������� ���������!** ??

---

## �������������� �������

- `TIME_INTEGRATION.md` - ��������� �������� ����������
- `QUICKSTART.md` - ������� ����� � ���������
- `ContentGuide.md` - �������� ���������� ����� Data Assets
- `BUGFIX_Tickable_Initialization.md` - �������� ��������

---

## ��������

���� ������� �� �������� ����� ���� ��������:

1. �������� `Log LogTemp VeryVerbose`
2. ���������� ��� ����, ������� � "Schedule Subsystem Initialized"
3. ���������, ��� `UTimeOfDayScheduleIntegration` �������� � GameState
4. ���������, ��� `ScheduleData` �������� � �������� ����������

**����� ������ ������:** ������ �������� `UTimeOfDayScheduleIntegration` � GameState! ??
