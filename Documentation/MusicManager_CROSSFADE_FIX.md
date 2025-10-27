# ?? MusicManager: ����������� Crossfade � Immediate

## ? ��������� ����

### 1. ��������� �� ������� (������)

**��������:**
```cpp
// ? ����������� (������ ���):
void UMusicManager::StartCrossfade(...)
{
    // Swap audio components
    UAudioComponent* Temp = PrimaryAudioComponent;
    PrimaryAudioComponent = SecondaryAudioComponent;
    SecondaryAudioComponent = Temp;
    
    // ...setup new track...
    
    SourceVolume = SecondaryAudioComponent->VolumeMultiplier; // ? ��������!
}
```

**������ �� ��������:**
1. ? Swap ����������� **�����** ������� `VolumeMultiplier`
2. ? `SecondaryAudioComponent` ������ ��������� �� **������** Primary (������� ��� ��� �����)
3. ? ������ ��������� �� **�������������** ����������
4. ? `SourceVolume` ������ = 1.0 (��������� ���������)

### 2. ������ ���������� � ����� ��������

**��������:**
```cpp
// ��� �������� �� ���� A ? ���� B:
1. ���� A: ������ ������ � Volume = 1.0
2. ���� B ����������� ���������
3. StartCrossfade:
   - Swap ����������� ?
   - SourceVolume = SecondaryAudioComponent->VolumeMultiplier
   - SecondaryAudioComponent ������ = ������ Primary
   - �� VolumeMultiplier ��� ������!
4. ���������: SourceVolume = 1.0, �� ������ ���� 0.5 (��������)
5. UpdateCrossfadeTimer:
   - OldTrackVolume = 1.0 * (1.0 - 0.0) = 1.0 (������!)
   - NewTrackVolume = 1.0 * 0.0 = 0.0 (����!)
   - ������ ������ ������ ������, ����� �� ������
6. ����� 2 �������:
   - OldTrackVolume = 1.0 * (1.0 - 1.0) = 0.0 (����!)
   - NewTrackVolume = 1.0 * 1.0 = 1.0 (������!)
 - ����� ������ ����������
```

**������:** ������ "����������" (������ ������) � "��������" (��������� �����������).

---

## ? �������

### ����������� StartCrossfade

```cpp
void UMusicManager::StartCrossfade(USoundBase* NewMusic, float Duration, float TargetVol)
{
    // ? ��������: ��������� ������� ��������� ����� swap
    float CurrentVolume = 0.0f;
    bool bHasOldMusic = false;
    
    if (PrimaryAudioComponent->IsPlaying())
 {
        CurrentVolume = PrimaryAudioComponent->VolumeMultiplier; // ? ��������� ����� SWAP!
  bHasOldMusic = true;
        UE_LOG(LogMusicManager, Log, TEXT("? Old music volume: %.2f"), CurrentVolume);
    }
    else
    {
  UE_LOG(LogMusicManager, Log, TEXT("?? No old music playing - starting fresh"));
  }

    // Swap audio components
    UAudioComponent* Temp = PrimaryAudioComponent;
    PrimaryAudioComponent = SecondaryAudioComponent;
  SecondaryAudioComponent = Temp;

    // Setup new track
    PrimaryAudioComponent->SetSound(NewMusic);
    PrimaryAudioComponent->SetVolumeMultiplier(0.0f);
    PrimaryAudioComponent->Play();

    // Setup crossfade state
    bIsCrossfading = true;
    CrossfadeProgress = 0.0f;
    CrossfadeDuration = Duration;
    TargetVolume = TargetVol;
    SourceVolume = CurrentVolume; // ? ���������� �����ͨ���� ���������!

    // ? ���� ���� ������ ������ - ������������� � ��������� ���������
    if (bHasOldMusic && SecondaryAudioComponent->IsPlaying())
    {
        SecondaryAudioComponent->SetVolumeMultiplier(SourceVolume);
        UE_LOG(LogMusicManager, Log, TEXT("? Set old music volume to: %.2f"), SourceVolume);
    }

    // Start timer...
}
```

---

## ?? ���������: �� vs �����

### �� (�� ��������):

| ��� | �������� | ��������� |
|-----|----------|-----------|
| 1 | Primary ������ ������ A (Vol=1.0) | ? ������ A |
| 2 | ������ ���������� �� B | |
| 3 | Swap ����������� | Primary?Secondary |
| 4 | `SourceVolume = Secondary->Volume` | ? = 1.0 (�����������!) |
| 5 | New track ������ � Vol=0.0 | ? �� ������ B |
| 6 | Old track ������ � Vol=1.0 | ? ������ A |
| 7 | ���������: Old=1.0?0.0, New=0.0?1.0 | ? ��������� ������������ |

### ����� (��������):

| ��� | �������� | ��������� |
|-----|----------|-----------|
| 1 | Primary ������ ������ A (Vol=1.0) | ? ������ A |
| 2 | ������ ���������� �� B | |
| 3 | **���������** `CurrentVolume = 1.0` | ? ���������! |
| 4 | Swap ����������� | Primary?Secondary |
| 5 | `SourceVolume = CurrentVolume` | ? = 1.0 (���������!) |
| 6 | New track ������ � Vol=0.0 | ? �� ������ B (����) |
| 7 | Old track ������ � Vol=1.0 | ? ������ A |
| 8 | ���������: Old=1.0?0.0, New=0.0?1.0 | ? ���������� ���������! |

---

## ?? �������� �����������

### 1. ���������� ��������� ����� swap

```cpp
// ? �����������:
Swap();
SourceVolume = SecondaryAudioComponent->VolumeMultiplier; // ������������ ���������!

// ? ���������:
float CurrentVolume = PrimaryAudioComponent->VolumeMultiplier; // ��������� ����� swap
Swap();
SourceVolume = CurrentVolume; // ���������� ���������� ��������
```

### 2. �������� ������� ������ ������

```cpp
bool bHasOldMusic = PrimaryAudioComponent->IsPlaying();

if (bHasOldMusic)
{
    CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
}
else
{
    CurrentVolume = 0.0f; // ��� ������ ������ - �������� � ����
}
```

### 3. ��������� ��������� ������ ������ ����� swap

```cpp
// ����� swap Secondary = ������ Primary
if (bHasOldMusic && SecondaryAudioComponent->IsPlaying())
{
  SecondaryAudioComponent->SetVolumeMultiplier(SourceVolume);
}
```

---

## ?? ������������

### �������� 1: ������ ���� � ����

```
1. ���� ���������� - ������ ���
2. ������ � ���� A (Priority: 10, Music: town_first)
3. ��������� ���������:
- CurrentVolume = 0.0 (��� ������ ������)
   - SourceVolume = 0.0
   - ���������: 0.0 ? 1.0
   - ������ ������ ���������� ?
```

**����:**
```
?? No old music playing - starting fresh
? Sound set successfully: town_first
? PrimaryAudioComponent is now playing
?? Crossfade setup: TargetVol=1.00, SourceVol=0.00, HasOldMusic=NO
?? Crossfade: Progress=0%, NewVol=0.00, OldVol=0.00
?? Crossfade: Progress=50%, NewVol=0.50, OldVol=0.00
?? Crossfade: Progress=100%, NewVol=1.00, OldVol=0.00
? Crossfade completed - Final volume: 1.00
```

### �������� 2: ������� ����� ������

```
1. � ���� A ������ town_first (Vol=1.0)
2. ��������� � ���� B (Priority: 20, Music: forest_theme)
3. ��������� ���������:
   - CurrentVolume = 1.0 (town_first ������)
   - SourceVolume = 1.0
   - ���������: town_first (1.0?0.0) + forest_theme (0.0?1.0)
   - ������� ������� ?
```

**����:**
```
? Old music volume: 1.00
? Sound set successfully: forest_theme
? PrimaryAudioComponent is now playing
?? Crossfade setup: TargetVol=1.00, SourceVol=1.00, HasOldMusic=YES
? Set old music volume to: 1.00
?? Crossfade: Progress=0%, NewVol=0.00, OldVol=1.00
?? Crossfade: Progress=50%, NewVol=0.50, OldVol=0.50
?? Crossfade: Progress=100%, NewVol=1.00, OldVol=0.00
? Stopped old track
? Crossfade completed - Final volume: 1.00
```

### �������� 3: Immediate �������

```
1. � ���� A ������ town_first
2. ��������� � ���� C (Transition: Immediate)
3. ��������� ���������:
   - town_first ��������������� ���������
   - boss_theme ���������� ���������
   - ��� ���������� ?
```

**����:**
```
?? Stopped old track
? Immediate transition to: boss_theme (Volume: 1.00)
```

---

## ?? ����������� �������

### ���� ������ �� ��� "���������� � ��������":

**��������� ����:**
```
?? Crossfade setup: TargetVol=?, SourceVol=?, HasOldMusic=?
```

**������ ����:**
- **HasOldMusic=YES** ? `SourceVol` ������ ��������� � ���������� ������ ������
- **HasOldMusic=NO** ? `SourceVol = 0.00`

**���� `SourceVol` ������������** ? �������� � ���������� ���������!

### ���� ��������� �� ������:

**���������:**
```cpp
// ������� (~)
Music.Debug

// �����:
Current Music: [������ �����������]
Master Volume: 1.00
Is Playing: Yes
```

**���� `Is Playing: No`** ? Sound Asset �� ��������������� (��. ���������� ������������).

---

## ?? �����

### ��������:

1. ? Swap ����������� **�����** ������� ���������
2. ? ������ `VolumeMultiplier` �� ������������� ����������
3. ? `SourceVolume` ������ = 1.0

### �������:

1. ? ��������� ��������� **�����** swap
2. ? ���������� ���������� �������� ��� `SourceVolume`
3. ? ��������� ������� ������ ������ (`bHasOldMusic`)
4. ? ������������� ��������� ������ ������ ����� swap

### ���������:

- ? **Crossfade ��������** - ������� ������� ����� �������
- ? **Immediate ��������** - ���������� �����
- ? **��� ���� "���������� � ��������"**
- ? **��������� �����������** ��� �������

---

**����:** 23 ������� 2025  
**������:** 1.3 (CROSSFADE FIX)  
**������:** ? ��� ���� ����������!
