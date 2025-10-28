# ? ���-���� �������� ������� ����������

## ?? ����������� ���� ���-���� ��� ������� �����������

---

## 1?? ������������� ����������

### ��� ������ � �����:
```
LogTemp: Schedule Subsystem Initialized
```

### ? ���� ������:
- [x] ���������� ��������
- [x] ������� ������ � ������

### ? ���� �� ������:
- [ ] ������ �� ������� � `.uproject`
- [ ] ������ ��� ������������� (��������� ������ error ����)

**��������:**
1. �������� `YourProject.uproject`
2. ���������, ��� ������ `ActorScheduleSystem` �������
3. ��������������� project files
4. ���������������� ������

---

## 2?? ���������� � TimeOfDay

### ��� ������ � �����:
```
LogTemp: ? TimeOfDayScheduleIntegration: Successfully connected!
LogTemp:    - Schedule system will be notified on every hour change
LogTemp:    - Initial time: 10:00 (Weekday: 1)
```

### ? ���� ������:
- [x] ���������� ��������
- [x] ����� ����� ������������ �������������

### ? ���� �� ������:
- [ ] ��������� `TimeOfDayScheduleIntegration` �� �������� � GameState
- [ ] GameState �� ���������� � GameMode
- [ ] ��������� �� �������

**��������:**
1. �������� GameState Blueprint
2. Add Component ? `TimeOfDayScheduleIntegration`
3. Compile + Save
4. �������� GameMode Blueprint
5. ���������� `Game State Class` = ��� GameState
6. Compile + Save
7. ������������� ����

---

## 3?? ����������� �����������

### ��� ������ � �����:
```
LogTemp: ?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

### ? ���� ������:
- [x] NPC ��������������� � �������
- [x] ���������� ����� �����������

### ? ���� �� ������:
- [ ] � ������ ��� `ScheduleComponent`
- [ ] `ScheduleData` �� ��������
- [ ] ����� �� �������� �� ������ (������ � Content Browser)
- [ ] ��������� �� ������� (`bAutoActivate = false`)

**��������:**
1. �������� ��� NPC Blueprint
2. �������� `ScheduleComponent` (���� ���)
3. � Details: ���������� `Schedule Data` = ��� Data Asset
4. ���������, ��� `Auto Activate = true`
5. Compile + Save
6. ���������� ����� �� ������ (Drag & Drop � viewport)
7. ������������� ����

---

## 4?? ��������� �������

### ��� ������ � ����� (������ ������� ���):
```
LogTemp: ? Schedule System: Time changed to 09:00 (Day 1)
LogTemp: - Evaluating 3 registered components
```

### ? ���� ������:
- [x] ����� ���������� � ������� ����������
- [x] ���������� �����������
- [x] ������ ����������� (���� ����� ���������)

### ? ���� �� ������:
- [ ] **���������� ������� �� ���������!**
- [ ] ��. ����� 2?? ����

**��������:**
1. ��������� � ������ 2??
2. �������� `TimeOfDayScheduleIntegration` � GameState
3. **��� �������� ��� ������ �������!**

---

## 5?? ���������� ����� (Verbose logs)

### �������� ��������� ����:
������� ���� (~ ������):
```
Log LogTemp Verbose
```

### ��� ������ � �����:
```
LogTemp:    ? Evaluated 3 schedule components
LogTemp: ?? Actor 'BP_NPC_1' - Starting action 'Work' (09:00-18:00)
```

### ? ���� ������:
- [x] ������ �����������
- [x] ������� ��������� �������������

### ? ���� �� ������:
- [ ] ���������� �� ��������� (������ `ScheduleData`)
- [ ] ����� �� ��������� � �����������
- [ ] Action �� ���������� ��� ������

**��������:**
1. �������� ��� `ScheduleData` asset
2. ���������, ��� ���� ������ � `Schedule Entries`
3. ��������� ����� ������/����� ������
4. ���������, ��� ������� ������� ����� �������� � ���� ��������
5. ���������, ��� `Action` �������� � ��������

---

## ?? �������� �������: ������ ������

| # | ��� ��������� | ��������� ��� | ������ |
|---|---------------|---------------|--------|
| 1 | ������������� | `Schedule Subsystem Initialized` | ? |
| 2 | ���������� | `TimeOfDayScheduleIntegration: Successfully connected!` | ? |
| 3 | ����������� | `Registered schedule component for actor: XXX` | ? |
| 4 | ����� | `Schedule System: Time changed to XX:XX` | ? |
| 5 | ������ | `? Evaluated N schedule components` | ? |

### ���� ������:

- [ ] **0/5** - ������� �� �������� ������
- [ ] **1/5** - ������ ������������� (����������� ��������)
- [ ] **2/5** - ���������� �� ���������
- [ ] **3/5** - ��� NPC � ������������
- [ ] **4/5** - ����� ��������, ��������� ��������� ����������
- [ ] **5/5** - ? **��� ��������!** ??

---

## ?? ������� ��� ������: ������ 1-2-3-4

���� ���� **���** ��� 4 ����:

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDayScheduleIntegration: Successfully connected!`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

**? ������� ��������� ���������!**

����� 5 (���������� �����) ������� ��:
- �������� ���������� (`ScheduleData`)
- �������� �������� �������
- ���������� `Action`

---

## ?? ������� ��� �������

```
// �������� ��������� ���� (��� ������ 5)
Log LogTemp Verbose

// ����� ��������� ���� (��� �������)
Log LogTemp VeryVerbose

// ������� ������� ����
Log LogTemp Log

// ������� ���������� � runtime (Blueprint/C++)
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
int32 Total, Active, Paused;
Subsystem->GetScheduleStats(Total, Active, Paused);
PrintString(FString::Printf(TEXT("Stats: %d total, %d active, %d paused"), Total, Active, Paused));
```

---

## ?? ��������� ����

����� ����, ��� ��� 5 ������� ?:

1. �������� `ScheduleData` assets ��� ������ ����� NPC
2. ��������� ���������� (�����, ��������, �������)
3. �������� `ScheduleComponent` � ����� NPC
4. ��������� ��������������� `ScheduleData`
5. ���������� `ScheduleAction` ������ ��� ����� �����
6. ���������� � ����������!

---

## ?? ������������

- **SUMMARY.md** - ������� ����� �� ��� ������
- **ANSWER_LOG_DIAGNOSTICS.md** - ��������� ������ �����
- **QUICK_CHECK.md** - ������� �������� (1 ������)
- **DIAGNOSTICS.md** - ������ �����������
- **TIME_INTEGRATION.md** - ��������� ���������� �������
- **QUICKSTART.md** - ������� �����
- **ContentGuide.md** - �������� ����������

---

## ? ����� ������ ������

### ?? #1: ��� ���������� ������� (90% �������)

**��������:**
- ����: `Schedule Subsystem Initialized`
- ���: `TimeOfDayScheduleIntegration: Successfully connected!`
- ���: `Time changed to XX:XX`

**�������:**
- GameState Blueprint ? Add Component ? `TimeOfDayScheduleIntegration`

---

### ?? #2: ��������� �� ���������������

**��������:**
- ����� � ����
- ���� `ScheduleComponent`
- ���: `Registered schedule component for actor: XXX`

**�������:**
- ���������, ��� `Schedule Data` ��������
- ���������, ��� `Auto Activate = true`
- ��������� BeginPlay ���� �� ������

---

### ?? #3: ������ �� �����������

**��������:**
- ��� ���� ���� (1-4)
- ����� ��������
- ������ �� �����������

**�������:**
- ��������� ����������: ��������� �� �����?
- �������� `Log LogTemp Verbose`
- ���������, ���������� �� `Action`

---

## ?? ���������

���� ����� ���� �������� ���-�� �� ��������:

1. �������� `Log LogTemp VeryVerbose`
2. ������������� ����
3. ���������� ���� �� ������ �� ������� ��������� �������
4. ��������� ��� ������ ����� ���-�����
5. �������� ������������ (DIAGNOSTICS.md)

**�����!** ??
