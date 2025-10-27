# ?? ��������� �������: MusicZoneVolume ������ �����!

## ? ��������

MusicZoneVolume ��� **�������** � ���������, ���� � ���������� `Show ? Volumes`. ��� ����� ������ gizmo (������� X,Y,Z).

---

## ??? �������� ������������

### ��� �� �������:

1. ? ������� **`AAudioVolume`** - �����, ��������
2. ? ������� **`AVolume`** - ������� ����� ��� ���� Volumes
3. ? ������� **`ABrush`** - �������� `AVolume`
4. ?? **����� ��������!**

---

## ?? ��������� ��������: `SetHidden(true)` � ABrush

### � �������� ���� UE - `ABrush::ABrush()`:

```cpp
ABrush::ABrush(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    BrushComponent = CreateDefaultSubobject<UBrushComponent>(TEXT("BrushComponent0"));
    // ... setup ...
    
    // ? ��� ��������:
    SetHidden(true); // �������� Brush �� ���������!
}
```

### ������ ��� ��������?

**`AMusicZoneVolume`** ����������� �� **`AVolume`**, ������� ����������� �� **`ABrush`**:

```
AActor ? ABrush ? AVolume ? AMusicZoneVolume
```

����� ���������� ����������� `ABrush`, �� ������������� `SetHidden(true)`, � ��� **�� ����������������** �� � `AVolume`, �� � ����� `AMusicZoneVolume`!

---

## ? �������: Override SetHidden � ������������

```cpp
AMusicZoneVolume::AMusicZoneVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // ... ��� ��������� ��������� ...

    // ? ��������: Override SetHidden(true) �� ABrush
    SetHidden(false);
}
```

**��� � ��!** ���� ������ ������ ��������!

---

## ?? ���������: �� � �����

| ������ | �� | ����� |
|--------|-----|-------|
| **SetHidden** | `true` (�� ABrush) | `false` (override) |
| **���������** | ? ������� | ? ����� |
| **Show ? Volumes** | ? �� �������� | ? �������� |

---

## ?? ��������� ��� ������������

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

    // Collision setup
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

    // ������������
    bColored = true;
    BrushColor = FColor(100, 200, 255, 255);
    
    // ? �������: Override SetHidden �� ABrush
    SetHidden(false);
    
#if WITH_EDITORONLY_DATA
    bActorLabelEditable = true;
#endif
}
```

---

## ?? �������� ������

### ���� 1: �������� � ���������

```
1. �������� �������� UE
2. Show ? Volumes ? ?
3. Content Browser ? Blueprint Class ? MusicZoneVolume
4. �������� BP_MusicZone_Test
5. ���������� �� �������
6. ���������: ������ ������� BOX! ?
```

### ���� 2: �������� ���������

```
1. �������� ����
2. Details ? Priority: 10 ? �������
3. Priority: 50 ? ����-����������
4. Priority: 100 ? �����-����������
5. Is Active: false ? �������
```

---

## ?? ������ ��� ��������?

### �������� ������� ������������:

```cpp
1. AActor::AActor() ? ������� �����
2. ABrush::ABrush() ? SetHidden(true) ?
3. AVolume::AVolume() ? ��������� SetHidden(true)
4. AMusicZoneVolume::AMusicZoneVolume() ? SetHidden(false) ?
```

**��������� ���������:** `Hidden = false` ? **Volume �����!**

---

## ?? �������� �������

### 1. SetHidden vs SetActorHiddenInGame

| ������� | �������� | ���� |
|---------|----------|------|
| `SetHidden(bool)` | ? ������ | ? ������ |
| `SetActorHiddenInGame(bool)` | ? �� ������ | ? ������ |

**�����:** ��� ��������� � ��������� ����� `SetHidden(false)`!

### 2. ������ ABrush �����?

**�����:** ������� Brushes ������������ ��� BSP ��������� � ���������� ��� ����� Level Mesh, ������� ���� ������ ������ ���� ������.

### 3. ������ Volumes ������ ���� ������?

**�����:** Volumes - ��� ����������� ������ (��������, ����). Level ���������� ����� �� ������ ��� ������!

---

## ? �����

**��������:** `ABrush::SetHidden(true)` ������� Volume  
**�������:** `AMusicZoneVolume::SetHidden(false)` override  
**���������:** Volume **�����** � ���������! ??

---

**����:** 23 ������� 2025  
**������:** 6.0 (�������� �������)  
**������:** ? 100% ��������!
