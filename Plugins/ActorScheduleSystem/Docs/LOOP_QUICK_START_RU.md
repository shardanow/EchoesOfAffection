# ������������ ������� - ������� ���������� (RU)

## ��� ���������?

� Composite Action ������ ����� **����������� ������ �������� �������**!

## ��� ������������?

###  1. �������� ��� Composite Action

� Content Browser ������� Data Asset ���� `ScheduleAction_Composite` ��� �������� �����.

### 2. �������� ������������

� ��������� ������:

```
? bExecuteSequentially = true   (������ ���� ��������!)
? bLoopChildActions = true      (�������� ������������)
   MaxLoops = 0       (0 = ����������, ��� ������� �����)
```

### 3. �������� �������� ������

```
Child Actions:
  [0] MoveToLocation (���� � ������)
  [1] PlayAnimation (��������)
  [2] Wait (30 ������)
  [3] MoveToLocation (���� �� �����)
  [4] Wait (10 ������)
```

### 4. ������!

����� ���������� ���������� ������ (#4), ������� **������������� �������� � �������** (#0) � ������ ���� ������!

## ������� ��������

### ����������� ���� (�� ������ ����������)
```
bLoopChildActions = true
MaxLoops = 0
```
**������������ ���**: ������� �����, �������, ���������� ��������

### ������������ ���������� ����������
```
bLoopChildActions = true
MaxLoops = 5
```
**������������ ���**: "������ ������� 5 ���", "��������� ������ 3 ����"

### ��� ������������ (��� ������)
```
bLoopChildActions = false
```
**������������ ���**: ����������� ������������������ ��������

## ����� ���� ���������������?

1. **��������� MaxLoops** (���� �� 0)
2. **��������� ���� ���������� �����������** (Schedule Entry TimeCondition)
3. **������� ������** (CancelAction, InterruptAction)

## ���������� �����

```
???????????????????????????????????
?  bLoopChildActions = true       ?
?  MaxLoops = 0 (infinite)     ?
???????????????????????????????????
    ?
         ?
    [Action 1] ????? [Action 2] ????? [Action 3]
         ?   ?
         ?        ?
         ???????????????? LOOP ??????????????
```

## �������

### � ����� (LogScheduleComposite):
```
Starting composite action with 5 children (Sequential: YES)
Starting first child action
Child action 0 completed, advancing to next
...
Child action 4 completed, advancing to next
Looping child actions (Loop 1/-1)     ? ������� ����� ����!
Restarting first child action
```

### � Visual Logger:
1. ��������� PIE (Play In Editor)
2. ������� **Ctrl+L** ? ��������� Visual Logger
3. �������� ������ NPC
4. �������� ��������� `LogScheduleComposite`

## ������ �������

**Q: ������ ���� �� ��������?**  
A: ���������, ��� `bExecuteSequentially = true`. ������������ �������� ������ ��� ����������������� ����������!

**Q: ��� ���������� ����������� ����?**  
A: ���� ��������� ��������� ���������� ���� � Schedule Entry, ���� �������� `CancelCurrentAction()` �� ����������.

**Q: ����� �� ������� ����� ���� � �����?**  
A: ��! �������� ��������� Composite Action:
```
Composite (Outer, MaxLoops = 0):
  ?? Composite (Inner, MaxLoops = 3)
  ?  ?? [Work Actions...]
  ?? Wait (Break 60 sec)
```

**Q: ������ �� �� ������������������?**  
A: ���, ��������� ������� ���������� (���� ������� � ������� ��������).

## ������ ������������

?? ��������� ��������: `Docs/FEATURE_Loop_Child_Actions.md`  
?? ��������� ������: `Docs/EXAMPLE_Looping_Actions.md`  
?? ����� ������������: `README.md`

---

**������**: v1.1  
**�����**: EchoesOfAffection Team  
**GitHub**: https://github.com/shardanow/EchoesOfAffection
