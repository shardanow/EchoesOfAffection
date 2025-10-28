# ������� �����: �������� ������ �� �������

## ? �� 5 �����

### 1. �������� Navigation Query Filter

**Content Browser ? Right-click ? Miscellaneous ? Data Asset ? NavigationQueryFilter**

��������: `NQF_RoadOnly`

**���������:**
```
Areas:
  [0] NavArea_Road:
      Override Travel Cost: ?
   Travel Cost Override: 0.5
  
  [1] NavArea_Default:
      Override Travel Cost: ?
      Travel Cost Override: 100.0
```

### 2. ��������� ������ � ������

1. **Place Actors ? Volumes ? Nav Modifier Volume**
2. ���������� ��� �������
3. **Details Panel:**
   - `Area Class`: `NavArea_Road`
4. **Build ? Build Paths**

### 3. ��������� ������ � MoveToLocation

**� ����� Schedule Entry:**
```
Action: MoveToLocation
  ?? Filter Class: NQF_RoadOnly  ? �������� ��������� ������
  ?? Location Selector: ActorTag
  ?? bUsePathfinding: ? true
  ?? Acceptance Radius: 100
```

### 4. ���������

1. **Play In Editor (PIE)**
2. **������� P** ? ������� NavMesh
3. **������ ������ ���� ������** (NavArea_Road)
4. **NPC ������ ���� ������ �� ������** ??

---

## ?? ���������� ��������

### ����� NavMesh (������� P):

- ?? **������** = ������ (NavArea_Road) - **NPC ���� �����**
- ?? **�����** = Default (NavArea_Default) - **NPC ��������**
- ?? **�������** = ����� (NavArea_Grass) - **NPC ��������**

### ���������� ����:

```
NPC ? ???????????? ? ����  ? (�� ������)
NPC ? ???????? ? ����      ? (��������)
```

---

## ?? ������ ��������

### ? NPC ���� �������� (�� �� ������)

**���������:**
1. `Filter Class` ���������� � MoveToLocation? ? **����� ������ �������!**
2. `bUsePathfinding = true`?
3. Nav Modifier Volume ��������?
4. NavMesh ����������? (Build ? Build Paths)

### ? NPC �� ����� ����� ����

**�������:**
- ���� ������ ���� **�� ������** (������ ����)
- ���: ����������� `Cost` ������ `IsExcluded`

---

## ?? ��������� ���������

| ���� | Travel Cost | ��������� |
|------|-------------|-----------|
| **������������** ������ | 0.5 | NPC ������� ������ ���� �������� |
| **��������** ����� | 10.0 - 100.0 | NPC ������ � ����� |
| **���������** ���� | IsExcluded = true | NPC �� ����� ���� ����� ���� |

**������������� ��������:**
```
������:    0.5  (����� ������)
�������:   0.7  (������)
�����:     10.0 (������, �� �����)
����: 100.0 (����� ���������)
������: IsExcluded = true (������ ������)
```

---

## ?? ������ ������������

��� ���������� ����������� ��������: [ROAD_NAVIGATION_RU.md](ROAD_NAVIGATION_RU.md)

��������:
- ? �������� ����������� NavArea ������� (C++)
- ? ����������� ��������� ��������
- ? ������� � ������������
- ? ������� ��� ������ ��������� (�����, �������, sci-fi)
- ? API Reference

---

## ?? ������� �������������

### ��������� NPC (������ �� �������):
```
NQF_StrictRoad:
  NavArea_Road: 0.5
  NavArea_Sidewalk: 0.6
  NavArea_Grass: IsExcluded = true
```

### ���������� �������� (����� �������):
```
NQF_GuardPatrol:
  NavArea_Road: 0.5
  NavArea_Grass: 5.0  ? �����, �� ��������
```

### ������ ��� �����:
```
NQF_Monster:
  NavArea_Forest: 1.0
  NavArea_Road: 10.0  ? �������� �����!
```

---

**������!** ��� NPC ������ ����� ������ �� ������� ????????

**������:** ���� ���-�� �� ��������, �������� [Troubleshooting](ROAD_NAVIGATION_RU.md#�������)
