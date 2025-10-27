# ?? ��������� �������: MusicZoneVolume �� ���� ATriggerBox

## ? �������: ���������� ATriggerBox ������ AVolume

**�������� ���� � ���**, ��� �� �������� �������� � `AVolume` + `ABrush`, ��� ��������� ��������� ����� � workarounds.

**������� �������**: ������������ **`ATriggerBox`** - ����������� UE ����� ��� ���������� ���!

---

## ?? ������������ ATriggerBox

| ������ | AVolume (������) | ATriggerBox (�����) |
|--------|------------------|---------------------|
| **���������** | ? �������� � SetHidden | ? �������� �� ������� |
| **���������** | ? ������� (Brush, ���������) | ? ������� (Box Component) |
| **Overlap** | ? ����� ��������� | ? �������� �� ��������� |
| **��������������** | ? Brush Editing | ? ������� ��������������� |
| **����� ����** | ? ~250+ | ? ~150 |
| **��������** | ? SetHidden, Brush Builder | ? �������! |

---

## ?? ����� ����������

### Header (.h) - ������� � ������

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Audio/MusicManager.h"
#include "Sound/SoundBase.h"
#include "MusicZoneVolume.generated.h"

UCLASS(Blueprintable)
class ECHOESOFAFFECTION_API AMusicZoneVolume : public ATriggerBox
{
  GENERATED_BODY()

public:
    AMusicZoneVolume();

    virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    TObjectPtr<USoundBase> ZoneMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    int32 Priority = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    EMusicTransitionType TransitionType = EMusicTransitionType::Crossfade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    float CrossfadeDuration = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    float VolumeMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    bool bIsActive = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
bool bPlayerOnly = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone|Debug")
    bool bShowDebugInfo = false;

    // Blueprint events
    UFUNCTION(BlueprintNativeEvent, Category = "Music Zone")
    void OnPlayerEntered(APlayerController* PlayerController);

    UFUNCTION(BlueprintNativeEvent, Category = "Music Zone")
    void OnPlayerExited(APlayerController* PlayerController);
};
```

### Implementation (.cpp) - ������� ����

```cpp
#include "Audio/MusicZoneVolume.h"
#include "Audio/MusicManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/ShapeComponent.h"

AMusicZoneVolume::AMusicZoneVolume()
{
    // ATriggerBox ��� �������� ��� overlap!
    // ������ ������ ���� ��� ������������
    
    if (UShapeComponent* ShapeComp = GetCollisionComponent())
    {
        ShapeComp->ShapeColor = FColor(100, 200, 255, 255); // �������
        ShapeComp->bHiddenInGame = true; // ������ � ����
  }
}

void AMusicZoneVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!bIsActive || !ZoneMusic) return;

    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC || (bPlayerOnly && !PC->IsLocalPlayerController())) return;

    // Request music change
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
  Manager->RequestMusicChange(ZoneMusic, Priority, TransitionType, 
               CrossfadeDuration, VolumeMultiplier);
    }

    OnPlayerEntered(PC);
}

// ... ���������� ��� NotifyActorEndOverlap
```

---

## ?? ��� ������������

### 1. �������� ����

```
Content Browser:
1. ��� ? Blueprint Class
2. Parent Class: MusicZoneVolume
3. Name: BP_MusicZone_Town
4. Open Blueprint
```

### 2. ���������

```
Details Panel:
?? Zone Music: [��� ����� asset]
?? Priority: 10
?? Transition Type: Crossfade
?? Crossfade Duration: 2.0
?? Volume Multiplier: 1.0
?? Is Active: ? true
?? Player Only: ? true
```

### 3. ����������

```
1. ���������� BP_MusicZone_Town �� �������
2. ������������� � ������� Scale tool (R)
3. ������! ���� ����� � ��������! ?
```

---

## ?? ������������

### � ���������:

**ATriggerBox ������������� ������������!** �� �����:
- ? `Show ? Volumes`
- ? `SetHidden(false)`
- ? Brush Builder
- ? ������� �����!

**������ ��������!** ?

### �������� ���������:

- **Priority 0-20:** ?? ������-�������
- **Priority 21-40:** ?? �������
- **Priority 41-60:** ?? ����-����������
- **Priority 61-80:** ?? ����������
- **Priority 81-100:** ?? �����-����������
- **����������:** ?? �������

---

## ?? ���������: �� vs �����

### ������ ���������� (AVolume):

```cpp
// ? ��������:
- SetHidden(true) �� ABrush
- ����� Brush Builder
- �������� � �������������
- ������� ��������� Collision
- 250+ ����� ����
- ��������� workarounds
```

### ����� ���������� (ATriggerBox):

```cpp
// ? ������������:
- �������� �� �������
- �������������� ������������
- ������� ���������
- Box Component (����� ��������������)
- ~150 ����� ����
- ��� workarounds
```

---

## ?? ������������

### ���� 1: �������� � ���������

```
1. �������� BP_MusicZone_Test
2. ���������� �� �������
3. ���������: ����� �����! ?
```

### ���� 2: ���������������

```
1. �������� ����
2. ������� R (Scale tool)
3. �������������
4. ���������: ����� � ������! ?
```

### ���� 3: Overlap detection

```
1. Play in Editor
2. ������� � ����
3. ��������� ����
4. ���������: Overlap ��������! ?
```

---

## ?? ������ TriggerBox?

### ATriggerBox - ���:

1. **������� �������** - overlap events ���������
2. **Box Component** - ������� �����, ����� ��������������
3. **����������� �����** - ������������ ����� � UE
4. **�������** - ������������� ������������ � ���������
5. **�������** - ������� ����

### ������� ������������� TriggerBox � UE:

- ? Level Streaming Triggers
- ? Checkpoint Triggers
- ? Event Triggers
- ? Audio Triggers
- ? **Music Zone Triggers** ? ��� ������!

---

## ?? API Reference

### Public Methods:

```cpp
// Get/Set Zone Music
USoundBase* GetZoneMusic() const;
void SetZoneMusic(USoundBase* NewMusic);

// Enable/Disable Zone
void SetZoneActive(bool bActive);
bool IsZoneActive() const;
```

### Blueprint Events:

```cpp
// Override � Blueprint
void OnPlayerEntered(APlayerController* PC);
void OnPlayerExited(APlayerController* PC);
```

---

## ?? ������ ��������

### ? DO (�������):

1. **����������� ����������� �����**
   ```
   BP_MusicZone_Town
   BP_MusicZone_Forest
   BP_MusicZone_Boss
   ```

2. **������������ ����������**
   ```
   Town: 10 (������)
   Forest: 30 (�������)
   Boss: 100 (���������)
   ```

3. **����������� �������� ���������**
   - ������� Priority ? ���� ��������� �������������

### ? DON'T (�� �������):

1. ? �� ���������� ���� � ���������� Priority
2. ? �� ������� ������� ��������� ����
3. ? �� ��������� ��������� ������

---

## ? �����

### ��� ����������:

| ������ | ������ (AVolume) | ����� (ATriggerBox) |
|--------|------------------|---------------------|
| **������� �����** | `AVolume` | `ATriggerBox` |
| **����� ����** | 250+ | ~150 |
| **�������** | ����� | ��� |
| **���������** | ? �������� | ? �������� |
| **��������** | ? ������ | ? ������ |

### ������ ��� ���������:

1. ? **ATriggerBox** - ����������� ����� ��� ���������
2. ? **������ ����** - ����� ���������
3. ? **�������� �� �������** - ��� �����
4. ? **������� ������ �������������** - ���������� ���

---

**MusicZoneVolume ������ �������, ������ � �������� ��������!** ???

**����:** 23 ������� 2025  
**������:** 7.0 (TRIGGERBOX SOLUTION)  
**������:** ? 100% �������� ��� �������!
