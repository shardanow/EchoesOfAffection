# Quick Start: �������� ������ � �������

## ?? ����� ������� ������

### � Blueprint NPC:

```
1. �������� BP_YourNPC
2. ������� DialogueInputBlockerComponent
3. � Details ? Default Blocking Settings:
   ?? Show Mouse Cursor = ? TRUE

������! ������ ����� ������������ � ���������� �������������.
```

## ?? ��� DialogueWidget

���� � ��� ���� ������ ������ � UI:

```
1. �������� DialogueInputBlockerComponent
2. ����������:
   ?? Show Mouse Cursor = ? TRUE
 ?? Enable Click Events = ? TRUE
   ?? Enable Mouse Over Events = ? TRUE (��� hover)
```

## ?? ���������� � Blueprint

```blueprint
Event: Start Dialogue
    ?
[Get Component: DialogueInputBlocker]
    ?
[Get Current Blocking Settings]
    ?
[Set bShowMouseCursor] = True
  ?
[Block Input] (� ����������� �����������)
```

## ? ��������� (v1.11)

? ������ ����� �� ����� �������  
? ����� ������� �� UI ���������  
? �������� hover �� �������  
? ������������� ���������� ����� �������  
? **�����:** ����� ����� ������������ � ���� ��������� (�� ����� ����!)  

## ?? ��� ���������� � v1.11?

### ? ������ �������� (v1.10):
```
��������� ������ ? ������ �����
��: ����� �������� �� viewport ����� ��������� ���������� ?
```

### ? ���������� (v1.11):
```
��������� ������ ? ������ �����
���������� �������� ����� ? (����� �������� �������������)
```

---

**��������:**  
- `Mouse_Cursor_Support_v1.10.md` - �������� ������������  
- `Input_Focus_Fix_v1.11.md` - ����������� ������

**������:** 1.11
