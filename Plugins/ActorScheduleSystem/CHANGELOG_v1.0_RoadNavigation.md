# CHANGELOG - Road Navigation Documentation

## v1.0 - 2025-01-XX

### ? ������������ ���������

������ ����� ���������� �� ������������� `FilterClass` ��� �������� NPC ������ �� �������.

#### ��������� �����:

1. **[ROAD_NAVIGATION_INDEX.md](Docs/ROAD_NAVIGATION_INDEX.md)**
   - ����������� ������ ���� ������������
   - ������� ��������� �� ������
   - Checklist � FAQ

2. **[ROAD_QUICK_START_RU.md](Docs/ROAD_QUICK_START_RU.md)** ??
   - ��������� �� 5 �����
   - ������ ����� �����������
- ��� ��������

3. **[ROAD_NAVIGATION_RU.md](Docs/ROAD_NAVIGATION_RU.md)** ??
   - ������ ��������� ����������� (20+ �������)
   - ��������� ���������
   - ����������� �������� (�����, �������, sci-fi)
   - Troubleshooting
   - API Reference

4. **[ROAD_VISUAL_GUIDE_RU.md](Docs/ROAD_VISUAL_GUIDE_RU.md)** ??
   - ���������� ����� � "���������"
   - ASCII-��������� ��������
   - ���������� �������
   - Checklist � ����������

5. **[ROAD_NAVIGATION_CPP.md](Docs/ROAD_NAVIGATION_CPP.md)** ??
   - C++ ������� �������� NavArea
   - ����������� �������� ��������
   - ������������ ���������� ����������
   - Unit �����
   - ����������� �������

#### ����������� �����:

- **[README.md](README.md)**
  - ��������� ������ "NavArea Costs for Roads"
  - ��������� ������ �� ��� ������������
  - ���������� ������ � ������� NavMesh

---

## ��� ������� �������������?

### 1. ������ (Quick Start)

? **�������� Navigation Query Filter** (Data Asset)  
? **�������� �����** (Nav Modifier Volume)  
? **���������� �������** (FilterClass � MoveToLocation)  
? **���������� ��������** (Press P)

**�����:** 5 �����  
**�������:** �������  
**����:** [ROAD_QUICK_START_RU.md](Docs/ROAD_QUICK_START_RU.md)

---

### 2. ��������� ����������� (Complete Guide)

? **�������� NavArea �������** (C++ �����������)  
? **��������� ��������** (���������, ����������)  
? **�������� ������** (Nav Modifier Volumes)  
? **���������� � Schedule System**  
? **�������** (Visual Logger, AI Debug)  
? **����������� ���������** (������������� �������, ������������ �����)  
? **������� ���������** (�����, �������, sci-fi)
? **Troubleshooting** (������ �������� � �������)  
? **Performance** (�����������)  
? **API Reference**

**�����:** 20+ �������  
**�������:** ��� ������  
**����:** [ROAD_NAVIGATION_RU.md](Docs/ROAD_NAVIGATION_RU.md)

---

### 3. ���������� ����������� (Visual Guide)

? **ASCII-���������** �������� Data Assets  
? **�����** ���������� Nav Modifier Volumes  
? **������������** ������ NavMesh  
? **����� �����** NPC  
? **Layout �������** (������, �����������, ������)  
? **���������� troubleshooting**  
? **Checklist � ����������**

**�����:** 10+ ����  
**�������:** ���������� ��������  
**����:** [ROAD_VISUAL_GUIDE_RU.md](Docs/ROAD_VISUAL_GUIDE_RU.md)

---

### 4. C++ ������� (For Programmers)

? **NavArea ������** (Road, Grass, Water)  
? **NavigationQueryFilter** (����������� ��������)  
? **������������ ���������� ���������**  
? **������������� ������ ���������** (RoadOnly, OffRoad, AvoidRoads)  
? **����������� �������� Nav Modifier Volumes**  
? **Debug ������������** �����  
? **Unit �����**

**�����:** 15+ �������� ����  
**�������:** C++ �����������  
**����:** [ROAD_NAVIGATION_CPP.md](Docs/ROAD_NAVIGATION_CPP.md)

---

## �������� ���� ������������

### ?? ������������� ������ ���������

- **�������:** ������� ����� (5 �����)
- **�������:** ���������� ����������� (�����)
- **�����������:** ������ ����������� (20+ �������)
- **�������:** C++ ������� (���)

### ?? ������ ��������

- ? �� �������� � ��������
- ? Blueprint � C++ �������
- ? ��� ��������� ��������
- ? Troubleshooting ��� ������� ����

### ?? ����

- **������� (RU):** ������ ������������
- **English:** README.md (�������)

### ?? ������������

- ASCII-��������� ��������
- ����� NavMesh ������
- Layout �������
- Code snippets

### ?? ������������ �������

1. **��������� �����** - ������, ��������, ������
2. **������� ���** - ��������, ���, ����
3. **Sci-Fi �������** - ��������, �������, ����������

---

## Use Cases

### ? ��� ������ ����� ������:

1. **NPC ����� ������ �� �������** ???
   - �� ������� ����� �����
   - ������� �����������
   - ������������ ������ ���� ���� ���� �������

2. **������ ���� NPC - ������ ���������**
   - �������� ? ������ ������ � ��������
   - �������� ? ����� ���� ����� ����� � �������
   - ������ ? �������� �����, ������������ ���

3. **������������ ����� �������**
   - ������� ����� ? ������
   - ������ ? ������������ ������
   - ������� ? ����� �������

4. **����������� ���������**
   - ������������� �������
   - ����������� �������� �����
   - Runtime ��������� NavMesh

---

## ������� ��������

### ������ 1: �������� (������ ������)

```
NavigationQueryFilter: NQF_MerchantStrict
  NavArea_Road: 0.5
  NavArea_Sidewalk: 0.6
  NavArea_Grass: IsExcluded = true
  NavArea_Building: IsExcluded = true
```

**���������:** ����� ������ �� ������� � ���������.

---

### ������ 2: �������� (����� �������)

```
NavigationQueryFilter: NQF_GuardPatrol
  NavArea_Road: 0.5
  NavArea_Grass: 5.0  ? �����, �� ������������
  NavArea_Building: IsExcluded = true
```

**���������:** ������������ ������, �� ����� ������ ����� �����.

---

### ������ 3: ������ (�������� �����)

```
NavigationQueryFilter: NQF_MonsterWild
  NavArea_Forest: 0.5 ? ������������ ���!
  NavArea_Road: 10.0  ? �������� �����
  NavArea_Water: IsExcluded = true
```

**���������:** ���������� � ����, �������� �����.

---

## ����������� ������

### �������������� ����

? **Navigation Query Filter** - ������ ���������  
? **NavArea ��������** - ����� Nav Modifier Volumes  
? **������������ ����� ��������** - � runtime  
? **������������� �������** - ��� ������ NPC  
? **����������� ��������** - ����� C++ API  
? **������������** - ����� NavMesh (Press P)  
? **�������** - AI Debug, Visual Logger

### �������������

- ? **Unreal Engine 5.4+**
- ? **Actor Schedule System v1.0+**
- ? **Blueprint � C++**

### Performance

- **��������� �������:** �����������
- **�����������:** ���������� � UE
- **���������������:** �� 100+ NPC ��� �������

---

## ������ � �������� �����

### �������� � ��������

- **GitHub Issues:** https://github.com/shardanow/EchoesOfAffection/issues
- **Email:** support@echoesofaffection.com

### ���������� ���������

���� �� ������ ������:
- �������������� �������
- ������ ����� ������������
- �����-���������

��������� � ����!

---

## ��������� ����

### ��� ��������:

1. ���������� [Quick Start](Docs/ROAD_QUICK_START_RU.md)
2. �������� ����� 1-4
3. ��������� ��������� (Press P)

### ��� �������:

1. ������� [Complete Guide](Docs/ROAD_NAVIGATION_RU.md)
2. ���������� ����������� ��������
3. ����������������� � �����������

### ��� �������������:

1. ������� [C++ Examples](Docs/ROAD_NAVIGATION_CPP.md)
2. �������� ����������� NavArea ������
3. ������������ � ����� �����

---

## Credits

**����� ������������:** EchoesOfAffection Team  
**���� ��������:** 2025-01-XX  
**������:** 1.0  
**��������:** MIT (������������), ��. LICENSE � ����� �������

---

## ��. �����

### ������������ Actor Schedule System:

- [README.md](../README.md) - ������� ������������
- [QUICKSTART.md](../QUICKSTART.md) - ������� ����� �������
- [FEATURE_Loop_Child_Actions.md](FEATURE_Loop_Child_Actions.md) - ������������ �������
- [INTEGRATION_RU.md](../INTEGRATION_RU.md) - ���������� � ������� ���������

### Unreal Engine ������������:

- [Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [NavArea](https://docs.unrealengine.com/5.0/en-US/API/Runtime/AIModule/Navigation/UNavArea/)
- [NavigationQueryFilter](https://docs.unrealengine.com/5.0/en-US/API/Runtime/NavigationSystem/NavFilters/UNavigationQueryFilter/)

---

**������������ ������! ??**

�������� ������ ����������� �� [ROAD_NAVIGATION_INDEX.md](Docs/ROAD_NAVIGATION_INDEX.md) � ��������� ������!
