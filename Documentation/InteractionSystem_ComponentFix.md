# ����������� ��������� InteractableComponent

## ��������

��� ������������� `InteractableComponent` � Blueprint �������, ������� �������������� �� ������������ ����� ��� ������������� � ���������� `EInteractionResult::NotInteractable`.

## �������

`InteractionComponent` �������� ������ �������, �������� ����������� `IInteractableInterface`, � �� ����� ����������, ������� ��������� ���� ���������.

## �������

��������� ��������� ������ � `InteractionComponent.cpp`:

### 1. `IsActorInteractable`
```cpp
bool UInteractionComponent::IsActorInteractable(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	// Check if actor implements the interface directly
	if (Actor->Implements<UInteractableInterface>())
	{
		return true;
	}

	// Check if actor has an InteractableComponent
	UInteractableComponent* InteractableComp = Actor->FindComponentByClass<UInteractableComponent>();
	return InteractableComp != nullptr;
}
```

### 2. `FindInteractables`
��������� ������ ������ `InteractableComponent`, ���� ����� �� ��������� ��������� ��������.

### 3. `UpdateFocusedInteractable`
�������� ��� ������ ��� � ������ ����������� ����������, ��� � � ������������.

### 4. `TryInteract`, `CompleteInteraction`, `CancelInteraction`
��� ������ �������������� ��������� ��� ��������� ����� ��������.

## ���������

������ ������� ��������� �������� �:
- ��������, ������������ `IInteractableInterface` �������� (C++)
- �������� � `InteractableComponent` (Blueprint)

## ������������� � Blueprint

1. �������� Actor Blueprint
2. �������� ��������� "Interactable Component"
3. ��������� Interaction Data � Details
4. ���������� �������:
   - On Begin Interact
   - On End Interact
   - On Cancel Interact
   - On Focused
   - On Unfocused

����� ������������� ����� ��������� �������� ��������������!

## ����������� ������

- �������� `#include "Interaction/InteractableComponent.h"` � `InteractionComponent.cpp`
- ��� ������ ���������� ������ �������� ���������� `UObject*` (��������� ��� �����)
- �������������� �������� ������������� � ������������� C++ ������������

## ���� �����������
2024 (������� ����)
