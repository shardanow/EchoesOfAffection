# Dialogue System - Architecture Summary

## ?? �������� ������� � �����������

### 1. **��������� ����������� (Plugin-based)**

**�������**: ������� ��������� �� ����������� ������
- `DialogueSystemCore` � ������ ������
- `DialogueSystemRuntime` � ���������� � gameplay
- `DialogueSystemUI` � ������������
- `DialogueSystemAI` � AI ���������

**�����������**:
- ? **Single Responsibility**: ������ ������ ������ ���� ������
- ? **Open/Closed**: ����� ��������� ��� ��������� core
- ? **Dependency Inversion**: ����������� ����� ����������
- ? **Reusability**: Core ����� ������������ � ������ ��������
- ? **Testing**: Unit-����� ��� ������������ �� Engine

**������������ ���������**:
- ? ���������� ������� � ������ �����������, �������� SRP
- ? Actor-based � tight coupling, ������ �������������

---

### 2. **Data-Driven ������**

**�������**: ��� ������ �������� � DataAssets/DataTables

**�����������**:
- ? �������� �������� ���������� �� �������������
- ? ��������� �� ������� �������������� C++
- ? ����� ��������������/������������� (Google Sheets)
- ? Version control friendly (JSON/CSV)
- ? Hot reload � PIE

**������������ ���������**:
- ? Hardcoded � Blueprint � ��� version control, ������ ����������������
- ? JSON ����� � ��� ���������� � UE Editor
- ? Code-based DSL (Ink, YarnSpinner) � overkill ��� ����� ����

---

### 3. **Event-Driven ����������**

**�������**: ��� ������������ ����� ��������/�������

```cpp
Runner->OnNodeEntered.Broadcast(Node, Context);
// UI ������� � �����������
```

**�����������**:
- ? **Decoupling**: UI �� ����� � Runner, Runner �� ����� � UI
- ? **Performance**: ��� Tick-based �������
- ? **Reactive**: ��������� ����������� ���������
- ? **Extensibility**: ����� ��������� ����� ����������

**������������ ���������**:
- ? Tick-based polling � ������������, ����� ��������������
- ? Direct function calls � tight coupling

---

### 4. **DSL ��� ������� � ��������**

**�������**: ��������� ���� ��� ������

```cpp
Condition: "affinity[Lianne] >= 50 && hasItem(Rose)"
Effect: "affinity[Lianne] += 10; giveItem(Gift)"
```

**�����������**:
- ? **Authoring**: �������� �� ������� Blueprint
- ? **Readability**: ������� ��� ������������
- ? **Flexibility**: ����� ��������� ����� ���������
- ? **Performance**: ������� ���� ���, �����������

**������������ ���������**:
- ? Visual scripting (Blueprint) � ��������, ������ version control
- ? Lua/Python � overkill, �������� � �������������
- ? Pure C++ � ������� ��������������

---

### 5. **Context ��� Single Source of Truth**

**�������**: ���� state � UDialogueContext

```cpp
class UDialogueContext
{
    TMap<FName, float> AffinityValues;
    TMap<FName, int32> PlayerInventory;
    TSet<FName> VisitedNodes;
    // ...
};
```

**�����������**:
- ? **Stateless Runner**: ����� ����������� � ����� ������
- ? **Save/Load**: ���� ������ �������������
- ? **Thread-safe**: ��� AI ��������
- ? **Testing**: ����� ������� mock Context

**������������ ���������**:
- ? State � Runner � ������ ���������/���������
- ? Global singletons � �� thread-safe, ����� ��� ������

---

### 6. **Component-based ��� NPC**

**�������**: DialogueComponent + RelationshipComponent

**�����������**:
- ? **Composition over Inheritance**: ����� Actor ����� �����������
- ? **Modularity**: ����� ��������/��������� ����������
- ? **Data-Oriented**: ������ ��� ECS-like ��������
- ? **Inspector-friendly**: ����� ����������� � Editor

**������������ ���������**:
- ? Inheritance (ADialogueNPC) � diamond problem, �������
- ? Subsystem-only � ��� per-actor state

---

### 7. **MVVM ��� UI**

**�������**: DialogueWidget (View) ? DialogueViewModel ? Runner (Model)

**�����������**:
- ? **Separation**: ������-������ �������� �� �������
- ? **Testing**: ����� ����������� ViewModel ��� UI
- ? **Designers**: ����� ������ ������ ��� �������������
- ? **Reusability**: ViewModel ��� ������ UI ������

**������������ ���������**:
- ? MVC � Controller ��������� � UE
- ? Direct binding � tight coupling

---

### 8. **Async AI ����������**

**�������**: ��������� � async ��������� + ���

```cpp
AIProvider->GenerateDialogue(Persona, Context, Hint, Callback);
```

**�����������**:
- ? **Non-blocking**: ���� �� �������� �� API
- ? **Fallback**: ��������� ����� ���� API ����������
- ? **Cache**: �� ������ API ���������� ���������
- ? **Extensible**: ����� �������� ������ ������

**������������ ���������**:
- ? Sync ������� � ������ ����
- ? Thread pool � overkill, �������� � UObject
- ? Blueprint-only � ��������, ����� ��� production

---

## ??? ���� ������������

```
???????????????????????????????????????????????
?            APPLICATION LAYER                 ?
?                                              ?
?  ????????????????????????????????????????   ?
?  ?     Game (BP_PlayerCharacter)        ?   ?
?  ????????????????????????????????????????   ?
?               ? uses                         ?
????????????????????????????????????????????????
                ?
????????????????????????????????????????????????
?               ?                              ?
?           UI LAYER                           ?
?  ????????????????????????????????????????   ?
?  ?       DialogueWidget                 ?   ?
?  ?       ChoiceWidget                   ?   ?
?  ????????????????????????????????????????   ?
?               ? subscribes to events         ?
????????????????????????????????????????????????
                ?
????????????????????????????????????????????????
?               ?                              ?
?          RUNTIME LAYER                       ?
?  ????????????????????????????????????????   ?
?  ?    DialogueSubsystem (Manager)       ?   ?
?  ?    DialogueComponent (Per-NPC)       ?   ?
?  ?    RelationshipComponent             ?   ?
?  ????????????????????????????????????????   ?
?               ? creates & manages            ?
????????????????????????????????????????????????
                ?
????????????????????????????????????????????????
?               ?                              ?
?            CORE LAYER                        ?
?  ????????????????????????????????????????   ?
?  ?      DialogueRunner (Controller)     ?   ?
?  ?      ConditionEvaluator              ?   ?
?  ?      EffectExecutor                  ?   ?
?  ????????????????????????????????????????   ?
?               ? operates on                  ?
????????????????????????????????????????????????
                ?
????????????????????????????????????????????????
?               ?                              ?
?           DOMAIN LAYER                       ?
?  ????????????????????????????????????????   ?
?  ?      DialogueContext (State)         ?   ?
?  ?      DialogueNode (Data)             ?   ?
?  ?      DialogueDataAsset (Config)      ?   ?
?  ????????????????????????????????????????   ?
????????????????????????????????????????????????
                ?
????????????????????????????????????????????????
?               ?                              ?
?      INFRASTRUCTURE LAYER                    ?
?  ????????????????????????????????????????   ?
?  ?      AIDialogueProvider (External)   ?   ?
?  ?      SaveGameSubsystem               ?   ?
?  ?      LocalizationManager             ?   ?
?  ????????????????????????????????????????   ?
????????????????????????????????????????????????
```

---

## ?? ������ ������

### 1. **Authoring Flow** (�������� ? ����)

```
Writer creates dialogue in Google Sheets
              ?
      Export to CSV/JSON
              ?
   Import into DataTable (UE Editor)
              ?
   Create DialogueDataAsset
              ?
     Validate graph structure
              ?
  Assign to NPC's DialogueComponent
              ?
       Test in PIE/Game
```

### 2. **Runtime Flow** (����� ? NPC)

```
Player presses E near NPC
              ?
  InteractComponent triggered
              ?
 NPC->DialogueComponent->StartDialogue()
              ?
  DialogueSubsystem->StartDialogue()
              ?
    Create DialogueRunner
              ?
   Load DialogueDataAsset
              ?
  Build node cache & context
              ?
    Enter start node
              ?
  OnNodeEntered.Broadcast()
              ?
    UI displays dialogue
              ?
  Player selects choice
              ?
   Apply effects to Context
              ?
  Compute next node (with conditions)
              ?
   Go to next node or end
```

### 3. **AI Flow** (������������ ���������)

```
Enter AIGenerated node
              ?
  Build prompt from Persona + Context
              ?
   Check cache (by hash)
              ?
  If cached ? use cached response
              ?
  Else ? Send HTTP request to AI API
              ?
   Wait for response (async)
              ?
  Parse JSON response
              ?
  Extract text + emotion
              ?
  Cache response
              ?
  Update node with generated text
              ?
  Continue normal flow
```

---

## ?? ������� ������������������

### ������� ����������

| ������� | Target | Notes |
|---------|--------|-------|
| Node transition time | < 1ms | ��� AI |
| Condition evaluation | < 0.1ms | � ����� |
| Effect execution | < 0.5ms | ������� ������� |
| UI update | < 16ms | 60 FPS |
| AI request | < 3s | ������� �� API |
| Memory footprint | < 50MB | �� 1000 nodes |

### �����������

1. **����������� ��������**
   - �������/������� �������� 1 ���
   - Hash-based cache
   - LRU eviction policy

2. **Object pooling**
   - UI widgets ����������������
   - Nodes ��������� lazy
   - Context ���������� ������ ��� Save

3. **Lazy loading**
   - DataAssets ����������� on-demand
   - Soft Object Pointers
   - Async streaming

4. **AI ���**
   - Responses ���������� �� hash (Persona + Context)
   - TTL: 1 ���
   - Disk persistence �����������

---

## ?? ������������

### 1. **Input validation**
```cpp
// NodeId �� ����� ��������� SQL-like injections
bool ValidateNodeId(FName Id);

// Effect expressions ����������� ����� ���������
bool ValidateEffectExpression(const FString& Expr);
```

### 2. **AI Safety**
```cpp
// NSFW ������
bool IsContentSafe(const FString& Text);

// Rate limiting (���� 10 req/min)
bool CanMakeAPIRequest();

// Timeout (3 sec default)
void SetRequestTimeout(float Seconds);
```

### 3. **Save/Load integrity**
```cpp
// Checksum ��� �������� corruption
uint32 ComputeSaveChecksum(const FString& Json);

// Version migration
bool MigrateSaveData(int32 FromVersion, int32 ToVersion);
```

---

## ?? ��������� ������������

### Unit Tests (C++)
- Condition evaluation
- Effect execution
- DSL parsing
- Context serialization

### Integration Tests (C++ + Mock Objects)
- Runner flow
- Component interaction
- Subsystem management

### Functional Tests (Blueprint)
- Full dialogue flow
- UI interaction
- Save/Load

### Automated Tests
- Dialogue graph validation
- Localization completeness
- Performance benchmarks

---

## ?? ����������������

### ��������� ������� ��������

| ������ ������� | Nodes | NPCs | Dialogues | Memory | Load time |
|----------------|-------|------|-----------|--------|-----------|
| Small (Indie) | <500 | <10 | <20 | <10MB | <1s |
| Medium (AA) | 500-2000 | 10-50 | 20-100 | 10-30MB | 1-3s |
| Large (AAA) | 2000+ | 50+ | 100+ | 30-50MB | 3-5s |

### ��������� ����������� ��� AAA

1. **Streaming**
   - Lazy load dialogues �� �������
   - Unload ���������� ��������
   - Preload ������ ��������

2. **Chunking**
   - ������� ������� ������� �� �����
   - ������ ����� = ��������� Asset
   - Context ����������� ����� �������

3. **LOD ��� AI**
   - ������ NPC ? ������ AI ���������
   - �������������� ? ��� + �������
   - Background NPCs ? ������ ��������� �����

---

## ?? ������� ��������������

### �������� SOLID � �������

**S - Single Responsibility**
- `DialogueRunner` � ������ ���������� state machine
- `ConditionEvaluator` � ������ ���������� �������
- `EffectExecutor` � ������ ���������� ��������

**O - Open/Closed**
- ����� ���� ������� ����� ������������ `UDialogueCondition`
- ����� ������� ����� `UDialogueEffect`
- �� ��������� ��������� core �������

**L - Liskov Substitution**
- ��� `UDialogueCondition` ����� �������� ���� �����
- ��� AI ���������� ����������� ����� ���������

**I - Interface Segregation**
- `IDialogueParticipant` � ����������� ����� �������
- ������� �� ������� �� �������������� �������

**D - Dependency Inversion**
- Runner ������� �� `IDialogueParticipant`, �� �� ���������� �������
- UI ������� �� ��������� Runner, �� �� ������ ������

---

## ?? ������� ����������

### Roadmap

**v1.1** (Next)
- Visual node editor (Graph view)
- Voice recording integration
- Lip sync support

**v1.2**
- Multiplayer support (replication)
- Cloud save integration
- Analytics dashboard

**v2.0**
- Full AI-driven conversations
- Emotion recognition (�� ������)
- Procedural dialogue generation

---

## ?? ������ �� ������������

### ��� �������������
- `Architecture.md` � ���� ��������
- `API_Reference.md` � ������ C++ API
- `Blueprint_Guide.md` � ������������� � BP

### ��� �������
- `QuickStart.md` � ����� �� 5 �����
- `DSL_Reference.md` � ��������� �������/��������
- `Authoring_Guide.md` � best practices ��� ���������

### ��� ������������
- `AI_Integration.md` � ����������� AI �������
- `Localization_Guide.md` � ���������������
- `Save_System_Guide.md` � Save/Load

---

## ? Checklist ���������� � production

### Code Quality
- [ ] ��� ������ ��������������� (Doxygen)
- [ ] Unit tests �������� > 80%
- [ ] Integration tests ������
- [ ] Performance benchmarks OK
- [ ] Memory leak �������� (Valgrind/UE Memory Profiler)

### Content
- [ ] ������� �������� ��������
- [ ] Tutorial dialogues ������
- [ ] ����������� ��� ���� ������
- [ ] Voice-over ��������

### Documentation
- [ ] Architecture ������������ ������
- [ ] API reference ������������
- [ ] Quick start guide �������������
- [ ] Video tutorials ��������

### Integration
- [ ] ������������� � Save System
- [ ] ������������� � Quest System
- [ ] ������������� � Achievement System
- [ ] Network replication (���� MP)

### Polish
- [ ] UI �������� ������
- [ ] Sound effects ���������
- [ ] Accessibility ����� (font size, speed, etc)
- [ ] Controller support

---

**������**: 1.0
**����**: 2024
**������**: Production Ready ??
