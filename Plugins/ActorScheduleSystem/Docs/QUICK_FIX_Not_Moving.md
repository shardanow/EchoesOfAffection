# ?? ������� �������: ����� �� ���������

## ? ��������

�� ������ � �����:
```
? Registered schedule component
? Time changed to 11:00
? Evaluating 1 registered components
? �� ����� �� ���������!
```

---

## ? ������� ������� (30 ������)

### 1. ��������� ��� ������� � Schedule Entry

�������� ��� `Schedule Entry Data Asset` (��������, `DA_Entry_GoToShop`).

#### ? ���� ����� **TimeOfDay:**

```
Time Type: TimeOfDay        ? ��������!
Start Hour: 11
Start Minute: 0
```

**�������� ��:**

```
Time Type: TimeRange        ? �������!
Start Hour: 11
Start Minute: 0
End Hour: 18     ? �������� �����
End Minute: 0
```

**������?**
- `TimeOfDay` �������� ������ � ������ ����� (11:00:00)
- � 11:01 �� ��� �� ���������
- ��� ���������� �������� ����������� `TimeRange`

---

### 2. �������� ��������� ����

� ������� ���� (~ ������):
```
Log LogTemp Verbose
```

������������� ���� � ��������� ����.

---

## ?? ��� �� ������ �������

### ����� �����������:

```
? Schedule System: Time changed to 11:00 (Day 0)
   Evaluating schedule for BP_Lianne_C_1 at 11:00
   Checking 1 entries
     Entry 'Go to Shop' (Priority 50): MATCH
       TimeRange: 11:00 in [11:00 - 18:00] ? YES
?? Starting action 'Go to Shop' for BP_Lianne_C_1
   ?? Selected location: X=1200.0 Y=300.0 Z=100.0
   ? Action started successfully
```

---

## ?? �������������� �����������

### ���� ��� ��� �� ��������, ���������:

#### 1. ���� �� Action � Entry?

�������� `Schedule Entry Data Asset`:
- **Action** ? ������ ���� �������� (��������, `Move To Location`)
- ���� ����� ? �������� � ��������� Action

#### 2. �������� �� Action ���������?

���� ����������� `Move To Location`:
- **Location Selector** ? ��������?
- **Target Location** ? ������?
- **Acceptance Radius** ? > 0

#### 3. ���� �� AIController � NPC?

�������� NPC Blueprint ? **Class Defaults**:
- **Auto Possess AI** ? `Placed in World or Spawned`
- **AIController Class** ? `AIController` (�������)

#### 4. ���� �� NavMesh?

� viewport ������� **P**:
- ������ ���� **������� ����** ������ NPC � ������� �����
- ���� ��� ? �������� `Nav Mesh Bounds Volume`

---

## ?? ���-���� ������� ��������

- [ ] Time Type = **TimeRange** (�� TimeOfDay)
- [ ] Start/End Hour ����������
- [ ] **Action** �������� � Entry
- [ ] **Location Selector** ��� **Target Location** ��������
- [ ] NPC ����� **AIController**
- [ ] **NavMesh** ��������� ���� ��������
- [ ] ���� `Verbose` ��������

---

## ?? ����� ������������ TimeOfDay vs TimeRange

### TimeOfDay - ��� ���������� �������:

? ���������� � 7:00  
? ��������� � ������� � 12:00  
? ���� ����� � 22:00  

### TimeRange - ��� ���������� ��������:

? �������� 9:00-18:00  
? ������������� ����� 20:00-06:00  
? ������� 12:00-13:00  
? **��������� � �����** (��� ������!)

---

## ?? ����

1. **��������** `Time Type` �� `TimeRange`
2. **�������** `End Hour` � `End Minute`
3. **�������������** ����
4. **���������** ���� (`Log LogTemp Verbose`)

**������!** ����� ������ ���������. ??

---

## ?? ������������

- **BUGFIX_Actor_Not_Moving.md** - ������ ����������� �������� ����
- **QUICKSTART.md** - ������� ����� � ���������
- **DIAGNOSTICS.md** - ����������� ���� �������

**�����!** ??
