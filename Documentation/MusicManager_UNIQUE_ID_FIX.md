# ?? MusicManager: ����������� �������� � ���������� Priority

## ? ��������

��� �������� ����� ������ � **���������� Priority** ������ �������� ������ � ����� ��������.

### ���� ����������:

```
[378] LogMusicManager: Music request: town_first (Priority: 10, Type: 0)
[378] LogMusicManager: ? Immediate transition to: town_first (Volume: 1.00)
[378] LogTemp: Player entered MusicZone 'BP_StartTown_Music_C_2' - Priority: 10

[31] LogMusicManager: Music request: town_second (Priority: 10, Type: 1)
[31] LogMusicManager: ?? StartCrossfade: NewMusic=town_second, Duration=2.00
[31] LogTemp: Player entered MusicZone 'BP_StartTown_Music_Other_C_4' - Priority: 10

[38] LogMusicManager: Released zone with priority: 10  ? ? ��������!
[38] LogTemp: Player exited MusicZone 'BP_StartTown_Music_C_2' - Priority: 10
```

**��� �����������:**
1. ? ������ � ���� A (Priority: 10) ? ������ town_first
2. ? ������ � ���� B (Priority: 10) ? ���������� ��������� �� town_second
3. ? ������� �� ���� A (Priority: 10) ? `ReleaseZone(10)` ������� **���** ����!
4. ? `FindHighestPriorityZone` ������ �� �������
5. ? ������ ���������������

---

## ?? �������� �������

### ������ ����������:

```cpp
// ? ��������: TMap<Priority, Request>
TMap<int32, FMusicRequest> ActiveZones;

void RequestMusicChange(USoundBase* NewMusic, int32 Priority, ...)
{
// ? ���� ��� ���� ����� ���������� Priority, ������ �������������� ������!
    ActiveZones.Add(Priority, Request);
}

void ReleaseZone(int32 Priority)
{
    // ? ������� ��� ���� � ���� Priority!
  ActiveZones.Remove(Priority);
}
```

**��������:** `TMap` ��������� **������ ���� ������� �� ����**!

---

## ? �������: ���������� ID ��� ������ ����

### ����� �����������:

```cpp
// ? TMap<UniqueID, Request> - ������ ���� ����� ���������� ID
TMap<int32, FMusicRequest> ActiveZones;

// ? ������� ��� ��������� ���������� ID
int32 NextZoneID = 0;

// ? ����� �������: RegisterMusicZone
int32 RegisterMusicZone(USoundBase* NewMusic, int32 Priority, ...)
{
    int32 ZoneID = NextZoneID++; // ���������� ���������� ID
    ActiveZones.Add(ZoneID, Request); // ��������� � ���������� ������
  return ZoneID; // ���������� ��� ������������ Release
}

// ? ��������: ReleaseZone �� ����������� ID
void ReleaseZone(int32 ZoneID)
{
    ActiveZones.Remove(ZoneID); // ������� ������ ��� ����
}
```

---

## ?? ���������: �� vs �����

### �� (�� ��������):

| �������� | ActiveZones | ��������� |
|----------|-------------|-----------|
| Enter Zone A (Priority: 10) | `{10: town_first}` | ? ������ town_first |
| Enter Zone B (Priority: 10) | `{10: town_second}` | ? **������������** A! |
| Exit Zone A (Priority: 10) | `{}` | ? ������� **���** ����! |

### ����� (��������):

| �������� | ActiveZones | ��������� |
|----------|-------------|-----------|
| Enter Zone A (Priority: 10) | `{0: town_first(P:10)}` | ? ������ town_first |
| Enter Zone B (Priority: 10) | `{0: town_first(P:10), 1: town_second(P:10)}` | ? ��������� A?B |
| Exit Zone A | `{1: town_second(P:10)}` | ? ������� **������** A! |

---

## ?? �������� ���������

### 1. MusicManager.h

```cpp
class UMusicManager : public UGameInstanceSubsystem
{
    // ? ����� �������: ����������� ���� � ���������� ID
  UFUNCTION(BlueprintCallable, Category = "Music")
    int32 RegisterMusicZone(USoundBase* NewMusic, int32 Priority, ...);

    // ? ��������: Release �� ����������� ID
UFUNCTION(BlueprintCallable, Category = "Music")
    void ReleaseZone(int32 ZoneID);

protected:
  // TMap<UniqueID, Request> - ���� ������ ���������� ID
    TMap<int32, FMusicRequest> ActiveZones;

    // ? �����: ������� ��� ��������� ���������� ID
    int32 NextZoneID = 0;
};
```

### 2. MusicManager.cpp

```cpp
int32 UMusicManager::RegisterMusicZone(...)
{
    // ���������� ���������� ID
    int32 ZoneID = NextZoneID++;

    // ��������� � ���������� ������
    ActiveZones.Add(ZoneID, Request);

    UE_LOG(LogMusicManager, Log, TEXT("?? Registered zone ID=%d: %s (Priority: %d)"),
   ZoneID, *NewMusic->GetName(), Priority);

    // ���� ���� � ��������� Priority
FindHighestPriorityZone(HighestRequest, HighestPriority);
    // ...

    return ZoneID; // ���������� ��� ������������ Release
}

void UMusicManager::ReleaseZone(int32 ZoneID)
{
    if (!ActiveZones.Contains(ZoneID))
    {
   UE_LOG(LogMusicManager, Warning, TEXT("?? ReleaseZone: Zone ID=%d not found!"), ZoneID);
        return;
  }

    // ������� ������ ��� ����
    ActiveZones.Remove(ZoneID);
    UE_LOG(LogMusicManager, Log, TEXT("??? Released zone ID=%d"), ZoneID);

    // ���� ��������� ���� � ��������� Priority
    FindHighestPriorityZone(NextRequest, NextPriority);
    // ...
}

bool UMusicManager::FindHighestPriorityZone(...)
{
    // ? ��������: ���� �� Priority �� Value, � �� �� Key
    for (const auto& Pair : ActiveZones)
    {
        const FMusicRequest& Request = Pair.Value;
   
        if (Request.Priority > OutPriority)
        {
        OutPriority = Request.Priority;
          OutRequest = Request;
            bFound = true;
        }
    }
    // ...
}
```

### 3. MusicZoneVolume.h

```cpp
class AMusicZoneVolume : public ATriggerBox
{
private:
    // ? �����: ���������� ID ���� � MusicManager
    int32 RegisteredZoneID = -1;
};
```

### 4. MusicZoneVolume.cpp

```cpp
void AMusicZoneVolume::HandlePlayerEntered(APlayerController* PC)
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
   // ? ������������ ���� � ��������� ID
        RegisteredZoneID = Manager->RegisterMusicZone(
      ZoneMusic, Priority, TransitionType, 
            CrossfadeDuration, VolumeMultiplier
        );

        UE_LOG(LogTemp, Log, TEXT("? Entered MusicZone '%s' - ID: %d, Priority: %d"),
            *GetName(), RegisteredZoneID, Priority);
    }
}

void AMusicZoneVolume::HandlePlayerExited(APlayerController* PC)
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
  {
        if (RegisteredZoneID >= 0)
        {
   // ? Release �� ����������� ID
     Manager->ReleaseZone(RegisteredZoneID);

    UE_LOG(LogTemp, Log, TEXT("? Exited MusicZone '%s' - Released ID: %d"),
  *GetName(), RegisteredZoneID);

         RegisteredZoneID = -1; // ���������� ID
        }
    }
}
```

---

## ?? ������������

### ��������: ��� ���� � ���������� Priority

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Immediate
- Zone B: Priority 10, Music: town_second, Transition: Crossfade
```

**����:**
```
1. ������ � Zone A
2. ������ � Zone B (������������� � A)
3. ������� �� Zone A (������� � B)
```

**��������� ���������:**
```
1. Zone A: town_first ������ ?
2. Zone B: ��������� town_first ? town_second ?
3. Exit A: town_second ���������� ������ ?
```

**����� ����:**
```
[378] LogMusicManager: ?? Registered zone ID=0: town_first (Priority: 10)
[378] LogTemp: ? Entered MusicZone 'BP_StartTown_Music_C_2' - ID: 0, Priority: 10

[31] LogMusicManager: ?? Registered zone ID=1: town_second (Priority: 10)
[31] LogTemp: ? Entered MusicZone 'BP_StartTown_Music_Other_C_4' - ID: 1, Priority: 10
[31] LogMusicManager: ?? Highest priority zone: Priority=10, Music=town_second

[38] LogMusicManager: ??? Released zone ID=0 (Priority: 10)
[38] LogTemp: ? Exited MusicZone 'BP_StartTown_Music_C_2' - Released ID: 0
[38] LogMusicManager: ?? Highest priority zone: Priority=10, Music=town_second
```

**���������:** ? town_second ���������� ������!

---

## ?? �������������� ������������

### 1. ��������� ��������� ��� � ���������� Priority

������ ����� ����� **��������� ���** � ���������� �����������:

```
Zone A: Priority 10, Music: town_first
Zone B: Priority 10, Music: town_second
Zone C: Priority 10, Music: town_third

����� � A+B+C ������������:
- ActiveZones = {0: town_first, 1: town_second, 2: town_third}
- ������ ��������� ������������������ (town_third)
```

### 2. ���������� ������������ ��� ������

```
����� � A+B+C, ������� �� C:
- ActiveZones = {0: town_first, 1: town_second}
- ������������� �� town_second ?
```

### 3. ��������� ������ Priority

```
Zone A: Priority 10
Zone B: Priority 20
Zone C: Priority 15

����� � A+B+C:
- ActiveZones = {0: A(P:10), 1: B(P:20), 2: C(P:15)}
- ������ B (��������� Priority) ?
```

---

## ?? �������� �������������

������ ������� `RequestMusicChange` �� ��� ��������:

```cpp
// ? ����� �������� �������� (��� Blueprint-�������������)
void RequestMusicChange(USoundBase* NewMusic, int32 Priority, ...)
{
    // ������ ������ ��� RegisterMusicZone
    RegisterMusicZone(NewMusic, Priority, ...);
}
```

**��:** ����� ������������ `RegisterMusicZone` + `ReleaseZone(ID)` ��� ������� ��������!

---

## ?? �����

### ��������:

1. ? `TMap<Priority, Request>` - ������ ���� ���� �� Priority
2. ? `ReleaseZone(Priority)` ������� **���** ���� � ���� Priority
3. ? ��� �������� ����� ������ � ���������� Priority ������ ��������

### �������:

1. ? `TMap<UniqueID, Request>` - ���������� ID ��� ������ ����
2. ? `RegisterMusicZone` ���������� ID
3. ? `ReleaseZone(ID)` ������� **������** ������ ����
4. ? `FindHighestPriorityZone` ���� �� Priority �� Value

### ���������:

- ? **��������� ���** � ���������� Priority �������� ���������
- ? **���������� ������������** ��� ������ �� ���
- ? **��������� ��������** ���������
- ? **�������� �������������** ���������

---

**����:** 24 ������� 2025  
**������:** 1.4 (UNIQUE ID FIX)  
**������:** ? ��� ���� ����������!
