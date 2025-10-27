# ?? �������: ������ MusicZoneVolume �� ��� ����� � ���������

## ? ��������

Volume ����������, �� ��� **�������** � ��������� ���� � ���������� `Show ? Volumes`. ��� ����� ������ gizmo (������� X,Y,Z).

---

## ??? ������������

### ��� �� �������:

1. ������� �������� ��� **`AAudioVolume`** �� Unreal Engine
2. �������� � ����� ����������� **`AMusicZoneVolume`**
3. ����� ����������� �������

---

## ?? ��������� ������

### ? ��� ��� (�����������):

```cpp
UBrushComponent* BrushComp = GetBrushComponent();
if (BrushComp)
{
 BrushComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BrushComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    BrushComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BrushComp->SetGenerateOverlapEvents(true);
    
    // ? ��� ��������:
    BrushComp->SetHiddenInGame(true); // �������� brush!
}
```

### ? ��� AudioVolume (���������):

```cpp
AAudioVolume::AAudioVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->bAlwaysCreatePhysicsState = true;

  bColored = true;
    BrushColor = FColor(255, 255, 0, 255);  // ������, ������������

    bEnabled = true;
  
    // ? ��� SetHiddenInGame!
}
```

---

## ?? ����������

### ��� ������ `SetHiddenInGame(true)`:

```cpp
void UPrimitiveComponent::SetHiddenInGame(bool bNewHidden, bool bPropagateToChildren)
{
    if (bHiddenInGame != bNewHidden)
    {
        bHiddenInGame = bNewHidden;
  
   // ? ��������: �������� ��������� ��� �������
   // ��� ������ �� ������ �� ����, �� � �� ��������!
    MarkRenderStateDirty();
    }
}
```

**��������:** `bHiddenInGame = true` **��������** Brush Component, � �� ���������� ��������� ���� ��� `Show ? Volumes`!

---

## ? ���������� �������

### ����� ����������� (��������� ������):

```cpp
AMusicZoneVolume::AMusicZoneVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ZoneMusic = nullptr;
    Priority = 10;
    TransitionType = EMusicTransitionType::Crossfade;
    CrossfadeDuration = 2.0f;
    VolumeMultiplier = 1.0f;
    bIsActive = true;
    bPlayerOnly = true;
    bShowDebugInfo = false;

    // ? Collision setup (����� ��� � AudioVolume)
    UBrushComponent* BrushComp = GetBrushComponent();
    if (BrushComp)
    {
   BrushComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
        BrushComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        BrushComp->SetCollisionResponseToAllChannels(ECR_Ignore);
  BrushComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        BrushComp->SetGenerateOverlapEvents(true);
     BrushComp->bAlwaysCreatePhysicsState = true;
  }

    // ? ������������ (����� ��� � AudioVolume)
    bColored = true;
    BrushColor = FColor(100, 200, 255, 255); // �������, ������������
    
    // ? ������: SetHiddenInGame(true)
    // ? Brush Component ������� �������!
    
#if WITH_EDITORONLY_DATA
    bActorLabelEditable = true;
#endif
}
```

---

## ?? �������� ���������

### 1. ������ `SetHiddenInGame(true)`

**����:**
```cpp
BrushComp->SetHiddenInGame(true); // ? �������� brush
```

**�����:**
```cpp
// ? ������ �� �������� SetHiddenInGame
// Brush Component ������� ������� �� ���������
```

### 2. �������� `SetCollisionProfileName`

**����:**
```cpp
// ������
```

**�����:**
```cpp
BrushComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
```

**�����:** ������������� ���������� ������� ��������, ��� � AudioVolume.

### 3. �������� `bAlwaysCreatePhysicsState`

**����:**
```cpp
// ������
```

**�����:**
```cpp
BrushComp->bAlwaysCreatePhysicsState = true;
```

**�����:** ����������� �������� ����������� ��������� ��� overlap detection.

### 4. �������� ������������ `BrushColor`

**����:**
```cpp
BrushColor = FColor(100, 200, 255, 128); // Alpha = 128 (��������������)
```

**�����:**
```cpp
BrushColor = FColor(100, 200, 255, 255); // Alpha = 255 (������������)
```

**�����:** ������ ���� ����� ������� � ���������.

---

## ?? ���������: �� � �����

| ������ | �� (�� ��������) | ����� (��������) |
|--------|------------------|-------------------|
| **��������� � ���������** | ? ������� | ? ����� |
| **Show ? Volumes** | ? �� �������� | ? �������� |
| **SetHiddenInGame** | ? true | ? �� �������� |
| **BrushColor Alpha** | ? 128 (��������������) | ? 255 (������������) |
| **Collision Profile** | ? �� ���������� | ? NoCollision_ProfileName |
| **bAlwaysCreatePhysicsState** | ? false | ? true |

---

## ?? ��� ���������, ��� ��������

### 1. �������� ��������

```
1. ��������� Unreal Editor
2. �������� ����� �������
```

### 2. �������� ����������� Volumes

```
Viewport ? Show ? Volumes ? ?
```

### 3. �������� ����

```
Content Browser ? Blueprint Class ? MusicZoneVolume
?? Name: BP_MusicZone_Test
```

### 4. ���������� �� ������

```
���������� BP_MusicZone_Test �� �������
```

### 5. ��������� ���������

**? �� ������ ������:**
- ������� �������������� box
- ������� ���� ����� ����������
- ��� ������ - ��������������

**? ���� ������ ������ gizmo:**
- ���������, ��� Show ? Volumes ��������
- ���������, ��� BrushColor �� ��������� (Alpha = 255)
- ���������������� ������

---

## ?? ������������

### ���� 1: ������� ���������

```
1. �������� BP_MusicZone
2. ���������� �� ������
3. Show ? Volumes ? ?
4. ���������: ������ ������� box ?
```

### ���� 2: ��������� ����������

```
1. �������� ����
2. Details ? Priority: 50
3. ���������: ���� ��������� ?
```

### ���� 3: �����������

```
1. Details ? Is Active: false
2. ���������: ���� ����� ������� ?
```

---

## ?? ��� �� ������

### 1. `SetHiddenInGame(true)` - ������!

```cpp
// ? �� ������� ��� ��� Volumes:
BrushComp->SetHiddenInGame(true);

// ? ������ �������� �������� �� ��������� (false)
```

**������:** `bHiddenInGame` ������ �� ��������� ����������, ���� � ���������!

### 2. ��������� ������ ����������� ������� UE

```
AudioVolume = ������ ��� Volume �������
?? Collision setup
?? BrushColor setup
?? ��� SetHiddenInGame
?? bAlwaysCreatePhysicsState = true
```

### 3. BrushColor Alpha �����

```cpp
// ? ������� ���������� - ����� �����:
BrushColor = FColor(R, G, B, 50);

// ? ������������ - ������ �����:
BrushColor = FColor(R, G, B, 255);

// ?? ������� �������:
BrushColor = FColor(R, G, B, 200);
```

---

## ?? ������

### �������� ���� � ����� ������:

```cpp
BrushComp->SetHiddenInGame(true); // ? ��� ������ �������� ���� Volume!
```

### ������� - ������ �:

```cpp
// ? ������ �� �������� SetHiddenInGame
// Brush Component ������� �������
```

### ������ �������� �� ����������� ������ UE:

```
���� AudioVolume �������� ? ��������� ��� ������!
�� ����������� ��������� ? ����������� ����������� �������!
```

---

## ? �����

**��������:** `SetHiddenInGame(true)` ������� Brush Component  
**�������:** ������ �����, ����������� ������ AudioVolume  
**���������:** MusicZoneVolume ������ **��������� �����** � ���������! ??

---

**����:** 23 ������� 2025  
**������:** 5.0 (��������� �����������)  
**������:** ? ��������!
