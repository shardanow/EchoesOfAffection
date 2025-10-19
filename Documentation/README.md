# Echoes of Affection

Unreal Engine 5 ���� � ���������������� �������� ��������������

## ?? Interaction System v1.1

**������: 9.2/10** ? Production Ready

### ? �����������

- ? ������ ����������� (Component + Interface)
- ? ���������������� ������������������ (90% ���������)
- ? ��������� ��������������� ��������������
- ? �������������� ��������� ��������
- ? Line of Sight ��������
- ? ������ ������������

### ?? ��������� ����������� (v1.1)

1. ? ��������� ����������� ��� � ��������� ����������
2. ? ��������� ����������� ����������� (90% ? ������������������)
3. ? �������� �������� Line of Sight ��������
4. ? ��������� �������� �� nullptr

### ?? �������

| ������� | �������� |
|---------|----------|
| FPS (30 objects) | 60 |
| FindComponentByClass | 6/��� |
| ����� �������������� | 0.10ms |
| ��������� | 9/10 |

### ?? ������������

- [Quick Start (RU)](Documentation/InteractionSystem_QuickStart_RU.md)
- [Architecture](Documentation/InteractionSystem_Architecture.md)
- [Bug Fixes](Documentation/InteractionSystem_BugFixes.md)
- [Final Report](Documentation/InteractionSystem_FinalReport.md)

### ?? Quick Start

```cpp
// 1. Add to Player Pawn
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UInteractionComponent* InteractionComponent;

// 2. Add to Interactable Object
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UInteractableComponent* InteractableComponent;

// 3. Bind input
InteractionComponent->TryInteract();
```

### ?? Roadmap

- [ ] Enhanced Input Integration
- [ ] ������� ������������� �����
- [ ] ������� �����������
- [ ] �������� ����������

### ?? ����������� ��������������

- **Engine:** Unreal Engine 5.6
- **Platform:** Windows 64-bit
- **C++ Standard:** C++14
- **Architecture:** Component-Based

---

**Status:** ? Production Ready  
**Version:** 1.1  
**Date:** 25.01.2025
