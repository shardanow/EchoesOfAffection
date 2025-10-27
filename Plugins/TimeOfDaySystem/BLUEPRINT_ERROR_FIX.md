# Blueprint Interface Error Fix

## ��������

���� �� ������ ������:
```
Assertion failed: 0 && "Do not directly call Event functions in Interfaces. 
Call Execute_RegisterListener instead."
```

��� ��������, ��� �� ��������� ������ ���������� �������� �� Blueprint.

## �������

### ������� 1: ����������� BP_ ������� ������ (�������������) ?

��� ������ Time of Day Subsystem ����� **��� ������**:

1. **������ ����������** (� _Implementation ���������) - ��� C++
2. **BP_ ������** - ��� Blueprint

**� Blueprint ������ ����������� BP_ ������:**

#### �������� ������ ����:

| ? ������ ���� (�������� ������) | ? ����� ���� (��������) |
|----------------------------------|--------------------------|
| `Register Listener` | `Register Listener (BP)` |
| `Unregister Listener` | `Unregister Listener (BP)` |
| `Get Current Time` | `Get Current Time (BP)` |
| `Get Current Phase` | `Get Current Phase (BP)` |
| `Get Normalized Time of Day` | `Get Normalized Time (BP)` |
| `Get Current Season` | `Get Current Season (BP)` |
| `Set Current Time` | `Set Current Time (BP)` |
| `Advance Time` | `Advance Time (BP)` |
| `Set Time Scale` | `Set Time Scale (BP)` |
| `Get Time Scale` | `Get Time Scale (BP)` |
| `Pause Time` | `Pause Time (BP)` |
| `Resume Time` | `Resume Time (BP)` |
| `Is Time Paused` | `Is Time Paused (BP)` |

### ������� 2: ����������� Blueprint Library

������ ������� ������ Subsystem, ����������� **UTimeOfDayBlueprintLibrary**:

```
Get Current Phase (World Context = Self)
  ?
  ?
[���������� ��������� - ETimeOfDayPhase]
```

��� ������� ������������� �������� Subsystem � �������� ���������� ������.

### ������� 3: ����������� TimeOfDayListenerComponent

��� ��������� ������� �������:

1. **�������� ��������� � Actor:**
   - � Blueprint Actor �������� Components
- Add Component ? `Time Of Day Listener Component`

2. **��������� �������:**
   - �������� ������ ������� (Receive Phase Events, etc.)
   - � Event Graph �������� ������ �� ���������
   - Bind � ��� event delegates

3. **��������� �������������:**
   - �������������� � Subsystem ��� BeginPlay
   - Unregisters ��� EndPlay
   - �������� ���� Blueprint �������

## ������� ������������� ����

### �� (�������� ������):
```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Register Listener [Self]  ? ? ������!
```

### ����� (��������):
```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Register Listener (BP) [Self]  ? ? ��������!
```

### ��� ��� ����� - ����������� ���������:
```
[Time Of Day Listener Component ��� � Components list]

Event BeginPlay
  ?
  ?
[������ �� ������ - ��������� ��� ��������������!]

// ��� ��������� ������:
Get Current Phase (BP)
  ?
  ?
Print String
```

## ��� ����� BP_ ���� � Blueprint

1. �������� Blueprint Event Graph
2. Right-click ? Add Function Call
3. � ������ �������: `BP Get Current` (� ��������� BP)
4. �� ������� ��� BP_ ������ �������
5. ��� ����� ����� `(BP)` � �������� ��� �������

## �������������� ���������

### Pure functions (Get-������)
BP_ Get-������ �������� ��� **BlueprintPure**, ������� �� ������� execution pin:

```
???????????????????????????????
? Get Current Phase (BP) ?
???????????????????????????????
? Target: Time Of Day Subsyst ? (no exec pin!)
? Return: ETimeOfDayPhase     ?
???????????????????????????????
```

### Impure functions (Set-������)
BP_ Set-������ ������� execution pin:

```
     ????????????????????????????????
  ???? Set Time Scale (BP)      ????
     ????????????????????????????????
     ? Target: Time Of Day Subsyst  ?
     ? New Time Scale: 2.0     ?
     ????????????????????????????????
```

## �������� ������������

����� ����������� �� **�� ������** ������ ������:
- ? "Do not directly call Event functions"
- ? "Call Execute_XXX instead"
- ? �� �������� ��� assertion errors

## ���� �� ��� ���� ������

1. ���������, ��� ����������� **BP_** ������ �������
2. ���������, ��� �������������� Blueprint ����� ���������
3. ������������� ��������
4. ��������� Output Log �� ������ ������

## ������ ������ ����������� �������������

```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem (World Context = Self)
  ?
  ?
Initialize With Settings [DA_TimeOfDay_Default]
  ?
  ?
Get Current Phase (BP)
  ?
  ?
Switch on ETimeOfDayPhase
  ?? Morning ? Print "Good Morning!"
?? Noon ? Print "Midday!"
  ?? Night ? Print "Good Night!"
  ?? Default ? Print "Unknown phase"
```

��! ������ ���� Time of Day System ����� �������� ��������� � Blueprint! ???

---

**�������������� ����������:**
- ������������: `README.md`
- ������� �����: `QUICKSTART.md`
- �����������: `ARCHITECTURE.md`
