// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIDialogueProvider.h"
#include "Core/DialogueContext.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Forward declaration to avoid including full header from different module
class URelationshipComponent;

void UAIDialogueProvider::GenerateDialogue(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt,
    FAIDialogueResponseDelegate OnComplete)
{
    if (!IsAPIAvailable())
    {
        FAIDialogueResponse ErrorResponse;
        ErrorResponse.bSuccess = false;
        ErrorResponse.ErrorMessage = TEXT("API is not available");
        OnComplete.ExecuteIfBound(ErrorResponse);
        return;
    }

    // Build full prompt
    FString FullPrompt = BuildPrompt(PersonaData, Context, UserPrompt);

    // Check cache first
    if (ResponseCache.Contains(FullPrompt))
    {
        OnComplete.ExecuteIfBound(ResponseCache[FullPrompt]);
        return;
    }

    // Send API request
    SendAPIRequest(FullPrompt, OnComplete);
}

FAIDialogueResponse UAIDialogueProvider::GenerateDialogueSync(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt)
{
    FAIDialogueResponse Response;
    Response.bSuccess = false;
    Response.ErrorMessage = TEXT("Synchronous AI generation is not recommended and not implemented by default");
    return Response;
}

bool UAIDialogueProvider::IsAPIAvailable() const
{
    return !APIKey.IsEmpty() && !Model.IsEmpty();
}

void UAIDialogueProvider::ClearCache()
{
    ResponseCache.Empty();
}

FString UAIDialogueProvider::BuildPrompt_Implementation(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt) const
{
    return UPromptBuilder::BuildFullPrompt(PersonaData, Context, UserPrompt);
}

void UAIDialogueProvider::SendAPIRequest_Implementation(
    const FString& Prompt,
    const FAIDialogueResponseDelegate& OnComplete)
{
    FAIDialogueResponse ErrorResponse;
    ErrorResponse.bSuccess = false;
    ErrorResponse.ErrorMessage = TEXT("SendAPIRequest not implemented in base class");
    OnComplete.ExecuteIfBound(ErrorResponse);
}

FAIDialogueResponse UAIDialogueProvider::ParseAPIResponse_Implementation(const FString& ResponseJson) const
{
    FAIDialogueResponse Response;
    Response.bSuccess = false;
    Response.ErrorMessage = TEXT("ParseAPIResponse not implemented in base class");
    return Response;
}

// OpenAI Implementation

FString UAIDialogueProvider_OpenAI::BuildPrompt_Implementation(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt) const
{
    return Super::BuildPrompt_Implementation(PersonaData, Context, UserPrompt);
}

void UAIDialogueProvider_OpenAI::SendAPIRequest_Implementation(
    const FString& Prompt,
    const FAIDialogueResponseDelegate& OnComplete)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *APIKey));

    // Build JSON request body
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("model"), Model);
    JsonObject->SetNumberField(TEXT("temperature"), Temperature);
    JsonObject->SetNumberField(TEXT("max_tokens"), MaxTokens);

    // Add messages
    TArray<TSharedPtr<FJsonValue>> Messages;

    // System message
    TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
    SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
    SystemMessage->SetStringField(TEXT("content"), Prompt);
    Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));

    JsonObject->SetArrayField(TEXT("messages"), Messages);

    // Convert to string
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);

    // Bind response callback
    Request->OnProcessRequestComplete().BindLambda(
        [this, OnComplete](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
        {
            FAIDialogueResponse Response;

            if (!bSucceeded || !HttpResponse.IsValid())
            {
                Response.bSuccess = false;
                Response.ErrorMessage = TEXT("HTTP request failed");
                OnComplete.ExecuteIfBound(Response);
                return;
            }

            int32 ResponseCode = HttpResponse->GetResponseCode();
            if (ResponseCode < 200 || ResponseCode >= 300)
            {
                Response.bSuccess = false;
                Response.ErrorMessage = FString::Printf(TEXT("HTTP error: %d"), ResponseCode);
                OnComplete.ExecuteIfBound(Response);
                return;
            }

            // Parse response
            Response = ParseAPIResponse(HttpResponse->GetContentAsString());
            OnComplete.ExecuteIfBound(Response);
        });

    Request->ProcessRequest();
}

FAIDialogueResponse UAIDialogueProvider_OpenAI::ParseAPIResponse_Implementation(const FString& ResponseJson) const
{
    FAIDialogueResponse Response;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        Response.bSuccess = false;
        Response.ErrorMessage = TEXT("Failed to parse JSON response");
        return Response;
    }

    // Parse OpenAI response format
    const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
    if (!JsonObject->TryGetArrayField(TEXT("choices"), Choices) || Choices->Num() == 0)
    {
        Response.bSuccess = false;
        Response.ErrorMessage = TEXT("No choices in response");
        return Response;
    }

    const TSharedPtr<FJsonObject>* ChoiceObject = nullptr;
    if (!(*Choices)[0]->TryGetObject(ChoiceObject))
    {
        Response.bSuccess = false;
        Response.ErrorMessage = TEXT("Invalid choice format");
        return Response;
    }

    const TSharedPtr<FJsonObject>* MessageObject = nullptr;
    if (!(*ChoiceObject)->TryGetObjectField(TEXT("message"), MessageObject))
    {
        Response.bSuccess = false;
        Response.ErrorMessage = TEXT("No message in choice");
        return Response;
    }

    FString Content;
    if (!(*MessageObject)->TryGetStringField(TEXT("content"), Content))
    {
        Response.bSuccess = false;
        Response.ErrorMessage = TEXT("No content in message");
        return Response;
    }

    Response.bSuccess = true;
    Response.GeneratedText = FText::FromString(Content);

    return Response;
}

// Prompt Builder Implementation

FString UPromptBuilder::BuildSystemPrompt(const FDialoguePersonaData& PersonaData)
{
    FString SystemPrompt = PersonaData.AISystemPrompt;

    if (SystemPrompt.IsEmpty())
    {
        SystemPrompt = FString::Printf(
            TEXT("You are %s. %s\n\nRespond to the user in character."),
            *PersonaData.DisplayName.ToString(),
            *PersonaData.Description.ToString()
        );
    }

    return SystemPrompt;
}

FString UPromptBuilder::BuildContextPrompt(const UDialogueSessionContext* Context)
{
    if (!Context)
    {
        return TEXT("");
    }

    FString ContextPrompt;

    // Add relationship context using BaseAffinityLevel instead
    if (Context->GetParticipants())
    {
        ContextPrompt += FString::Printf(TEXT("Current affinity level: %.0f\n"), 
   Context->GetParticipants()->GetBaseAffinityLevel());
    }

    // Add player context
    if (Context->GetPlayer())
    {
   ContextPrompt += TEXT("Speaking with: ");
     ContextPrompt += Context->GetPlayer()->GetName();
  ContextPrompt += TEXT("\n");
    }

    // Add conversation history
    if (Context->GetState() && Context->GetState()->GetConversationHistory().Num() > 0)
    {
        ContextPrompt += TEXT("\nRecent conversation:\n");

        const TArray<FConversationEntry>& History = Context->GetState()->GetConversationHistory();
        int32 Count = FMath::Min(History.Num(), 5);
        for (int32 i = History.Num() - Count; i < History.Num(); ++i)
        {
            const FConversationEntry& Entry = History[i];
            ContextPrompt += TEXT("- ");
            ContextPrompt += Entry.SpeakerName.ToString();
            ContextPrompt += TEXT(": ");
            ContextPrompt += Entry.DialogueText.ToString();
            ContextPrompt += TEXT("\n");
        }
    }

    return ContextPrompt;
}

FString UPromptBuilder::BuildFewShotExamples(const FDialoguePersonaData& PersonaData)
{
    if (PersonaData.ExamplePhrases.Num() == 0)
    {
        return TEXT("");
    }

    FString Examples = TEXT("\nExample responses:\n");

    for (const FString& Example : PersonaData.ExamplePhrases)
    {
        Examples += FString::Printf(TEXT("- %s\n"), *Example);
    }

    return Examples;
}

FString UPromptBuilder::BuildFullPrompt(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt)
{
    FString FullPrompt;

    // System prompt
    FullPrompt += BuildSystemPrompt(PersonaData);
    FullPrompt += TEXT("\n\n");

    // Few-shot examples
    FString Examples = BuildFewShotExamples(PersonaData);
    if (!Examples.IsEmpty())
    {
        FullPrompt += Examples;
        FullPrompt += TEXT("\n");
    }

    // Context
    FString ContextPrompt = BuildContextPrompt(Context);
    if (!ContextPrompt.IsEmpty())
    {
        FullPrompt += ContextPrompt;
        FullPrompt += TEXT("\n");
    }

    // User prompt
    FullPrompt += TEXT("User: ");
    FullPrompt += UserPrompt;
    FullPrompt += TEXT("\n\nRespond in character:");

    return FullPrompt;
}
