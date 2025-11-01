# Quest System: Objective Execution Order Guide

## ?? Overview

This guide explains how objectives are executed in the Quest System and how to control their order.

---

## ?? Current Behavior

### ? **No Built-in Sequential Execution**

By default, **all objectives within a single phase are checked in parallel**. The system processes them in the order they appear in the array, but **any objective can progress** as long as its conditions match an event.

**Example:**
```cpp
Phase: "Collect Items"
  ?? Objective 1: Talk to Lianne     (Can complete at any time)
  ?? Objective 2: Collect 5 Apples   (Can complete at any time)
  ?? Objective 3: Return to Lianne   (Can complete at any time)
```

? **Problem:** Player can complete Objective 3 before Objective 1!

---

## ? Solutions for Sequential Objectives

### **Option 1: Use Phases (? RECOMMENDED)**

**Phases naturally enforce order!** Each phase must complete before the next one starts.

#### Example: Sequential Quest Flow
```
Quest: "Help Lianne"
  ?
  ?? Phase 1: "Introduction"
  ?   ?? Objective: Talk to Lianne
  ? ?? Auto-advances to Phase 2 when complete
  ?
  ?? Phase 2: "Collection"
  ?   ?? Objective: Collect 5 Apples  (can be parallel)
  ?   ?? Objective: Collect 3 Flowers (can be parallel)
  ?   ?? Auto-advances to Phase 3 when all complete
  ?
  ?? Phase 3: "Return"
      ?? Objective: Return to Lianne
      ?? Quest completes
```

#### How to Configure in Quest Data Asset

```cpp
// Phase 1: Talk to Lianne
FQuestPhase Phase1;
Phase1.PhaseId = "P_TalkToLianne";
Phase1.DisplayName = FText::FromString("Introduction");
Phase1.bRequireAllObjectives = true;

// Add objective
FQuestObjective Obj1;
Obj1.ObjectiveId = "O_TalkToLianne";
Obj1.Description = FText::FromString("Talk to Lianne");
Phase1.Objectives.Add(Obj1);

// Configure transition
Phase1.Transition.bAutoAdvance = true;  // Auto-progress to next phase
Phase1.Transition.NextPhaseId = "P_Collection"; // Next phase ID

// Phase 2: Collect Items
FQuestPhase Phase2;
Phase2.PhaseId = "P_Collection";
Phase2.DisplayName = FText::FromString("Collection");
Phase2.bRequireAllObjectives = true;

// ... add objectives ...

Phase2.Transition.bAutoAdvance = true;
Phase2.Transition.NextPhaseId = "P_Return";

// Phase 3: Return
FQuestPhase Phase3;
Phase3.PhaseId = "P_Return";
// ... etc ...
```

#### ? **Benefits of Using Phases:**
- ? Clean, logical separation of quest steps
- ? Easy to read in UI ("Current Phase: Collection")
- ? Automatic progression via `bAutoAdvance`
- ? Clear narrative structure
- ? Supports branching (different NextPhaseId based on conditions)

---

### **Option 2: Add `PreviousObjectiveId` Dependency**

If you need strict order **within a single phase**, you can add objective dependencies.

#### Configuration Example

```cpp
// First objective (no dependency)
FQuestObjective Obj1;
Obj1.ObjectiveId = "O_TalkToLianne";
Obj1.Description = FText::FromString("Talk to Lianne");
Obj1.PreviousObjectiveId = NAME_None; // No dependency

// Second objective (requires Obj1)
FQuestObjective Obj2;
Obj2.ObjectiveId = "O_CollectApples";
Obj2.Description = FText::FromString("Collect 5 Apples");
Obj2.PreviousObjectiveId = "O_TalkToLianne"; // ? Must complete first!

// Third objective (requires Obj2)
FQuestObjective Obj3;
Obj3.ObjectiveId = "O_ReturnToLianne";
Obj3.Description = FText::FromString("Return to Lianne");
Obj3.PreviousObjectiveId = "O_CollectApples"; // ? Chain dependency
```

#### ?? **Note:** This feature is **not yet implemented** by default. You would need to add this field to `FQuestObjective` struct and modify `QuestEventBus::BroadcastEvent` to check dependencies.

---

### **Option 3: Priority-Based Execution**

Add a priority field to control which objectives are checked first.

#### Configuration Example

```cpp
FQuestObjective Obj1;
Obj1.Priority = 1; // Higher priority (checked first)

FQuestObjective Obj2;
Obj2.Priority = 2; // Medium priority

FQuestObjective Obj3;
Obj3.Priority = 3; // Lower priority
```

Objectives are sorted by priority before checking conditions.

#### ?? **Note:** This feature is also **not yet implemented** by default.

---

## ?? Recommended Approach

### **Use Phases for Sequential Steps**

For most quest designs, **phases are the cleanest solution**:

```
? DO: Use phases for major sequential steps
   Phase 1: "Get the quest"
   Phase 2: "Do the task"
   Phase 3: "Turn in the quest"

? DO: Use parallel objectives within a phase for flexible gameplay
   Phase: "Collect Resources"
     ?? Objective: 5 Apples  } Can be done
     ?? Objective: 3 Flowers } in any order

? DON'T: Use strict ordering within a phase for tasks that could be done in any order
```

---

## ?? Common Patterns

### Pattern 1: **Linear Quest (Strict Sequence)**
```
Phase 1: Talk to NPC
  ?? Advance to Phase 2

Phase 2: Do Task
  ?? Advance to Phase 3

Phase 3: Return to NPC
  ?? Complete Quest
```

### Pattern 2: **Collection Quest (Parallel Objectives)**
```
Phase 1: "Collect Items"
  ?? Objective: 5 Apples   } Both required,
  ?? Objective: 3 Flowers  } any order OK
  ?? Advance to Phase 2

Phase 2: "Turn In"
  ?? Objective: Return to Quest Giver
```

### Pattern 3: **Branching Quest**
```
Phase 1: "Choose Side"
  ?? Objective A: Help Faction A ? Next: Phase_A
  ?? Objective B: Help Faction B ? Next: Phase_B

Phase_A: "Faction A Path"
  ?? ...

Phase_B: "Faction B Path"
  ?? ...
```

---

## ?? Summary

| Method | Use Case | Complexity | Recommended |
|--------|----------|------------|-------------|
| **Phases** | Sequential major steps | ? Easy | ? **YES** |
| **Parallel Objectives** | Flexible tasks within a step | ? Easy | ? **YES** |
| **PreviousObjectiveId** | Strict order within phase | ?? Medium | ?? Not implemented yet |
| **Priority** | Custom sorting logic | ?? Medium | ?? Not implemented yet |

---

## ?? Quick Start: Creating a Sequential Quest

1. **Define your quest steps** (e.g., Talk ? Collect ? Return)
2. **Create a Phase for each major step**
3. **Set `bAutoAdvance = true`** on each phase
4. **Set `NextPhaseId`** to chain phases together
5. **Add objectives** to each phase (parallel if needed)

**Example Blueprint:**
```
Quest: "Fetch Quest"
  Phase 1: "Get Quest"     ? Talk to NPC       ? Auto to Phase 2
  Phase 2: "Collect Items"   ? Get 5 Apples      ? Auto to Phase 3
  Phase 3: "Turn In"     ? Return to NPC     ? Complete
```

---

## ? FAQ

**Q: Can I have optional objectives in a sequence?**  
A: Yes! Mark objectives as `bIsOptional = true`. They won't block phase progression.

**Q: What if I need branching paths?**  
A: Use different `NextPhaseId` values based on which objective completes first, or use conditions in `Transition`.

**Q: Can objectives in different phases progress at the same time?**  
A: No. Only objectives in the **current active phase** can progress.

**Q: How do I show the current phase in UI?**  
A: Get `QuestSaveData.CurrentPhaseId` and look up `FQuestPhase.DisplayName` in your Quest Asset.

---

## ?? Related Documentation

- [Quest System Quick Start](QUICK_START.md)
- [Quest Conditions Reference](QUEST_CONDITIONS_COMPLETE_REFERENCE.md)
- [Quest Actor Component Guide](QUEST_ACTOR_COMPONENT_GUIDE.md)

---

**Last Updated:** 2025-11-01  
**Quest System Version:** 2.0
