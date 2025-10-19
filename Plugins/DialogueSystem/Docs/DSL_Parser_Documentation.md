# Dialogue System DSL Parser Documentation

## Overview
This document describes the Domain-Specific Language (DSL) parsers for the Dialogue System, which allow for text-based definition of conditions and effects in dialogues.

## Condition DSL (`UDialogueConditionEvaluator`)

### Syntax Overview

The condition evaluator supports a rich DSL for expressing complex game conditions:

#### Logical Operators
- `&&` or `and` - Logical AND
- `||` or `or` - Logical OR  
- `!` or `not` - Logical NOT
- `()` - Grouping/Precedence

#### Comparison Operators
- `==` or `=` - Equality
- `!=` - Inequality
- `>=` - Greater than or equal
- `<=` - Less than or equal
- `>` - Greater than
- `<` - Less than

### Supported Condition Types

#### 1. Item Conditions
```
hasItem(ItemName)
hasItem(ItemName, Count)
has(Rose, 2)
```
Checks if the player has a specific item, optionally with a minimum count.

#### 2. Memory/Flag Conditions
```
memory(FirstMeeting)
memory(HasTalkedToNPC, true)
visited(Node_002)
```
- `memory(Key)` - Checks if a boolean flag is true
- `memory(Key, Value)` - Checks if a boolean flag equals Value
- `visited(NodeId)` - Sugar syntax for `memory(NodeId_Visited, true)`

#### 3. Relationship Conditions
```
affinity[Lianne] >= 50
affinity[Marcus] > 75
```
Checks the relationship affinity level with an NPC.

#### 4. Variable Conditions
```
variable[QuestProgress] == 3
var[PlayerLevel] >= 10
```
Checks custom variables with numeric or string comparison.

#### 5. Time Conditions
```
time == Morning
time == Evening
```
Checks the current game time against a gameplay tag.

#### 6. Tag/World State Conditions
```
tag(Weather.Rainy)
hasTag(Event.Festival)
```
Checks if a specific gameplay tag is active in the world state.

### Example Expressions

#### Simple Conditions
```
hasItem(Rose)
affinity[Lianne] >= 50
memory(FirstMeeting)
```

#### Complex Conditions
```
hasItem(Rose) && affinity[Lianne] >= 30
(memory(QuestStarted) && !visited(Node_Boss)) || affinity[Marcus] > 80
time == Evening && hasItem(Letter) && variable[QuestStage] == 2
```

#### Nested Conditions
```
(affinity[Lianne] >= 50 || hasItem(Gift)) && !memory(GaveGift)
```

## Effect DSL (`UDialogueEffectExecutor`)

### Syntax Overview

The effect executor supports modifications to game state through a DSL:

#### Operators
- `+=` - Add/Increment
- `-=` - Subtract/Decrement
- `=` - Set/Assign
- `;` - Statement separator (for multiple effects)

### Supported Effect Types

#### 1. Inventory Effects
```
giveItem(Rose, 1)
give(Sword)
removeItem(Key, 1)
remove(Potion, 3)
```
Adds or removes items from inventory.

#### 2. Currency Effects
```
gold += 100
gold -= 50
```
Modifies player's gold/currency.

#### 3. Relationship Effects
```
affinity[Lianne] += 10
affinity[Marcus] -= 5
trust[Lianne] += 15
trust[Marcus] -= 10
```
Modifies relationship values with NPCs.

#### 4. Memory/Flag Effects
```
setMemory(FirstMeeting, true)
memory(QuestCompleted, true)
memory(HasSeenEvent)
```
Sets boolean flags or memories.

#### 5. Variable Effects
```
setVariable(QuestProgress, 3)
var(PlayerLevel, 10)
```
Sets custom variables.

#### 6. Quest Effects
```
startQuest(HelpLianne)
quest(FindArtifact)
completeQuest(TutorialQuest)
```
Starts or completes quests.

#### 7. World State Effects
```
addTag(Weather.Sunny)
removeTag(Event.Festival)
```
Adds or removes gameplay tags from world state.

### Example Expressions

#### Single Effects
```
giveItem(Rose, 1)
affinity[Lianne] += 10
startQuest(RomanceLianne)
```

#### Multiple Effects
```
giveItem(Rose, 1); gold -= 25; affinity[Lianne] += 5
setMemory(GaveGift); startQuest(LianneQuest); affinity[Lianne] += 10
```

#### Complex State Changes
```
gold += 500; giveItem(Artifact); completeQuest(FindArtifact); addTag(Quest.Completed)
```

## Tokenizer Implementation

### Features
- String literal support (with quotes)
- Multi-character operator recognition (`>=`, `<=`, `==`, `!=`, `&&`, `||`, `::`)
- Whitespace handling
- Parentheses and bracket grouping
- Member access notation (dots in identifiers like `Time.Morning`)
- Numeric literal support (including decimals and negative numbers)

### Token Types
- Identifiers (alphanumeric + underscores + dots)
- Operators (`+`, `-`, `*`, `/`, `=`, `!`, `<`, `>`, `&`, `|`, `:`)
- Multi-char operators (`+=`, `-=`, `==`, `!=`, `<=`, `>=`, `&&`, `||`, `::`)
- Delimiters (`(`, `)`, `[`, `]`, `,`)
- String literals (enclosed in `"` or `'`)
- Numeric literals (integers and floats)

## Parser Architecture

### Condition Parser
Follows a recursive descent parser pattern:
1. **ParseExpression** - Handles OR operators (lowest precedence)
2. **ParseTerm** - Handles AND operators
3. **ParseAtom** - Handles atomic conditions and NOT operators

### Effect Parser
Single-pass parser that identifies effect type and delegates to specialized parsers:
- **ParseSingleEffect** - Routes to specific effect parsers based on keyword
- Specialized parsers for each effect type (e.g., `ParseGiveItemEffect`, `ParseAffinityEffect`)

## Usage Examples

### In C++ Code
```cpp
// Create evaluator/executor
UDialogueConditionEvaluator* ConditionEval = NewObject<UDialogueConditionEvaluator>();
UDialogueEffectExecutor* EffectExec = NewObject<UDialogueEffectExecutor>();

// Evaluate a condition
bool bResult = ConditionEval->EvaluateString(
    TEXT("hasItem(Rose) && affinity[Lianne] >= 30"), 
    DialogueContext
);

// Execute effects
EffectExec->ExecuteString(
    TEXT("giveItem(Rose, 1); affinity[Lianne] += 10; startQuest(RomanceLianne)"),
    DialogueContext
);
```

### In Blueprint
Both evaluators are Blueprint-callable and can be used directly in Blueprint graphs.

### In Data Assets
Store condition and effect strings directly in dialogue data assets:
```cpp
UPROPERTY(EditAnywhere, Category = "Dialogue")
FString ConditionExpression = TEXT("affinity[Lianne] >= 50 && hasItem(Rose)");

UPROPERTY(EditAnywhere, Category = "Dialogue")
FString EffectExpression = TEXT("affinity[Lianne] += 10; memory(GaveGift, true)");
```

## Performance Considerations

### Caching
Both parsers implement caching mechanisms:
- **ConditionCache**: Maps condition strings to parsed condition objects
- **EffectCache**: Maps effect strings to parsed effect objects

This prevents re-parsing the same expressions multiple times.

### Best Practices
1. Parse conditions/effects once during asset loading
2. Store the parsed objects for runtime evaluation
3. Use simple conditions when possible (avoid deep nesting)
4. Batch multiple effects into compound effects when appropriate

## Error Handling

Currently, the parsers use a lenient approach:
- Invalid tokens are skipped
- Malformed expressions return nullptr
- No syntax error messages (planned for future)

## Future Enhancements

Potential improvements:
1. Error reporting with line/column information
2. Support for more comparison operators (`contains`, `startsWith`, etc.)
3. Function calls with multiple arguments
4. Mathematical expressions (`affinity[NPC] += gold * 0.1`)
5. Conditional effects (`if hasItem(Gold) then gold -= 50 else removeItem(Gold)`)
6. Variable interpolation in strings
7. Comments in DSL expressions
8. Type checking at parse time
