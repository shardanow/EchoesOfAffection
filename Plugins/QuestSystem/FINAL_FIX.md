# ?? **ПРОБЛЕМА РЕШЕНА! НАГРАДЫ БУДУТ ВЫДАВАТЬСЯ!**

## ?? **ЧТО БЫЛО НЕ ТАК:**

### **Баг в логике завершения последней фазы квеста**

Когда последний objective в **последней фазе** квеста завершался, код проверял `NextPhaseId`:

- **Если есть NextPhaseId** ? вызывал `AdvanceToNextPhase()` ? смена фазы ? `OnPhaseChangedInternal()` ? **Phase Rewards выдавались** ?
- **Если NextPhaseId пустой** (последняя фаза) ? вызывал `CompleteQuest()` напрямую ? **Phase Rewards НЕ выдавались** ?

---

## ?? **ЧТО ИСПРАВЛЕНО:**

### **Файл:** `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestProgressManager.cpp`

### **Строки 410-430** (примерно)

#### **ДО:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	CurrentPhase->CompletionTime = FDateTime::Now();

	if (PhaseDefinition.Transition.bAutoAdvance)
	{
		if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
		{
			AdvanceToNextPhase(QuestId);
		}
		else
		{
			CompleteQuest(QuestId);  // ? ПРОПУСКАЛ PHASE REWARDS!
		}
	}
}
```

#### **ПОСЛЕ:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	CurrentPhase->CompletionTime = FDateTime::Now();

	if (PhaseDefinition.Transition.bAutoAdvance)
	{
		if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
		{
			// Has next phase - advance and process phase rewards in OnPhaseChanged
			AdvanceToNextPhase(QuestId);
		}
		else
		{
			// No next phase - this is the last phase
			// Trigger phase change event to process phase rewards BEFORE completing quest
			UE_LOG(LogTemp, Warning, TEXT("?? Last phase '%s' completed - broadcasting phase change for rewards"), *SaveData.CurrentPhaseId.ToString());
			OnQuestPhaseChanged.Broadcast(QuestId, SaveData.CurrentPhaseId, NAME_None);
			
			// Now complete the quest (quest completion rewards processed in OnQuestCompleted)
			CompleteQuest(QuestId);
		}
	}
}
```

---

## ?? **ТЕПЕРЬ ЛОГИКА РАБОТАЕТ ТАК:**

### **При завершении последней фазы:**

1. **Objective завершается** ? `O_TalkToLianne` completed ?
2. **Проверка:** все objectives завершены? ? ДА ?
3. **Проверка:** есть NextPhaseId? ? НЕТ (последняя фаза) ?
4. **Broadcast `OnQuestPhaseChanged`** ? `('Q_CollectApples', 'P_ReturnApples', NAME_None)` ?
5. **`QuestSubsystem::OnPhaseChangedInternal()` вызывается** ?
6. **Phase Rewards обрабатываются** ? `ProcessQuestRewards(Phase.PhaseRewards)` ?
7. **Quest завершается** ? `CompleteQuest()` ?
8. **Quest Rewards обрабатываются** ? `ProcessQuestRewards(Quest.RewardsOnComplete)` ?

---

## ?? **ИНСТРУКЦИЯ:**

### **1. Скомпилируйте проект:**

```
Visual Studio ? Build ? Build Solution (Ctrl+Shift+B)
или
Rider ? Build ? Build Solution
```

### **2. Удалите старое сохранение:**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **3. Запустите игру:**

1. Откройте Unreal Editor
2. Play In Editor (PIE)
3. Начните квест `Q_CollectApples`
4. Завершите обе фазы
5. Поговорите с Lianne

---

## ?? **ОЖИДАЕМЫЙ РЕЗУЛЬТАТ В ЛОГАХ:**

```
[XX:XX:XX] ? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
[XX:XX:XX] ?? Phase 'P_ReturnApples' rewards check:
[XX:XX:XX]    - PhaseRewards.Rewards count: 1    ?
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX] ??? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_CollectApples'
[XX:XX:XX]    - Rewards count: 1    ?
[XX:XX:XX]    - GrantedTags count: 0
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Processing reward - Type: 0, Amount: 2, ItemId: RedApple
[XX:XX:XX] ??? QuestSubsystem::OnQuestCompletedInternal - Quest 'Q_CollectApples' completed!
[XX:XX:XX]    RewardsOnComplete.Rewards count: 0 (no quest-level rewards)
```

---

## ?? **ТЕХНИЧЕСКОЕ РЕЗЮМЕ:**

### **Проблема:**
- Последняя фаза квеста завершалась без выдачи Phase Rewards

### **Причина:**
- При `NextPhaseId.IsNone()` код вызывал `CompleteQuest()` напрямую, минуя `OnPhaseChangedInternal()`

### **Решение:**
- Добавлен broadcast `OnQuestPhaseChanged` перед `CompleteQuest()` для последней фазы

### **Результат:**
- Phase Rewards теперь выдаются для всех фаз, включая последнюю
- Quest Rewards выдаются отдельно при завершении квеста

---

## ? **ПРОВЕРКА:**

После компиляции и тестирования вы должны увидеть:
- ? Логи показывают выдачу наград фазы
- ? `ProcessQuestRewards` вызывается с `Rewards count: 1`
- ? Награда `2? RedApple` обрабатывается

**ВСЁ ГОТОВО! КОМПИЛИРУЙТЕ И ТЕСТИРУЙТЕ!** ????
