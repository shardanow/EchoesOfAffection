# ������� ��������: �������� �� ������� ����������?

## ?? ��� ������ � �����

### 1. ��� ������� ���� (PIE)

����� ��� ������ � **Output Log**:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components
```

**������:** ? ���������� ���������� ��������

---

### 2. ���������� � TimeOfDay

���� �� �������� `TimeOfDayScheduleIntegration` � GameState:

```
? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

**������:** ? ���������� ������� ��������

? **���� ����� ���:**
- �������� ��� **GameState Blueprint**
- �������� ��������� `TimeOfDayScheduleIntegration`
- ��������� � �������������

---

### 3. ����������� NPC

����� NPC � `ScheduleComponent` ���������� � ����:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

**������:** ? NPC ���������������

? **���� ����� ���:**
- ���������, ��� � ������ ���� `ScheduleComponent`
- ���������, ��� `ScheduleData` �������� � ����������
- ���������, ��� ����� �������� �� ������ (�� ������ � Content Browser)

---

### 4. ��������� �������

������ ������� ��� �� ������ ������:

```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 3 registered components
```

**������:** ? ������� ������� ��������, ���������� �����������

? **���� ����� ���:**
- **���������� ������� �� ���������!**
- �������� `TimeOfDayScheduleIntegration` � GameState (��. ����� 2)

---

## ?? ������� �����������: ��� ���������� �������

### ������� 1: �������������� ���������� (�������������)

1. �������� **GameState Blueprint** (��� �������� �����)
2. **Add Component** ? `TimeOfDayScheduleIntegration`
3. **Compile** � **Save**
4. ������������� ����

**������!** ������ ������ ��������� ��� ���� �� ������� 2 � 4.

---

### ������� 2: ������ ���������� (���� ��� GameState)

�������� � ����� persistent actor (��������, GameMode):

**C++:**
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
UTimeOfDayScheduleIntegration* ScheduleIntegration;
```

**Blueprint:**
- Add Component ? `TimeOfDayScheduleIntegration`
- ������!

---

## ?? �������� ����������

### � Blueprint ��� C++:

```cpp
// �������� ����������
int32 TotalActors, ActiveActions, PausedActions;
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

// ������� � ���
UE_LOG(LogTemp, Log, TEXT("?? Schedule Stats:"));
UE_LOG(LogTemp, Log, TEXT("   Total registered actors: %d"), TotalActors);
UE_LOG(LogTemp, Log, TEXT("   Active actions: %d"), ActiveActions);
UE_LOG(LogTemp, Log, TEXT("   Paused actions: %d"), PausedActions);
```

---

## ? ���-����: "��� ��������!"

���� � ����� ���� **���** ��� ������:

- ? `Schedule Subsystem Initialized`
- ? `TimeOfDayScheduleIntegration: Successfully connected!`
- ? `Registered schedule component for actor: XXX`
- ? `Schedule System: Time changed to XX:XX`

**? ������� �������� ���������! ??**

---

## ? ���-����: "���-�� �� ���!"

| �������� | ��� ��������� |
|----------|---------------|
| ��� ������������� | ������ ������� � `.uproject`? |
| ��� ���������� ������� | �������� `TimeOfDayScheduleIntegration` � GameState? |
| ��� ����������� NPC | ���� `ScheduleComponent`? ���� `ScheduleData`? ����� � ����? |
| ��� ��������� ������� | ���������� ������� ���������? (��. ����� 4) |

---

## ?? ������� �������

��� ����� ��������� �����, � ������� ���� �������:

```
Log LogTemp Verbose
```

��� ����� ��������� �����:

```
Log LogTemp VeryVerbose
```

������� ������� ����:

```
Log LogTemp Log
```

---

## ?? �������������� ����������

- **������ �����������:** `DIAGNOSTICS.md`
- **��������� ����������:** `TIME_INTEGRATION.md`
- **�������� ����������:** `ContentGuide.md`
- **������� �����:** `QUICKSTART.md`

---

## ?? ����� ������ ������

**90% �������:** ������ �������� `TimeOfDayScheduleIntegration` � GameState!

**�������:** 
1. GameState Blueprint
2. Add Component ? `TimeOfDayScheduleIntegration`
3. Compile + Save
4. Profit! ??
