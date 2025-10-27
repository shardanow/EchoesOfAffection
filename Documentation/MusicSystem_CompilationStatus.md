# ? ����������� ������� - ������ ����������

## ?? ����� �������

### ������������ ����� (.h)
- ? `Source/EchoesOfAffection/Public/Audio/MusicManager.h` - **�����**
- ? `Source/EchoesOfAffection/Public/Audio/MusicZoneVolume.h` - **�����**

### ���������� (.cpp)
- ? `Source/EchoesOfAffection/Private/Audio/MusicManager.cpp` - **�����**
- ? `Source/EchoesOfAffection/Private/Audio/MusicZoneVolume.cpp` - **�����**

### ������������
- ? `Source/EchoesOfAffection/EchoesOfAffection.Build.cs` - **��������** (�������� GameplayTags)

### ������������
- ? `Documentation/MusicSystem_Guide.md` - ������ �����������
- ? `Documentation/MusicSystem_QuickStart.md` - ������� �����

## ?? �������� ���������

### MusicManager.h
```cpp
// ? ���� (� �������):
class UMusicManager : public UGameInstanceSubsystem, public FTickableGameObject

// ? ����� (����������):
class UMusicManager : public UGameInstanceSubsystem
{
    // ���������� FTimerHandle ������ FTickableGameObject
    FTimerHandle CrossfadeTimerHandle;
    void UpdateCrossfadeTimer();
}
```

### MusicManager.cpp
```cpp
// ���������� TimerManager ��� ����������
void UMusicManager::StartCrossfade(...)
{
    World->GetTimerManager()->SetTimer(
  CrossfadeTimerHandle,
        this,
      &UMusicManager::UpdateCrossfadeTimer,
        0.016f, // ~60 FPS
 true    // Loop
    );
}

void UMusicManager::UpdateCrossfadeTimer()
{
    // ���������� ���������� ������ 16ms
}
```

## ?? ���������� �������

### ����� Visual Studio

1. **��������** `EchoesOfAffection.sln` � Visual Studio
2. **��������** ������������: `Development Editor`
3. **��������** ������: `Build ? Build Solution` (Ctrl+Shift+B)

### ����� Unreal Engine Editor

1. **��������** Unreal Editor (���� ������)
2. **���** �� `EchoesOfAffection.uproject`
3. **��������** `Generate Visual Studio project files`
4. **��������** `.sln` � �������������

### ����� ��������� ������

```powershell
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"

# ��������� �������
"D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" ^
    -projectfiles ^
  -project="EchoesOfAffection.uproject" ^
    -game

# ����������
"D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" ^
    EchoesOfAffectionEditor ^
    Win64 ^
    Development ^
    -Project="EchoesOfAffection.uproject" ^
    -WaitMutex
```

## ? �������� �����������������

### 1. ���������� ������ �������?

���� ������:
```
Build succeeded.
    0 Warning(s)
    0 Error(s)
```

**? �������!** ������� ������ � �������������.

### 2. ������������ � ���������

1. **��������** ������ � Unreal Editor
2. **Content Browser** ? ��� ? `Blueprint Class`
3. **�������** `MusicZoneVolume`
4. **��������** `BP_MusicZone_Test`
5. **����������** �� ������

### 3. �������� ���������� ������

� ��������� ������� `~` (������) � �������:

```
Music.Debug    // �������� ��������� �������
Music.Stop     // ���������� ������
```

## ?? ������� �������

### ������: "Cannot open include file MusicManager.h"

**�������:**
1. ���������, ��� ���� ����������: `Source/EchoesOfAffection/Public/Audio/MusicManager.h`
2. ������������ ������: ��� �� `.uproject` ? `Generate Visual Studio project files`

### ������: "Unresolved external symbol UMusicManager"

**�������:**
1. ���������, ��� `MusicManager.cpp` ��������� � `Source/EchoesOfAffection/Private/Audio/`
2. ���������������� ���������: `Build ? Rebuild Solution`

### ������: "FTickableGameObject: no such base class"

**�������:**
��� ������ **����������**. ���������, ��� `MusicManager.h` ��������:
```cpp
class ECHOESOFAFFECTION_API UMusicManager : public UGameInstanceSubsystem
// ���: , public FTickableGameObject
```

### ������: "GameplayTags module not found"

**�������:**
��������� `EchoesOfAffection.Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(new string[] { 
    "Core", "CoreUObject", "Engine", "InputCore", 
    "EnhancedInput", "GameplayTags"  // ? ������ ����!
});
```

## ?? ��������� �������

```
EchoesOfAffection/
??? Source/EchoesOfAffection/
?   ??? Public/Audio/
?   ?   ??? MusicManager.h     ? �����
?   ?   ??? MusicZoneVolume.h       ? �����
?   ??? Private/Audio/
?   ?   ??? MusicManager.cpp        ? �����
??   ??? MusicZoneVolume.cpp     ? �����
?   ??? EchoesOfAffection.Build.cs  ? ��������
?
??? Documentation/
?   ??? MusicSystem_Guide.md   ? �����
?   ??? MusicSystem_QuickStart.md   ? �����
?
??? Content/Audio/Music/  (�������� ��� ������)
    ??? Town_Theme.wav
    ??? Battle_Music.wav
    ??? ...
```

## ?? ��������� ����

1. ? **����������** - �������� ������
2. ? **������������** - �������� �������� ����
3. ? **����������** - �������� ������ � ����
4. ? **���������** - ��������� ���������

## ?? �������������� �������

- **������ ����**: `Documentation/MusicSystem_Guide.md`
- **������� �����**: `Documentation/MusicSystem_QuickStart.md`
- **������� �������������**: ��. ������������

---

**������� ������ � �������������!** ???

��������� ����������: 23 ������� 2025
