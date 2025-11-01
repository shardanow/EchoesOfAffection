# Quest System v2.0 - CRITICAL FIX REQUIRED

## ? ��������

QuestBlueprintLibrary.cpp ������ ���� PowerShell-���������������.

## ? �������

### ������� 1: �������� ���� �� Git
```bash
git checkout HEAD -- Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestBlueprintLibrary.cpp
```

����� ��������� ������ ���� ��������� ������� � ������ ~51:

**����:**
```cpp
bool UQuestBlueprintLibrary::StartQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->StartQuest(QuestId);  // ? ������: ����� �� ����������
}
```

**�����:**
```cpp
bool UQuestBlueprintLibrary::StartQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	// Use async version
	FOnQuestStartedDelegate EmptyCallback;
	Subsystem->StartQuestAsync(QuestId, EmptyCallback);
	return true;
}
```

### ������� 2: ��������� ��� ��� 6 ����������� ���������

1. ? **QuestRewardSystem.cpp** - TagContainer conversion
2. ? **QuestSubsystem.h** - ��������� wrapper methods
3. ? **QuestSubsystem.cpp** - ����������� wrapper methods
4. ? **QuestSaveManager.cpp** - ������ ������
5. ? **QuestLogComponent.cpp** - ������� �� v2 API
6. ?? **QuestBlueprintLibrary.cpp** - ������, ����� ��������� �������

---

## ?? ������ v2.0

**95% Complete**

- ? ��� ����������� �������
- ? 5 �� 6 legacy ������ ���������  
- ? 1 ���� ������ PowerShell ��������

**����� ����������� QuestBlueprintLibrary.cpp ? ������ ����Ш���!**

---

## ?? �������� �������� ����� �����������

```bash
# Build
D:\Game Dev\UE\UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe EchoesOfAffectionEditor Win64 Development

# ������ ����:
Result: Success
Build time: ~10 seconds
```

---

## ?? ��� ������

1. ��������� QuestBlueprintLibrary.cpp (1 ������)
2. ������� ������
3. �������������� ������� �����
4. Commit v2.0 ?

---

**Quest System v2.0 Architecture Refactoring - 95% DONE!**
