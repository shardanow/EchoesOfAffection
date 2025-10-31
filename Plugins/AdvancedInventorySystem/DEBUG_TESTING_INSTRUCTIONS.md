# ?? ������������ � ������ �������

## ��� ����������

��������� ��������� ���� �:
1. `CharacterNeedsComponent::ModifyNeedValue_Implementation()` 
2. `ItemEffectExecutorComponent::ExecuteNeedsModifications()`

������ �� ������ **�����** ��� �������� �����!

---

## ���� ��� ������������

### 1. ������������� Unreal Editor

**Live Coding / Hot Reload ������������ ��������� �������������.**

### 2. ������������ �������

- ������� ���������
- �������� ��� �� ������� (Apple)
- ������� "Use"

### 3. ��������� Output Log

������� **Window ? Developer Tools ? Output Log**

---

## ��������� ����

### ���� ��������� �� ����������:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Error: Target bp_MyAnimeCharacter_C_0 does NOT implement UNeedsSystemInterface!
```

**�������:** �������� ��������� `NeedsSystemInterface` � Blueprint ���������.

---

### ���� ��������� ���������� �� ������� ������:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Log: Target implements interface! Casting...
LogTemp: Log: Cast successful! Executing 1 needs modifications...
LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
```

**��:**
- **���** ���� "CharacterNeedsComponent::ModifyNeedValue_Implementation called!"

**�������:** ������� ���������� ������! ����� �������� ���� � `ModifyNeed` Blueprint �������.

---

### ���� ��� �������� ���������:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
LogTemp: Log: Target implements interface! Casting...
LogTemp: Log: Cast successful! Executing 1 needs modifications...
LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
LogTemp: Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: Needs.Hunger, Delta: 30.000000, Owner: bp_MyAnimeCharacter_C_0
LogTemp: Log: Current value: 70.000000, New value will be: 100.000000
LogTemp: Need value changed: Needs.Hunger 70.00 ? 100.00
```

---

## ��������� ��������

### �������� 1: ��������� �� ����������

**���:**
```
Error: Target does NOT implement UNeedsSystemInterface!
```

**�������:**
1. `BP_PlayerCharacter` ? **Class Settings**
2. **Interfaces** ? **Add** ? `NeedsSystemInterface`
3. **Compile** ? **Save**

---

### �������� 2: ������� ���������� ������

**���:**
```
Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
(�� ��� ������ CharacterNeedsComponent::ModifyNeedValue_Implementation)
```

**�������:**

� `BP_PlayerCharacter` ? **My Blueprint** ? **Interfaces** ? **NeedsSystemInterface** ? **ModifyNeed**:

**�������� ����:**
```
Event ModifyNeed
  ?
Get Component by Class (CharacterNeedsComponent)
  ?
Modify Need Value
  - Need Tag: Need Tag (�� ���������)
  - Delta: Delta Value (�� ���������)
```

---

### �������� 3: ��������� �� ������

**���:**
```
Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! ...
Error: Need data NOT FOUND for tag: Needs.Hunger
```

**�������:**
- ��������� ��� ��� `Needs.Hunger` **�����** ��������� � ����� � ����������
- **Project Settings** ? **Gameplay Tags** ? �������� `Needs.Hunger`
- ��������� ��� ��������� ��������������� (BeginPlay ������)

---

### �������� 4: Effect �� ��������

**���:**
```
Log: Executing 0 needs modifications...
```

**�������:**
- ������� Data Asset ������� (`DA_Effect_RestoreHunger`)
- **Needs Modifications** ? ��������� ��� ������ �� ������
- �������� �����������:
  - **Need Tag**: `Needs.Hunger`
  - **Modification Amount**: `30.0`
  - **bClamp Value**: ?

---

## �������� ��������� �����

���� ��� ��������, �� �������:

```
[2025.10.29-17.00.00:000][  0]LogTemp: Context Menu: Used item Apple, Success: true
[2025.10.29-17.00.00:001][  0]LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_Effect_RestoreHunger
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Target implements interface! Casting...
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Cast successful! Executing 1 needs modifications...
[2025.10.29-17.00.00:001][  0]LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
[2025.10.29-17.00.00:002][  0]LogTemp: Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: Needs.Hunger, Delta: 30.000000, Owner: bp_MyAnimeCharacter_C_0
[2025.10.29-17.00.00:002][  0]LogTemp: Log: Current value: 70.000000, New value will be: 100.000000
[2025.10.29-17.00.00:003][  0]LogNeedsSystem: Need value changed: Needs.Hunger from 70.00 to 100.00
```

---

## ��������� ����

1. ������������� ��������
2. ������������ �������
3. ����������� ���� �� Output Log
4. ��������� ��� ���� - � ����� ��� ��������!

**� ������ ������ �� ����� ������ ��������! ??**
