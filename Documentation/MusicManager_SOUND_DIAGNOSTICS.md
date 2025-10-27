# MusicManager - �����������: ������ �� ������

## ?? �������

**��������� ������, �� ������ ���!**

```
LogMusicManager: ? SecondaryAudioComponent is now playing!
LogMusicManager: ?? Crossfade: Progress=51%, NewVol=0.52  ? ��������� �����
LogMusicManager: ? Crossfade completed - Final volume: 1.00  ? ������ ���������
```

**��**: ������ �� ������! ??

## ?? ��������� �������

### 1. Sound Asset ��������

- **���� �� ��������** - Sound Asset ������
- **������������ ������** - UE5 �� ����� ������������
- **������������� ������** - CompressedData ����

### 2. Audio Device ��������

- **Audio Device == null** - �������� ���������� �� ����������������
- **��� ��������� �������** - ��� ������ ������
- **���������� ���������** - ��� ����� ������

### 3. Sound Class ���������

- **Volume == 0** � Sound Class
- **Muted** - Sound Class ��������
- **������������ Attenuation** - ���� ������� �����

## ? ����������� �����������

### 1. Sound Asset Diagnostics

```cpp
UE_LOG(LogMusicManager, Log, TEXT("?? Sound Asset Diagnostics:"));
UE_LOG(LogMusicManager, Log, TEXT("   - Sound Class: %s"), ...);
UE_LOG(LogMusicManager, Log, TEXT("   - Duration: %.2f seconds"), ...);
UE_LOG(LogMusicManager, Log, TEXT("   - Is Valid Asset: %s"), ...);

// ���� SoundWave
if (USoundWave* SoundWave = Cast<USoundWave>(Sound))
{
    UE_LOG(LogMusicManager, Log, TEXT("   - NumChannels: %d"), ...);
    UE_LOG(LogMusicManager, Log, TEXT("   - SampleRate: %d"), ...);
    UE_LOG(LogMusicManager, Log, TEXT("   - Compressed Data Size: %d"), ...);
}
```

**��� ���������**:
- ? Sound Class - ���������� �� �����?
- ? Duration - ���� �� ������?
- ? NumChannels - ���������� �� ���-�� �������?
- ? SampleRate - ���������� �� �������?
- ? Compressed Data Size - **��������!** ���� == 0 ? ���� ������!

### 2. Audio Device Diagnostics

```cpp
if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
{
  UE_LOG(LogMusicManager, Log, TEXT("?? Audio Device Diagnostics:"));
    UE_LOG(LogMusicManager, Log, TEXT("   - Audio Device: Valid"));
    UE_LOG(LogMusicManager, Log, TEXT("   - Device ID: %d"), ...);
UE_LOG(LogMusicManager, Log, TEXT("   - Max Channels: %d"), ...);
}
else
{
    UE_LOG(LogMusicManager, Error, TEXT("? Audio Device is NULL!"));
}
```

**��� ���������**:
- ? Audio Device Valid - ���� �� �������� ����������?
- ? Device ID - ���������� �� ����������?
- ? Max Channels - ���� �� ��������� ������?

## ?? ������������

### 1. ��������� ���� � ����� ������������

```bash
# ������������� ������
# ��������� PIE
# ������� � MusicZone
```

### 2. ��������� Output Log

����� ����� ����:

```
LogMusicManager: ?? Sound Asset Diagnostics:
LogMusicManager:    - Sound Class: SoundCue  ? ����� ���?
LogMusicManager:    - Duration: 120.50 seconds  ? ���� �� ������?
LogMusicManager:    - Is Valid Asset: Yes
LogMusicManager:    - Sound Wave: Yes
LogMusicManager:  - NumChannels: 2  ? ������?
LogMusicManager:    - SampleRate: 44100  ? 44kHz?
LogMusicManager:    - Compressed Data Size: 5242880  ? ��������! ������ ���� > 0!

LogMusicManager: ?? Audio Device Diagnostics:
LogMusicManager:    - Audio Device: Valid  ? ������ ���� Valid!
LogMusicManager:    - Device ID: 0
LogMusicManager:    - Max Channels: 32
```

### 3. ������������ ����������

#### ? ���� Compressed Data Size == 0:

```
LogMusicManager:    - Compressed Data Size: 0  ? ��������!
```

**�������**: Sound Asset ��������!

1. �������� Sound Asset � ���������
2. ������� **Reimport**
3. ��������� ������ ����� (������ ���� WAV/OGG)
4. ��������� ������ ����� (> 0 bytes)

#### ? ���� Audio Device == NULL:

```
LogMusicManager: ? Audio Device is NULL!
```

**�������**: �������� ���������� �� ����������������!

1. ������������� ��������
2. ��������� Project Settings ? Audio
3. ���������, ��� Audio Device �� ��������

#### ?? ���� Duration == 0:

```
LogMusicManager:    - Duration: 0.00 seconds  ? ��������!
```

**�������**: Sound Asset �� ��������!

1. ���������, ��� Sound Asset **�� Streaming**
2. � Sound Asset ? Compression Settings ? Loading Behavior = **LoadOnDemand** (��� **ForceInline**)

## ?? �������������� ��������

### ���������� �������

```
# �������� ��� �������� �����
au.Debug.Sounds

# �������� Audio Device
au.Debug.AudioDevice

# ��������� ��������� Master
au.3dVisualize.Enabled 1
```

### Blueprint Debug

�������� � Blueprint ��������:

```blueprint
BeginPlay:
  - Get Sound Asset
  - Print String: Duration
  - Print String: NumChannels
```

## ?? ��������� ��������

### ���������� Sound Asset:

```
Duration: > 0 seconds  ? ��������!
NumChannels: 1 ��� 2 (Mono/Stereo)
SampleRate: 44100 ��� 48000
Compressed Data Size: > 0  ? ��������!
```

### ���������� Audio Device:

```
Audio Device: Valid  ? ��������!
Device ID: >= 0
Max Channels: > 0  ? ��������!
```

## ?? ���� ��������

1. **��������� ����** � ����� ������������
2. **���������� ����** Sound Asset Diagnostics
3. **���������**:
   - Compressed Data Size > 0?
   - Audio Device Valid?
   - Duration > 0?
4. **���� �� OK** - �������� � ������ �����
5. **���� ���-�� FAIL** - �������� �������� ����

## ?? ��������� ��� ����

���������� � ��������� ��� ��� ���� ��:

```
LogMusicManager: ?? Sound Asset Diagnostics:
```

��:

```
LogMusicManager: ?? Audio Device Diagnostics:
```

� ����� �������� ��������!

---
**������**: ?? ����������� ���������
**����**: 2025-01-29
**��������� ���**: ��������� � ��������� ����
