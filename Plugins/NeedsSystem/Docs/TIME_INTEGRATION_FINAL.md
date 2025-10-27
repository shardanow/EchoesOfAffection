# Needs System - Time Integration COMPLETE ?

## ��������� ������ v1.0.2

### ? ��� �����������

1. **Event-Driven Updates**
   - �������� �� `TimeOfDaySubsystem::OnHourChanged`
   - ���������� ������ ������� ��� (�� ������ ����!)
   - �������������� ���������� polling ��� �������� Time System

2. **Dynamic Time Scale Reading**
   - ������ `RealSecondsPerGameDay` �� �������� Time System
   - ������ `TimeScale` multiplier �����������
   - **��� hardcoded ��������** - �� �� ��������

3. **Optional Dependency**
   - �������� � � ��� Time System
   - Conditional compilation (`WITH_TIME_SYSTEM=1/0`)
   - Graceful fallback �� real-time mode

4. **Public API � TimeOfDaySubsystem**
   - �������� `GetSettings()` �����
   - ������ � `TimeConversion` ���������� ��� ������� ������

### ?? ��� ������������

#### � Time System (�������������):

```
DA_NeedsSettings:
Use Game Time: ?

DA_TimeOfDaySettings:
  Real Seconds Per Game Day: 1200  (20 ����� = 1 ����)
  Time Scale: 1.0

���������:
  - 1 ������� ��� = 50 �������� ������
  - ����������� ���������������� � ������ ����/����
  - Decay rate 5.0/��� = -5 ������� ������ 50 ������
```

#### ��� Time System (fallback):

```
DA_NeedsSettings:
  Use Game Time: ?
  Real Time Scale: 1.0  (1 ������� ��� = 1 �������� ���)

���������:
  - ���������� �� Time System
  - Decay rate 5.0/��� = -5 ������� ������ �������� ���
```

### ?? ������� ��������

**������� ����� (10 ����� �� ����):**
```
Real Seconds Per Game Day: 600
Time Scale: 1.0
? 1 ������� ��� = 25 �������� ������
```

**2x ��������:**
```
Real Seconds Per Game Day: 1200
Time Scale: 2.0
? 1 ������� ��� = 25 �������� ������
```

**�������� �����:**
```
Use Game Time: ?
Real Time Scale: 1.0
? 1 ������� ��� = 1 �������� ���
```

### ?? ���

**�������� �� �������:**
```cpp
#if WITH_TIME_SYSTEM
UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>();
TimeSubsystem->OnHourChanged.AddDynamic(this, &UNeedsSubsystem::OnTimeSystemHourChanged);
#endif
```

**������ ��������:**
```cpp
const UTimeOfDaySettings* Settings = TimeSubsystem->GetSettings();
float RealSecondsPerGameDay = Settings->TimeConversion.RealSecondsPerGameDay;
float TimeScale = Settings->TimeConversion.TimeScale;
```

### ?? �������

```
needs.debug 1  // �������� debug overlay
```

�������� � �����:
```
? NeedsSubsystem: Successfully subscribed to Time System events
? NeedsSubsystem: Updated needs for 1 game hour (Total: X hours)
```

### ?? ������������������

**� Time System:**
- ? Zero polling overhead
- ? ���������� ����� 1 ��� � ������� ���
- ? LOD ������� �������� (Near/Far/Parked)

**��� Time System:**
- Polling � ������������� �������� (default 10 Hz)
- Batch processing
- LOD �����������

### ? CHECKLIST ����������

- [x] �������� `GetSettings()` � TimeOfDaySubsystem
- [x] �������� �� `OnHourChanged` event
- [x] ������������ ������ `RealSecondsPerGameDay`
- [x] ������������ ������ `TimeScale`
- [x] Conditional compilation (`WITH_TIME_SYSTEM`)
- [x] Optional plugin dependency
- [x] Event-driven mode (no polling)
- [x] Fallback mode (real time)
- [x] ������������ ���������
- [x] ������ ���������� �������

### ?? ��������� ����

1. ? �������� �������� (���� ������)
2. ? ������ ��� ������
3. ? �������� ������
4. ? ��������� `DA_NeedsSettings`:
   - `Use Game Time = ?`
5. ? ��������� `DA_TimeOfDaySettings`:
   - `Real Seconds Per Game Day = 1200`
   - `Time Scale = 1.0`
6. ? ����������!

### ?? �������

**� Time System:**
```
GameSecondsPerRealSecond = (86400 / RealSecondsPerGameDay) * TimeScale

������ (1200s/����, 1.0x):
  = (86400 / 1200) * 1.0
  = 72 ������� ������ �� �������� �������
  = 0.02 ������� ���� �� �������� �������
  = 50 �������� ������ �� ������� ���
```

**��� Time System:**
```
DeltaGameHours = DeltaTime * (RealTimeScale / 3600)

������ (RealTimeScale 1.0):
  = DeltaTime * (1.0 / 3600)
  = 1 �������� ��� = 1 ������� ���
```

---

## ? ������!

**������:** 1.0.2 (COMPLETE)  
**����:** 25 ������� 2025  
**������:** Production Ready  
**��� TODO ���������!** ??
