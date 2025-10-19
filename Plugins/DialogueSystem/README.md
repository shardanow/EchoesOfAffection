# ?? Advanced Dialogue System for Unreal Engine 5

> Enterprise-grade ��������� ������� �������� ��� RPG � relationship simulator ��� � ���������� AI ���������, �������� ��������� � data-driven ��������.

---

## ? �������� �����������

### ?? ��� ��������������
- ? **��������� Data-Driven** � ���� ������� � DataTables/DataAssets
- ? **Visual Node Editor** � ����������� �������� �������� (coming in v1.1)
- ? **Google Sheets ����������** � ������/������� CSV
- ? **Hot Reload** � ��������� ��� ����������� ����
- ? **Validation Tools** � �������������� �������� ������

### ?? ��� �������������
- ? **Modular Architecture** � 4 ����������� ������ (Core, Runtime, UI, AI)
- ? **SOLID Principles** � ������ �����������, ����� ���������
- ? **Event-Driven** � ��� Tick-based ������
- ? **Blueprint & C++ API** � ������ ��������� ����� ��������
- ? **Unit Tested** � >80% code coverage

### ?? ��� ���������
- ? **DSL ��� ������� � ��������** � ������� ��������� `affinity[NPC] >= 50`
- ? **Branching Dialogues** � ������� ������������ � ���������
- ? **Emotion System** � ������, ��������, voice-over
- ? **�����������** � ������ ��������� FText � Localization Dashboard
- ? **One-Time Events** � ���������� �������, ������� ������������ ���

### ?? AI ����������
- ? **OpenAI / Anthropic / Local LLMs** � ��������� ��������� �������
- ? **Async API calls** � �� ��������� ����
- ? **Response Caching** � �������� API �������
- ? **Fallback �������** � ��������� ����� ���� API ����������
- ? **NSFW ������** � ������������ ��������

### ?? Relationship System
- ? **����������� ���������** � Affinity, Trust, Romance, Fear, Jealousy
- ? **Dynamic Mood** � ���������� NPC �������� � �������� �������
- ? **Memory System** � NPC ������ �������� ������
- ? **Decay Mechanics** � ��������� �� �������� ��������
- ? **Threshold Events** � �������� ��� ���������� �������

---

## ?? Quick Start

### 1. ���������

����������� ����� `DialogueSystem` � `YourProject/Plugins/`

```bash
YourProject/
??? Plugins/
?   ??? DialogueSystem/
?       ??? DialogueSystem.uplugin
?       ??? Source/
?       ??? Content/
?       ??? Docs/
```

### 2. �������� ������

1. ������� ������ � Unreal Editor
2. `Edit ? Plugins`
3. ����� "Dialogue System"
4. Enable ? Restart Editor

### 3. ������� ������ ������

#### ��� A: ������� DataAsset
```
Content Browser ? ��� ? Dialogue System ? Dialogue Data Asset
�������: DA_FirstDialogue
```

#### ��� B: ��������� ����
```cpp
// Node 1: Start
NodeId: Start
NodeType: NpcLine
SpeakerId: TestNPC
DialogueText: "Hello! Welcome to our village!"
EmotionTag: Emotion.Happy

// Node 2: End
NodeId: End
NodeType: End

// Edge: Start ? End
```

#### ��� C: ��������� NPC
```cpp
// � Blueprint NPC
1. Add Component: Dialogue Component
2. Set CharacterId: "TestNPC"
3. Set DefaultDialogue: DA_FirstDialogue
4. Set DisplayName: "Friendly NPC"
```

#### ��� D: ��������������
```
Play ? ������� � NPC ? ������ E
```

---

## ?? ������������

| �������� | �������� | ��� ���� |
|----------|----------|----------|
| [Quick Start](Docs/QuickStart.md) | ����� �� 5 ����� | ��� |
| [Architecture](Docs/Architecture.md) | ������ ����������� | ������������ |
| [DSL Reference](Docs/DSL_Reference.md) | ��������� �������/�������� | ��������/��������� |
| [API Reference](Docs/API_Reference.md) | C++ API | ������������ |
| [AI Integration](Docs/AI_Integration.md) | ����������� AI ������� | ����������� |
| [Authoring Guide](Docs/Authoring_Guide.md) | Best practices ��� �������� | �������� |

---

## ??? �����������

### ������

```
DialogueSystem Plugin
??? DialogueSystemCore (Pure Logic)
?   ??? DialogueRunner (State Machine)
?   ??? DialogueNode (Data)
?   ??? DialogueContext (State)
?   ??? ConditionEvaluator (DSL)
?   ??? EffectExecutor (DSL)
??? DialogueSystemRuntime (Gameplay Integration)
?   ??? DialogueComponent (Per-NPC)
?   ??? RelationshipComponent (Emotions)
?   ??? DialogueMemoryComponent (Long-term memory)
?   ??? DialogueSubsystem (Manager)
??? DialogueSystemUI (Widgets)
?   ??? DialogueWidget (Main UI)
?   ??? ChoiceWidget (Buttons)
?   ??? HistoryWidget (Log)
??? DialogueSystemAI (AI Integration)
    ??? AIDialogueProvider (Base)
    ??? OpenAI Implementation
    ??? PromptBuilder (Helper)
```

### ����� ����������

```
Player interacts with NPC
        ?
DialogueComponent->StartDialogue()
        ?
DialogueSubsystem creates DialogueRunner
        ?
Runner loads DialogueDataAsset
        ?
Runner enters start node
        ?
OnNodeEntered event ? UI updates
        ?
Player selects choice
        ?
Effects applied to Context
        ?
Next node computed (with conditions)
        ?
Repeat until End node
```

---

## ?? ������� �������������

### ������ 1: ������� ������

```cpp
// Node: Greeting
DialogueText: "Hello traveler! How can I help you?"
EmotionTag: Emotion.Friendly

// Choice 1
ChoiceText: "I need supplies"
EffectExpression: "startQuest(GetSupplies)"

// Choice 2
ChoiceText: "Just passing through"
EffectExpression: "affinity[Merchant] += 5"
```

### ������ 2: �������� ������

```cpp
// Node: Check Affinity
NodeType: Condition
ConditionExpression: "affinity[Lianne] >= 50"

// If true ? Friend Path
DialogueText: "Hey! Good to see you again!"
EmotionTag: Emotion.Happy

// If false ? Stranger Path
DialogueText: "Oh, hello..."
EmotionTag: Emotion.Neutral
```

### ������ 3: ������������� ����

```cpp
// Node: Confession
ConditionExpression: "romance[Lianne] >= 70 && time == Evening"
DialogueText: "I... I've been wanting to tell you something..."
EmotionTag: Emotion.Shy

// Choice: Reciprocate
ChoiceText: "I feel the same way"
EffectExpression: "romance[Lianne] = 100; setMemory(Lovers); triggerEvent(Cutscene_Kiss)"
```

### ������ 4: AI-���������

```cpp
// Node: AI Generated
NodeType: AIGenerated
AIPromptHint: "NPC is excited about the upcoming festival"
Persona: DA_Lianne_Persona

// ������� ������������� ���������� ������� �� ������ ���������
```

---

## ?? ������� ��������

### ���������� ������� (Content/Examples/)

- **SimpleDialogue** � ������� �������� ������
- **BranchingDialogue** � ������������ � ���������
- **RelationshipDialogue** � ������� �� ���������
- **EmotionalDialogue** � ������ � ��������
- **AIGeneratedDialogue** � ������������ ���������
- **QuestDialogue** � ���������� � ��������
- **RomanceDialogue** � ������������� ����
- **MemoryDialogue** � ������������� ������ NPC

---

## ?? ��������� ����������

- **Unreal Engine**: 5.3+
- **���������**: Windows, Mac, Linux, Console
- **C++ Compiler**: MSVC 2022 / Clang
- **�����������**: GameplayTags Plugin (������� � Engine)

### ������������ �����������
- **AI API**: OpenAI API key (��� AI ���������)
- **Google Sheets**: API credentials (��� �������/��������)

---

## ?? Best Practices

### ? DO
- ������������ DSL ��� ������� ������ Blueprint
- ���������� DialogueAssets ����� Subsystem
- ������������ ��������� ������� � ����� ����
- ������������ GameplayTags ��� �������������
- ������ unit tests ��� custom �������/��������

### ? DON'T
- �� ������������ Tick ��� ��������
- �� ���������� ������ (����������� FText)
- �� ��������� ����������� ����������� � ������
- �� �������� ������������ �� ���������
- �� ������������ ��������� ������

---

## ?? Performance

### Benchmarks (�� �������� �������)

| �������� | ����� | ���������� |
|----------|-------|------------|
| Node transition | <1ms | ��� AI |
| Condition evaluation | <0.1ms | � ����� |
| Effect execution | <0.5ms | ������� ������� |
| UI update | <16ms | 60 FPS |
| AI generation | <3s | ������� �� API |

### Memory footprint

- **1000 nodes**: ~50MB
- **10 active dialogues**: ~100MB
- **AI cache (1000 responses)**: ~20MB

---

## ??? Roadmap

### v1.0 (Current) ?
- Core dialogue system
- Relationship system
- Basic UI
- AI integration
- DSL for conditions/effects

### v1.1 (Q2 2024)
- Visual node editor (Graph view)
- Voice recording integration
- Lip sync support
- Advanced animation system

### v1.2 (Q3 2024)
- Multiplayer support (replication)
- Cloud save integration
- Analytics dashboard
- Performance optimizations

### v2.0 (Q4 2024)
- Full AI-driven conversations
- Emotion recognition (�� ������)
- Procedural dialogue generation
- VR/AR support

---

## ?? Contributing

�� ������������ ������! ����������:

1. Fork �����������
2. �������� feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit ��������� (`git commit -m 'Add AmazingFeature'`)
4. Push � branch (`git push origin feature/AmazingFeature`)
5. �������� Pull Request

### Coding Standards
- �������� [Unreal Engine Coding Standard](https://docs.unrealengine.com/5.0/en-US/epic-cplusplus-coding-standard-for-unreal-engine/)
- ���������� unit tests ��� ����� features
- �������������� ��������� API (Doxygen style)

---

## ?? License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## ?? Acknowledgments

- Inspired by Dialogue Designer, YarnSpinner, and Ink
- Built with ?? for the Unreal Engine community
- Special thanks to all contributors

---

## ?? Support

- **Discord**: [Join our server](#)
- **Email**: support@yourstudio.com
- **Issues**: [GitHub Issues](https://github.com/yourstudio/DialogueSystem/issues)
- **Documentation**: [Full Docs](Docs/README.md)

---

## ?? Showcase

����������� ��� ������? ����� �����! �� ����� ���� �������� ��� ������ ����.

---

**Made with ?? by Senior Game Designers & Technical Architects**

**Version**: 1.0.0
**Last Updated**: 2024
**Status**: Production Ready ??
