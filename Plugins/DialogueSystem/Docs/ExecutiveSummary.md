# ?? Dialogue System - Executive Summary

## TL;DR

**��� ���?**
���������������� ��������� ������� �������� ��� Unreal Engine 5, ���������� ���������������� ��� RPG � relationship simulator ���.

**������� ������������:**
1. ? **��������� ���������** � 4 ����������� ���� (Core ? Runtime ? UI ? AI)
2. ? **Data-Driven** � ���� ������� � ��������, �� � ����
3. ? **AI-Ready** � ���������� � OpenAI/Claude/Local LLMs
4. ? **Relationship System** � ������, �������, ���������, ������ NPC
5. ? **DSL** � ������� ���� ��� ������� (`affinity >= 50 && hasItem(Rose)`)
6. ? **Production-Ready** � unit tests, ������������, �������

---

## ?? ��� ���� ��� �������?

### ? �������� �������� ����:
- �� ������� RPG ��� relationship simulator
- ����� ������� ������������ �������
- ����� ������� ��������� � NPC
- ����������� ����� �������� (>1000 ��������)
- ������ AI-��������� ������
- �������: ������������ + �������� + ��������� �������� �����������

### ? �� �������� ����:
- ����� ������ ������� �������� ������� (overkill)
- Shooter � ������������ ���������
- ��� �������� �� ���������� (���� quick start �� 5 �����)
- ����� ������� UI (���� ������ ������� ������)

---

## ??? ����������� � 3 ������������

1. **Core** (������ ������) ? ������ DataAssets, ��������� state machine
2. **Runtime** (����������) ? ������������� � gameplay, ��������� �����������
3. **UI** (�������) ? ������������� �� ������� � ����������

**��� ���� ���������**, ����� �������� UI ��� �������� multiplayer ��� ��������� Core.

---

## ?? �������� ������

| ����� | ��� ������ | ������ ������������� |
|-------|-----------|---------------------|
| `UDialogueRunner` | State machine ������� | `Runner->StartDialogue(Asset, Player, NPC)` |
| `UDialogueContext` | ��������� ���� | `Context->GetAffinity("Lianne")` |
| `UDialogueNode` | ���� ����� | `Node->GetResolvedText(Context)` |
| `UDialogueComponent` | ��������� NPC | `NPC->DialogueComp->StartDialogue(Player)` |
| `URelationshipComponent` | ��������� | `Comp->ModifyAffinity(10)` |
| `UDialogueSubsystem` | ���������� �������� | `Subsystem->GetActiveDialogue()` |

---

## ?? Workflow � 6 �����

```
1. �������� ������ ������ � Google Sheets
              ?
2. ������� � CSV ? ������ � DataTable (UE Editor)
              ?
3. �������� DialogueDataAsset, ��������� �����
              ?
4. �������� � NPC ����� DialogueComponent
              ?
5. ����� ��������������� ? Runner ��������� ? UI ����������
              ?
6. ������� ����������� � Context ? ��������� ��������
```

**��������**: �������� ����� �������� ������� � hot-reload ��� ����������� ����.

---

## ?? DSL �������

### ������� (Conditions)
```cpp
// �������� ���������
affinity[Lianne] >= 50

// �������� ���������
hasItem(Rose) && gold >= 100

// ����������� �������
(romance[Lianne] >= 70 && time == Evening) || worldState(Festival.Active)

// ������
memory(FirstMeeting) && !visited(Node_Secret)
```

### ������� (Effects)
```cpp
// �������� ���������
affinity[Lianne] += 10; romance[Lianne] += 5

// ���� ������� � ������
giveItem(Sword); gold += 100

// ������ �����
startQuest(HelpLianne); setMemory(QuestStarted)

// ������� �������
triggerEvent(Cutscene_Kiss); setMood(Happy)
```

---

## ?? Relationship System

### ��������
```
Affinity:  -100 (����) ?? +100 (������ ����)
Trust:        0 (�� ��������) ?? 100 (������ �������)
Romance:      0 (��� ������) ?? 100 (������)
Fear:         0 (�� ������) ?? 100 (����)
Respect:      0 (���������) ?? 100 (��������)
Jealousy:     0 (����������) ?? 100 (��������)
```

### ���������� (Mood)
```
Neutral ? Happy ? Excited
      ?
      Sad ? Angry ? Fearful
```

### ������
```
NPC ������:
- ���������� ���� ��������
- �������� ������ (�������, �����������)
- ������ ������� (������ �������, ���������)
- ��������� ����� (������������� ���������)
```

---

## ?? AI Integration

### �������������� ����������
- **OpenAI** (GPT-4, GPT-3.5)
- **Anthropic** (Claude 3)
- **Local LLMs** (LM Studio, Ollama, Kobold)
- **Custom API** (����� OpenAI-compatible endpoint)

### ��� ��������
```
1. ���� ���� AIGenerated ���������
2. ������� ������ ������:
   - System: "You are Lianne, a shy village girl..."
   - Context: "Time: Evening, Affinity: 75, Player gave rose"
   - Hint: "React to the gift with bashfulness"
3. Async HTTP ������ � API
4. ������� ������ (text + emotion)
5. ����������� (����� �� ������� API)
6. Fallback �� ��������� ����� ���� ������
```

### ������������
- ? NSFW ������
- ? Rate limiting (���� 10 req/min)
- ? Timeout (3 sec)
- ? API key �� � assets (Config ����)

---

## ?? Performance

### �������� �������
```
Operation              Time      Notes
??????????????????????????????????????????
Start dialogue         <5ms      Load + Init
Node transition        <1ms      Without AI
Condition evaluation   <0.1ms    Cached
Effect execution       <0.5ms    Basic effects
UI update              <16ms     60 FPS target
AI generation          <3s       Depends on API

Memory
??????????????????????????????????????????
1000 nodes             ~50MB
10 active dialogues    ~100MB
AI cache (1000)        ~20MB
```

### �����������
- ����������� �������� DSL
- Object pooling ��� UI
- Lazy loading DataAssets
- Async AI requests

---

## ?? Best Practices Summary

### Authoring
```
? DO
- ������������ ��������� �������
- ������������ �������� NodeId (�� "Node_001")
- ��������� ������� �������� ����� ��������������
- ������������ ����� ����� ��������

? DON'T
- ����������� �����������
- ���������� ����� (affinity += 37)
- ������� ������� ������� (>10 �����)
- �������� ��� �����������
```

### Programming
```
? DO
- ������������ ������� ������ Tick
- �������� UI �� ������ (MVVM)
- ������ unit tests ��� �������/��������
- ��������������� ��������� API

? DON'T
- ���������� ������
- ������ ����������� (����������� ����������)
- ����������� AI �������
- Ignore ���������
```

---

## ?? Getting Started (5 �����)

```bash
# ��� 1: ���������� ������
���������� DialogueSystem ? YourProject/Plugins/

# ��� 2: ��������
Edit ? Plugins ? Enable "Dialogue System" ? Restart

# ��� 3: ������� DataAsset
Content Browser ? ��� ? Dialogue System ? Dialogue Data Asset

# ��� 4: ��������� ����
Node Start: "Hello!"
Node End: (End type)
Edge: Start ? End

# ��� 5: ��������� � NPC
Add Component: DialogueComponent
Set DefaultDialogue: ��� DataAsset

# ��� 6: Test!
Play ? ������� � NPC ? ������ E
```

---

## ?? ��� ������ ������?

| ���� ��... | ������� |
|-----------|---------|
| **�������** | [QuickStart.md](Docs/QuickStart.md) |
| **�����������** | [Architecture.md](Docs/Architecture.md) |
| **��������/��������** | [DSL_Reference.md](Docs/DSL_Reference.md) |
| **������������ AI** | [AI_Integration.md](Docs/AI_Integration.md) |
| **������ ��** | [README.md](README.md) ? ��� ������ |

---

## ?? �������� ������� � ������

### 1. ������ ��������� �����������?
**������������**: ���������� �������
**�����**: 4 ������ (Core, Runtime, UI, AI)
**������**: 
- ����� ����������� Core ��� Engine
- UI ����� �������� ��� ��������� ������
- AI ���������� (�� ����� � ��������� ������)
- ������ ������ ������ ���� ���� (SRP)

### 2. ������ Data-Driven?
**������������**: Blueprint logic, Code-based
**�����**: DataAssets + DataTables
**������**:
- �������� �������� ����������
- Version control friendly (JSON/CSV)
- Hot reload ��� ����������
- ������� � Excel/Sheets ���������

### 3. ������ DSL?
**������������**: Visual scripting, Lua, Pure BP
**�����**: ��������� DSL (`affinity >= 50`)
**������**:
- ������������� (������� ��� ������������)
- ������������ (���������� � ������ �������)
- �������� ������, ����������
- ����� ��������� ������ �����������

### 4. ������ Event-Driven?
**������������**: Tick-based polling
**�����**: �������� � �������
**������**:
- ��� ������ �������� ������ ����
- Decoupling (UI �� ����� � Runner)
- ����� ��������� ����������
- Performance (������ ����� �����)

### 5. ������ Context as State?
**������������**: State � Runner, Global singletons
**�����**: ���� state � UDialogueContext
**������**:
- Stateless Runner (����� �����������)
- Save/Load ��������� (1 ������)
- Thread-safe ��� AI
- ����� ����������� (mock Context)

---

## ?? Use Cases

### Use Case 1: Dating Sim
```
- 5 �������, ������ �� ����� ��������
- 50+ �������� �� �������
- Romance, Affinity, Jealousy system
- Multiple endings (������� �� ���������)
- AI ��������� ��� random small talk

��������? ? ��, ��������
```

### Use Case 2: AAA RPG
```
- 100+ NPC
- 1000+ ��������
- ���������� � quest system
- Voice-over ��� ���� ������
- ����������� �� 10 ������

��������? ? ��, ��������������
```

### Use Case 3: Multiplayer Co-op
```
- ������� ������ replicate����
- ����� ������ ������ �� ������ �������
- Save/Load ����� Cloud

��������? ?? �������� (replication � v1.2)
```

### Use Case 4: VR Social Game
```
- Voice input �� �������
- AI ��������� �� ����
- ���� ��������������� ��������

��������? ?? �������� (AI ����, voice input ���)
```

---

## ?? Troubleshooting

### ��������: ������ �� �����������
```
���������:
1. DialogueComponent �� NPC?
2. DefaultDialogue ����������?
3. CharacterId ��������� � SpeakerId � �����?
4. ���� ������� (ValidateGraph)?
5. bCanStartDialogue = true?
```

### ��������: UI �� ������������
```
���������:
1. DialogueWidget �������� �� viewport?
2. BindToRunner() ������?
3. �������� ���������?
4. UI Layer �� �������� ������ ��������?
```

### ��������: ������� �� ��������
```
���������:
1. ��������� DSL ���������?
2. Context ���������������?
3. Affinity/Items/Memory �����������?
4. ��� �� ������� (ClearCache)?
```

### ��������: AI �� ����������
```
���������:
1. API key ���������� � Config?
2. �������� ���������� ����?
3. Quota API �� ���������?
4. Fallback ����� ������������?
```

---

## ? Checklist: ����� �� ������ � production?

### Content
- [ ] ��� ���� ��������� (��� ������ DialogueText)
- [ ] ��� Edge ��������� ������������ ����
- [ ] Start node ����������
- [ ] End node �������� �� Start
- [ ] ��������� ����� �������� (ValidateGraph = true)
- [ ] ����������� ��� ���� ������

### Integration
- [ ] NPC ����� DialogueComponent
- [ ] DialogueComponent �������� (CharacterId, Dialogue)
- [ ] RelationshipComponent ��������
- [ ] Portrait ��������
- [ ] UI ������ ��������

### Testing
- [ ] ������ ������������� � PIE
- [ ] ��� �������� ������ ���������
- [ ] ������� �������� ���������
- [ ] ������� ����������� ���������
- [ ] Save/Load ��������

### Polish
- [ ] ������ �����������
- [ ] Voice-over ������� (���� ����)
- [ ] Typewriter �������� ���������
- [ ] �������� ����/���� ��������
- [ ] �������� ������� ���������

---

## ?? ��� ������ ��� ������� ����� ��������?

### vs Dialogue Designer
```
? ����� ��������� (4 ���� vs �������)
? ���������� Relationship �������
? AI ���������� out-of-the-box
? ����� performance (event-driven)
? ��� visual editor (����, v1.1)
```

### vs YarnSpinner
```
? �������� ���������� � UE
? Blueprint + C++ API
? Relationship system �������
? Data-Driven (DataAssets)
? ����� ������ ecosystem
```

### vs Custom in-house system
```
? Production-ready (�� ����� ������ � ����)
? �������������� � �������������
? ��������� � �����������
? Community support (�����������)
? Regular updates
```

---

**������**: 1.0
**������**: Production Ready ??
**����� ������**: ~10 �����
**��������� ���**: [QuickStart.md](Docs/QuickStart.md)
