# ? ������������ ������ - Road Navigation

## ��� ���� �������

### ?? 5 ����� ���������� (����� ~30 �������)

1. **[ROAD_NAVIGATION_INDEX.md](Docs/ROAD_NAVIGATION_INDEX.md)** - ��������� �� ���� ������������
2. **[ROAD_QUICK_START_RU.md](Docs/ROAD_QUICK_START_RU.md)** - ������� ����� (5 �����)
3. **[ROAD_NAVIGATION_RU.md](Docs/ROAD_NAVIGATION_RU.md)** - ������ ����������� (20+ �������)
4. **[ROAD_VISUAL_GUIDE_RU.md](Docs/ROAD_VISUAL_GUIDE_RU.md)** - ���������� ����������� (�����)
5. **[ROAD_NAVIGATION_CPP.md](Docs/ROAD_NAVIGATION_CPP.md)** - C++ ������� ��� �������������

### ?? ����������� �����

- **[README.md](README.md)** - ��������� ������ � ���������� �������� �� Road Navigation

### ?? Changelog

- **[CHANGELOG_v1.0_RoadNavigation.md](CHANGELOG_v1.0_RoadNavigation.md)** - ������� ���������

---

## ������� ����������

### ������:
> "������ �������� ��� ���� ��� � ���� ��������� ������ move to location ������ �� ������? � ��� � ����� ������� ����� ����������� �������������?"

### �����:
? **��, ��������� ��������������!**

������������ �������� `FilterClass` � `MoveToLocation` Action + `NavigationQueryFilter` Data Asset.

---

## ��� ������������ (Quick Start)

### 1. �������� Navigation Query Filter

```
Content Browser ? Right-click ? Data Asset ? NavigationQueryFilter
���: NQF_RoadOnly

���������:
  Areas:
    [0] NavArea_Road: Cost = 0.5
    [1] NavArea_Default: Cost = 100.0
```

### 2. ��������� ������

```
Place Actors ? Nav Modifier Volume
���������� ��� �������
Area Class: NavArea_Road
Build ? Build Paths
```

### 3. ��������� � Action

```
Schedule Entry ? MoveToLocation Action
  Filter Class: NQF_RoadOnly
  bUsePathfinding: true
```

### 4. ������! ??

NPC ������ ����� **������ �� �������**, �� ������� ����� �����!

**��������:** ������� **P** � PIE ? ������ ������ ���� ?? ������.

---

## ������������ �� �������

### ?? ������� (5 �����)
[ROAD_QUICK_START_RU.md](Docs/ROAD_QUICK_START_RU.md)
- ����������� ���������
- ������ ����� �����������
- Blueprint/Data Asset ������

### ?? ���������� ��������
[ROAD_VISUAL_GUIDE_RU.md](Docs/ROAD_VISUAL_GUIDE_RU.md)
- ��������� "���������" (ASCII)
- ����� � ���������
- ���������� �������
- Checklist

### ?? ��������� ��������
[ROAD_NAVIGATION_RU.md](Docs/ROAD_NAVIGATION_RU.md)
- ������ ����������� (20+ �������)
- ��� �����������
- ����������� ��������
- Troubleshooting
- API Reference

### ?? C++ ����������������
[ROAD_NAVIGATION_CPP.md](Docs/ROAD_NAVIGATION_CPP.md)
- �������� NavArea �������
- ����������� �������� ��������
- ������������ ����������
- Unit �����

---

## ������� �������������

### ������ 1: ��������� NPC

```cpp
FilterClass: NQF_CityStrict
  NavArea_Road: 0.5      // ������ - ������������
  NavArea_Sidewalk: 0.7  // �������� - �����
  NavArea_Grass: 100.0   // ����� - ��������
  NavArea_Building: IsExcluded  // ������ - ���������
```

**���������:** NPC ���� �� �������/���������, ������� �����, �� ������ � ������.

---

### ������ 2: �������� �� �������

```cpp
FilterClass: NQF_GuardPatrol
  NavArea_Road: 0.5
  NavArea_Grass: 5.0  // �����, �� ������������
```

**���������:** ������������ ������, �� ����� ������� ����� �����.

---

### ������ 3: ������ ������

```cpp
FilterClass: NQF_Monster
  NavArea_Forest: 0.5  // ������������ ���
  NavArea_Road: 10.0   // �������� �����
```

**���������:** ���������� � ����, �������� �����������.

---

## ����������� ������

### ��� ������������:

1. **FilterClass** - �������� � `MoveToLocation` Action
2. **NavigationQueryFilter** - Data Asset � �����������
3. **Nav Modifier Volume** - �������� ����� � ������
4. **NavArea** - ���� �������� (Road, Grass, Water � �.�.)

### ��� ��������:

```
MoveToLocation Action
  ?
FilterClass (NavigationQueryFilter)
  ?
NavMesh � ��������� (Road = 0.5, Grass = 100.0)
  ?
AI pathfinding �������� ������� ����
  ?
NPC ���� �� ������! ????????
```

### Performance:

- ? ����������� ��������� �������
- ? ����������� �������� � UE
- ? �������������� �� 100+ NPC

---

## ���������� ��������

### ������� P � PIE:

```
���������:
    ??   ??
  [?? ����� ??]
  
???[?? ������ ??]??? ? NPC ���� �����
  
  [?? ����� ??]
    ??      ??

�����������:
    ??   ??
  [?? ����� ??]
      ? NPC �������
???[?? ������ ??]??? ? ������ ����� (�� Road)
      ?
  [?? ����� ??]
```

**�����:**
- ?? ������ = NavArea_Road (���������!)
- ?? ����� = NavArea_Default
- ?? ������� = NavArea_Grass

---

## Troubleshooting

### ? NPC ���� �������� (�� �� ������)

**���������:**
1. `Filter Class` ���������� � MoveToLocation?
2. `bUsePathfinding = true`?
3. Nav Modifier Volume ��������?
4. NavMesh ����������? (Build ? Build Paths)

**�������:** �������� [Troubleshooting](Docs/ROAD_NAVIGATION_RU.md#������-��������)

---

### ? NPC �� ������� ����

**�������:** ���� ��� ������ ��� ������ �� ���������.

**�������:**
1. ��������� ���� �� ������ (������ ����)
2. ���: ����������� `Cost` ������ `IsExcluded`

---

## ��������� ����

### ��� ���:

1. ? ���������� [Quick Start](Docs/ROAD_QUICK_START_RU.md) (5 �����)
2. ? ���������� ��������� � ����� �������
3. ? ��� ��������� �������� [Troubleshooting](Docs/ROAD_NAVIGATION_RU.md#�������)

### �������������:

- ������� [������ �����������](Docs/ROAD_NAVIGATION_RU.md) ��� ����������� ���������
- ���������� [C++ �������](Docs/ROAD_NAVIGATION_CPP.md) ���� ��������� � �����
- ����������� [���������� �����������](Docs/ROAD_VISUAL_GUIDE_RU.md) ��� �������

---

## ����� ��� ��������

### ������� �����:
1. **[ROAD_NAVIGATION_INDEX.md](Docs/ROAD_NAVIGATION_INDEX.md)** ? **START HERE**
2. [ROAD_QUICK_START_RU.md](Docs/ROAD_QUICK_START_RU.md) (5 �����)

### �����������:
3. [ROAD_NAVIGATION_RU.md](Docs/ROAD_NAVIGATION_RU.md) (������ �����������)
4. [ROAD_VISUAL_GUIDE_RU.md](Docs/ROAD_VISUAL_GUIDE_RU.md) (�����)
5. [ROAD_NAVIGATION_CPP.md](Docs/ROAD_NAVIGATION_CPP.md) (C++)

### Changelog:
6. [CHANGELOG_v1.0_RoadNavigation.md](CHANGELOG_v1.0_RoadNavigation.md)

---

## �����

? **����������� �������������:** ��, ���������!  
? **������������ �������:** 5 ������, ~30 �������  
? **������� ���������:** �����, �������, sci-fi  
? **Troubleshooting:** ������  
? **��� ���� �������:** �� ������� �� ��������

**������ � �������������! ??**

---

**�����:** EchoesOfAffection Team  
**����:** 2025-01-XX  
**������:** 1.0
