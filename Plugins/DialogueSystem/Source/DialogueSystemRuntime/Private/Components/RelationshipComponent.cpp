// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/RelationshipComponent.h"
#include "JsonObjectConverter.h"

URelationshipComponent::URelationshipComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 1.0f; // Tick every second for decay
}

void URelationshipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bEnableDecay)
    {
        ProcessDecay(DeltaTime);
    }

    ProcessMood(DeltaTime);
}

void URelationshipComponent::ApplyStimulus(const FRelationshipStimulus& Stimulus)
{
    // Store in history
    EventHistory.Add(Stimulus);
    if (EventHistory.Num() > MaxEventHistory)
    {
        EventHistory.RemoveAt(0);
    }

    // Apply modifiers based on stimulus type and NPC personality
    float ModifiedIntensity = Stimulus.Intensity;

    if (Stimulus.Intensity > 0.0f)
    {
        ModifiedIntensity *= PositiveSensitivity;
    }
    else
    {
        ModifiedIntensity *= NegativeSensitivity;
    }

    // Apply the stimulus
    switch (Stimulus.Type)
    {
    case ERelationshipStimulusType::Gift:
    case ERelationshipStimulusType::Compliment:
    case ERelationshipStimulusType::Help:
        ModifyAffinity(ModifiedIntensity);
        ModifyTrust(ModifiedIntensity * 0.5f);
        break;

    case ERelationshipStimulusType::Insult:
    case ERelationshipStimulusType::Threat:
        ModifyAffinity(ModifiedIntensity);
        ModifyRespect(ModifiedIntensity * -0.3f);
        ModifyFear(FMath::Abs(ModifiedIntensity) * 0.3f);
        break;

    case ERelationshipStimulusType::Betrayal:
        ModifyAffinity(ModifiedIntensity * 2.0f);
        ModifyTrust(ModifiedIntensity * 3.0f);
        break;

    case ERelationshipStimulusType::Flirt:
        ModifyRomance(FMath::Abs(ModifiedIntensity));
        ModifyAffinity(ModifiedIntensity * 0.5f);
        break;

    case ERelationshipStimulusType::Positive:
        ModifyAffinity(ModifiedIntensity);
        break;

    case ERelationshipStimulusType::Negative:
        ModifyAffinity(ModifiedIntensity);
        break;
    }

    // Update mood based on recent events
    UpdateMoodFromAttributes();
}

void URelationshipComponent::ModifyAffinity(float Delta)
{
    float OldValue = Affinity;
    Affinity = FMath::Clamp(Affinity + Delta, -100.0f, 100.0f);

    if (!FMath::IsNearlyEqual(OldValue, Affinity))
    {
        OnRelationshipChanged.Broadcast(FName("Affinity"), OldValue, Affinity);
        CheckThresholds(OldValue, Affinity);
    }
}

void URelationshipComponent::ModifyTrust(float Delta)
{
    float OldValue = Trust;
    ClampAttribute(Trust, 0.0f, 100.0f);
    Trust = FMath::Clamp(Trust + Delta, 0.0f, 100.0f);

    if (!FMath::IsNearlyEqual(OldValue, Trust))
    {
        OnRelationshipChanged.Broadcast(FName("Trust"), OldValue, Trust);
    }
}

void URelationshipComponent::ModifyRespect(float Delta)
{
    float OldValue = Respect;
    Respect = FMath::Clamp(Respect + Delta, 0.0f, 100.0f);

    if (!FMath::IsNearlyEqual(OldValue, Respect))
    {
        OnRelationshipChanged.Broadcast(FName("Respect"), OldValue, Respect);
    }
}

void URelationshipComponent::ModifyRomance(float Delta)
{
    float OldValue = Romance;
    Romance = FMath::Clamp(Romance + Delta, 0.0f, 100.0f);

    if (!FMath::IsNearlyEqual(OldValue, Romance))
    {
        OnRelationshipChanged.Broadcast(FName("Romance"), OldValue, Romance);
    }
}

void URelationshipComponent::ModifyFear(float Delta)
{
    float OldValue = Fear;
    Fear = FMath::Clamp(Fear + Delta, 0.0f, 100.0f);

    if (!FMath::IsNearlyEqual(OldValue, Fear))
    {
        OnRelationshipChanged.Broadcast(FName("Fear"), OldValue, Fear);
    }
}

void URelationshipComponent::SetMood(ENPCMoodState NewMood)
{
    if (CurrentMood != NewMood)
    {
        ENPCMoodState OldMood = CurrentMood;
        CurrentMood = NewMood;
        MoodDuration = 0.0f;

        OnMoodChanged.Broadcast(OldMood, NewMood);
    }
}

FText URelationshipComponent::GetRelationshipRank() const
{
    if (Affinity >= 80.0f)
    {
        return FText::FromString(TEXT("Best Friend"));
    }
    else if (Affinity >= 50.0f)
    {
        return FText::FromString(TEXT("Friend"));
    }
    else if (Affinity >= 20.0f)
    {
        return FText::FromString(TEXT("Acquaintance"));
    }
    else if (Affinity >= -20.0f)
    {
        return FText::FromString(TEXT("Neutral"));
    }
    else if (Affinity >= -50.0f)
    {
        return FText::FromString(TEXT("Disliked"));
    }
    else
    {
        return FText::FromString(TEXT("Enemy"));
    }
}

void URelationshipComponent::ResetRelationship()
{
    Affinity = 0.0f;
    Trust = 50.0f;
    Respect = 50.0f;
    Romance = 0.0f;
    Fear = 0.0f;
    Jealousy = 0.0f;
    CurrentMood = ENPCMoodState::Neutral;
    MoodDuration = 0.0f;
    EventHistory.Empty();
}

void URelationshipComponent::SaveToJson(FString& OutJson) const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    JsonObject->SetNumberField(TEXT("Affinity"), Affinity);
    JsonObject->SetNumberField(TEXT("Trust"), Trust);
    JsonObject->SetNumberField(TEXT("Respect"), Respect);
    JsonObject->SetNumberField(TEXT("Romance"), Romance);
    JsonObject->SetNumberField(TEXT("Fear"), Fear);
    JsonObject->SetNumberField(TEXT("Jealousy"), Jealousy);
    JsonObject->SetNumberField(TEXT("CurrentMood"), static_cast<int32>(CurrentMood));

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}

bool URelationshipComponent::LoadFromJson(const FString& Json)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }

    Affinity = JsonObject->GetNumberField(TEXT("Affinity"));
    Trust = JsonObject->GetNumberField(TEXT("Trust"));
    Respect = JsonObject->GetNumberField(TEXT("Respect"));
    Romance = JsonObject->GetNumberField(TEXT("Romance"));
    Fear = JsonObject->GetNumberField(TEXT("Fear"));
    Jealousy = JsonObject->GetNumberField(TEXT("Jealousy"));
    CurrentMood = static_cast<ENPCMoodState>(JsonObject->GetIntegerField(TEXT("CurrentMood")));

    return true;
}

void URelationshipComponent::ProcessDecay(float DeltaTime)
{
    // Decay affinity toward target value
    if (!FMath::IsNearlyEqual(Affinity, AffinityDecayTarget, 0.1f))
    {
        float DecayAmount = AffinityDecayRate * DeltaTime;
        if (Affinity > AffinityDecayTarget)
        {
            Affinity = FMath::Max(Affinity - DecayAmount, AffinityDecayTarget);
        }
        else
        {
            Affinity = FMath::Min(Affinity + DecayAmount, AffinityDecayTarget);
        }
    }

    // Decay other attributes toward neutral values
    float OtherDecay = OtherDecayRate * DeltaTime;

    if (Trust > 50.0f)
    {
        Trust = FMath::Max(Trust - OtherDecay, 50.0f);
    }
    else if (Trust < 50.0f)
    {
        Trust = FMath::Min(Trust + OtherDecay, 50.0f);
    }

    if (Respect > 50.0f)
    {
        Respect = FMath::Max(Respect - OtherDecay, 50.0f);
    }
    else if (Respect < 50.0f)
    {
        Respect = FMath::Min(Respect + OtherDecay, 50.0f);
    }

    // Fear and Romance decay to zero
    if (Fear > 0.0f)
    {
        Fear = FMath::Max(Fear - OtherDecay, 0.0f);
    }

    if (Romance > 0.0f)
    {
        Romance = FMath::Max(Romance - OtherDecay * 0.5f, 0.0f); // Romance decays slower
    }

    if (Jealousy > 0.0f)
    {
        Jealousy = FMath::Max(Jealousy - OtherDecay, 0.0f);
    }
}

void URelationshipComponent::ProcessMood(float DeltaTime)
{
    MoodDuration += DeltaTime;

    // Only change mood if minimum duration has passed
    if (MoodDuration >= MinMoodDuration)
    {
        UpdateMoodFromAttributes();
    }
}

void URelationshipComponent::UpdateMoodFromAttributes()
{
    ENPCMoodState NewMood = ENPCMoodState::Neutral;

    // Determine mood based on relationship attributes
    if (Fear > 60.0f)
    {
        NewMood = ENPCMoodState::Fearful;
    }
    else if (Affinity > 60.0f && Romance > 50.0f)
    {
        NewMood = ENPCMoodState::Excited;
    }
    else if (Affinity > 60.0f)
    {
        NewMood = ENPCMoodState::Happy;
    }
    else if (Affinity < -40.0f)
    {
        NewMood = ENPCMoodState::Angry;
    }
    else if (Trust < 30.0f)
    {
        NewMood = ENPCMoodState::Anxious;
    }
    else if (Affinity < -20.0f)
    {
        NewMood = ENPCMoodState::Sad;
    }
    else if (Romance < 20.0f && Trust < 40.0f)
    {
        NewMood = ENPCMoodState::Bored;
    }

    if (NewMood != CurrentMood)
    {
        SetMood(NewMood);
    }
}

void URelationshipComponent::CheckThresholds(float OldAffinity, float NewAffinity)
{
    // Check for crossing major thresholds
    struct FThreshold
    {
        float Value;
        FName Name;
    };

    TArray<FThreshold> Thresholds = {
        { 80.0f, FName("BestFriend") },
        { 50.0f, FName("Friend") },
        { 20.0f, FName("Acquaintance") },
        { -20.0f, FName("Disliked") },
        { -50.0f, FName("Enemy") }
    };

    for (const FThreshold& Threshold : Thresholds)
    {
        if ((OldAffinity < Threshold.Value && NewAffinity >= Threshold.Value) ||
            (OldAffinity >= Threshold.Value && NewAffinity < Threshold.Value))
        {
            OnThresholdReached.Broadcast(Threshold.Name);
        }
    }
}

void URelationshipComponent::ClampAttribute(float& Value, float Min, float Max)
{
    Value = FMath::Clamp(Value, Min, Max);
}
