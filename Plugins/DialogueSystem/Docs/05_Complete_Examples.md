# Complete Integration Examples

## �����

���� �������� �������� ������ ��������� ������� ���������� ���� ������� �������� � ��� ������ Unreal Engine 5. ������� ��������� ��� �������� �������� ������������� �� �������� ������� �� ������� AI-������������ ���������� � �������� ���������.

---

## ������ 1: ������� ������ � NPC (�� ������ �� �����)

### ����
������� ������� ������ � NPC "������", ������� ������������ ������ � ���������� ��������� ��������� ������.

### ��� 1: ���������� �������

1. **��������� �������**:
   - `Edit ? Plugins ? Search "Dialogue System"`
   - ��������� ������� �� �������
   - ������������� ��������

2. **�������� �����**:
   ```
   Content/
   ??? Dialogues/          # Data Assets ��������
   ??? NPCs/               # Blueprints NPC
   ?   ??? Lianne/
   ??? UI/                 # UI �������
   ?   ??? Dialogue/
   ??? Data/               # ������������
       ??? Personas/
   ```

### ��� 2: �������� Data Asset �������

1. **�������� ������**:
   - `Right Click � Content/Dialogues ? Miscellaneous ? Data Asset`
   - �������� �����: `DialogueDataAsset`
   - ��������: `DA_Dialogue_Lianne_Greeting`

2. **���������� �����**:

```
Nodes[0]: (����������� NPC)
  - Node Id: "node_greeting"
  - Node Type: Dialogue
  - Dialogue Text: "������! � ���� ���� ������! ��� ����?"
  - Speaker Name: "Lianne"
  - Speaker Portrait: T_Portrait_Lianne
  - Animation Name: "Talk_Happy"
  - Camera Tag: "Camera.Closeup"

Nodes[1]: (����� ������)
  - Node Id: "node_player_choice"
  - Node Type: PlayerChoice
  - Dialogue Text: "" (�����)

Nodes[2]: (����������� �����)
  - Node Id: "node_response_friendly"
  - Node Type: Dialogue
  - Dialogue Text: "�������! � ���� ���� ���� ������. ����� ���������!"
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Happy"

Nodes[3]: (����������� �����)
  - Node Id: "node_response_neutral"
  - Node Type: Dialogue
  - Dialogue Text: "������. ��� ���� ������?"
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Neutral"

Nodes[4]: (������ �����)
  - Node Id: "node_response_rude"
  - Node Type: Dialogue
  - Dialogue Text: "�����... ����� �� �������."
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Sad"
```

3. **�������� ������**:

```
Connections[0]: (����������� � ������)
  - From Node Id: "node_greeting"
  - To Node Id: "node_player_choice"
  - Choice Text: "" (�����������)

Connections[1]: (����������� �����)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_friendly"
  - Choice Text: "������, ������! ����� ��� ���� ������!"
  - Effect: "affinity[Lianne] += 10; memory(FirstMeeting, true)"
  - Priority: 1

Connections[2]: (����������� �����)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_neutral"
  - Choice Text: "������. �� ���������."
  - Effect: "affinity[Lianne] += 2"
  - Priority: 2

Connections[3]: (������ �����)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_rude"
  - Choice Text: "��� �������. ����."
  - Effect: "affinity[Lianne] -= 5"
  - Priority: 3
```

4. **��������� ���������� ����**:
   ```
   Start Node Id: "node_greeting"
   ```

### ��� 3: �������� NPC Blueprint

1. **�������� Blueprint**:
   - `Right Click � Content/NPCs/Lianne ? Blueprint Class ? Character`
   - ��������: `BP_NPC_Lianne`

2. **���������� �����������**:

```blueprintue
// � BP_NPC_Lianne, Components Panel:

Add Component:
  ? DialogueComponent
  ? RelationshipComponent
  ? NPCMemoryComponent
  ? CapsuleComponent (��� ����������)
```

3. **��������� DialogueComponent**:

```
DialogueComponent (Details Panel):
  - Character Id: "Lianne"
  - Display Name: "������"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Greeting
  - Can Start Dialogue: true
  - Interaction Distance: 300.0
```

4. **��������� RelationshipComponent**:

```
RelationshipComponent (Details Panel):
  - Affinity: 0.0
  - Trust: 0.0
  - Romance: 0.0
  - Status: Neutral
```

### ��� 4: �������� UI �������

1. **�������� ChoiceWidget**:
   - `Right Click � Content/UI/Dialogue ? User Interface ? Widget Blueprint`
   - Parent Class: `ChoiceWidget`
   - ��������: `WBP_ChoiceButton`

2. **��������� �������� WBP_ChoiceButton**:

```
Button (Name: "ChoiceButton") **REQUIRED**
??? Overlay
    ??? Background (Image)
    ?   - Color: (R=0.1, G=0.1, B=0.1, A=0.8)
    ??? HotkeyText (Text Block) [Name: "HotkeyText"] *Optional*
    ?   - Text: "[1]"
    ?   - Color: Yellow
    ?   - Position: Top-Left
    ??? ChoiceText (Text Block) [Name: "ChoiceText"] **REQUIRED**
        - Text: "Choice Text Here"
        - Font: Roboto, Size 16
        - Color: White
        - Padding: (10, 5, 10, 5)
```

3. **�������� DialogueWidget**:
   - `Right Click � Content/UI/Dialogue ? User Interface ? Widget Blueprint`
   - Parent Class: `DialogueWidget`
   - ��������: `WBP_DialogueWidget`

4. **��������� �������� WBP_DialogueWidget**:

```
Canvas Panel
??? Background (Image)
?   - Brush: SolidColor
?   - Color: (R=0, G=0, B=0, A=0.7)
?   - Anchors: Fill Screen
?
??? MainContainer (Vertical Box)
?   - Anchors: Bottom Center
?   - Position: (0, -200)
?   - Size: (800, 400)
?   ?
?   ??? PortraitContainer (Horizontal Box)
?   ?   ??? SpeakerPortrait (Image) [Name: "SpeakerPortrait"] **REQUIRED**
?   ?       - Size: (100, 100)
?   ?       - Brush: T_Portrait_Placeholder
?   ?
?   ??? TextContainer (Vertical Box)
?   ?   ??? SpeakerName (Text Block) [Name: "SpeakerName"] **REQUIRED**
?   ?   ?   - Text: "Speaker Name"
?   ?   ?   - Font: Roboto-Bold, Size 20
?   ?   ?   - Color: Yellow
?   ?   ?
?   ?   ??? DialogueText (Rich Text Block) [Name: "DialogueText"] **REQUIRED**
?   ?       - Text: "Dialogue text goes here..."
?   ?       - Font: Roboto, Size 16
?   ?       - Color: White
?   ?       - Auto Wrap Text: true
?   ?
?   ??? ChoicesContainer (Vertical Box) [Name: "ChoicesContainer"] **REQUIRED**
?       - Padding: (0, 20, 0, 0)
```

5. **��������� � Details Panel WBP_DialogueWidget**:

```
DialogueWidget (Details Panel):
  - Choice Widget Class: WBP_ChoiceButton
  - Enable Typewriter: true
  - Show Portrait: true
  - Show Emotions: false
```

6. **�������� �������� � WBP_DialogueWidget**:

```
// Animations Panel ? New Animation: "Intro_Anim" (0.5 sec)
Track: Canvas Panel
  - 0.0s: Render Opacity = 0.0, Translation Y = -50
  - 0.5s: Render Opacity = 1.0, Translation Y = 0

// New Animation: "Outro_Anim" (0.5 sec)
Track: Canvas Panel
  - 0.0s: Render Opacity = 1.0, Translation Y = 0
  - 0.5s: Render Opacity = 0.0, Translation Y = 50
```

7. **���������� Blueprint Events � WBP_DialogueWidget**:

```blueprintue
// Event Graph:

// Event: Play Intro Animation
Event Play Intro Animation
    ? Play Animation [Intro_Anim]

// Event: Play Outro Animation
Event Play Outro Animation
    ? Play Animation [Outro_Anim]
```

### ��� 5: ���������� � �������

1. **� BP_ThirdPersonCharacter (��� ����� ������)**:

```blueprintue
// Variables:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [Default: WBP_DialogueWidget]
- CurrentDialogueNPC (AActor*)

// Event BeginPlay:
Event BeginPlay
    ? Parent: BeginPlay

// Input Action: Interact (E)
Event InputAction Interact
    ? Get Overlapping Actors (Self) ? Actors
    ? For Each Loop (Actors)
        ? Cast to BP_NPC_Lianne ? LianneNPC
        ? Branch (Cast Success)
            True:
                ? Set CurrentDialogueNPC (LianneNPC)
                ? LianneNPC ? Get Component by Class [DialogueComponent] ? DialogueComp
                ? DialogueComp ? Start Dialogue (Self)
                ? Branch (return value)
                    True:
                        ? Show Dialogue Widget
                    False:
                        ? Print String "Cannot start dialogue"
```

2. **Custom Function: Show Dialogue Widget**:

```blueprintue
Function: Show Dialogue Widget

// Inputs: (none)
// Outputs: (none)

// Implementation:
? Branch (DialogueWidgetInstance == null)
    True:
        ? Create Widget [DialogueWidgetClass] ? DialogueWidgetInstance
        ? Add to Viewport (DialogueWidgetInstance)
    
? Get Game Instance
    ? Get Subsystem [DialogueSubsystem]
    ? Get Active Dialogue ? ActiveRunner

? DialogueWidgetInstance ? Bind To Runner (ActiveRunner)
? DialogueWidgetInstance ? Play Intro Animation

? Get Player Controller
    ? Set Input Mode UI Only
    ? Set Show Mouse Cursor (true)
```

3. **Custom Function: Hide Dialogue Widget**:

```blueprintue
Function: Hide Dialogue Widget

// Inputs: (none)
// Outputs: (none)

// Implementation:
? Branch (DialogueWidgetInstance != null)
    True:
        ? DialogueWidgetInstance ? Unbind From Runner
        ? DialogueWidgetInstance ? Play Outro Animation
        ? Delay (0.5)
        ? Remove from Parent (DialogueWidgetInstance)
        ? Set DialogueWidgetInstance = null
        
        ? Get Player Controller
            ? Set Input Mode Game Only
            ? Set Show Mouse Cursor (false)
```

### ��� 6: ��������� ������� �������

```blueprintue
// � BP_NPC_Lianne, Event Graph:

// Variables:
- DialogueComp (UDialogueComponent*)

Event BeginPlay
    ? Parent: BeginPlay
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    
    // �������� �� �������
    ? DialogueComp ? On Dialogue Started
        ? Add Event [Handle Dialogue Started]
    
    ? DialogueComp ? On Dialogue Ended
        ? Add Event [Handle Dialogue Ended]

// Custom Event: Handle Dialogue Started
Event Handle Dialogue Started
    Runner (��������)
    ? Print String "Dialogue Started with Lianne"
    ? Play Animation Montage [AM_Talk_Idle]
    ? Set AI State [InDialogue]

// Custom Event: Handle Dialogue Ended
Event Handle Dialogue Ended
    ? Print String "Dialogue Ended"
    ? Stop Animation Montage
    ? Set AI State [Idle]
    
    // ����� Hide Dialogue Widget �� ������
    ? Get Player Character
        ? Cast to BP_ThirdPersonCharacter
        ? Hide Dialogue Widget
```

### ��� 7: ������������

1. **���������� NPC**:
   - ���������� `BP_NPC_Lianne` � �������
   - ���������� ����� � ������ ������ ������

2. **������ ����**:
   - `Alt + P` (Play In Editor)
   - ��������� � NPC
   - ������� `E` ��� ������ �������

3. **��������**:
   - ������ ������ ��������� � ���������
   - ������� ������ ������ ������������
   - ������ ��������� 3 ������ ������
   - ��� ������ ������ ���������� affinity (��������� ����� Debug)

---

## ������ 2: ������ � ��������� � �������

### ����
������� ������, ������� ���������� � ����������� �� ���������� �������� ������ � ��� ��������� � NPC.

### ��� 1: �������� ������� � ���������

1. **�������� Data Asset**:
   - �������� `DA_Dialogue_Lianne_Conditional`

2. **���������� �����**:

```
Nodes[0]: (������ �������)
  - Node Id: "node_first_meeting"
  - Node Type: Dialogue
  - Dialogue Text: "������! � ������. ������� �������������!"
  - Speaker Name: "Lianne"

Nodes[1]: (��������� �������)
  - Node Id: "node_returning"
  - Node Type: Dialogue
  - Dialogue Text: "�, �� ��������! ���� ����� ���� ������!"
  - Speaker Name: "Lianne"

Nodes[2]: (������� ���������)
  - Node Id: "node_high_affinity"
  - Node Type: Dialogue
  - Dialogue Text: "������, �������! � ��� �� ���� �������! ??"
  - Speaker Name: "Lianne"

Nodes[3]: (����� ������)
  - Node Id: "node_choice"
  - Node Type: PlayerChoice
```

3. **�������� ������ � ���������**:

```
Connections[0]: (������ ������� - ������ ���� �� �����������)
  - From Node Id: "START" (����������� ��������� ����)
  - To Node Id: "node_first_meeting"
  - Condition: "!memory(MetLianne)"
  - Effect: "memory(MetLianne, true)"

Connections[1]: (��������� ������� - ������ ���������)
  - From Node Id: "START"
  - To Node Id: "node_returning"
  - Condition: "memory(MetLianne) && affinity[Lianne] < 50"

Connections[2]: (������� ���������)
  - From Node Id: "START"
  - To Node Id: "node_high_affinity"
  - Condition: "memory(MetLianne) && affinity[Lianne] >= 50"

Connections[3-5]: (�������� � ������)
  - From Node Id: "node_first_meeting" / "node_returning" / "node_high_affinity"
  - To Node Id: "node_choice"
```

### ��� 2: ���������� BP_NPC_Lianne

```blueprintue
// � BP_NPC_Lianne:

// �������� Default Dialogue:
DialogueComponent:
  - Default Dialogue: DA_Dialogue_Lianne_Conditional
```

### ��� 3: ������������ ������

```blueprintue
// � BP_ThirdPersonCharacter, �������� Debug �������:

// Input Action: Debug Affinity (F1)
Event InputAction Debug Affinity
    ? Get Actor of Class [BP_NPC_Lianne] ? Lianne
    ? Lianne ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Affinity ? Affinity
    ? Print String ("Affinity: " + ToString(Affinity))
    
    ? Lianne ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Get Memory ("MetLianne") ? HasMet
    ? Print String ("Has Met: " + ToString(HasMet))
```

---

## ������ 3: ������� ������� ����� �������

### ����
������� ������, ������� �������� �����, ����������� �������� � ��������� �����.

### ��� 1: �������� ���������� �������

```
Nodes[0]: (����������� ������)
  - Node Id: "node_quest_offer"
  - Node Type: Dialogue
  - Dialogue Text: "������ ������ ���? ��� ����� ����� ������ ������ � ����."
  - Speaker Name: "Lianne"

Nodes[1]: (�����: �������/���������)
  - Node Id: "node_quest_choice"
  - Node Type: PlayerChoice

Nodes[2]: (����� ������)
  - Node Id: "node_quest_accepted"
  - Node Type: Dialogue
  - Dialogue Text: "�������! �� ��� ����� ��������!"
  - Speaker Name: "Lianne"

Nodes[3]: (����� ��������)
  - Node Id: "node_quest_declined"
  - Node Type: Dialogue
  - Dialogue Text: "��... �� �����, ����� � ������ ���."
  - Speaker Name: "Lianne"

Nodes[4]: (����� � ���������)
  - Node Id: "node_quest_progress"
  - Node Type: Dialogue
  - Dialogue Text: "����� ������?"
  - Speaker Name: "Lianne"

Nodes[5]: (����� ��������)
  - Node Id: "node_quest_complete"
  - Node Type: Dialogue
  - Dialogue Text: "�� ����� ���! ������� �������! ���, ������ �������."
  - Speaker Name: "Lianne"

Connections:
[0]: START ? node_quest_offer
     Condition: "!memory(QuestOffered)"
     
[1]: node_quest_offer ? node_quest_choice

[2]: node_quest_choice ? node_quest_accepted
     Choice Text: "�������, � ������!"
     Effect: "startQuest(FindFlower); memory(QuestOffered, true); memory(QuestActive, true); affinity[Lianne] += 10"

[3]: node_quest_choice ? node_quest_declined
     Choice Text: "������, � �����."
     Effect: "memory(QuestOffered, true); affinity[Lianne] -= 5"

[4]: START ? node_quest_progress
     Condition: "memory(QuestActive) && !hasItem(RareFlower)"

[5]: START ? node_quest_complete
     Condition: "memory(QuestActive) && hasItem(RareFlower)"
     Effect: "completeQuest(FindFlower); removeItem(RareFlower, 1); gold += 100; affinity[Lianne] += 20; memory(QuestActive, false); memory(QuestCompleted, true)"
```

### ��� 2: ���������� � �������� �������

```cpp
// � ����� DialogueService (C++):

void UMyDialogueService::StartQuest(const FString& QuestId)
{
    UQuestManager* QuestManager = GetGameInstance()->GetSubsystem<UQuestManager>();
    if (QuestManager)
    {
        QuestManager->StartQuest(FName(*QuestId));
        UE_LOG(LogDialogue, Log, TEXT("Quest started: %s"), *QuestId);
    }
}

void UMyDialogueService::CompleteQuest(const FString& QuestId)
{
    UQuestManager* QuestManager = GetGameInstance()->GetSubsystem<UQuestManager>();
    if (QuestManager)
    {
        QuestManager->CompleteQuest(FName(*QuestId));
        UE_LOG(LogDialogue, Log, TEXT("Quest completed: %s"), *QuestId);
    }
}

bool UMyDialogueService::HasItem(const FString& ItemName, int32 Count) const
{
    UInventoryComponent* Inventory = GetPlayerInventory();
    if (Inventory)
    {
        return Inventory->HasItem(FName(*ItemName), Count);
    }
    return false;
}
```

---

## ������ 4: AI-������������ �������

### ����
������� ������������ ������, ��� ������ NPC ������������ � ������� AI (OpenAI GPT).

### ��� 1: ��������� API �����

1. **�������� .env �����**:
   - � ����� ������� �������� `Config/.env`
   - ��������: `OPENAI_API_KEY=sk-proj-your-key-here`

2. **��������� � Project Settings**:
   ```
   Edit ? Project Settings ? Plugins ? Dialogue System ? AI:
     - API Key: (���������� �� .env)
     - Model: "gpt-4"
     - Temperature: 0.7
     - Max Tokens: 150
   ```

### ��� 2: �������� Persona Data

1. **�������� Data Asset**:
   - `Right Click ? Miscellaneous ? Data Asset`
   - �����: `DialoguePersonaData`
   - ��������: `DA_PersonaData_Lianne`

2. **����������**:

```
Character Name: "������"

Description: 
"������� ���������, 25 ���. ������, ������������ � ���������� ��������. 
����� �������, ����� � ��������. ������� �����������, �� ���������."

Personality:
  - "������"
  - "����������"
  - "�����������"
  - "�����������"
  - "�������������"
  - "������"

Backstory:
"������ ������� � ��������� ���������� �������. � ������� ������� ����� ����������. 
��������� � �����, ����� ������� � �������������� �����. ������ ����� ���� � ��������� 
������ � �������� ��� ������ ���������. ������� � ������� ��������."

Speech Style:
"������� ����� � �����. ����� ���������� �������������� ��������. 
��������� ��� ���������. ������ ���������� ���������� � ������������."

Example Dialogues:
  [0]: "User: ������! ? Lianne: �, ������! *���������* ���� ���� ������! ������� ����� ���������� ����, ������?"
  [1]: "User: ��� ����? ? Lianne: ���������! ������� ����� �������� �����... �� ��� ����� �������������! �����... *�����������* �����������."
  [2]: "User: ��� ���� ��������? ? Lianne: ��� �������� �������� �������. �������� �����... ��� ����� ������ � ��������."
```

### ��� 3: �������� AI Dialogue Component

```blueprintue
// � BP_NPC_Lianne, ��������:

// Variables:
- AIProvider (UAIDialogueProvider_OpenAI*)
- PersonaData (FDialoguePersonaData) [Load from DA_PersonaData_Lianne]
- bUseAIDialogue (bool) [Default: true]

Event BeginPlay
    ? Parent: BeginPlay
    
    ? Branch (bUseAIDialogue)
        True:
            ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
            ? Load Persona Data from Asset ? PersonaData
            ? Print String "AI Dialogue Enabled"
```

### ��� 4: ��������� ���������� ����� ������

```blueprintue
// � WBP_DialogueWidget, ��������:

// New Widget: Editable Text Box (Name: "PlayerInputBox")
PlayerInputBox:
  - Hint Text: "Type your message..."
  - On Text Committed: ? Handle Player Input

// Event: Handle Player Input
Event Handle Player Input
    Text (��������)
    Commit Method (��������)
    
    ? Branch (Commit Method == OnEnter)
        True:
            ? Get Current NPC ? NPC
            ? Cast to BP_NPC_Lianne ? LianneNPC
            ? LianneNPC ? Generate AI Response (Text)
            ? Clear Text (PlayerInputBox)

// � BP_NPC_Lianne:

Function: Generate AI Response
Inputs: UserPrompt (String)

// Implementation:
? Get Component by Class [DialogueComponent] ? DialogueComp
? DialogueComp ? Get Context ? Context

? AIProvider ? Generate Dialogue
    Persona Data: PersonaData
    Context: Context
    User Prompt: UserPrompt
    On Complete: [Bind to: OnAIResponse]

// Custom Event: OnAIResponse
Event OnAIResponse
    Response (��������: FAIDialogueResponse)
    
    ? Branch (Response.bSuccess)
        True:
            // ������� ������������ ����
            ? Create Dialogue Node ? NewNode
                - Dialogue Text: Response.GeneratedText
                - Speaker Name: "Lianne"
                - Tags: [Response.SuggestedEmotion]
            
            // ��������� ��������� ���������
            ? Get Component by Class [RelationshipComponent] ? RelComp
            ? RelComp ? Modify Affinity (Response.SuggestedAffinityDelta)
            
            // ���������� � UI
            ? Get Player Character
                ? Cast to BP_ThirdPersonCharacter
                ? Get Dialogue Widget Instance
                ? Update Node Display (NewNode)
        
        False:
            ? Print String ("AI Error: " + Response.ErrorMessage)
            // Fallback �� ����������������� ������
            ? Use Default Dialogue
```

### ��� 5: ����������� ������

```cpp
// � C++ (UAIDialogueProvider_OpenAI::BuildPrompt_Implementation):

FString UAIDialogueProvider_OpenAI::BuildPrompt_Implementation(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt) const
{
    // System Prompt
    FString SystemPrompt = FString::Printf(TEXT(
        "You are %s, a character in a video game.\n\n"
        "Character Description:\n%s\n\n"
        "Personality Traits: %s\n\n"
        "Backstory:\n%s\n\n"
        "Speech Style:\n%s\n\n"
        "Instructions:\n"
        "- Stay in character at all times\n"
        "- Respond naturally and conversationally\n"
        "- Keep responses concise (1-2 sentences)\n"
        "- Use the speech style described above\n"
        "- Show emotion through text (*smiles*, *looks down*, etc.)\n"
        "- Consider the current context\n\n"
    ),
    *PersonaData.CharacterName,
    *PersonaData.Description,
    *FString::Join(PersonaData.Personality, TEXT(", ")),
    *PersonaData.Backstory,
    *PersonaData.SpeechStyle
    );

    // Context Prompt
    FString ContextPrompt = TEXT("");
    if (Context)
    {
        float Affinity = Context->GetRelationshipValue(TEXT("Lianne"), TEXT("affinity"));
        FString TimeOfDay = Context->GetCurrentTimeString();
        
        ContextPrompt = FString::Printf(TEXT(
            "Current Context:\n"
            "- Time: %s\n"
            "- Your affinity with the player: %.0f/100 (%s)\n"
            "- Recent events: %s\n\n"
        ),
        *TimeOfDay,
        Affinity,
        Affinity > 70 ? TEXT("Very friendly") : (Affinity > 30 ? TEXT("Friendly") : TEXT("Neutral")),
        *Context->GetRecentEventsString()
        );
    }

    // Few-Shot Examples
    FString FewShotExamples = TEXT("Example Interactions:\n\n");
    for (const FString& Example : PersonaData.ExampleDialogues)
    {
        FewShotExamples += Example + TEXT("\n");
    }
    FewShotExamples += TEXT("\n");

    // Full Prompt
    FString FullPrompt = FString::Printf(TEXT(
        "%s%s%sUser: %s\n\n%s:"
    ),
    *SystemPrompt,
    *ContextPrompt,
    *FewShotExamples,
    *UserPrompt,
    *PersonaData.CharacterName
    );

    return FullPrompt;
}
```

---

## ������ 5: ������ ���������� � �����������

### ����
������� ������� � ����������� ��������� ��������, ��������� � ������ NPC ����� �������� ����.

### ��� 1: �������� SaveGame ������

```cpp
// � C++ (MySaveGame.h):

UCLASS()
class UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // ��������� ��������
    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TMap<FString, bool> DialogueMemories;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TMap<FString, float> DialogueVariables;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TArray<FName> CompletedDialogues;

    // ��������� � NPC
    UPROPERTY(VisibleAnywhere, Category = "Relationships")
    TMap<FString, float> NPCAffinityLevels;

    UPROPERTY(VisibleAnywhere, Category = "Relationships")
    TMap<FString, int32> NPCRelationshipStatus;

    // ������ NPC
    UPROPERTY(VisibleAnywhere, Category = "Memory")
    TMap<FString, TMap<FString, bool>> NPCMemories;

    UPROPERTY(VisibleAnywhere, Category = "Memory")
    TMap<FString, TMap<FString, int32>> NPCNodeVisitCounts;

    // ����������
    UPROPERTY(VisibleAnywhere, Category = "Meta")
    FDateTime SaveTime;

    UPROPERTY(VisibleAnywhere, Category = "Meta")
    int32 SaveVersion = 1;
};
```

### ��� 2: ���������� ����������

```cpp
// � C++ (MyGameInstance ��� SaveSystem):

void UMyGameInstance::SaveGame()
{
    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
    );

    if (!SaveGame)
        return;

    // ���������� ��������� �������� ����� Subsystem
    UDialogueSubsystem* DialogueSubsystem = GetSubsystem<UDialogueSubsystem>();
    if (DialogueSubsystem)
    {
        // ��������� JSON �������������
        FString DialogueStateJson = DialogueSubsystem->SaveStateToJson();
        // ������� � ��������� � SaveGame ���������
        
        // ��� ������������ ���������� ����������
        DialogueSubsystem->SaveDialogueState();
    }

    // ���������� ���������
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NPC"), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        URelationshipComponent* RelComp = Actor->FindComponentByClass<URelationshipComponent>();
        if (RelComp)
        {
            FString NPCId = RelComp->GetOwner()->GetName();
            SaveGame->NPCAffinityLevels.Add(NPCId, RelComp->GetAffinity());
            SaveGame->NPCRelationshipStatus.Add(NPCId, (int32)RelComp->GetStatus());
        }

        UNPCMemoryComponent* MemoryComp = Actor->FindComponentByClass<UNPCMemoryComponent>();
        if (MemoryComp)
        {
            FString NPCId = MemoryComp->GetOwner()->GetName();
            SaveGame->NPCMemories.Add(NPCId, MemoryComp->GetAllMemories());
            SaveGame->NPCNodeVisitCounts.Add(NPCId, MemoryComp->GetAllVisitCounts());
        }
    }

    // ����������
    SaveGame->SaveTime = FDateTime::Now();

    // ��������� � ����
    UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("MainSaveSlot"), 0);
    
    UE_LOG(LogTemp, Log, TEXT("Game Saved at %s"), *SaveGame->SaveTime.ToString());
}
```

### ��� 3: ���������� ��������

```cpp
void UMyGameInstance::LoadGame()
{
    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::LoadGameFromSlot(TEXT("MainSaveSlot"), 0)
    );

    if (!SaveGame)
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found"));
        return;
    }

    // �������� ��������� ��������
    UDialogueSubsystem* DialogueSubsystem = GetSubsystem<UDialogueSubsystem>();
    if (DialogueSubsystem)
    {
        DialogueSubsystem->LoadDialogueState();
    }

    // �������� ��������� (����� spawn NPC)
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NPC"), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        FString NPCId = Actor->GetName();

        URelationshipComponent* RelComp = Actor->FindComponentByClass<URelationshipComponent>();
        if (RelComp && SaveGame->NPCAffinityLevels.Contains(NPCId))
        {
            RelComp->SetAffinity(SaveGame->NPCAffinityLevels[NPCId]);
            RelComp->SetStatus((ERelationshipStatus)SaveGame->NPCRelationshipStatus[NPCId]);
        }

        UNPCMemoryComponent* MemoryComp = Actor->FindComponentByClass<UNPCMemoryComponent>();
        if (MemoryComp && SaveGame->NPCMemories.Contains(NPCId))
        {
            MemoryComp->LoadMemories(SaveGame->NPCMemories[NPCId]);
            MemoryComp->LoadVisitCounts(SaveGame->NPCNodeVisitCounts[NPCId]);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Game Loaded from %s"), *SaveGame->SaveTime.ToString());
}
```

### ��� 4: ��������������

```blueprintue
// � BP_GameMode:

Event BeginPlay
    ? Set Timer by Function Name
        Function Name: "AutoSave"
        Time: 300.0 (������ 5 �����)
        Looping: true

Function: AutoSave
? Get Game Instance
    ? Cast to MyGameInstance
    ? Save Game
? Print String "Auto-saved"
```

---

## ����������

��� ������� ��������� �������� �������� ������������� DialogueSystem �� ������� ����������� �������� �� ������� AI-������������ ���������� � ����������� ���������. ������� ��������� ��������� � ����� ���� ������������ ��� ����� ����� ������ �������.

### �������������� �������:

- `01_DialogueSystemCore_Module.md` � ����������� � ���� �������
- `02_DialogueSystemRuntime_Module.md` � Runtime ���������� � Subsystem
- `03_DialogueSystemUI_Module.md` � UI ������� � �� ���������
- `04_DialogueSystemAI_Module.md` � AI ���������� � ������ ����������
- `DSL_Parser_Documentation.md` � ��������� DSL ��� ������� � ��������

### ������ �� �������:

1. **�������� �����������**:
   ```cpp
   LogDialogue: Log
   LogDialogueAI: Verbose
   ```

2. **����������� Debug Draw**:
   - ����������� ������� affinity level ��� NPC
   - ����������� ��������� �������

3. **���������� ��������**:
   - ������� � �������� �������
   - ���������� ���������� �������, �������, AI

4. **������������ ������������������**:
   - ����������� Unreal Insights
   - ��������� ���������� ���� ��������

### ��������� �����������:

- AI ������� ������� ��������-���������� (���� �� ����������� ��������� ������)
- ������� ������� ����� �������� ����� ������
- Typewriter ������ ����� ���� ��������� ��� ����� ������� �������

��� ��������� � �������� ��. README.md � ����� �������.
