# ?? MusicZoneVolume - ���������� ���������� (��� AudioVolume)

## ? ��� ���� ����������:

### ? �������� ������ ������:
1. **DrawDebugHelpers � Tick** ? �������� ���� � ����
2. **Persistent drawing ������ ����** ? ������� CPU
3. **Billboard Component** ? �������� ���������
4. **Ƹ���� ���� ��������** ? Debug ��������� �� ���������
5. **������������ ������������** ? �� ������������ ����������� ������� UE

### ? ����� ���������� (����������):

������ `MusicZoneVolume` �������� **����� ��� ��**, ��� ����������� `AudioVolume`:

- ? **������ Brush Component** - ����������� ������� UE
- ? **�������� Tick** - ��� ��������
- ? **�������� DrawDebug** - ��� �����
- ? **����� ����� Show ? Volumes** - ����������� ������
- ? **���� ������� �� ����������** - �������������
- ? **������� ����������** - �� �����

---

## ?? ��� ������������:

### 1. �������� Blueprint ����

```
Content Browser ? ��� ? Blueprint Class
?? Parent Class: MusicZoneVolume
?? Name: BP_MusicZone_Town
```

### 2. ��������� ����

```
Details Panel:
?? Zone Music: [��� ����� asset]
?? Priority: 10 (0-100)
?? Transition Type: Crossfade
?? Crossfade Duration: 2.0
?? Volume Multiplier: 1.0
?? Is Active: ? true
```

### 3. ���������� �� ������

```
1. ���������� BP_MusicZone_Town �� �������
2. ����������� Brush Editing ��� ��������� �����
3. ��� ������ ������������� (R)
```

### 4. �������� ������������

**� ���������:**
```
Viewport ? Show ? Volumes ? ?
```

**��� ����� �������:**
```
show VOLUMES
```

**������!** ���� ����� ��� �������������� ������� box!

---

## ?? �������� ��������� (��������������)

���� ���� **������������� ��������** � ����������� �� Priority:

| Priority | ���� | �������� |
|----------|------|----------|
| 0-20 | ?? ������-������� | ������� ������, ������ ��������� |
| 21-40 | ?? ������� | �������, �������-������ |
| 41-60 | ?? ����-���������� | ������ ����, ������� |
| 61-80 | ?? ���������� | �������, �������-������� |
| 81-100 | ?? �����-���������� | �����, ��������� ��������� |

**���������� ����** (`bIsActive = false`):
- ?? **�������** - ���� ���������

---

## ?? ��������� ����� ����

### ������ 1: Brush Editing (�����)

```
1. �������� ����
2. Modes Panel ? Brush Editing
3. �������������� �������/�����/����
4. F5 - Apply changes
```

### ������ 2: Scale (������)

```
1. �������� ����
2. ������� R (Scale tool)
3. ������������� �� ����
```

### ������ 3: Manual (������ ��������)

```
Details ? Transform:
?? Location: (X, Y, Z)
?? Rotation: (Pitch, Yaw, Roll)
?? Scale: (X, Y, Z)
```

---

## ?? ������� �������������

### ������ 1: ����� (ambient ������)

```cpp
Name: BP_MusicZone_Town
Priority: 10
Zone Music: Town_Peaceful_Ambient
Transition Type: Crossfade
Crossfade Duration: 3.0
Volume Multiplier: 0.8
������: ������� (���� �����)
����: ������-�������
```

### ������ 2: ����-�����

```cpp
Name: BP_MusicZone_Boss
Priority: 100
Zone Music: Boss_Epic_Battle
Transition Type: Immediate
Volume Multiplier: 1.0
������: ������� (�����)
����: �����-����������
```

### ������ 3: ��������� �������

```cpp
Name: BP_MusicZone_Secret
Priority: 50
Zone Music: Mystery_Theme
Transition Type: Crossfade
Crossfade Duration: 2.0
bShowDebugInfo: true ? ���������� ��������� �� ������
������: ��������� (�������)
����: ����-����������
```

---

## ?? ������������������

### ������ ������ (DrawDebug):
- ? Tick ������ ����
- ? DrawDebugBox ? 1
- ? DrawDebugSphere ? 1
- ? DrawDebugArrow ? 1
- ? DrawDebugLine ? 12
- ? DrawDebugString ? 1
- **�����:** ~16 draw calls ������ ����!
- **FPS drop:** -10-20 FPS ��� 5+ �����

### ����� ������ (Brush Component):
- ? ��� Tick
- ? ��� DrawDebug
- ? ������ ����������� Brush ���������
- **�����:** 0 �������������� draw calls!
- **FPS drop:** 0 FPS (��� �������)

---

## ?? ��������� ��������

| ������ | DrawDebug (������) | Brush Component (�����) |
|--------|-------------------|------------------------|
| **������������������** | ? ���� | ? �������� |
| **������** | ? ������ debug ��������� | ? ������� |
| **������������** | ? ���������, ������� | ? ������, ����������� |
| **�������������** | ? �� ��� � UE | ? ��� AudioVolume |
| **��������������** | ? ������ | ? Brush Editing |
| **���** | ? 300+ ����� | ? ~200 ����� |

---

## ?? ��� �������� � ���������

### � ���������� Show ? Volumes:

```
     [�������������� ������� box]
  
?? Priority 10: ������-�������
?? Priority 50: ����-����������
?? Priority 100: �����-����������
?? Inactive: �������
```

### ����� �������� ��������� ���:

```
    ?????????????????????????
    ?  Town (Priority 10)  ?  ? �������, �������
    ?  ????????????????     ?
    ?  ? Boss Arena   ?     ?  ? ���������, ���������� (100)
    ?  ? (Priority 100) ?     ?
    ?  ????????????????     ?
 ?????????????????????????

? ����� � ���� Boss ? ������ Boss ������
? ����� ����� ? ������ Town ������
```

---

## ?? �������

### ���������� �������:

```cpp
// �������� ��� Volumes
show VOLUMES

// �������� collision (������������)
show COLLISION

// �������� bounds
show BOUNDS

// ������� ����������� �������
Music.Debug

// ���������� ������
Music.Stop
```

### Debug ��������� �� ������:

```cpp
Details ? Show Debug Info: ? true
```

��� �����/������ �� ���� �������� ���������:
- ?? **Entered: BP_MusicZone_Town (Priority: 10)**
- ?? **Exited: BP_MusicZone_Town**

---

## ?? Blueprint �������

����� �������������� � Blueprint:

### OnPlayerEntered

```blueprint
Event OnPlayerEntered
?? Player Controller: APlayerController
?? Your custom logic here
```

**������� �������������:**
- �������� UI ���������
- ������� ���������
- ������������ �������
- ���������� ���������

### OnPlayerExited

```blueprint
Event OnPlayerExited
?? Player Controller: APlayerController
?? Your custom logic here
```

**������� �������������:**
- ������ UI
- ������������ ���������
- �������������� �������

---

## ?? ���������

### �������� ���������:

| �������� | ��� | �������� | �� ��������� | �������� |
|----------|-----|----------|--------------|----------|
| **Zone Music** | USoundBase* | - | null | ������ ��� ���� |
| **Priority** | int32 | 0-100 | 10 | ��������� ���� |
| **Transition Type** | Enum | - | Crossfade | ��� �������� |
| **Crossfade Duration** | float | 0.1-10.0 | 2.0 | ������������ ���������� |
| **Volume Multiplier** | float | 0.0-1.0 | 1.0 | ��������� ������ |
| **Is Active** | bool | - | true | ������� �� ���� |
| **Player Only** | bool | - | true | ������ ��� ������ |
| **Show Debug Info** | bool | - | false | ���������� ��������� |

---

## ?? ������ ��������

### ? DO (�������):

1. **����������� ����������** ����������:
   - 0-20: Ambient ������
   - 20-40: �������
   - 40-60: �������
   - 60-80: ��������
   - 80-100: �����

2. **������������ ����** ��� ������� ���������:
   ```
 Town (10) ??overlap?? Battle (60)
   ```

3. **����������� Crossfade** ��� ambient:
   ```cpp
   Transition Type: Crossfade
   Crossfade Duration: 3.0 // ������ �������
   ```

4. **����������� Immediate** ��� ������:
   ```cpp
   Transition Type: Immediate
   // ������ ����� ������!
   ```

### ? DON'T (�� �������):

1. ? �� ���������� ���� � ���������� Priority
2. ? �� ������� ������� ��������� ����
3. ? �� ��������� ��������� ������
4. ? �� ����������� Priority > 100

---

## ?? Troubleshooting

### ��������: "�� ���� ���� � ���������"

**�������:**
```
Show ? Volumes ? ? �������� �������!
```

### ��������: "���� �� �����������"

**�������:**
1. ? `Is Active` = true?
2. ? `Zone Music` ���������?
3. ? Collision �������?
4. ? MusicManager ����������?

**�������� � �����:**
```
LogTemp: MusicZoneVolume 'BP_MusicZone_Town' initialized - Priority: 10, Music: Town_Theme
```

### ��������: "������ �� ��������"

**�������:**
1. �������� ������� (~)
2. �������: `Music.Debug`
3. ���������:
   - Current Music
   - Active Zones
   - Priorities

---

## ?? �����������

### ������������:

```
AActor
?? AVolume (Brush Component)
      ?? AMusicZoneVolume (��� �����)
```

### ����������:

```cpp
AMusicZoneVolume
?? UBrushComponent (�� AVolume)
?   ?? Collision: QueryOnly
?   ?? Overlap Events: true
?   ?? Visual: �������������� box
?
?? USceneComponent (Root)
```

### ��������������:

```
Player ? Overlap ? MusicZoneVolume
   ?
            HandlePlayerEntered
 ?
       MusicManager::RequestMusicChange
?
      Priority Check
         ?
               Music Change!
```

---

## ? �����

### ��� ��������:

1. ? **������������������** - ��� �����
2. ? **��������** - �������� ��� AudioVolume
3. ? **������������** - ����������� ������� UE
4. ? **��������������** - Brush Editing
5. ? **�������** - ��� ����������
6. ? **�������������** - �� ����� UE �������������

### ��� ������:

1. ? DrawDebugHelpers
2. ? Tick
3. ? Billboard Component
4. ? ������� CPU
5. ? ������ ������
6. ? ��������� ������������

---

**MusicZoneVolume ������ �������� ��������� - ��� ����������� UE Volume!** ???

**����:** 23 ������� 2025  
**������:** 4.0 (���������� ����������)
