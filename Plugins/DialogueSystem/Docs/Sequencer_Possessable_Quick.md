# ? БЫСТРЫЙ ОТВЕТ: POSSESSABLE vs SPAWNABLE

## ? **ВАШ ВОПРОС:**
> "Если я перемещаю actor в Sequencer, то они меняют свою позицию в левеле глобально?"

---

## ? **КОРОТКИЙ ОТВЕТ:**

### **POSSESSABLE:**
```
ДА! Изменения в Sequencer ГЛОБАЛЬНО меняют актора в уровне!
```

### **SPAWNABLE:**
```
НЕТ! Sequencer создаёт временную копию, оригинал НЕ трогается!
```

---

## ?? **ЧТО ВЫБРАТЬ?**

### **Используйте SPAWNABLE если:**
- ? NPC гуляют/перемещаются динамически
- ? Нужна чистая кинематографика
- ? Dating Sim / Visual Novel
- ? **90% случаев!**

### **Используйте POSSESSABLE если:**
- ? Нужна передача предмета (item exchange)
- ? Нужен ТОТ ЖЕ актор после Sequence
- ? State должен сохраниться
- ? **10% случаев**

---

## ??? **КАК ИСПОЛЬЗОВАТЬ:**

### **SPAWNABLE (рекомендую):**

```
Sequencer:
+ Track ? Actor ? "Spawnable"
Browse: BP_MyNPC
? Creates "Spawned_BP_MyNPC" ?

Result:
?? Оригинальный NPC продолжает гулять
?? Временная копия для Sequence
```

### **POSSESSABLE + RESTORE:**

```
Sequencer:
+ Track ? Actor to Sequencer
Select: BP_MyNPC from World

DialogueNode:
Effect: "PlaySequence(path, true, true, true)"
              ?
          restore actors

Result:
?? NPC телепортируется на время Sequence
?? Позиция восстанавливается после
```

---

## ?? **КРИТИЧЕСКАЯ ОШИБКА:**

```cpp
// ? НЕПРАВИЛЬНО (для динамических NPC):
Sequencer ? + Track ? Actor to Sequencer (Possessable)
Lia Transform ? (X=100, Y=100)
? После Sequence Lia ТЕЛЕПОРТИРУЕТСЯ глобально!

// ? ПРАВИЛЬНО:
Sequencer ? + Track ? Actor ? Spawnable
Spawned_Lia Transform ? (X=100, Y=100)
? Оригинальная Lia НЕ трогается!
```

---

## ?? **ПОДРОБНЕЕ:**

`Sequencer_Possessable_vs_Spawnable.md` - полное руководство

---

**TL;DR:** Используйте **SPAWNABLE** для Dating Sim! Оригинальные акторы не трогаются! ??
