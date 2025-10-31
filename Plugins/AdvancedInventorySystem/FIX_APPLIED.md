# ? ����������� ���������!

## ?? �������� ������� � ����������!

### ��� ���� �� ���:

� ����� �����:
```
LogTemp: -> Target implements interface! Casting...
LogTemp: Error: -> Cast to INeedsSystemInterface FAILED!
```

**��������:** C++ ������� ������� `Cast<INeedsSystemInterface>(Target)`, ������� **�� ��������** � Blueprint-����������� ����������!

Blueprint ���������� ������ ������� � C++ �����. ����� ������������ `Execute_` ������� ��������.

---

## ?? ��� ����������:

**����:**
```cpp
INeedsSystemInterface* NeedsInterface = Cast<INeedsSystemInterface>(Target);
if (!NeedsInterface) {
    return; // ? FAIL ��� Blueprint!
}
NeedsInterface->Execute_ModifyNeed(...);
```

**�����:**
```cpp
// ������ �������� Execute_ �������� �� UObject
INeedsSystemInterface::Execute_ModifyNeed(Target, NeedTag, Amount, bClamp);
```

---

## ?? ��������� ����:

### ������� 1: Hot Reload (���� �������� ������)

1. � ��������� ������ **Ctrl+Alt+F11** (Live Coding compile)
2. ��������� ����������
3. ������������ ������� �����

### ������� 2: ���������� ��������� (�������������)

1. **������� Unreal Editor**
2. **������� �����**
3. ������������ ������� Apple
4. ��������� ����

---

## ? ��������� ���� ����� �����������:

```
LogTemp: Context Menu: Used item Apple, Success: true
LogTemp: Warning: ExecuteNeedsModifications: Target=bp_MyAnimeCharacter_C_0, Effect=DA_RestoreHunger
LogTemp: Log: Target implements interface! Executing 1 needs modifications...
LogTemp: Log: Calling ModifyNeed: Tag=Needs.Hunger, Amount=30.000000
LogTemp: Warning: CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: Needs.Hunger, Delta: 30.000000, Owner: bp_MyAnimeCharacter_C_0
LogTemp: Log: Current value: 70.000000, New value will be: 100.000000
LogTemp: Need value changed: Needs.Hunger 70.00 ? 100.00
```

---

## ?? ������ ��� ��������:

### Blueprint Interface ������ � C++:

**�� ��������:**
```cpp
IMyInterface* Interface = Cast<IMyInterface>(Object); // ? Fail ��� BP!
```

**��������:**
```cpp
if (Object->Implements<UMyInterface>()) {
    IMyInterface::Execute_MyFunction(Object, params); // ? OK!
}
```

`Execute_` ������� ������������� ������������ UHT � �������� �:
- ? C++ ������������ ����������
- ? Blueprint ������������ ����������
- ? ���������� ������������

---

## ?? ������ ������ ��������!

����� ����������� ��������� � ����������:
1. ����������� ������� Apple
2. ����������� ������ ����������!
3. � ����� ����� ������ trace �������

**�������� ������! ??**
