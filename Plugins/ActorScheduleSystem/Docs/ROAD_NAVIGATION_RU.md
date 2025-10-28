# �������� ������ �� ������� - ������ �����������

## �����

� ������� `ScheduleAction_MoveToLocation` **��������� �������������� �������� ������ �� �������** ����� �������� **Navigation Query Filter** (������ ������������� ��������).

��� ��������� NPC ��������� ������ �� ������������ �������� NavMesh (��������, ������, ��������) � �������� ������ (�����, ����, ����������).

---

## ��� ��� ��������?

### �����������

```
Actor Schedule System
      ?
MoveToLocation Action
      ?
FilterClass (NavigationQueryFilter)
      ?
NavMesh + NavArea Costs
      ?
AI �������� ���� �� �������
```

### �������� ����������:

1. **NavMesh** - ����� ��������� � ������
2. **NavArea** - ������� NavMesh � ������� ���������� (������, �����, ����)
3. **NavigationQueryFilter** - ������, ������������ "���������" ������� �� ������ �������
4. **FilterClass** � `MoveToLocation` - ��������� ������ � ��������

---

## ��������� ���������

### ��� 1: �������� NavArea ������

#### 1.1. �������� NavArea ��� ������

**File ? New C++ Class ? Navigation Area ? Name: `NavArea_Road`**

```cpp
// NavArea_Road.h
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Road.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Road : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Road();
};
```

```cpp
// NavArea_Road.cpp
#include "NavArea_Road.h"

UNavArea_Road::UNavArea_Road()
{
	// ������ ��������� - AI ������������ ������
	DefaultCost = 1.0f;
	FixedAreaEnteringCost = 0.0f;
	DrawColor = FColor::Yellow; // ��� �������
	bIsMetaArea = false;
}
```

#### 1.2. �������� NavArea ��� �����/����������

```cpp
// NavArea_Grass.h
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Grass.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Grass : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Grass();
};
```

```cpp
// NavArea_Grass.cpp
#include "NavArea_Grass.h"

UNavArea_Grass::UNavArea_Grass()
{
	// ������� ��������� - AI �������� �����
	DefaultCost = 10.0f;  // � 10 ��� ������!
	FixedAreaEnteringCost = 0.0f;
	DrawColor = FColor::Green;
	bIsMetaArea = false;
}
```

### ��� 2: �������� Navigation Query Filter

**Content Browser ? Right-click ? Miscellaneous ? Data Asset ? NavigationQueryFilter**

��������: `NQF_RoadOnly` (Navigation Query Filter - Road Only)

#### ��������� �������:

1. **��������** `NQF_RoadOnly`
2. **� ������ Areas ��������:**

| Area Class | Travel Cost Override | Entering Cost Override | Is Excluded |
|------------|---------------------|------------------------|-------------|
| `NavArea_Road` | ? 0.5 | - | ? |
| `NavArea_Grass` | ? 100.0 | - | ? |
| `NavArea_Default` | ? 50.0 | - | ? |

**���������� ����������:**
- `0.5` - ������ (����� ������, AI ������� � ������ �������)
- `100.0` - ����� (����� ������, AI ����� ��������)
- `50.0` - Default (������, �� �� ��������� ��� �����)

**������������� - ������ ����������:**

���� ������ **��������� ���������** �������� ��� �����:

| Area Class | Is Excluded |
|------------|-------------|
| `NavArea_Grass` | ? true |
| `NavArea_Default` | ? true |

### ��� 3: ��������� NavMesh � ������

#### 3.1. �������� Nav Modifier Volume

1. **Place Actors Panel ? Volumes ? Nav Modifier Volume**
2. **����������** ��� �������
3. **�������������** ����� ������� ������

#### 3.2. ��������� Area Class

� ��������� **Nav Modifier Volume**:
- `Area Class`: `NavArea_Road`

��������� ��� ���� ����� � ������.

#### 3.3. ������������ NavMesh

1. **������� P** � ��������� ? ������� NavMesh
2. **Build ? Build Paths** (��� ������ �������� Nav Modifier Volume)
3. **���������**: ������ ������ ���� ������� (NavArea_Road), ��������� ������� (NavArea_Grass)

### ��� 4: ��������� MoveToLocation Action

#### � ����� Schedule Entry:

1. **��������** Schedule Entry Data Asset
2. **� Action ? MoveToLocation:**
   - `Filter Class`: ? �������� `NQF_RoadOnly`
   - `Acceptance Radius`: 100
   - `bUsePathfinding`: ? true

#### ������ � Blueprint/Data Asset:

```
Schedule Entry: "���� �� ������"
?? Time Condition: 08:00-09:00
?? Action: MoveToLocation
   ?? Filter Class: NQF_RoadOnly  ? ��� ���!
   ?? Location Selector: ActorTag (Workplace)
   ?? Acceptance Radius: 100
   ?? bUsePathfinding: true
```

---

## ������������ ������

### ��������: NPC ���� �� ������ ������ �� �������

#### 1. �������� Filter

**Content/Navigation/NQF_RoadOnly**:
```
Areas:
  [0] NavArea_Road:
      TravelCostOverride = 0.5
      bOverrideTravelCost = true
  
  [1] NavArea_Grass:
  TravelCostOverride = 1000.0  // ����� ���������
      bOverrideTravelCost = true
```

#### 2. �������� Schedule Entry

**Content/Schedules/Entries/DA_Entry_GoToWork**:
```
Entry Name: "���� �� ������"
Time Condition:
  Time Type: TimeRange
  Start Hour: 8
  Start Minute: 0
  End Hour: 9
  End Minute: 0

Action: MoveToLocation
  Filter Class: NQF_RoadOnly
  Location Selector: ActorTag
    Required Tags: Location.Workplace
    Selection Mode: Closest
  Acceptance Radius: 100
  bUsePathfinding: true
```

#### 3. ���������� �������

```
�������:
  ?? LocationMarker "���" (Tag: Location.Home)
  ?? LocationMarker "������" (Tag: Location.Workplace)
  ?? Nav Modifier Volume (��� �������, AreaClass: NavArea_Road)
  ?? NPC � ScheduleComponent
```

### ���������:

- ? NPC ���� **������ �� ������ ����** (������)
- ? NPC **�� �������** ����� �����
- ? ���� ������ �������, NPC ��� ����� ������� ��

---

## �������

### ������������ NavMesh

1. **������� P** � PIE (Play In Editor)
2. **����� ��������**:
   - ?? ������ = `NavArea_Road` (������)
   - ?? ������� = `NavArea_Grass` (�����)
   - ?? ����� = `NavArea_Default` (������� ����)

### �������� ����

1. **�������� AI Debug**:
   - Console: `showdebug AI`
   - ���: `ai.debug.nav.show 1`

2. **��������� ����**:
   - ����� ������ ���� **������ �� �������**
   - ���� ����� ���� �������� ? ������ �� ��������

### �����������

� `ScheduleAction_MoveToLocation.cpp` ��� ���� ����:

```cpp
if (FilterClass)
{
    MoveRequest.SetNavigationFilter(FilterClass);
    UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Using FilterClass: %s"), *FilterClass->GetName());
}
```

��������� Output Log �� ������� ���� ������.

---

## ����������� ���������

### ������ ������� ��� ������ NPC

#### ������: �������� vs ��������

**NQF_MerchantFilter** (������ ������):
```
NavArea_Road: Cost = 0.5
NavArea_Grass: Excluded = true
```

**NQF_GuardFilter** (����� ���� ����� ����� � �������):
```
NavArea_Road: Cost = 1.0
NavArea_Grass: Cost = 5.0  // �����, �� ������������
```

### ������������ ����� �������

```cpp
// � Blueprint ��� C++
void ASomeActor::SwitchToOffRoadMode()
{
UScheduleAction_MoveToLocation* MoveAction = GetMoveAction();
    MoveAction->FilterClass = NQF_OffRoad;
}
```

### ��������������� �������

**NQF_RoadAndSidewalk**:
```cpp
Areas:
  NavArea_Road: Cost = 0.5
  NavArea_Sidewalk: Cost = 0.6
  NavArea_Grass: Cost = 100.0
```

NPC ����� ������������ ������, �� ����� ������������ �������.

---

## ������ ��������

### ? NPC ��� ����� ���� ��������

**�������:**
1. `FilterClass` �� ���������� ? ��������� Data Asset
2. NavMesh �� �������� ? ������� P, ��������� �����
3. `bUsePathfinding = false` ? ��������!

**�������:**
```
MoveToLocation:
  ? FilterClass: NQF_RoadOnly
  ? bUsePathfinding: true
  ? Nav Modifier Volumes ���������
```

### ? NPC ���������� / �� ����� ����� ����

**�������:**
1. ���� ��������� **���** ������
2. ������ **������� �������** (��� ������� excluded)

**�������:**
- ����������� `TravelCostOverride` ������ `IsExcluded`
- ���: ���������� ���� �� ������

### ? NavMesh �� ������������ ������

**�������:**
1. **������������ NavMesh**: Build ? Build Paths
2. **��������� Nav Modifier Volume**: Area Class ����������?
3. **Restart Editor** (������ ��������)

---

## ������������������

### ��������� ����������

- ? **�����������** - ������ ����������� ������ ��� ������� ����
- ? **����������** �������
- ? **�� ������ �� FPS** ��� �������� ���������� NPC

### �����������

1. **����������� ������ Nav Modifier Volumes** - ����������� ����
2. **�� ���������� ������� �������** - 3-5 �������� ����������
3. **��������������� �������** ����� NPC

---

## ������� �������������

### 1. ��������� �����

```
NavArea_Road (0.5)        - ������
NavArea_Sidewalk (0.7)    - ��������
NavArea_Park (5.0) - ����� (�����, �� ������������)
NavArea_Building (excluded) - ������ (���������)
```

### 2. ������� ���

```
NavArea_Path (0.5)        - ��������
NavArea_Forest (10.0)     - ��� (������ ����)
NavArea_Water (excluded)  - ���� (���������� ������)
NavArea_Cliff (excluded)  - ����� (������)
```

### 3. Sci-Fi �������

```
NavArea_Corridor (0.5)    - ��������
NavArea_Room (1.0)        - �������
NavArea_Vent (excluded)   - ���������� (������ ��� ������)
NavArea_Airlock (excluded) - ���� (������)
```

---

## API Reference

### UScheduleAction_MoveToLocation

```cpp
/** Filter class for navigation (e.g., for road-only navigation) */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
TSubclassOf<class UNavigationQueryFilter> FilterClass;
```

**�������������:**
- ���������� � Data Asset ��� Blueprint
- ������ ����������� ������������� ��� ������ `MoveTo`

### NavigationQueryFilter

**�������� ��������:**

| �������� | ��� | �������� |
|----------|-----|----------|
| `Areas` | Array | ������ ��������������� ��� NavArea |
| `TravelCostOverride` | float | ��������� ������� |
| `EnteringCostOverride` | float | ��������� ����� � ������� |
| `bIsExcluded` | bool | ��������� ��������� ������� |

**C++ Example:**

```cpp
void AMyActor::SetupRoadFilter()
{
    UNavigationQueryFilter* Filter = NewObject<UNavigationQueryFilter>();
    
    // ������ - ������
 FNavigationFilterArea RoadArea;
    RoadArea.AreaClass = UNavArea_Road::StaticClass();
    RoadArea.bOverrideTravelCost = true;
    RoadArea.TravelCostOverride = 0.5f;
    Filter->Areas.Add(RoadArea);
  
    // ����� - ������
    FNavigationFilterArea GrassArea;
    GrassArea.AreaClass = UNavArea_Grass::StaticClass();
    GrassArea.bOverrideTravelCost = true;
    GrassArea.TravelCostOverride = 100.0f;
    Filter->Areas.Add(GrassArea);
    
    MoveAction->FilterClass = Filter->GetClass();
}
```

---

## ��. �����

- [Official UE Documentation: Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)
- [NavArea Documentation](https://docs.unrealengine.com/5.0/en-US/API/Runtime/AIModule/Navigation/UNavArea/)
- [README.md](../README.md) - ����� ������������ ������� ����������
- [QUICKSTART.md](../QUICKSTART.md) - ������� �����

---

## Changelog

### v1.0 - Initial Release
- ? ��������� `FilterClass` � `MoveToLocation`
- ? ���������� � Navigation Query Filter
- ? �������������� ���������� ������� ��� ��������

### v1.1 - Documentation
- ? ��������� ������ ������������
- ? ������� ��������� NavArea
- ? ����������� �� �������

---

**�����**: EchoesOfAffection Team  
**����**: 2025-01-XX  
**������**: 1.0
