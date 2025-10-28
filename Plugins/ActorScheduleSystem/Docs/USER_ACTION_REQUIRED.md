# ? �������: ������ ����� �� ��������� �� ����������

## ?? ���� ��������

�� ������ ����:
```
? Registered schedule component for actor: BP_Lianne_C_1
? Schedule System: Time changed to 11:00 (Day 0)
? Evaluating 1 registered components
```

**�� ����� BP_Lianne �� ���������!**

---

## ?? ����������� ���������

�� ��������� **2 ����������� ����** � �������:

### ? Bug #1: ������������ ��������� �������
- **����:** ��������� ������� �������� �������� �������
- **�����:** �������� ����� `GetCurrentTime()` ��� ����������� ���������

### ? Bug #2: ������������ �����������
- **����:** ���������� ������ ������ �� ��������
- **�����:** ��������� ���� �� ������ ����

---

## ?? ��� ����� ������� ������

### ��� 1: ����������������� ������ ? (��� �������)

������ ������� �������������:
```
Target is up to date
Result: Succeeded
```

---

### ��� 2: ��������� ��������� Schedule Entry

#### �������� ��� Schedule Entry Data Asset

��������: `DA_Entry_GoToShop` ��� ��� �� ��� �������.

#### ��������� Time Condition:

**���� ����� TimeOfDay:**

```
Time Condition:
  Time Type: TimeOfDay        ? ? ��������!
  Start Hour: 11
  Start Minute: 0
```

**�������� �� TimeRange:**

```
Time Condition:
  Time Type: TimeRange        ? ? �������!
  Start Hour: 11
  Start Minute: 0
  End Hour: 18      ? ����������� ��������!
  End Minute: 0
```

#### ��������� ���������

---

### ��� 3: ��������� Action � Entry

� ��� �� Schedule Entry:

1. **Action** ? ������ ���� ��������
2. ���� ����������� `Move To Location`:
   - **Location Selector** ? ��������? (��������, `Actor Tag Selector`)
   - **Tags** ? �������? (��������, `Location.Shop`)
   - **Acceptance Radius** ? 100.0 (��� ������)

---

### ��� 4: ��������� NPC

�������� **BP_Lianne** Blueprint:

1. **Components** ? ���� `Schedule Component`? ?
2. **Schedule Data** ? �������� ��� ScheduleData? ?

**Class Defaults:**
3. **Auto Possess AI** ? `Placed in World or Spawned` ?
4. **AIController Class** ? `AIController` (�������) ?

---

### ��� 5: ��������� NavMesh

1. �������� ��� �������
2. ������� **P** (�������� ���������)
3. ������ ���� **������� ����** ������ NPC � ������� �����

**���� ���:**
- **Place Actors** ? **Volumes** ? `Nav Mesh Bounds Volume`
- ��������� ����� ������� ��� ���� ��������
- ������� **P** ����� - ������ ��������� ������� ����

---

### ��� 6: �������� ��������� ����

**��������� ���� (Play)**

**�������� ������� (~)** � �������:
```
Log LogTemp Verbose
```

---

### ��� 7: ��������� ����

**Output Log** ������ ��������:

```
? Schedule System: Time changed to 11:00 (Day 0)
   - Evaluating 1 registered components
   Evaluating schedule for BP_Lianne_C_1 at 11:00 (Day 0)
   Checking X entries for BP_Lianne_C_1
     Entry 'Go to Shop' (Priority 50): MATCH
     TimeRange: 11:00 (660) in [11:00 (660) - 18:00 (1080)] ? YES
?? Starting action 'Go to Shop' for BP_Lianne_C_1 (Priority: 50)
   ?? Selected location: X=1200.0 Y=300.0 Z=100.0
   ? Action started successfully
```

**���� ������ ���** ? ? **��������!**

---

## ?? Troubleshooting

### ���� "No matching entry found"

```
?? No matching entry found for BP_Lianne_C_1 at 11:00
```

**�������:**

1. **Time Type = TimeOfDay** (������ TimeRange)
   - �������: �������� �� `TimeRange` ��� ������� ����

2. **������������ Start/End Hour**
   - �������: ���������, ��� ������� ����� ������ � ��������

3. **DaysOfWeek �� �������� ������� ����**
   - �������: � Schedule Entry ? Days Of Week ? �������� ������ (��� ���) ��� �������� `0`

---

### ���� "Entry has no Action assigned"

```
?? Entry 'XXX' has no Action assigned!
```

**�������:**

1. �������� Schedule Entry
2. **Action** ? �������� ��� ��������� Action
3. ���������

---

### ���� "Failed to select location"

```
?? Failed to select location for schedule entry: XXX
```

**�������:**

1. **Location Selector �� ��������**
   - �������: � Entry ? Location Selector ? �������� `Actor Tag Selector`

2. **�� ������ ����� � ������ �����**
   - �������: 
 - �������� Target Point
     - �������� ��� (��������, `Location.Shop`)
     - ���������� �� ������

3. **����� ������� ������**
   - �������: � Location Selector ? Max Distance ? ��������� ��� ��������� 0 (��� �����������)

---

### ����� �� ���������, ���� ���� ������� "Action started"

**�������:**

1. **��� AIController**
   - �������: � NPC Blueprint ? Class Defaults ? Auto Possess AI = `Placed in World`

2. **��� NavMesh**
   - �������: �������� `Nav Mesh Bounds Volume` (��. ��� 5)

3. **������� ����� �����������**
   - �������: ��������� ��� ����� �� ������� ���� NavMesh

---

## ? ��������� ���-����

�������� �� ������ � ���������:

- [ ] ������ ���������������� (��� ������� ?)
- [ ] Schedule Entry ���������� `TimeRange` (�� `TimeOfDay`)
- [ ] End Hour � End Minute �����������
- [ ] Action �������� � Entry
- [ ] Location Selector ��������
- [ ] Target Point � ����� ���������� �� ������
- [ ] NPC ����� Schedule Component
- [ ] NPC ����� AIController
- [ ] NavMesh ��������� ���� ��������
- [ ] ���� Verbose ��������
- [ ] ���� ���������� "Action started successfully"

**���� ��� ?** ? ����� ������ ���������!

---

## ?? ������ ����������

### TimeOfDay vs TimeRange

| ��� | ��������� | ����� ������������ |
|-----|-----------|-------------------|
| **TimeOfDay** | ����������� **������** � ������ ����� (11:00:00) | ���������� �������: ����������, ������� |
| **TimeRange** | ������� ���� �������� (11:00 - 18:00) | **���������� ��������: ��������, ������, �������** |

**��� �������� ������ ������ ����������� TimeRange!**

---

## ?? �������������� ������������

- **BUGFIX_Actor_Not_Moving.md** - ����������� ������ �����
- **QUICK_FIX_Not_Moving.md** - �����-������� �������
- **QUICKSTART.md** - ������ ����������� �� �������

---

## ?? ����

1. ? ���� ���������� � ����
2. ? ������ �������������
3. ?? ��� ����� �������� **Time Type** �� **TimeRange**
4. ?? ��������� ��������� ��������� �� ���-�����
5. ? �������� ���� Verbose
6. ? �����������!

**����� ���������� ���� ����� ����� ����� ���������!** ??

**�����!** ??
