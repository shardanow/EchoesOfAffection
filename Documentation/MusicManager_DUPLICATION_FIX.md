# ?? MusicManager: ����������� ������������ ������ ��� Crossfade

## ? ��������

��� �������� �� **Immediate** � **Crossfade** ������ **�������������** - ������ ���� ����� **������** (�� Primary � �� Secondary).

### ��� �����������:

```
1. Immediate: town_first ������ �� Primary ?
2. Crossfade ����������:
   - Swap �����������
   - Primary (������ Secondary) = town_second
   - Secondary (������ Primary) = town_first (��� ������!)
3. ��������������� ������ ������:
   - Secondary->SetSound(town_first)
   - Secondary->Play() ? ? ������������!
4. ���������: town_first ������ �� ���� ����������� ������������!
```

### ���� ����������:

```
[912] ?? StartCrossfade: NewMusic=town_second
[912] ? Old music: town_first, Volume: 1.00
[912] ?? Swapped components
[912] ? Sound set successfully: town_second
[912] ? PrimaryAudioComponent is now playing
[912] ? Set old music on Secondary: town_first ? ? ��������!
[912] (Secondary ��� ����� town_first!)
```

---

## ?? �������� �������

### ����� swap Secondary ����� ��� ������ ������ ������!

```cpp
// ? ������ ���:
void StartCrossfade(...)
{
    // 1. ��������� OldSound
    OldSound = PrimaryAudioComponent->Sound; // town_first
    
    // 2. Swap
    Swap(Primary, Secondary);
    // ������ Secondary = ������ Primary (������� ����� town_first!)
    
    // 3. ��������������� ������ ������
    if (bHasOldMusic && OldSound)
    {
   Secondary->SetSound(OldSound); // ? ��� ����������!
   Secondary->Play(); // ? ������������!
    }
}
```

**��������:** ����� swap `SecondaryAudioComponent` **��� �������� � ������** ������ ������!  
��������� ����� `SetSound` + `Play` �������� **������������**.

---

## ? �������: �������� ����� ���������������

### ����� ������:

```cpp
void StartCrossfade(USoundBase* NewMusic, float Duration, float TargetVol)
{
    // 1. ��������� ������ ������
    USoundBase* OldSound = nullptr;
    float CurrentVolume = 0.0f;
    bool bHasOldMusic = false;
    
    if (PrimaryAudioComponent->IsPlaying())
    {
        OldSound = PrimaryAudioComponent->Sound;
  CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
      bHasOldMusic = true;
    }

    // 2. ? ������������� Secondary ����� swap
    if (SecondaryAudioComponent->IsPlaying())
    {
        SecondaryAudioComponent->Stop();
    }

    // 3. Swap �����������
    Swap(PrimaryAudioComponent, SecondaryAudioComponent);

    // 4. ����������� ����� ����
    PrimaryAudioComponent->SetSound(NewMusic);
    PrimaryAudioComponent->SetVolumeMultiplier(0.0f);
    PrimaryAudioComponent->Play();

    // 5. ? ��������: ���������, �� ������ �� Secondary ��� ������ ������!
    if (bHasOldMusic && OldSound)
    {
 bool bSecondaryAlreadyPlaying = 
            SecondaryAudioComponent->IsPlaying() && 
       SecondaryAudioComponent->Sound == OldSound;
        
        if (bSecondaryAlreadyPlaying)
        {
       // Secondary ��� ������ - ������ ������������� ���������
 SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
        }
        else
   {
       // Secondary �� ������ - ������������� Sound � ���������
      SecondaryAudioComponent->SetSound(OldSound);
    SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
   SecondaryAudioComponent->Play();
  }
    }
}
```

---

## ?? ���������: �� vs �����

### �� (������������):

| ��� | Primary | Secondary | ��������� |
|-----|---------|-----------|-----------|
| 1. Immediate | Playing: town_first | Empty | ? ������ town_first |
| 2. Save OldSound | OldSound = town_first | | |
| 3. Swap | Empty | Playing: town_first | |
| 4. Setup New | Playing: town_second | Playing: town_first | |
| 5. Restore Old | Playing: town_second | ? **Playing: town_first (x2!)** | ? **������������!** |
| **����** | | | ? town_first ������ ������ |

### ����� (����������):

| ��� | Primary | Secondary | ��������� |
|-----|---------|-----------|-----------|
| 1. Immediate | Playing: town_first | Empty | ? ������ town_first |
| 2. Save OldSound | OldSound = town_first | | |
| 3. Stop Secondary | | Stopped ? | |
| 4. Swap | Empty | Was: town_first | |
| 5. Setup New | Playing: town_second | | |
| 6. Check Secondary | | Not playing ? | |
| 7. Restore Old | Playing: town_second | ? Playing: town_first (x1) | ? ���������! |
| **����** | | | ? ��������� town_first ? town_second |

---

## ?? �������� ���������

### 1. ��������� Secondary ����� swap

```cpp
// ? �����: ������������� Secondary
if (SecondaryAudioComponent->IsPlaying())
{
    SecondaryAudioComponent->Stop();
}
```

**�����:** �����������, ��� ����� swap Secondary **�� ������** ������ ������.

### 2. �������� ����� ���������������

```cpp
// ? �����: ���������, �� ������ �� ���
bool bSecondaryAlreadyPlaying = 
    SecondaryAudioComponent->IsPlaying() && 
  SecondaryAudioComponent->Sound == OldSound;

if (bSecondaryAlreadyPlaying)
{
    // ������ ���������
    SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
}
else
{
    // Sound + Play
    SecondaryAudioComponent->SetSound(OldSound);
 SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
    SecondaryAudioComponent->Play();
}
```

**�����:** �������� **������������** ������.

---

## ?? ������������

### ���� 1: Immediate ? Crossfade

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Immediate
- Zone B: Priority 20, Music: town_second, Transition: Crossfade, Duration: 2.0

��������:
1. ������ � Zone A
2. ��������� � Zone B
```

**��������� ��������� (����� �����������):**
```
1. Zone A: town_first ������ ��������� ?
2. Zone B: ��������� 2 �������: town_first ? town_second ?
   - town_first ������ ���� ��� ?
   - town_second ������ ���������� ?
```

**����:**
```
[872] ? Immediate transition to: town_first
[596] ?? StartCrossfade: NewMusic=town_second, Duration=2.00
[596] ? Old music: town_first, Volume: 1.00
[596] ?? Secondary component was playing - stopping it ? ? �����!
[596] ?? Swapped components
[596] ? Sound set successfully: town_second
[596] ? PrimaryAudioComponent is now playing
[596] ? Set old music on Secondary: town_first, Volume: 1.00 ? ? ������ ���� ���!
[596] ?? Crossfade: Progress=51%, NewVol=0.52, OldVol=0.48
[596] ? Crossfade completed
```

### ���� 2: Crossfade ? Crossfade

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Crossfade
- Zone B: Priority 20, Music: town_second, Transition: Crossfade

��������:
1. ������ � Zone A
2. ��������� � Zone B
```

**��������� ���������:**
```
1. Zone A: town_first ������ ���������� ?
2. Zone B: ��������� town_first ? town_second ?
```

---

## ?? �����������

### ���� ������ �� ��� �����������:

**��������� ����:**
```
?? Secondary component was playing - stopping it
```

**���� �� ������ ���� ���** ? Secondary �� �����, �������� � ������ �����.

**���� ������:**
```
? Secondary already playing old music: town_first
```

? Secondary **��� �����** ������ ������ (������ ���������).

### ���� ��������� �� �������:

**���������:**
```
?? Crossfade: Progress=51%, NewVol=0.52, OldVol=0.48
```

**���� `OldVol` �� ��������** ? �������� � `UpdateCrossfadeTimer()`.

---

## ?? �����

### ��������:

1. ? ����� swap Secondary **��� �����** ������ ������
2. ? ��������� `SetSound` + `Play` �������� **������������**
3. ? ������ ������ **������** ������������

### �������:

1. ? **�������������** Secondary ����� swap
2. ? **���������**, �� ������ �� Secondary ��� ������ ������
3. ? **�� ���������������**, ���� Secondary ��� ������

### ���������:

- ? **��� ������������** ������
- ? **��������� ��������** ���������
- ? **Immediate ? Crossfade** �������� ���������
- ? **Crossfade ? Crossfade** �������� ���������

---

**����:** 24 ������� 2025  
**������:** 1.7 (DUPLICATION FIX)  
**������:** ? ��� ���� ����������! ������� �������� ��������!
