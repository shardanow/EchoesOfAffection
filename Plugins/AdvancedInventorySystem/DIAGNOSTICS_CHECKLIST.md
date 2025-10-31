# ?? ����������� ��������: ������� �� �����������

## ��������

`ExecuteItemEffects` ���������� �������, �� ������ �� ����������� � `CharacterNeedsComponent`.

## �������

`ItemEffectExecutorComponent::ExecuteNeedsModifications()` ���������:
```cpp
if (!Target->Implements<UNeedsSystemInterface>())
{
    return; // ? ������� �����!
}
```

���� ��������� �� ���������� ��������� � Blueprint, ������ **����� ������������**.

---

## ? ��������� �����������

### ��� 1: ��������� ��������� � Blueprint

1. ������� `BP_PlayerCharacter`
2. **Class Settings** ? **Details** ������ ? **Interfaces**
3. **����������� ������ ����:** `NeedsSystemInterface`

**���� ���** ? Add ? ������� `NeedsSystemInterface` ? Compile

---

### ��� 2: ��������� ���������� ������� ����������

� **My Blueprint** ������ ������ ����:

```
Interfaces
  ?? NeedsSystemInterface
      ?? ModifyNeed (������ ���� �����������!)
      ?? GetNeedValue (������ ���� �����������!)
```

**���� ������� ����, �� ������:**

#### ModifyNeed:

������� ���� ? ������ ���� ����:

```
Event ModifyNeed
  ?? Need Tag (input parameter)
  ?? Delta Value (input parameter)
  ?? b Clamp (input parameter)
  
Get Component by Class
  ?? Character Needs Component
  
Modify Need Value
  ?? Need Tag: ���������� �������� Need Tag
  ?? Delta: ���������� �������� Delta Value
```

**���������:** ���� �� ������� �� ���������� � �����?

#### GetNeedValue:

```
Event GetNeedValue
  ?? Need Tag (input parameter)
  
Get Component by Class
  ?? Character Needs Component
  
Get Need Value
  ?? Need Tag: ���������� �������� Need Tag
  
Return Node
  ?? ���������� Return Value �� Get Need Value
```

---

### ��� 3: �������� ����������� ��� �������

� `BP_PlayerCharacter` ? Event Graph:

#### �������� ���� � ModifyNeed:

```
Event ModifyNeed
?
Print String ("ModifyNeed called!")
  ?
Print String (NeedTag.ToString())
  ?
Print String (DeltaValue)
  ?
Get Component by Class...
```

#### �������� ���� � OnItemUsedHandler:

```
OnItemUsedHandler
  ?
Print String ("OnItemUsedHandler called!")
  ?
Print String (Item.GetName())
  ?
Branch...
```

---

### ��� 4: ��������� Data Asset �������

������� `DA_Effect_RestoreHunger`:

**����������� ���������:**
- ? **Trigger Type**: `OnUse`
- ? **Target Type**: `Self`
- ? **Needs Modifications**:
  - ? **[0] Need Tag**: `Needs.Hunger` (������ ����������!)
  - ? **[0] Modification Amount**: `30.0` (��� ���� ��������)
  - ? **[0] bClamp Value**: ? (�������)

**��������� ����:**
- Project Settings ? Gameplay Tags
- ������ ������������ ��� `Needs.Hunger`

---

### ��� 5: ��������� �������� ������� � ��������

������� `ItemDataAsset` (����):

**Item Effects** ������ ���������:
- ? `DA_Effect_RestoreHunger`

���� ������ ������ ? ������ **+** ? ������� ������

---

### ��� 6: ��������� ���������� �� ���������

� `BP_PlayerCharacter` ? **Components** ������ ������ ����:

- ? `CharacterNeedsComponent`
- ? `InventoryComponent`
- ? `ItemEffectExecutorComponent`

**���� ���** ? Add Component ? ����� � ��������

---

## ?? ����������� �������

### �������� ���� � C++

���� � ��� ���� ������ � ���������� �������, �������� ����:

**� `ItemEffectExecutorComponent.cpp`:**

```cpp
void UItemEffectExecutorComponent::ExecuteNeedsModifications(UItemEffectDataAsset* Effect, AActor* Target)
{
    if (!Effect || !Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("ExecuteNeedsModifications: Effect or Target is null!"));
        return;
    }

    // ?? �������� ��������
    if (!Target->Implements<UNeedsSystemInterface>())
    {
  UE_LOG(LogTemp, Error, TEXT("ExecuteNeedsModifications: Target %s does NOT implement INeedsSystemInterface!"), *Target->GetName());
        return;
    }

  UE_LOG(LogTemp, Log, TEXT("ExecuteNeedsModifications: Target %s DOES implement interface!"), *Target->GetName());

    INeedsSystemInterface* NeedsInterface = Cast<INeedsSystemInterface>(Target);
    if (!NeedsInterface)
    {
        UE_LOG(LogTemp, Error, TEXT("ExecuteNeedsModifications: Failed to cast to INeedsSystemInterface!"));
 return;
    }

    // Execute all needs modifications
    for (const FNeedsModification& NeedMod : Effect->NeedsModifications)
    {
        if (NeedMod.NeedTag.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("Modifying need: %s by %f"), *NeedMod.NeedTag.ToString(), NeedMod.ModificationAmount);
            NeedsInterface->Execute_ModifyNeed(Target, NeedMod.NeedTag, NeedMod.ModificationAmount, NeedMod.bClampValue);
        }
    }
}
```

---

## ?? ��������� ���� (���� ��� ��������)

```
LogTemp: OnItemUsedHandler called!
LogTemp: Bread
LogTemp: ExecuteNeedsModifications: Target BP_Player_C_0 DOES implement interface!
LogTemp: Modifying need: Needs.Hunger by 30.000000
LogTemp: ModifyNeed called!
LogTemp: Needs.Hunger
LogTemp: 30.0
LogTemp: Need value changed: Needs.Hunger 70.00 ? 100.00
```

---

## ? ���� ������ ��� ������

```
LogTemp: Error: ExecuteNeedsModifications: Target BP_Player_C_0 does NOT implement INeedsSystemInterface!
```

**������ ��������� �� �������� ��� �� �������������!**

### �������:

1. **Class Settings** ? **Interfaces** ? **Add** ? `NeedsSystemInterface`
2. **Compile** Blueprint
3. **Save** Blueprint
4. **������������� ��������** (������ ���������)

---

## ?? �������������� ������� (���� ��������� �� ��������)

���� Blueprint ��������� �� �����-�� ������� �� ��������, �����:

### ������� A: ������� Blueprint Function Library

������� `BP_ItemEffectHelpers`:

```
Function: ApplyNeedEffect
  Inputs:
    - Target (Actor)
    - Need Tag (Gameplay Tag)
    - Amount (Float)
  
  Body:
  Get Component by Class (Target, CharacterNeedsComponent)
   ?
    Modify Need Value
      - Need Tag: Need Tag
    - Delta: Amount
```

�������� �� `OnItemUsedHandler`:

```
OnItemUsedHandler
?
Get Effects from Item
  ?
For Each (Effects)
    ?
    For Each (Needs Modifications)
        ?
    ApplyNeedEffect(Self, Need Tag, Amount)
```

### ������� B: ������ ����� � Blueprint

� `OnItemUsedHandler` ����� ����� `Execute Item Effects`:

```
Execute Item Effects
  ?
Get Item Data ? Get Effects By Trigger (OnUse)
  ?
For Each (Effects)
    ?
  Get Needs Modifications
        ?
  For Each (Modification)
   ?
            Get Character Needs Component
           ?
         Modify Need Value
    - Need Tag: Modification.Need Tag
         - Delta: Modification.Amount
```

---

## ?? �������� �������

- [ ] ��������� `NeedsSystemInterface` �������� � BP_PlayerCharacter
- [ ] ������� `ModifyNeed` ����������� � ������
- [ ] ������� `GetNeedValue` ����������� � ������
- [ ] ���������� ��������� (CharacterNeedsComponent, InventoryComponent, ItemEffectExecutorComponent)
- [ ] Data Asset ������� �������� (Trigger Type = OnUse, Target Type = Self)
- [ ] ������ �������� � ItemDataAsset
- [ ] ��� `Needs.Hunger` ����������
- [ ] ��� � ������� ��������� � ����� � �������
- [ ] ��������� ���� ��� �������
- [ ] Blueprint ������������� � ��������

---

## ?? ����� ������ ��������

**��������� ��������, �� ������� �� ����������� (������)!**

���������, ��� � ����� ������� `ModifyNeed` � `GetNeedValue` ���� **�������� ����**, � �� ������ ����� � Return Node!
