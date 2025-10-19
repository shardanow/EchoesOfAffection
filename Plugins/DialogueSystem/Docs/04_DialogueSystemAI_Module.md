# DialogueSystemAI Module Documentation

## ����� ������

**DialogueSystemAI** � ��� ������ ���������� � AI/LLM (Large Language Models) ��� ������������ ��������� ��������. ������������ OpenAI GPT, Anthropic Claude, ��������� ������ (LM Studio, Ollama) � ��������� API.

## �����������

```
DialogueSystemAI/
??? Public/
?   ??? AIDialogueProvider.h  # ���������� AI ���������
??? Private/
    ??? ...                   # ����������
```

---

## �������� ������

### 1. UAIDialogueProvider

**����������**: ����������� ������� ����� ��� AI �����������. ��������� ������������ ������� �� ������ ������� NPC, ��������� � ������� ������.

#### �������� ��������:

```cpp
// API ���� (�������� � Project Settings ��� .env �����)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
FString APIKey;

// ������ (��������: gpt-4, gpt-3.5-turbo)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
FString Model = TEXT("gpt-4");

// ����������� (0.0 - 2.0) - ������������ ������������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
float Temperature = 0.7f;

// ������������ ���������� ������� � ������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
int32 MaxTokens = 150;
```

#### �������� ������:

```cpp
// ������������� ������ (����������)
UFUNCTION(BlueprintCallable, Category = "AI")
void GenerateDialogue(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt,
    FAIDialogueResponseDelegate OnComplete
);

// ������������� ������ (���������, ��� ������)
UFUNCTION(BlueprintCallable, Category = "AI")
FAIDialogueResponse GenerateDialogueSync(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
);

// ��������� ����������� API
UFUNCTION(BlueprintPure, Category = "AI")
virtual bool IsAPIAvailable() const;

// �������� ���
UFUNCTION(BlueprintCallable, Category = "AI")
void ClearCache();
```

#### ���������������� ������ (Blueprint Native Events):

```cpp
// ��������� ������
UFUNCTION(BlueprintNativeEvent, Category = "AI")
FString BuildPrompt(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
) const;

// ��������� HTTP ������
UFUNCTION(BlueprintNativeEvent, Category = "AI")
void SendAPIRequest(
    const FString& Prompt,
    const FAIDialogueResponseDelegate& OnComplete
);

// ������� JSON �����
UFUNCTION(BlueprintNativeEvent, Category = "AI")
FAIDialogueResponse ParseAPIResponse(const FString& ResponseJson) const;
```

---

### 2. FAIDialogueResponse

**����������**: ���������, �������������� ����� �� AI.

```cpp
USTRUCT(BlueprintType)
struct FAIDialogueResponse
{
    GENERATED_BODY()

    // ������� �� ������
    UPROPERTY(BlueprintReadWrite)
    bool bSuccess = false;

    // ��������������� �����
    UPROPERTY(BlueprintReadWrite)
    FText GeneratedText;

    // ������ (���� ����)
    UPROPERTY(BlueprintReadWrite)
    FString ErrorMessage;

    // ������������ ������ (���)
    UPROPERTY(BlueprintReadWrite)
    FGameplayTag SuggestedEmotion;

    // ������������ ��������� ���������
    UPROPERTY(BlueprintReadWrite)
    float SuggestedAffinityDelta = 0.0f;
};
```

---

### 3. UAIDialogueProvider_OpenAI

**����������**: ���������� ���������� ��� OpenAI API (GPT-3.5, GPT-4).

#### ������������� � Blueprint:

**��� 1. ��������� API �����**:

```
// � Project Settings ? Plugins ? Dialogue System ? AI:
- API Key: "sk-..."
- Model: "gpt-4"
- Temperature: 0.7
- Max Tokens: 150
```

**��� 2. �������� ����������**:

```blueprintue
Event BeginPlay
    ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
    ? Set API Key (AIProvider, "sk-...")
    ? Set Model (AIProvider, "gpt-4")
    ? Set Temperature (AIProvider, 0.7)
    ? Set Max Tokens (AIProvider, 150)
```

**��� 3. ��������� �������**:

```blueprintue
Event OnPlayerAskQuestion
    UserPrompt (��������: FString) // "�������� � ����"
    ? Get NPC Persona Data ? PersonaData
    ? Get Dialogue Context ? Context
    ? AIProvider ? Generate Dialogue
        Persona Data: PersonaData
        Context: Context
        User Prompt: UserPrompt
        On Complete: [Bind to Event: OnAIResponseReceived]

// Custom Event: OnAIResponseReceived
Event OnAIResponseReceived
    Response (��������: FAIDialogueResponse)
    ? Branch (Response.bSuccess)
        True:
            ? Get Generated Text (Response) ? Text
            ? Display Dialogue Text (Text)
            ? Get Suggested Emotion (Response) ? Emotion
            ? Play Emotion Animation (Emotion)
        False:
            ? Get Error Message (Response) ? Error
            ? Print String (Error)
```

---

### 4. FDialoguePersonaData

**����������**: ���������, ����������� ������� NPC ��� AI.

```cpp
USTRUCT(BlueprintType)
struct FDialoguePersonaData
{
    GENERATED_BODY()

    // ��� ���������
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CharacterName;

    // �������� ���������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Description;

    // ����� ���������
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Personality;

    // �����������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Backstory;

    // ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SpeechStyle;

    // ������� �������� (few-shot)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> ExampleDialogues;
};
```

#### ������������� � Blueprint:

**�������� Persona Data**:

```blueprintue
// � Data Asset ��� Blueprint:

PersonaData:
  - Character Name: "������"
  - Description: "������� ������� 25 ���, ���������, ������������ � �����������."
  - Personality:
      [0]: "������"
      [1]: "����������"
      [2]: "������� �����������"
      [3]: "�������������"
  - Backstory: "������� � ��������� �������, ��������� � ����� ������� ���������..."
  - Speech Style: "������� �����, ���������� �������������� ��������, ����� ���������."
  - Example Dialogues:
      [0]: "User: ��� ����? ? Lianne: �, ���������! ������� ����� �������� �����, ����������� �� ����� �������!"
      [1]: "User: ��� ���� ��������? ? Lianne: ��� �������� �������� �������. �������� �����... ��� ����� ������."
```

---

### 5. UPromptBuilder

**����������**: ��������� ����� ��� ���������� ��������.

#### �������� ������:

```cpp
// ��������� system prompt
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildSystemPrompt(const FDialoguePersonaData& PersonaData);

// ��������� context prompt (���������, ���������, �����)
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildContextPrompt(const UDialogueSessionContext* Context);

// ��������� few-shot �������
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildFewShotExamples(const FDialoguePersonaData& PersonaData);

// ������� ������ ������
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildFullPrompt(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
);
```

#### ������������� � Blueprint:

```blueprintue
// ���������� ���������� �������:

Event OnBuildCustomPrompt
    ? Build System Prompt (PersonaData) ? SystemPrompt
    ? Build Context Prompt (Context) ? ContextPrompt
    ? Build Few Shot Examples (PersonaData) ? FewShotExamples
    ? Format String:
        "{SystemPrompt}\n\n{ContextPrompt}\n\n{FewShotExamples}\n\nUser: {UserPrompt}\n\nLianne:"
        ? FullPrompt
    ? Return FullPrompt
```

---

## ��������� ������� �������������

### ������ 1: ������� ���������� OpenAI (Blueprint)

**��� 1. ��������� API �����**:

```
// ������ 1: � Project Settings
Edit ? Project Settings ? Plugins ? Dialogue System ? AI:
  - API Key: "sk-proj-..."
  - Model: "gpt-3.5-turbo"

// ������ 2: � .env ����� (����������)
Config/DefaultEngine.ini:
[DialogueSystemAI]
OpenAIApiKey=sk-proj-...
```

**��� 2. �������� Persona Data Asset**:

1. �������� Data Asset: `DA_PersonaData_Lianne`
2. ���������:

```cpp
Character Name: "������"
Description: "������� ���������, 25 ���. ������ � ������������."
Personality:
  - "������"
  - "����������"
  - "�����������"
  - "������� �����������"
Backstory: "������ ������� � ��������� ������� � ����. � ������� ������� ����� ����������..."
Speech Style: "������� �����, ���������� �������������� ��������. ����� ���������."
Example Dialogues:
  [0]: "User: ������! ? Lianne: �, ������! *���������* ���� ���� ������!"
  [1]: "User: ��� ����? ? Lianne: ���������! ������� �������� �����. �� ��� �����... �������������."
```

**��� 3. ���������� AI Provider � NPC**:

```blueprintue
// � BP_NPC_Lianne:

// ����������:
- AIProvider (UAIDialogueProvider_OpenAI*)
- PersonaData (FDialoguePersonaData) [Load from DA_PersonaData_Lianne]

Event BeginPlay
    ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
    ? Load Persona Data from Asset ? PersonaData
```

**��� 4. ��������� �������� ������**:

```blueprintue
// � BP_NPC_Lianne:

Event OnPlayerAskQuestion
    UserPrompt (��������: FString)
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Get Context ? Context
    ? AIProvider ? Generate Dialogue
        Persona Data: PersonaData
        Context: Context
        User Prompt: UserPrompt
        On Complete: [Bind to Custom Event: OnAIResponse]

// Custom Event: OnAIResponse
Event OnAIResponse
    Response (��������: FAIDialogueResponse)
    ? Branch (Response.bSuccess)
        True:
            ? Get Generated Text (Response) ? Text
            ? Create Dialogue Node (Text) ? NewNode
            ? Add Node to Dialogue (NewNode)
            ? Display Node (NewNode)
        False:
            ? Print String ("AI Error: " + Response.ErrorMessage)
```

---

### ������ 2: ���������� � ������������� ��������� (C++)

```cpp
// � ����� DialogueComponent

#include "AIDialogueProvider.h"

void UMyDialogueComponent::GenerateAIResponse(const FString& UserPrompt)
{
    // �������� AI Provider
    UAIDialogueProvider_OpenAI* AIProvider = NewObject<UAIDialogueProvider_OpenAI>();
    AIProvider->APIKey = GetAPIKeyFromConfig();
    AIProvider->Model = TEXT("gpt-4");
    AIProvider->Temperature = 0.7f;
    AIProvider->MaxTokens = 150;

    // �������� Persona Data
    FDialoguePersonaData PersonaData = LoadPersonaData();

    // �������� Context
    UDialogueSessionContext* Context = GetDialogueContext();

    // ������� ������� ��� ��������� ������
    FAIDialogueResponseDelegate OnComplete;
    OnComplete.BindLambda([this](FAIDialogueResponse Response)
    {
        if (Response.bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("AI Response: %s"), *Response.GeneratedText.ToString());

            // ������� ����� ���� ������� � ��������������� �������
            UDialogueNode* NewNode = NewObject<UDialogueNode>();
            NewNode->DialogueText = Response.GeneratedText;
            NewNode->SpeakerName = FName(TEXT("Lianne"));

            // ���� AI ���������� ������, ��������� �
            if (Response.SuggestedEmotion.IsValid())
            {
                NewNode->Tags.AddTag(Response.SuggestedEmotion);
            }

            // �������� ���� � ������
            AddDynamicNode(NewNode);

            // ���� AI ���������� ��������� ���������, ��������� ���
            if (Response.SuggestedAffinityDelta != 0.0f)
            {
                URelationshipComponent* RelComp = GetRelationshipComponent();
                if (RelComp)
                {
                    RelComp->ModifyAffinity(Response.SuggestedAffinityDelta);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AI Error: %s"), *Response.ErrorMessage);
            // Fallback �� ����������������� ������
            UseFallbackDialogue();
        }
    });

    // ������������� �����
    AIProvider->GenerateDialogue(PersonaData, Context, UserPrompt, OnComplete);
}
```

---

### ������ 3: �������� ���������� AI ���������� (Blueprint)

**��� 1. �������� Blueprint ������**:

1. �������� Blueprint �����, ����������� �� `AIDialogueProvider`
2. ��������: `BP_CustomAIProvider`

**��� 2. ��������������� �������**:

```blueprintue
// Event: Build Prompt
Event Build Prompt (Override)
    Persona Data (��������)
    Context (��������)
    User Prompt (��������)
    ? Format String:
        "You are {PersonaData.CharacterName}. {PersonaData.Description}\n\n" +
        "Current time: {Context.CurrentTime}\n" +
        "Affinity: {Context.Affinity}\n\n" +
        "User: {UserPrompt}\n" +
        "{PersonaData.CharacterName}:"
    ? Return Value

// Event: Send API Request (Override)
Event Send API Request (Override)
    Prompt (��������)
    On Complete (��������: �������)
    ? Create HTTP Request
        URL: "https://your-custom-api.com/generate"
        Method: POST
        Headers:
            - "Authorization": "Bearer {APIKey}"
            - "Content-Type": "application/json"
        Body: {"prompt": "{Prompt}", "max_tokens": {MaxTokens}}
    ? Bind Response ? On HTTP Response Received
    ? Send Request

// Custom Event: On HTTP Response Received
Event On HTTP Response Received
    Response (��������: HTTP Response)
    ? Parse JSON (Response.Content) ? ParsedJSON
    ? Create AI Dialogue Response:
        - bSuccess: true
        - Generated Text: ParsedJSON.text
        - Suggested Emotion: ParsedJSON.emotion
    ? Execute Delegate (On Complete, AI Response)

// Event: Parse API Response (Override)
Event Parse API Response (Override)
    Response Json (��������)
    ? Parse JSON ? JSON Object
    ? Get Field (JSON Object, "text") ? Text
    ? Get Field (JSON Object, "emotion") ? EmotionString
    ? Convert to Gameplay Tag (EmotionString) ? EmotionTag
    ? Create AI Dialogue Response:
        - bSuccess: true
        - Generated Text: Text
        - Suggested Emotion: EmotionTag
    ? Return Value
```

---

### ������ 4: ������������� ��������� ������ (LM Studio)

**��� 1. ��������� LM Studio**:

1. ���������� LM Studio
2. ��������� ������ (��������, Mistral 7B)
3. ��������� ��������� ������ (�� ���������: `http://localhost:1234`)

**��� 2. �������� ���������� ��� LM Studio**:

```cpp
// � C++:

UCLASS()
class UAIDialogueProvider_LMStudio : public UAIDialogueProvider
{
    GENERATED_BODY()

protected:
    virtual void SendAPIRequest_Implementation(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete) override
    {
        // URL ���������� ������� LM Studio
        FString URL = TEXT("http://localhost:1234/v1/chat/completions");

        // ������� HTTP ������
        TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
        Request->SetURL(URL);
        Request->SetVerb(TEXT("POST"));
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

        // ������� JSON ����
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        JsonObject->SetStringField(TEXT("model"), Model);
        
        TArray<TSharedPtr<FJsonValue>> MessagesArray;
        TSharedPtr<FJsonObject> MessageObject = MakeShared<FJsonObject>();
        MessageObject->SetStringField(TEXT("role"), TEXT("user"));
        MessageObject->SetStringField(TEXT("content"), Prompt);
        MessagesArray.Add(MakeShared<FJsonValueObject>(MessageObject));
        
        JsonObject->SetArrayField(TEXT("messages"), MessagesArray);
        JsonObject->SetNumberField(TEXT("temperature"), Temperature);
        JsonObject->SetNumberField(TEXT("max_tokens"), MaxTokens);

        FString JsonString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        Request->SetContentAsString(JsonString);

        // ��������� ������
        Request->OnProcessRequestComplete().BindLambda(
            [this, OnComplete](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
            {
                FAIDialogueResponse AIResponse;
                if (bSuccess && Response.IsValid())
                {
                    AIResponse = ParseAPIResponse_Implementation(Response->GetContentAsString());
                }
                else
                {
                    AIResponse.bSuccess = false;
                    AIResponse.ErrorMessage = TEXT("HTTP Request failed");
                }
                OnComplete.ExecuteIfBound(AIResponse);
            });

        // ��������� ������
        Request->ProcessRequest();
    }

    virtual FAIDialogueResponse ParseAPIResponse_Implementation(const FString& ResponseJson) const override
    {
        FAIDialogueResponse Response;
        
        // ������� JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);
        
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // LM Studio ���������� ������ OpenAI
            const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
            if (JsonObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
            {
                TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
                TSharedPtr<FJsonObject> Message = Choice->GetObjectField(TEXT("message"));
                FString Content = Message->GetStringField(TEXT("content"));
                
                Response.bSuccess = true;
                Response.GeneratedText = FText::FromString(Content);
            }
        }
        else
        {
            Response.bSuccess = false;
            Response.ErrorMessage = TEXT("Failed to parse response");
        }

        return Response;
    }
};
```

**��� 3. �������������**:

```blueprintue
Event BeginPlay
    ? Create Object [UAIDialogueProvider_LMStudio] ? AIProvider
    ? Set Model (AIProvider, "local-model")
    ? Set Temperature (AIProvider, 0.7)
    ? Set Max Tokens (AIProvider, 150)
```

---

### ������ 5: ����������� ������ ����������

**��� 1. �������� ���������� �������**:

```cpp
// � PersonaData:

FString SystemPrompt = FString::Printf(TEXT(
    "You are %s, a character in a video game.\n\n"
    "Character Description:\n%s\n\n"
    "Personality Traits:\n%s\n\n"
    "Backstory:\n%s\n\n"
    "Speech Style:\n%s\n\n"
    "Instructions:\n"
    "- Stay in character at all times\n"
    "- Respond in a way that matches the personality\n"
    "- Keep responses concise (1-2 sentences)\n"
    "- Use the speech style described above\n"
    "- Consider the current context (time, location, relationship)\n"
    "- Suggest an emotion tag from: [Happy, Sad, Angry, Surprised, Neutral]\n"
    "- Suggest affinity change: [-10 to +10]\n\n"
    "Format your response as JSON:\n"
    "{\n"
    "  \"text\": \"your dialogue here\",\n"
    "  \"emotion\": \"Happy\",\n"
    "  \"affinity_delta\": 5\n"
    "}"
),
*PersonaData.CharacterName,
*PersonaData.Description,
*FString::Join(PersonaData.Personality, TEXT(", ")),
*PersonaData.Backstory,
*PersonaData.SpeechStyle
);
```

**��� 2. ���������� ���������**:

```cpp
FString ContextPrompt = FString::Printf(TEXT(
    "Current Context:\n"
    "- Time: %s\n"
    "- Location: %s\n"
    "- Affinity Level: %.0f/100\n"
    "- Recent Events: %s\n"
    "- Player has: %s\n"
),
*GetCurrentTimeString(Context),
*GetCurrentLocationString(Context),
GetAffinityLevel(Context),
*GetRecentEventsString(Context),
*GetPlayerInventoryString(Context)
);
```

**��� 3. Few-shot �������**:

```cpp
FString FewShotExamples = TEXT(
    "Example Interactions:\n\n"
    "User: Hi!\n"
    "Lianne: {\"text\": \"Oh, hello! *smiles warmly* It's so nice to see you!\", \"emotion\": \"Happy\", \"affinity_delta\": 2}\n\n"
    "User: I brought you a gift.\n"
    "Lianne: {\"text\": \"For me? *eyes light up* You're so thoughtful! I love it!\", \"emotion\": \"Surprised\", \"affinity_delta\": 10}\n\n"
    "User: I need to go.\n"
    "Lianne: {\"text\": \"Oh... *looks down* Okay, see you later then.\", \"emotion\": \"Sad\", \"affinity_delta\": -2}\n\n"
);
```

**��� 4. ��������� ������**:

```cpp
FString FullPrompt = FString::Printf(TEXT(
    "%s\n\n%s\n\n%s\n\nUser: %s\n\nLianne:"
),
*SystemPrompt,
*ContextPrompt,
*FewShotExamples,
*UserPrompt
);
```

---

## ������ ��������

1. **����������� �����������**: AI ������� �������, ��������� ������ ������.

2. **������������� ������**: �������������� `MaxTokens` ��� �������� ����� �������.

3. **������������� ������**: ������ ������ fallback �� ����������������� �������.

4. **Rate limiting**: �� ����������� ������� ����� �������� ������������.

5. **������������ API ������**: ������� ����� � `.env` ������, �� � ����.

6. **���������� �������**: ����������������� � ��������� ��� ������ �����������.

7. **����������� JSON ������**: ����������������� ������ ����� �������.

8. **��������� ������ ��� ������������**: ����������� LM Studio �� ����� ����������.

---

## ����������

**DialogueSystemAI** ��������� ��������� ������������, ����� �������, ������� ��������� �� �������� ������ � �������� ����. ���������� � LLM ��������� ����� ����������� ��� immersive storytelling � �������������� �������� �����.

��� ��������� �������������� ���������� ��.:
- `01_DialogueSystemCore_Module.md` � ���� �������
- `02_DialogueSystemRuntime_Module.md` � Runtime ����������
- `03_DialogueSystemUI_Module.md` � UI �������
- `05_Complete_Examples.md` � ������ ������� ����������
