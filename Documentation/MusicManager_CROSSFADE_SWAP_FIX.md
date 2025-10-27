# ?? MusicManager: ����������� Crossfade ����� Immediate

## ? ��������

��� �������� �� **Immediate** � **Crossfade** ������ �� ����������������:

```
��������:
1. ������ � Zone A (Immediate) ? town_first ������ ?
2. ��������� � Zone B (Crossfade) ? town_first �������� ?, town_second �� ������ ?
```

### ���� ����������:

```
[872] ? Immediate transition to: town_first (Volume: 1.00)
[596] ?? StartCrossfade: NewMusic=town_second, Duration=2.00
[596] ? Old music volume: 1.00
[596] ? Sound set successfully: town_second
[596] ? PrimaryAudioComponent failed to play! ? ��������!
```

---

## ?? �������� �������

### �������� swap �����������

```cpp
// ? ������ ���:
void StartCrossfade(USoundBase* NewMusic, ...)
{
    // 1. ��������� ������ ���������
    float CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
    
    // 2. Swap �����������
    Swap(PrimaryAudioComponent, SecondaryAudioComponent);
    
    // 3. ������������� Sound �� ����� Primary (������ Secondary)
 PrimaryAudioComponent->SetSound(NewMusic);
    PrimaryAudioComponent->Play(); // ? �� ������!
    
// 4. �������� ���������� ��������� ������ ������
    SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume); // ? �� Sound �� ����������!
}
```

**��������:**

1. ? **Immediate** ����� �� `PrimaryAudioComponent`
2. ? **Crossfade** ������ swap ? ����� ������ �� `SecondaryAudioComponent` (������� ��� **���������**)
3. ? `SecondaryAudioComponent` **�� ��� ���������������** ��� **�� ���� Sound**
4. ? ����� swap ������ ������ **��������** - ��� ������ �� `OldSound`!

---

## ? �������

### 1. ��������� Sound ����� swap

```cpp
// ? ����� ���:
void StartCrossfade(USoundBase* NewMusic, ...)
{
    // 1. ��������� ��������� � Sound
    float CurrentVolume = 0.0f;
    USoundBase* OldSound = nullptr; // ? �����!
    bool bHasOldMusic = false;
    
    if (PrimaryAudioComponent->IsPlaying())
    {
        CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
        OldSound = PrimaryAudioComponent->Sound; // ? ���������!
        bHasOldMusic = true;
    }
    
    // 2. ������������� Secondary ����� swap
    if (SecondaryAudioComponent->IsPlaying())
    {
      SecondaryAudioComponent->Stop();
  }
    
    // 3. Swap
    Swap(PrimaryAudioComponent, SecondaryAudioComponent);
    
    // 4. ����������� ����� ����
    PrimaryAudioComponent->SetSound(NewMusic);
    PrimaryAudioComponent->SetVolumeMultiplier(0.0f);
    PrimaryAudioComponent->Play(); // ? ������!
    
    // 5. ? ��������: ����������� ������ ���� �� Secondary
    if (bHasOldMusic && OldSound)
    {
        SecondaryAudioComponent->SetSound(OldSound); // ? ���������������!
        SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
      
        if (!SecondaryAudioComponent->IsPlaying())
    {
      SecondaryAudioComponent->Play(); // ? ������ ������ ������!
        }
    }
}
```

### 2. �������� ����������� ����������

```cpp
// ? ���������, ��� ��������� ���������������
if (!PrimaryAudioComponent->IsRegistered())
{
    UE_LOG(LogMusicManager, Error, TEXT("? PrimaryAudioComponent is NOT registered!"));
    
    // ������� ����������������
    if (UWorld* World = GetWorld())
    {
 PrimaryAudioComponent->RegisterComponentWithWorld(World);
        UE_LOG(LogMusicManager, Warning, TEXT("?? Re-registered PrimaryAudioComponent"));
    }
}
```

### 3. ��������� ����������� ������

```cpp
if (!PrimaryAudioComponent->IsPlaying())
{
    UE_LOG(LogMusicManager, Error, TEXT("? PrimaryAudioComponent failed to play!"));
    UE_LOG(LogMusicManager, Error, TEXT("   - IsRegistered: %s"), 
        PrimaryAudioComponent->IsRegistered() ? TEXT("Yes") : TEXT("No"));
    UE_LOG(LogMusicManager, Error, TEXT("   - Sound: %s"), 
  PrimaryAudioComponent->Sound ? *PrimaryAudioComponent->Sound->GetName() : TEXT("None"));
    UE_LOG(LogMusicManager, Error, TEXT("   - bAutoActivate: %s"), 
        PrimaryAudioComponent->bAutoActivate ? TEXT("Yes") : TEXT("No"));
    return;
}
```

---

## ?? ���������: �� vs �����

### �� (�� ��������):

| ��� | Primary | Secondary | ��������� |
|-----|---------|-----------|-----------|
| 1. Immediate | Playing: town_first | Empty | ? ������ town_first |
| 2. Swap | Empty ? | Playing: town_first | |
| 3. SetSound | Sound: town_second | Playing: town_first | |
| 4. Play | ? �� ������ | Playing: town_first | |
| **����** | | | ? ������ ��� ������ town_first |

**��������:** ����� swap `PrimaryAudioComponent` ��� **������** (������ `SecondaryAudioComponent`), � ������ �� ������!

### ����� (��������):

| ��� | Primary | Secondary | ��������� |
|-----|---------|-----------|-----------|
| 1. Immediate | Playing: town_first | Empty | ? ������ town_first |
| 2. Save Sound | OldSound = town_first ? | | |
| 3. Stop Secondary | | Stopped | |
| 4. Swap | Empty | Was: town_first | |
| 5. Setup New | Sound: town_second, Vol: 0 | | |
| 6. Play New | ? Playing: town_second | | |
| 7. Setup Old | Playing: town_second | ? Sound: town_first, Playing | |
| **����** | | | ? ��������� town_first ? town_second |

---

## ?? �������� ���������

### 1. ���������� Sound ����� swap

```cpp
// ? ������: ������ ���������
float CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;

// ? �����: ��������� + Sound
float CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
USoundBase* OldSound = PrimaryAudioComponent->Sound; // ?!
```

### 2. ��������� Secondary ����� swap

```cpp
// ? �����: ��������, ��� Secondary ������
if (SecondaryAudioComponent->IsPlaying())
{
    SecondaryAudioComponent->Stop();
}
```

### 3. �������������� ������ ������ ����� swap

```cpp
// ? �����: ����������� Secondary � ���������� Sound
if (bHasOldMusic && OldSound)
{
    SecondaryAudioComponent->SetSound(OldSound);
    SecondaryAudioComponent->SetVolumeMultiplier(CurrentVolume);
    
    if (!SecondaryAudioComponent->IsPlaying())
    {
        SecondaryAudioComponent->Play();
    }
}
```

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

**��������� ���������:**
```
1. Zone A: town_first ������ ��������� ?
2. Zone B: ��������� 2 �������: town_first ? town_second ?
```

**���� (����� �����������):**
```
[872] ? Immediate transition to: town_first (Volume: 1.00)
[596] ?? StartCrossfade: NewMusic=town_second, Duration=2.00
[596] ? Old music: town_first, Volume: 1.00
[596] ?? Swapped components - Primary=SecondaryMusicComponent, Secondary=PrimaryMusicComponent
[596] ? Sound set successfully: town_second
[596] ? PrimaryAudioComponent is now playing
[596] ? Set old music on Secondary: town_first, Volume: 1.00
[596] ?? Crossfade setup: TargetVol=1.00, SourceVol=1.00, HasOldMusic=YES
[596] ?? Crossfade: Progress=26%, NewVol=0.16, OldVol=0.84
[596] ?? Crossfade: Progress=51%, NewVol=0.52, OldVol=0.48
[596] ? Crossfade completed - Final volume: 1.00
```

### ���� 2: Crossfade ��� ������ ����

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Crossfade, Duration: 2.0

��������:
1. ������ � Zone A (��� ���������� ������)
```

**��������� ���������:**
```
1. Zone A: town_first ������ ���������� � 0% ? 100% ?
```

**����:**
```
[872] ?? StartCrossfade: NewMusic=town_first, Duration=2.00
[872] ?? No old music playing - starting fresh
[872] ?? Swapped components
[872] ? Sound set successfully: town_first
[872] ? PrimaryAudioComponent is now playing
[872] ?? Crossfade setup: TargetVol=1.00, SourceVol=0.00, HasOldMusic=NO
[872] ?? Crossfade: Progress=26%, NewVol=0.16, OldVol=0.00
[872] ? Crossfade completed - Final volume: 1.00
```

---

## ?? ����������� �������

### ���� ������ �� ��� �� ������ ����� �����������:

**��������� ����:**
```
? PrimaryAudioComponent failed to play!
   - IsRegistered: No ? ��������!
   - Sound: town_second
   - bAutoActivate: No
```

**�������:**
```cpp
// ��������� �� ��������������� - ��� ������������� �������� ���������:
if (!PrimaryAudioComponent->IsRegistered())
{
    PrimaryAudioComponent->RegisterComponentWithWorld(World);
}
```

### ���� ������ ������ �� ��������:

**��������� ����:**
```
? Set old music on Secondary: town_first, Volume: 1.00
?? Crossfade: Progress=51%, NewVol=0.52, OldVol=0.48
```

**���� `OldVol` �� ��������** ? �������� � `UpdateCrossfadeTimer()`.

---

## ?? �����

### ��������:

1. ? ����� Immediate ? Crossfade ������ �� ������
2. ? Swap ����������� **�� ��������** ������ ������
3. ? `SecondaryAudioComponent` ��� **������** ����� swap

### �������:

1. ? ��������� **Sound** (�� ������ ���������) ����� swap
2. ? ������������� `Secondary` ����� swap
3. ? **���������������** ������ ������ �� `Secondary` ����� swap
4. ? ��������� ����������� ����������
5. ? ��������� ����������� ��� �����������

### ���������:

- ? **Immediate ? Crossfade** �������� ���������
- ? **Crossfade ��� ������ ����** �������� ���������
- ? **������ ������** ��������� ��������
- ? **����� ������** ��������� ����������
- ? **��������� ����** ��� �������

---

**����:** 24 ������� 2025  
**������:** 1.6 (CROSSFADE SWAP FIX)  
**������:** ? ��� ���� ����������! ������� �������� ��������!
