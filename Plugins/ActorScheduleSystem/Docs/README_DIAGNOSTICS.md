# Schedule System Diagnostic Guide

## ?? ������� ����� �� ��� ������

**Q:** ��� �����, ��� ������� �������� ���������?  
**A:** � **Output Log** (Window ? Developer Tools ? Output Log)

---

## ?? ��� ������� ������

### ? ��� ��������:

```
[LogTemp] Schedule Subsystem Initialized        ?
[LogTemp] TimeOfDaySubsystem initialized...     ?
```

### ? ��� �� ��������:

```
TimeOfDayScheduleIntegration: Successfully...? ���
Registered schedule component...           ? ���
Schedule System: Time changed...     ? ���
```

**��������:** ������� ��������, �� **�� �������** ? ������ **�� �����������**

---

## ? ������� ����������� (30 ������)

### 1. GameState Blueprint
### 2. Add Component ? `TimeOfDayScheduleIntegration`
### 3. Compile + Save
### 4. Restart Game

**������!**

---

## ?? ����� ����������� �������:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   
? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)

?? Registered schedule component for actor: BP_NPC_1 (Total: 1)

? Schedule System: Time changed to 11:00 (Day 1)
   - Evaluating 1 registered components
```

---

## ?? ����������� ���-���� ������

- [ ] `Schedule Subsystem Initialized`
- [ ] `TimeOfDayScheduleIntegration: Successfully connected!`
- [ ] `Registered schedule component: XXX`
- [ ] `Time changed to XX:XX` (������ ���)

**��� 4 ������ ?** = **������� ��������!** ??

---

## ?? ������ ������������

- **FINAL_ANSWER.md** - ������ ����� �� ��� ������
- **CHECKLIST.md** - ��������� ���-����
- **QUICK_CHECK.md** - ������� �������� (1 ���)
- **DIAGNOSTICS.md** - ������ �����������
- **SUMMARY.md** - ������� ������

---

## ?? ������� �������

```
Log LogTemp Verbose        // ��������� ����
Log LogTemp VeryVerbose    // ����� ���������
Log LogTemp Log // �������
```

---

## ?? ��������� ����

1. �������� `TimeOfDayScheduleIntegration` � GameState ? **������**
2. ������� NPC � `ScheduleComponent`
3. ������� `ScheduleData` asset
4. ��������� ����������
5. Profit! ??
