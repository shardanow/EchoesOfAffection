# DialogueSystemAI Module Documentation

## Обзор модуля

**DialogueSystemAI** — это модуль интеграции с AI/LLM (Large Language Models) для динамической генерации диалогов. Поддерживает OpenAI GPT, Anthropic Claude, локальные модели (LM Studio, Ollama) и кастомные API.

## Архитектура

```
DialogueSystemAI/
??? Public/
?   ??? AIDialogueProvider.h  # Провайдеры AI генерации
??? Private/
    ??? ...                   # Реализация
```

---

## Ключевые классы

### 1. UAIDialogueProvider

**Назначение**: Абстрактный базовый класс для AI провайдеров. Позволяет генерировать диалоги на основе персоны NPC, контекста и промпта игрока.

#### Основные свойства:

```cpp
// API ключ (хранится в Project Settings или .env файле)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
FString APIKey;

// Модель (например: gpt-4, gpt-3.5-turbo)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
FString Model = TEXT("gpt-4");

// Температура (0.0 - 2.0) - контролирует креативность
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
float Temperature = 0.7f;

// Максимальное количество токенов в ответе
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
int32 MaxTokens = 150;
```

#### Основные методы:

```cpp
// Сгенерировать диалог (асинхронно)
UFUNCTION(BlueprintCallable, Category = "AI")
void GenerateDialogue(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt,
    FAIDialogueResponseDelegate OnComplete
);

// Сгенерировать диалог (синхронно, для тестов)
UFUNCTION(BlueprintCallable, Category = "AI")
FAIDialogueResponse GenerateDialogueSync(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
);

// Проверить доступность API
UFUNCTION(BlueprintPure, Category = "AI")
virtual bool IsAPIAvailable() const;

// Очистить кэш
UFUNCTION(BlueprintCallable, Category = "AI")
void ClearCache();
```

#### Переопределяемые методы (Blueprint Native Events):

```cpp
// Построить промпт
UFUNCTION(BlueprintNativeEvent, Category = "AI")
FString BuildPrompt(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
) const;

// Отправить HTTP запрос
UFUNCTION(BlueprintNativeEvent, Category = "AI")
void SendAPIRequest(
    const FString& Prompt,
    const FAIDialogueResponseDelegate& OnComplete
);

// Парсить JSON ответ
UFUNCTION(BlueprintNativeEvent, Category = "AI")
FAIDialogueResponse ParseAPIResponse(const FString& ResponseJson) const;
```

---

### 2. FAIDialogueResponse

**Назначение**: Структура, представляющая ответ от AI.

```cpp
USTRUCT(BlueprintType)
struct FAIDialogueResponse
{
    GENERATED_BODY()

    // Успешен ли запрос
    UPROPERTY(BlueprintReadWrite)
    bool bSuccess = false;

    // Сгенерированный текст
    UPROPERTY(BlueprintReadWrite)
    FText GeneratedText;

    // Ошибка (если есть)
    UPROPERTY(BlueprintReadWrite)
    FString ErrorMessage;

    // Предложенная эмоция (тег)
    UPROPERTY(BlueprintReadWrite)
    FGameplayTag SuggestedEmotion;

    // Предложенное изменение отношений
    UPROPERTY(BlueprintReadWrite)
    float SuggestedAffinityDelta = 0.0f;
};
```

---

### 3. UAIDialogueProvider_OpenAI

**Назначение**: Конкретная реализация для OpenAI API (GPT-3.5, GPT-4).

#### Использование в Blueprint:

**Шаг 1. Настройка API ключа**:

```
// В Project Settings ? Plugins ? Dialogue System ? AI:
- API Key: "sk-..."
- Model: "gpt-4"
- Temperature: 0.7
- Max Tokens: 150
```

**Шаг 2. Создание провайдера**:

```blueprintue
Event BeginPlay
    ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
    ? Set API Key (AIProvider, "sk-...")
    ? Set Model (AIProvider, "gpt-4")
    ? Set Temperature (AIProvider, 0.7)
    ? Set Max Tokens (AIProvider, 150)
```

**Шаг 3. Генерация диалога**:

```blueprintue
Event OnPlayerAskQuestion
    UserPrompt (параметр: FString) // "Расскажи о себе"
    ? Get NPC Persona Data ? PersonaData
    ? Get Dialogue Context ? Context
    ? AIProvider ? Generate Dialogue
        Persona Data: PersonaData
        Context: Context
        User Prompt: UserPrompt
        On Complete: [Bind to Event: OnAIResponseReceived]

// Custom Event: OnAIResponseReceived
Event OnAIResponseReceived
    Response (параметр: FAIDialogueResponse)
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

**Назначение**: Структура, описывающая персону NPC для AI.

```cpp
USTRUCT(BlueprintType)
struct FDialoguePersonaData
{
    GENERATED_BODY()

    // Имя персонажа
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CharacterName;

    // Описание персонажа
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Description;

    // Черты характера
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Personality;

    // Предыстория
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Backstory;

    // Манера речи
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SpeechStyle;

    // Примеры диалогов (few-shot)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> ExampleDialogues;
};
```

#### Использование в Blueprint:

**Создание Persona Data**:

```blueprintue
// В Data Asset или Blueprint:

PersonaData:
  - Character Name: "Лианна"
  - Description: "Молодая женщина 25 лет, художница, мечтательная и романтичная."
  - Personality:
      [0]: "Добрая"
      [1]: "Творческая"
      [2]: "Немного застенчивая"
      [3]: "Оптимистичная"
  - Backstory: "Выросла в маленькой деревне, переехала в город изучать искусство..."
  - Speech Style: "Говорит мягко, использует художественные метафоры, часто улыбается."
  - Example Dialogues:
      [0]: "User: Как дела? ? Lianne: О, прекрасно! Сегодня такой красивый закат, вдохновляет на новую картину!"
      [1]: "User: Что тебе нравится? ? Lianne: Мне нравится рисовать природу. Особенно цветы... они такие нежные."
```

---

### 5. UPromptBuilder

**Назначение**: Утилитный класс для построения промптов.

#### Основные методы:

```cpp
// Построить system prompt
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildSystemPrompt(const FDialoguePersonaData& PersonaData);

// Построить context prompt (инвентарь, отношения, время)
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildContextPrompt(const UDialogueSessionContext* Context);

// Построить few-shot примеры
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildFewShotExamples(const FDialoguePersonaData& PersonaData);

// Собрать полный промпт
UFUNCTION(BlueprintPure, Category = "AI|Prompt")
static FString BuildFullPrompt(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt
);
```

#### Использование в Blueprint:

```blueprintue
// Построение кастомного промпта:

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

## Пошаговые примеры использования

### Пример 1: Базовая интеграция OpenAI (Blueprint)

**Шаг 1. Настройка API ключа**:

```
// Способ 1: В Project Settings
Edit ? Project Settings ? Plugins ? Dialogue System ? AI:
  - API Key: "sk-proj-..."
  - Model: "gpt-3.5-turbo"

// Способ 2: В .env файле (безопаснее)
Config/DefaultEngine.ini:
[DialogueSystemAI]
OpenAIApiKey=sk-proj-...
```

**Шаг 2. Создание Persona Data Asset**:

1. Создайте Data Asset: `DA_PersonaData_Lianne`
2. Заполните:

```cpp
Character Name: "Лианна"
Description: "Молодая художница, 25 лет. Добрая и мечтательная."
Personality:
  - "Добрая"
  - "Творческая"
  - "Романтичная"
  - "Немного застенчивая"
Backstory: "Лианна выросла в маленькой деревне у моря. С детства мечтала стать художницей..."
Speech Style: "Говорит мягко, использует художественные метафоры. Часто улыбается."
Example Dialogues:
  [0]: "User: Привет! ? Lianne: О, привет! *улыбается* Рада тебя видеть!"
  [1]: "User: Как дела? ? Lianne: Прекрасно! Сегодня рисовала закат. Он был таким... вдохновляющим."
```

**Шаг 3. Добавление AI Provider в NPC**:

```blueprintue
// В BP_NPC_Lianne:

// Переменные:
- AIProvider (UAIDialogueProvider_OpenAI*)
- PersonaData (FDialoguePersonaData) [Load from DA_PersonaData_Lianne]

Event BeginPlay
    ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
    ? Load Persona Data from Asset ? PersonaData
```

**Шаг 4. Обработка вопросов игрока**:

```blueprintue
// В BP_NPC_Lianne:

Event OnPlayerAskQuestion
    UserPrompt (параметр: FString)
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Get Context ? Context
    ? AIProvider ? Generate Dialogue
        Persona Data: PersonaData
        Context: Context
        User Prompt: UserPrompt
        On Complete: [Bind to Custom Event: OnAIResponse]

// Custom Event: OnAIResponse
Event OnAIResponse
    Response (параметр: FAIDialogueResponse)
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

### Пример 2: Интеграция с динамическими диалогами (C++)

```cpp
// В вашем DialogueComponent

#include "AIDialogueProvider.h"

void UMyDialogueComponent::GenerateAIResponse(const FString& UserPrompt)
{
    // Получить AI Provider
    UAIDialogueProvider_OpenAI* AIProvider = NewObject<UAIDialogueProvider_OpenAI>();
    AIProvider->APIKey = GetAPIKeyFromConfig();
    AIProvider->Model = TEXT("gpt-4");
    AIProvider->Temperature = 0.7f;
    AIProvider->MaxTokens = 150;

    // Получить Persona Data
    FDialoguePersonaData PersonaData = LoadPersonaData();

    // Получить Context
    UDialogueSessionContext* Context = GetDialogueContext();

    // Создать делегат для обработки ответа
    FAIDialogueResponseDelegate OnComplete;
    OnComplete.BindLambda([this](FAIDialogueResponse Response)
    {
        if (Response.bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("AI Response: %s"), *Response.GeneratedText.ToString());

            // Создать новый узел диалога с сгенерированным текстом
            UDialogueNode* NewNode = NewObject<UDialogueNode>();
            NewNode->DialogueText = Response.GeneratedText;
            NewNode->SpeakerName = FName(TEXT("Lianne"));

            // Если AI предложила эмоцию, применить её
            if (Response.SuggestedEmotion.IsValid())
            {
                NewNode->Tags.AddTag(Response.SuggestedEmotion);
            }

            // Добавить узел в диалог
            AddDynamicNode(NewNode);

            // Если AI предложила изменение отношений, применить его
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
            // Fallback на предустановленный диалог
            UseFallbackDialogue();
        }
    });

    // Сгенерировать ответ
    AIProvider->GenerateDialogue(PersonaData, Context, UserPrompt, OnComplete);
}
```

---

### Пример 3: Создание кастомного AI провайдера (Blueprint)

**Шаг 1. Создание Blueprint класса**:

1. Создайте Blueprint класс, наследующий от `AIDialogueProvider`
2. Назовите: `BP_CustomAIProvider`

**Шаг 2. Переопределение методов**:

```blueprintue
// Event: Build Prompt
Event Build Prompt (Override)
    Persona Data (параметр)
    Context (параметр)
    User Prompt (параметр)
    ? Format String:
        "You are {PersonaData.CharacterName}. {PersonaData.Description}\n\n" +
        "Current time: {Context.CurrentTime}\n" +
        "Affinity: {Context.Affinity}\n\n" +
        "User: {UserPrompt}\n" +
        "{PersonaData.CharacterName}:"
    ? Return Value

// Event: Send API Request (Override)
Event Send API Request (Override)
    Prompt (параметр)
    On Complete (параметр: делегат)
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
    Response (параметр: HTTP Response)
    ? Parse JSON (Response.Content) ? ParsedJSON
    ? Create AI Dialogue Response:
        - bSuccess: true
        - Generated Text: ParsedJSON.text
        - Suggested Emotion: ParsedJSON.emotion
    ? Execute Delegate (On Complete, AI Response)

// Event: Parse API Response (Override)
Event Parse API Response (Override)
    Response Json (параметр)
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

### Пример 4: Использование локальной модели (LM Studio)

**Шаг 1. Настройка LM Studio**:

1. Установите LM Studio
2. Загрузите модель (например, Mistral 7B)
3. Запустите локальный сервер (по умолчанию: `http://localhost:1234`)

**Шаг 2. Создание провайдера для LM Studio**:

```cpp
// В C++:

UCLASS()
class UAIDialogueProvider_LMStudio : public UAIDialogueProvider
{
    GENERATED_BODY()

protected:
    virtual void SendAPIRequest_Implementation(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete) override
    {
        // URL локального сервера LM Studio
        FString URL = TEXT("http://localhost:1234/v1/chat/completions");

        // Создать HTTP запрос
        TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
        Request->SetURL(URL);
        Request->SetVerb(TEXT("POST"));
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

        // Создать JSON тело
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

        // Обработка ответа
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

        // Отправить запрос
        Request->ProcessRequest();
    }

    virtual FAIDialogueResponse ParseAPIResponse_Implementation(const FString& ResponseJson) const override
    {
        FAIDialogueResponse Response;
        
        // Парсить JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);
        
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // LM Studio использует формат OpenAI
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

**Шаг 3. Использование**:

```blueprintue
Event BeginPlay
    ? Create Object [UAIDialogueProvider_LMStudio] ? AIProvider
    ? Set Model (AIProvider, "local-model")
    ? Set Temperature (AIProvider, 0.7)
    ? Set Max Tokens (AIProvider, 150)
```

---

### Пример 5: Продвинутый промпт инжиниринг

**Шаг 1. Создание системного промпта**:

```cpp
// В PersonaData:

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

**Шаг 2. Добавление контекста**:

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

**Шаг 3. Few-shot примеры**:

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

**Шаг 4. Финальный промпт**:

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

## Лучшие практики

1. **Используйте кэширование**: AI запросы дорогие, кэшируйте частые ответы.

2. **Ограничивайте токены**: Устанавливайте `MaxTokens` для контроля длины ответов.

3. **Обрабатывайте ошибки**: Всегда имейте fallback на предустановленные диалоги.

4. **Rate limiting**: Не отправляйте слишком много запросов одновременно.

5. **Безопасность API ключей**: Храните ключи в `.env` файлах, не в коде.

6. **Тестируйте промпты**: Экспериментируйте с промптами для лучших результатов.

7. **Используйте JSON формат**: Структурированные ответы легче парсить.

8. **Локальные модели для тестирования**: Используйте LM Studio во время разработки.

---

## Заключение

**DialogueSystemAI** позволяет создавать динамические, живые диалоги, которые реагируют на действия игрока и контекст игры. Интеграция с LLM открывает новые возможности для immersive storytelling и персонализации игрового опыта.

Для получения дополнительной информации см.:
- `01_DialogueSystemCore_Module.md` — Ядро системы
- `02_DialogueSystemRuntime_Module.md` — Runtime компоненты
- `03_DialogueSystemUI_Module.md` — UI виджеты
- `05_Complete_Examples.md` — Полные примеры интеграции
