# ?? ����������� �� ������� ������� ��������

## ��������: �� ������������ ����� ������� � ������ ������

### ���� ��� �����������:

## 1. ���������� � ������������

**�������� Unreal Editor** � ��������� ����������:

```bash
cd "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles"
.\Build.bat EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -WaitMutex
```

��� ����������� **Ctrl+Alt+F11** � ��������� ��� Live Coding.

## 2. �������� �����

����� ������� ������� �������� **Output Log** � Unreal Editor � ������� ������:

### �������� ����� � ����:
```
LogTemp: DialogueWidget::HandleNodeEntered - Node: [NodeId]
LogTemp: DialogueWidget::HandleNodeEntered - Dialogue Text: [Your text here]
```

? **���� ������ �����** - ������ �������� ���������  
? **���� ����� ������** - �������� � DialogueNode data

### �������� typewriter:
```
LogTemp: DialogueWidget::StartTypewriter - FullText: '[Your text]', bEnableTypewriter: 1, bIsTyping: 1
LogTemp: DialogueWidget::StartTypewriter - Set DialogueText to: ''
```

? **���� FullText �� ������** - ����� �������  
? **���� DialogueText is NULL** - ������ �� ��������

### �������� �������� �������:
```
LogTemp: DialogueWidget::CreateChoiceButtons - Creating X choices
LogTemp: DialogueWidget::CreateChoiceButtons - Creating choice 0: [Choice text]
LogTemp: ChoiceWidget::SetChoiceData - Node: [NodeId], Index: 0
LogTemp: ChoiceWidget::UpdateVisuals - Setting choice text: '[Choice text]'
```

? **���� ������ ������ �������** - ������ ����  
? **���� "ChoicesContainer is NULL"** - ������ �� ��������  
? **���� "ChoiceWidgetClass is NULL"** - ����� �� ����������

## 3. �������� ������� � Blueprint

### DialogueWidget (WBP_DialogueMain):

1. **�������� ������** � ���������
2. **Hierarchy** - ��������� �������:
   - ? `SpeakerName` (Text Block) - � ������ ������!
   - ? `DialogueText` (Rich Text Block) - � ������ ������!
   - ? `ChoicesContainer` (Vertical Box) - � ������ ������!

3. **Details Panel** ? **Class Settings**:
   - Parent Class: `DialogueWidget` (C++)
   - ? ��������� ��� ��� ���������� ������������ �����

4. **Details Panel** ? **Default** (��� ������):
   - ? **Choice Widget Class** ������ ���� ���������� �� `WBP_DialogueChoice`
   - ? **Enable Typewriter** = `true` (���� ����� ������ ������)
   - ? **Characters Per Second** > 0

### ChoiceWidget (WBP_DialogueChoice):

1. **Hierarchy** - ���������:
   - ? `ChoiceButton` (Button) - � ������ ������!
   - ? `ChoiceText` (Rich Text Block) - � ������ ������!

2. **Parent Class**: `ChoiceWidget` (C++)

## 4. ������� ���� � ����������� Typewriter

���� ����� �� ����������, **��������� typewriter** ��� �����:

� `WBP_DialogueMain` ? **Details**:
- **Enable Typewriter** = `false`

��� ������� ����� ����� ��� ��������.

## 5. �������� DialogueNode Data Asset

�������� ��� Dialogue Data Asset � ���������:

1. **Dialogue Text** �� ������:
   ```
   DialogueText: "Hello, traveler!"
   ```

2. **Choice Text** �� ������ ��� ����� ������:
   ```
   ChoiceText: "Tell me more"
   ```

3. **NodeType** ����������:
   - `NpcLine` - ��� ������ NPC
   - `PlayerChoice` - ��� ������� ������

## 6. �������� �������� �������

� ����� Blueprint ���� (GameMode, PlayerController � �.�.):

```blueprint
// ��������� ��� ������ ��������� ������
Create Widget ? Class = WBP_DialogueMain

// ��������� ��� ������ �������� �� �����
Add to Viewport

// ��������� ��� Runner ��������
Get Dialogue Runner ? Widget ? Bind To Runner

// ��������� ��� ������ �������
Runner ? Start Dialogue
```

## 7. ������ ������

### ? ������ �� ������������ ������
**�������**: �� �������� � Viewport ��� Z-Order ������  
**�������**: `Add to Viewport` � ��������� Z-Order

### ? ����� ������
**�������**: 
- DialogueText ������ �� �������� (������������ ���)
- DialogueNode.DialogueText ������
- Typewriter ������� �� Characters Per Second = 0

**�������**: 
1. ��������� ��� �������: ����� `DialogueText`
2. ��������� Data Asset
3. ��������� ��������� Typewriter

### ? ������ ������ �� ����������
**�������**:
- ChoicesContainer �� ��������
- ChoiceWidgetClass �� ����������
- DialogueRunner �� ���������� choices

**�������**:
1. ��������� ���: ����� `ChoicesContainer`
2. ���������� Choice Widget Class
3. ��������� ��� DialogueNode ����� connections

### ? ������ ���� �� ��� ������
**�������**:
- ChoiceText ������ �� �������� � WBP_DialogueChoice
- DialogueNode.ChoiceText ������

**�������**:
1. � WBP_DialogueChoice ��������� ���: ����� `ChoiceText`
2. � Data Asset ��������� ChoiceText

## 8. �������� � ����������� ��������

�������� �������� ������ � ����������� �����:

**WBP_TestDialogue** (��������� DialogueWidget):

```
Canvas Panel
??? Vertical Box
    ??? Text Block "SpeakerName"
    ??? Rich Text Block "DialogueText"
    ??? Vertical Box "ChoicesContainer"
```

**WBP_TestChoice** (��������� ChoiceWidget):

```
Button "ChoiceButton"
??? Rich Text Block "ChoiceText"
```

���� � ����� �������� �������� - �������� � ����� ������� �������.

## 9. ������� ��� ������� ��������

� **Output Log** �������� �������:

```
// �������� ��� �������� �������
Widget.Debug.Show

// �������� ������ ����������� �������
Widget.Debug.Dump WBP_DialogueMain

// �������� ��������� �����������
Log LogTemp Verbose
```

## 10. ��� ������?

���� ����� ���� �������� �� ��������:

1. ? **������������� ���** - Output Log ? Save to File
2. ? **�������� ���������**:
 - Hierarchy �������
   - Details Panel � �����������
   - Blueprint ��� ������� �������
3. ? **��������� ������ �������** - ��������� ��� ��� ����� ���������

## �������� ������� �������

- **~** (������) - ������� �������
- **F1** - ������ UI (��������� ��� ������ ������ �� ������)
- **Ctrl+Shift+.** - Widget Reflector (������� UI)
- **Alt+P** - PIE (Play In Editor)

---

**����� ���������� � ������ ��������� ���� � �������� Output Log ��� �������!**
