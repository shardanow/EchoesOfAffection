# Blueprint Setup Guide - NPC Memory System

## ������� �����

### ��� 1: ���������� ����������� � NPC

1. �������� Blueprint ������ NPC (��������, `BP_NPC_Villager`)
2. � ������ Components �������� ��������� ����������:
   - `DialogueComponent` (���� ��� �� ��������)
   - `RelationshipComponent` (���� ��� �� ��������)
   - `NPCMemoryComponent` ? **�����**

### ��� 2: ��������� NPCMemoryComponent

� Details ������ ��� NPCMemoryComponent ���������:

**Basic Settings:**
- `Max Memories`: 100 (��� �������� NPC), 150 (��� ������� NPC)
- `Max Topics`: 50

**Decay Settings:**
- `Enable Memory Decay`: ? (�������������)
- `Base Decay Rate`: 0.05 (����������� ���������)
- `Min Importance Threshold`: 10.0 (���������� ������ ������������)
- `Emotional Memory Bonus`: 0.5 (������������� ������� �������� ������)

**Personality Settings:**
- `Memory Retention Multiplier`: 
  - 0.5 = ������ ������ (������, �������)
  - 1.0 = ���������� ������ (����������� NPC)
  - 2.0 = �������� ������ (������, ������)
  
- `Negative Bias`:
  - 0.5 = �������� (������ �������)
  - 1.0 = ����������������
  - 2.0 = ��������� (������ ������ ������)

**Performance:**
- `Decay Update Interval`: 60.0 (��������� decay ��� � ������)

### ��� 3: �������� DialogueDataAsset � �������������� ������

#### ������ 1: ������ ������� vs ��������� �������

**Node: "Greeting"** (Start Node)

```
Conditions:
?? Memory Count Check
?  ?? Memory Type: Dialogue Event
?  ?? Min Count: 0
?  ?? Max Count: 0
?
Dialogue Text: "Hello, stranger. I haven't seen you around here before."

Effects:
?? Create Memory
   ?? Memory Type: Dialogue Event
   ?? Description: "First meeting with player"
   ?? Importance: 50.0
   ?? Emotion: Neutral
   ?? Remember Player: true
```

**Node: "Greeting_Repeat"** (Start Node Alternative)

```
Conditions:
?? Memory Count Check
?  ?? Memory Type: Dialogue Event
?  ?? Min Count: 1
?
Dialogue Text: "Oh, hello again! Good to see you."

Effects:
?? Discuss Topic
   ?? Topic Name: "Greeting"
   ?? Interest Modifier: -2.0  // ���� ������ �������� � ��������� ������������
```

#### ������ 2: �������� � ��� ��������

**Node: "Request_Help"**

```
Dialogue Text: "Could you help me find my lost ring? It's very precious to me."

Choices:
?? "I'll help you" ? [Accept_Promise]
?? "Sorry, I'm busy" ? [Decline_Help]
```

**Node: "Accept_Promise"**

```
Dialogue Text: "Thank you so much! I knew I could count on you!"

Effects:
?? Remember Promise
?  ?? Promise ID: "FindLostRing"
?  ?? Description: "Player promised to find my lost ring"
?  ?? Importance: 90.0
?  ?? Related Tags: Quest.FindRing
?
?? Modify Affinity: +10.0
```

**Node: "Check_Promise"** (� ��������� �������)

```
Conditions:
?? Remembers Event
   ?? Memory ID: "FindLostRing"
   ?? Min Importance: 80.0

Dialogue Text: "Have you found my ring yet?"

[���� ����� ������ ������ - ������� � �������]
```

#### ������ 3: ������ ������

**Node: "Reveal_Secret"**

```
Player Choice: "I have to tell you something... I'm actually a prince in disguise."

Effects:
?? Remember Secret
?  ?? Secret ID: "PlayerRoyalty"
?  ?? Description: "Player revealed they are royalty"
?  ?? Importance: 95.0
?  ?? Secret Tags: Secret.Identity, Important
?
?? Modify Trust: +20.0
```

**Node: "Reference_Secret"** (����� � ������ �������)

```
Conditions:
?? Remembers Event
   ?? Memory ID: "PlayerRoyalty"
   ?? Min Freshness: 0.3

Dialogue Text: "Your secret is safe with me, your highness." [�����������]
```

#### ������ 4: ������� �� �������

**Node: "Receive_Gift"** (When player gives item)

```
Effects:
?? Create Memory
?  ?? Memory Type: Item Given
?  ?? Description: "Player gave me beautiful flowers"
?  ?? Importance: 70.0
?  ?? Emotion: Joyful
?  ?? Context Tags: Event.Gift, Item.Flowers
?
?? Modify Affinity: +15.0
?? Modify Romance: +5.0
```

**Node: "Remember_Gift"** (Next dialogue)

```
Conditions:
?? Has Memories With Tags
   ?? Required Tags: Event.Gift, Item.Flowers
   ?? Require All Tags: true
   ?? Min Memories Found: 1

Dialogue Text: "I still have those beautiful flowers you gave me. They brighten my day every time I look at them!"

Effects:
?? Recall Memory
   ?? Memory ID: [Gift Memory ID]  // ��������� ������
```

#### ������ 5: ������������ �������

**Node: "Betrayal"**

```
Player Choice: "I told the guards about your plans."

Effects:
?? Create Memory
?  ?? Memory Type: Player Choice
?  ?? Description: "Player betrayed me to the guards"
?  ?? Importance: 95.0
?  ?? Emotion: Traumatic
?  ?? Context Tags: Event.Betrayal, Emotion.Anger
?
?? Modify Affinity: -40.0
?? Modify Trust: -60.0
?? Modify Fear: +30.0
```

**Node: "After_Betrayal"** (��� ������� �������)

```
Conditions:
?? Has Memories With Tags
   ?? Required Tags: Event.Betrayal
   ?? Min Memories Found: 1

Dialogue Text: "You... Stay away from me. I can never trust you again."

[��������� ����������� ��������]
```

### ��� 4: ������������ ��������� �����������

�������� ������� � Blueprint NPC:

**Function: "Generate Dynamic Greeting"**

```blueprint
Get Memory Component ? Get Recent Memories (Count: 1)
?
?? If Array Length > 0
?  ?
?  ?? Switch on Emotion
?     ?? Joyful ? Set Greeting: "It's wonderful to see you again!"
?     ?? Traumatic ? Set Greeting: "What do you want?"
?     ?? Nostalgic ? Set Greeting: "Ah, you remind me of better times..."
?     ?? Default ? Set Greeting: "Hello."
?
?? Else ? Set Greeting: "Hello, stranger."
```

### ��� 5: �������� ���������� ��������

**Function: "Check Broken Promises"**

```blueprint
Get Memory Component ? Get Memories By Type (Type: Promise)
?
?? For Each Memory
   ?
   ?? Check if Quest Completed [Your Quest System Logic]
   ?
   ?? If NOT Completed AND Age > 7 Days
   ?  ?
   ?  ?? Effects:
   ?     ?? Modify Trust: -10.0
   ?     ?? Modify Affinity: -5.0
   ?     ?? Create Memory:
   ?        ?? Type: Player Choice
   ?        ?? Description: "Player broke their promise"
   ?        ?? Emotion: Regretful
   ?        ?? Importance: 80.0
   ?
   ?? If Completed
      ?? Effects:
         ?? Modify Trust: +20.0
         ?? Create Memory:
            ?? Type: Player Choice
            ?? Description: "Player kept their promise"
            ?? Emotion: Positive
            ?? Importance: 75.0
```

## ����������� �������

### ������ 6: ������� ������� �� ��������� ����� ������

**Event Graph Node: "Update Relationship Based On Memories"**

```blueprint
Event: Every 60 seconds (Timer)
?
Get Memory Component ? Get Memories About Actor (Player)
?
?? Count Positive Emotions
?? Count Negative Emotions
?
?? Calculate Ratio = (Positive - Negative) / Total
?
?? If Ratio > 0.5
?  ?? Slowly increase Affinity (+1 per minute)
?
?? Else If Ratio < -0.5
   ?? Slowly decrease Affinity (-1 per minute)
```

### ������ 7: ����������� ������� ��� ���������

**Node: "Romance_Available"**

```
Conditions:
?? Memory Emotional Balance
?  ?? Min Positive Memories: 5
?  ?? Max Negative Memories: 1
?
?? Has Memories With Tags
?  ?? Required Tags: Event.Gift, Event.Compliment
?  ?? Require All Tags: false
?  ?? Min Memories Found: 3
?
?? Topic Discussed
?  ?? Topic Name: "Family"
?  ?? Min Discussion Count: 2
?
?? Affinity Check: >= 75

Dialogue Text: "You know... I've really enjoyed spending time with you..."
```

### ������ 8: ������� �� ������������� ����

**Node: "Topic_Politics"**

```
Conditions:
?? Topic Discussed
   ?? Topic Name: "Politics"
   ?? Min Discussion Count: 0
   ?? Max Discussion Count: 0

Dialogue Text: "Politics? It's a complicated topic..."

Effects:
?? Discuss Topic
   ?? Topic Name: "Politics"
   ?? Interest Modifier: -5.0
```

**Node: "Topic_Politics_Repeat"**

```
Conditions:
?? Topic Discussed
   ?? Topic Name: "Politics"
   ?? Min Discussion Count: 2

Dialogue Text: "Can we talk about something else? Politics bore me."

Effects:
?? Discuss Topic
   ?? Topic Name: "Politics"
   ?? Interest Modifier: -10.0
```

## ������ �� �������

### 1. ������ ��������

```
0-30:   ������������ ������� (������� �����������)
31-60:  ������� �������� (���������� ���������)
61-85:  ������ ������� (�������, ������)
86-100: ����������� ������� (��������, �������, �������������)
```

### 2. ������������� �����

����������� ���� ������������:

```
Event
?? Event.Gift
?  ?? Event.Gift.Flower
?  ?? Event.Gift.Jewelry
?? Event.Help
?? Event.Betrayal

Emotion
?? Emotion.Happy
?? Emotion.Sad
?? Emotion.Angry

Topic
?? Topic.Family
?? Topic.Politics
?? Topic.Romance

Quest
?? Quest.Main
?? Quest.Side
```

### 3. �������������� ������

```
������� ? Memory + Affinity + Romance
�������� ? Memory + Trust
������ ? Memory + Trust + [Unlock new dialogues]
������������� ? Memory + Fear + Affinity decrease + [Block dialogues]
```

### 4. ������������

�������� debug ������� ��� ������������:

**Function: "Debug Memory System"**

```blueprint
Print String: "=== NPC Memory Debug ==="

Get Memory Component
?? Get Memory Count
?  ?? Print: "Total Memories: [Count]"
?
?? Get All Memories
?  ?? For Each
?     ?? Print: "[Type] - [Description] - Importance: [Value]"
?
?? Get All Topics
   ?? For Each
      ?? Print: "Topic: [Name] - Interest: [Value] - Count: [Count]"
```

## ������ �������� � �������

### ��������: ������ ����������� ������� ������

**�������:**
- ��������� `Max Memories`
- ��������� `Base Decay Rate`
- ����������� `Prune Old Memories` ������������

### ��������: ������������ ���������� ������� ������

**�������:**
- ��������� `Memory Retention Multiplier`
- ��������� `Base Decay Rate`
- ��������� �������� �������
- ����������� ������������� �������

### ��������: NPC �� ������ ������ �������

**�������:**
- ��������� ��� `Enable Memory Decay` �� ��������� ������ �������
- ��������� `Importance` ������� (>= 70)
- ����������� `Recall Memory` � �������� ��� ���������� ������

### ��������: ������������������

**�������:**
- ��������� `Decay Update Interval`
- ��������� `Max Memories`
- ����������� ���������� `Enable Memory Decay` ��� ������� NPC

## ���������� � Quest System

������ ���������� � ����� �����-��������:

**Quest Begin:**
```blueprint
On Quest Started
?? NPC Memory Component ? Create Memory
   ?? Type: Quest Event
   ?? Description: "Started [Quest Name]"
   ?? Importance: 70.0
   ?? MetaData: {"QuestId": "[Quest ID]"}
```

**Quest Complete:**
```blueprint
On Quest Completed
?? NPC Memory Component ? Find Memories With Tags (Quest.[QuestId])
   ?? Modify Memory Importance: +20.0
   ?? Create New Memory:
      ?? Type: Player Choice
      ?? Description: "Completed [Quest Name]"
      ?? Emotion: Joyful
      ?? Importance: 85.0
```

**Quest Failed:**
```blueprint
On Quest Failed
?? NPC Memory Component ? Create Memory
   ?? Type: Player Choice
   ?? Description: "Failed [Quest Name]"
   ?? Emotion: Negative
   ?? Importance: 70.0
   ?? Effect: Modify Trust: -15.0
```

## ����������

NPCMemoryComponent - ��� ������ ���������� ��� �������� �����, �������������� NPC. ����������������� � ���������� ������������ ������, ������ � ��������� ��� �������� ���������� ����������!
