# Road Navigation - Documentation Index

## ?? �������� ���������� �����������

### ��� �������� (Blueprint/Data Assets)

1. **[������� ����� (5 �����)](ROAD_QUICK_START_RU.md)** ??
   - ����������� ��������� ��� ������ ������
   - ������ ����� �����������
   - Blueprint/Data Asset ������

2. **[���������� �����������](ROAD_VISUAL_GUIDE_RU.md)** ??
   - ��������� ���������
   - ����� � ���������
   - ���������� �������

### ��� ������� �������������

3. **[������ ����������� (RU)](ROAD_NAVIGATION_RU.md)** ??
   - ��������� �������� ���� ������������
   - ����������� ���������
   - ������� ��� ������ ���������
   - Troubleshooting

### ��� ������������� (C++)

4. **[C++ Examples](ROAD_NAVIGATION_CPP.md)** ??
   - �������� NavArea �������
   - ����������� �������� ��������
   - ������������ ���������� ����������
   - Unit �����

---

## ?? ������� ����������

### ��� ����� Road Navigation?

�������, ����������� NPC ��������� **������ �� �������**, ��������� ���������� ���� ����� �����/����.

**�������� �����:**
- `FilterClass` � `MoveToLocation` Action
- `NavigationQueryFilter` Data Asset
- `NavArea` �������� � ������ (������, �����, ���� � �.�.)

---

## ?? �������� ����������

### 1. NavigationQueryFilter (������)

**���:** Data Asset, ������������ ��������� ������� �� ������ �������� NavMesh

**��� �������:**
```
Content Browser ? Right-click ? Miscellaneous ? Data Asset
? NavigationQueryFilter
```

**���������:**
```
Areas:
  NavArea_Road: Cost = 0.5 (������ - AI ������������)
NavArea_Grass: Cost = 100.0 (������ - AI ��������)
```

**������������:**
- [������� ����� - ��� 1](ROAD_QUICK_START_RU.md#1-��������-navigation-query-filter)
- [������ ����������� - ��� 2](ROAD_NAVIGATION_RU.md#���-2-��������-navigation-query-filter)
- [C++ Example](ROAD_NAVIGATION_CPP.md#roa-dnavigationfiltercpp)

---

### 2. Nav Modifier Volume (�������� �����)

**���:** Actor, ����������� ������� NavMesh ��� "������"

**��� �����:**
```
Place Actors Panel ? Volumes ? Nav Modifier Volume
```

**���������:**
- ���������� ��� �������
- �������������� ����� ������� ������
- `Area Class`: `NavArea_Road`

**������������:**
- [������� ����� - ��� 2](ROAD_QUICK_START_RU.md#2-���������-������-�-������)
- [������ ����������� - ��� 3](ROAD_NAVIGATION_RU.md#���-3-���������-navmesh-�-������)
- [���������� ����������� - ��� 2](ROAD_VISUAL_GUIDE_RU.md#���-2-��������-�����-�-������)

---

### 3. FilterClass � MoveToLocation

**���:** �������� � `MoveToLocation` Action, ����������� ������

**��� ���������:**
```
Schedule Entry ? Action: MoveToLocation ? Filter Class
```

**��������:** ��� ��������� `NavigationQueryFilter`

**������������:**
- [������� ����� - ��� 3](ROAD_QUICK_START_RU.md#3-���������-������-�-movetolocation)
- [������ ����������� - ��� 4](ROAD_NAVIGATION_RU.md#���-4-���������-movetolocation-action)
- [���������� ����������� - ��� 3](ROAD_VISUAL_GUIDE_RU.md#���-3-���������-movetolocation-action)

---

## ?? ���������� ��������

### ������� P � PIE

**����� NavMesh:**

| ���� | �������� | NavArea |
|------|----------|---------|
| ?? ������ | ������ (������ ���������) | `NavArea_Road` |
| ?? ����� | Default (������� ���������) | `NavArea_Default` |
| ?? ������� | ����� (������� ���������) | `NavArea_Grass` |
| ?? �����-����� | ���� (����� ������) | `NavArea_Water` |

**���������� �������:**

```
    ??      ??      ??
  [?? ����� ??]
  
???[?? ������ ??]??? ? NPC ���� �����
  
  [?? ����� ??]
    ??  ??    ??
```

**������������:**
- [���������� ����������� - ��� 4](ROAD_VISUAL_GUIDE_RU.md#���-4-����������-��������)

---

## ?? ������� ����� (TL;DR)

1. **�������� ������:**
   - Content Browser ? Data Asset ? NavigationQueryFilter
   - ���: `NQF_RoadOnly`
   - Areas: Road (0.5), Default (100.0)

2. **��������� ������:**
   - Place Actors ? Nav Modifier Volume
   - ��� �������, Area Class = `NavArea_Road`
 - Build ? Build Paths

3. **��������� � Action:**
   - Schedule Entry ? MoveToLocation
   - Filter Class = `NQF_RoadOnly`

4. **���������:**
   - PIE ? ������ P
   - ������ = ?? ������
   - NPC ���� �� ������ ?

**������ ����:** [������� �����](ROAD_QUICK_START_RU.md)

---

## ?? Troubleshooting

### ��������: NPC ���� ��������

**�������:**

1. ��������� `FilterClass` � MoveToLocation ? **����� ������ �������!**
2. `bUsePathfinding = true`?
3. Nav Modifier Volume ��������?
4. NavMesh ����������? (Build ? Build Paths)

**���������:** [Troubleshooting](ROAD_NAVIGATION_RU.md#������-��������)

---

### ��������: NPC �� ������� ����

**�������:**

1. ���� �� ������ (������ ����)?
2. ����������� `Cost` ������ `IsExcluded`
3. ������ ��������� ����� � ����?

**���������:** [Troubleshooting](ROAD_NAVIGATION_RU.md#?-npc-����������--��-�����-�����-����)

---

## ?? ������� ��������

### ��������� �����

```
NavArea_Road: 0.5 (������)
NavArea_Sidewalk: 0.7 (��������)
NavArea_Park: 5.0 (����� - �����, �� ������������)
NavArea_Building: IsExcluded = true (������ - ���������)
```

**���������:** NPC ������������ ������, ����� ���� �� ��������, �������� �����, �� ����� ����� � ������.

---

### ������� ���

```
NavArea_Path: 0.5 (��������)
NavArea_Forest: 10.0 (��� - ������)
NavArea_Water: IsExcluded = true (���� - ����������)
```

**���������:** NPC ���� �� ���������, ������� ���, �� ����� �������.

---

### Sci-Fi �������

```
NavArea_Corridor: 0.5 (��������)
NavArea_Room: 1.0 (�������)
NavArea_Vent: IsExcluded = true (���������� - ������ ��� ������)
```

**���������:** NPC ��������� �� ���������, ����� �������� � �������, �� ����� � ����������.

**������ ��������:** [������ ����������� - �������](ROAD_NAVIGATION_RU.md#�������-�������������)

---

## ?? ��� �������������

### �������� NavArea

```cpp
UCLASS()
class UNavArea_Road : public UNavArea
{
	GENERATED_BODY()
public:
	UNavArea_Road() {
		DefaultCost = 1.0f;
		DrawColor = FColor::Yellow;
	}
};
```

### �������� �������

```cpp
URoadNavigationFilter* Filter = NewObject<URoadNavigationFilter>();

FNavigationFilterArea Area;
Area.AreaClass = UNavArea_Road::StaticClass();
Area.bOverrideTravelCost = true;
Area.TravelCostOverride = 0.5f;
Filter->Areas.Add(Area);
```

### ���������� � Action

```cpp
MoveAction->FilterClass = URoadNavigationFilter::StaticClass();
```

**������ �������:** [C++ Examples](ROAD_NAVIGATION_CPP.md)

---

## ?? �������������� �������

### ����������� ������������ Unreal Engine

- [Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [NavArea](https://docs.unrealengine.com/5.0/en-US/API/Runtime/AIModule/Navigation/UNavArea/)
- [NavigationQueryFilter](https://docs.unrealengine.com/5.0/en-US/API/Runtime/NavigationSystem/NavFilters/UNavigationQueryFilter/)

### ������������ Actor Schedule System

- [README.md](../README.md) - ������� ������������
- [QUICKSTART.md](../QUICKSTART.md) - ������� ����� ������� ����������
- [INTEGRATION_RU.md](../INTEGRATION_RU.md) - ���������� � ������� ���������

---

## ? Checklist ����� ��������

��������� ����� ��� ��� ��������� ����:

- [ ] **NavigationQueryFilter ������** (��������, `NQF_RoadOnly`)
- [ ] **Areas ���������** (Road = 0.5, Default = 100.0)
- [ ] **Nav Modifier Volumes ���������** ��� ��������
- [ ] **Area Class** = `NavArea_Road` � ������ Volume
- [ ] **NavMesh ����������** (Build ? Build Paths)
- [ ] **FilterClass ����������** � MoveToLocation Action
- [ ] **bUsePathfinding = true** � MoveToLocation
- [ ] **��� ������� P ������ ������** ??
- [ ] **NPC ���� �� ������** ????????

**��� ������� �����? ������! ??**

---

## ?? �������� ������

### ������� ������

1. **[FAQ](ROAD_NAVIGATION_RU.md#������-��������)** - ������ �������
2. **[Troubleshooting](ROAD_NAVIGATION_RU.md#�������)** - ������� �������
3. **[���������� �����������](ROAD_VISUAL_GUIDE_RU.md#-troubleshooting-���������)** - ����� � ��������

### ��������

- **GitHub Issues:** https://github.com/shardanow/EchoesOfAffection/issues
- **Email:** [support@echoesofaffection.com](mailto:support@echoesofaffection.com)

---

**������:** 1.0  
**���������:** 2025-01-XX  
**�����:** EchoesOfAffection Team

---

## ?? ������� ���������

### v1.0 - Initial Release (2025-01-XX)

? **���������:**
- ��������� `FilterClass` � `MoveToLocation`
- ���������� � `NavigationQueryFilter`
- �������������� ���������� ������� ��� ��������

? **������������:**
- ������� ����� (RU)
- ������ ����������� (RU)
- ���������� ����������� (RU)
- C++ �������

? **�������:**
- ��������� �����
- ������� ���
- Sci-Fi �������

---

**�������� ���������� ����������� ���� � ���������! ??**
