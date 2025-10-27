# ?? MusicManager: ����������� ������ ���� ��� ���������� Priority

## ? ��������

��� ������� **���������� ��� � ���������� Priority**, ������� �������� **������** ������������������ ����, � �� **���������** (����� �����).

### ���� ����������:

```
[872] Registered zone ID=0: town_first (Priority: 10) ? ���� A
[872] Highest priority zone: Priority=10, Music=town_first ?

[596] Registered zone ID=1: town_second (Priority: 10) ? ���� B
[596] Highest priority zone: Priority=10, Music=town_first ? ? ��������!
      ^^^ ������ ����: town_second
```

**��� �����������:**
1. ? ������ � ���� A (ID=0, Priority: 10) ? Plays town_first
2. ? ������ � ���� B (ID=1, Priority: 10) ? ��������������
3. ? `FindHighestPriorityZone` �������� **������** (ID=0, town_first)
4. ? ������ **�� ��������**, ���� �� � ����� ����!

---

## ?? �������� �������

### ������ ����������:

```cpp
// ? ��������: ��� ���������� Priority ���������� ������ ����
bool UMusicManager::FindHighestPriorityZone(...)
{
    for (const auto& Pair : ActiveZones)
    {
 if (Request.Priority > OutPriority)
        {
            OutPriority = Request.Priority;
  OutRequest = Request;
      bFound = true;
        }
        // ? ���� Priority ����� - ������ �� ������!
        // ���������: ���������� ������ ��������� ����
  }
}
```

**��������:** TMap �� ����������� ������� ��������, �� ������ ��� ������� ����������.  
���� ���� A ���� ��������� ������, ��� � ����� ������� ������.

---

## ? �������: �������� ���� � ���������� ID

### ������:

1. **��������� Priority** ? ������ ���������
2. **���������� Priority** ? �������� ���� � **������� ID** (�����)

### ����� ����������:

```cpp
bool UMusicManager::FindHighestPriorityZone(FMusicRequest& OutRequest, int32& OutPriority)
{
    OutPriority = -1;
    bool bFound = false;
    int32 SelectedZoneID = -1; // ? ����������� ID ��������� ����

    for (const auto& Pair : ActiveZones)
    {
        const FMusicRequest& Request = Pair.Value;
        const int32 ZoneID = Pair.Key; // ? �������� ID ����
    
        // ��������� ���������
        if (Request.Priority > OutPriority)
        {
   // ����� ��������� ��������� - ���� ��� ����
            OutPriority = Request.Priority;
      OutRequest = Request;
            SelectedZoneID = ZoneID;
  bFound = true;
        }
        else if (Request.Priority == OutPriority && ZoneID > SelectedZoneID)
     {
// ? ���������� ��������� - ���� ���� � ������� ID (�����)
      OutRequest = Request;
     SelectedZoneID = ZoneID;
  UE_LOG(LogMusicManager, Log, TEXT("?? Same priority %d - selected newer zone ID=%d (%s)"),
        OutPriority, ZoneID, *Request.Music->GetName());
        }
    }

    if (bFound)
    {
  UE_LOG(LogMusicManager, Log, TEXT("?? Highest priority zone: ID=%d, Priority=%d, Music=%s"),
       SelectedZoneID, OutPriority, *OutRequest.Music->GetName());
    }

    return bFound;
}
```

---

## ?? ���������: �� vs �����

### ��������: ��� ���� � Priority 10

| �������� | ActiveZones | ������ (�����) | ����� (������) |
|----------|-------------|----------------|----------------|
| Enter Zone A (ID=0) | `{0: A(P:10)}` | Plays A ? | Plays A ? |
| Enter Zone B (ID=1) | `{0: A(P:10), 1: B(P:10)}` | ? Plays A | ? **Crossfade A?B** |
| Exit Zone A | `{1: B(P:10)}` | ? Plays B | ? Plays B |

### ����� ���� (����� �����������):

```
[872] Registered zone ID=0: town_first (Priority: 10)
[872] ?? Highest priority zone: ID=0, Priority=10, Music=town_first ?

[596] Registered zone ID=1: town_second (Priority: 10)
[596] ?? Same priority 10 - selected newer zone ID=1 (town_second) ? ? �����!
[596] ?? Highest priority zone: ID=1, Priority=10, Music=town_second ? ? ���������!
[596] ?? StartCrossfade: NewMusic=town_second, Duration=2.00 ? ? ���������!
```

---

## ?? �������� ���������

### ��������� �������� ID ��� ���������� Priority:

```cpp
// ? ������: ������������ ���������� Priority
if (Request.Priority > OutPriority)
{
    OutRequest = Request;
}

// ? �����: ��� ���������� Priority - ���� ���� � ������� ID
if (Request.Priority > OutPriority)
{
    OutRequest = Request;
    SelectedZoneID = ZoneID;
}
else if (Request.Priority == OutPriority && ZoneID > SelectedZoneID)
{
    // ���������� ��������� - �������� ����� ����
    OutRequest = Request;
    SelectedZoneID = ZoneID;
}
```

---

## ?? ������������

### ���� 1: ������� ����� ������ � ���������� Priority

```
Setup:
- Zone A: Priority 10, Music: town_first, Transition: Immediate
- Zone B: Priority 10, Music: town_second, Transition: Crossfade
```

**��������:**
```
1. ������ � Zone A
2. ������ � Zone B (�������������)
3. ������� �� Zone A
```

**��������� ��������� (����� �����������):**
```
1. Enter A: town_first ������ ?
2. Enter B: ��������� town_first ? town_second ?
3. Exit A: town_second ���������� ������ ?
```

**����:**
```
? Registered zone ID=0: town_first (Priority: 10)
? Registered zone ID=1: town_second (Priority: 10)
?? Same priority 10 - selected newer zone ID=1 (town_second)
?? StartCrossfade: NewMusic=town_second, Duration=2.00
?? Crossfade: Progress=51%, NewVol=0.52, OldVol=0.48
? Crossfade completed - Final volume: 1.00
```

### ���� 2: ������ Priority

```
Setup:
- Zone A: Priority 10
- Zone B: Priority 20
- Zone C: Priority 15
```

**��������:**
```
1. ������ � A, B, C ������������
```

**��������� ���������:**
```
Plays B (��������� Priority: 20) ?
```

**����:**
```
? Registered zone ID=0: zone_a (Priority: 10)
? Registered zone ID=1: zone_b (Priority: 20)
? Registered zone ID=2: zone_c (Priority: 15)
?? Highest priority zone: ID=1, Priority=20, Music=zone_b
```

---

## ?? ������ ID �����?

### ID = ������� �����������

```cpp
// ������� ������������� ��� ������ �����������
int32 NextZoneID = 0;

int32 RegisterMusicZone(...)
{
    int32 ZoneID = NextZoneID++; // 0, 1, 2, 3...
    ActiveZones.Add(ZoneID, Request);
    return ZoneID;
}
```

**������� ID = ����� ���� = ����� ���������� ������!**

### ������:

```
Zone A ����������������: ID=0, Priority=10
Zone B ����������������: ID=1, Priority=10
Zone C ����������������: ID=2, Priority=10

��� ������:
- Priority ���������� (10)
- ID: 0 < 1 < 2
- �������� ID=2 (Zone C) - ����� �����!
```

---

## ?? ����������� ������

### ������� ������ ����:

1. **��������� Priority** ? ������ ���������
2. **���������� Priority** ? ��������� **�����** ���� (������� ID)
3. **��� ���** ? ������ ���������������

### �������:

```
Situation 1: ������ Priority
- Zones: A(P:10), B(P:20), C(P:15)
- Winner: B (Priority: 20) ?

Situation 2: ���������� Priority
- Zones: A(ID:0, P:10), B(ID:1, P:10), C(ID:2, P:10)
- Winner: C (ID: 2, newest) ?

Situation 3: ��������� ������
- Zones: A(ID:0, P:10), B(ID:1, P:20), C(ID:2, P:10)
- Winner: B (Priority: 20) ?
  (Priority ������ ID!)
```

---

## ?? �����

### ��������:

1. ? ��� ���������� Priority ���������� **������** ����
2. ? ��� ����� � ����� ���� ������ **�� ��������**
3. ? ��������� **�� ����������** ����� ������ � ���������� Priority

### �������:

1. ? ����������� **ID** ��������� ����
2. ? ��� ���������� Priority ? �������� ���� � **������� ID** (�����)
3. ? ��������� **��������** ���������

### ���������:

- ? **���������� �����** ���� ��� ���������� Priority
- ? **��������� ��������** ����� ����� ������
- ? **���� ����������** ����� ���� ������� � ������
- ? **������ ��������** ��� ����� � ����� ����

---

**����:** 24 ������� 2025  
**������:** 1.5 (ZONE SELECTION FIX)  
**������:** ? ��� ���� ����������! ������� �������� ��������!
