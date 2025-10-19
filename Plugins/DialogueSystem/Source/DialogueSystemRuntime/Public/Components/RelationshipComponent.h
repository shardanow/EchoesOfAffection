// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RelationshipComponent.generated.h"

/**
 * ��� ������� ��� ��������� ���������
 */
UENUM(BlueprintType)
enum class ERelationshipStimulusType : uint8
{
    /** ������� */
    Gift,
    
    /** ���������� */
    Compliment,
    
    /** ����������� */
    Insult,
    
    /** ������ */
    Help,
    
    /** ������������� */
    Betrayal,
    
    /** ����� */
    Flirt,
    
    /** ������ */
    Threat,
    
    /** ����� ���������� �������� */
    Positive,
    
    /** ����� ���������� �������� */
    Negative
};

/**
 * ������ �������
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FRelationshipStimulus
{
    GENERATED_BODY()

    /** ��� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    ERelationshipStimulusType Type = ERelationshipStimulusType::Positive;

    /** ���� ����������� (-100 to +100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    float Intensity = 10.0f;

    /** �������� (��� ������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    TObjectPtr<AActor> Source;

    /** �������������� �������� (����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    FGameplayTagContainer ContextTags;

    /** �������� (��� �����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    FText Description;
};

/**
 * ��������� ���������� NPC
 */
UENUM(BlueprintType)
enum class ENPCMoodState : uint8
{
    Neutral,
    Happy,
    Sad,
    Angry,
    Fearful,
    Excited,
    Bored,
    Anxious
};

/**
 * ��������
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRelationshipChanged, FName, AttributeName, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoodChanged, ENPCMoodState, OldMood, ENPCMoodState, NewMood);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRelationshipThresholdReached, FName, ThresholdName);

/**
 * ��������� ���������� ����������� NPC � �������
 * 
 * ��������:
 * - Affinity (��������): -100 to +100
 * - Trust (�������): 0 to 100
 * - Respect (��������): 0 to 100
 * - Romance (���������): 0 to 100
 * - Fear (�����): 0 to 100
 * - Jealousy (��������): 0 to 100
 * 
 * �����������:
 * - �������������� decay (��������� ����������� � ������������)
 * - Threshold ������� (��������, Affinity >= 50 => "Friend" ����)
 * - ������������ �� ������� NPC (�����������, ������, � �.�.)
 * - ���������� � ������� (���������� ������ �������)
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API URelationshipComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URelationshipComponent();

    //~ Begin Events

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnRelationshipChanged OnRelationshipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnMoodChanged OnMoodChanged;

    UPROPERTY(BlueprintAssignable, Category = "Relationship|Events")
    FOnRelationshipThresholdReached OnThresholdReached;

    //~ End Events

protected:
    //~ Begin Core Attributes

    /** �������� (-100 ����, 0 �������, +100 ������ ����) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
    float Affinity = 0.0f;

    /** ������� (0 - �� ��������, 100 - ������ �������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Trust = 50.0f;

    /** �������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Respect = 50.0f;

    /** ��������� (��� �������� ���������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Romance = 0.0f;

    /** ����� ����� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Fear = 0.0f;

    /** �������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship", meta = (ClampMin = "0", ClampMax = "100"))
    float Jealousy = 0.0f;

    //~ End Core Attributes

    //~ Begin Mood System

    /** ������� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
    ENPCMoodState CurrentMood = ENPCMoodState::Neutral;

    /** ������� ������� NPC ��������� � ������� ���������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mood")
    float MoodDuration = 0.0f;

    /** ����������� ������������ ���������� (���) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
    float MinMoodDuration = 30.0f;

    //~ End Mood System

    //~ Begin Decay Settings

    /** ������� �� �������������� decay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    bool bEnableDecay = true;

    /** �������� decay ��� Affinity (������ � �������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float AffinityDecayRate = 0.5f;

    /** ������� �������� ��� decay Affinity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float AffinityDecayTarget = 0.0f;

    /** �������� decay ��� ������ ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
    float OtherDecayRate = 1.0f;

    //~ End Decay Settings

    //~ Begin Modifiers (from Persona)

    /** ����������� ������� �� ���������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float PositiveSensitivity = 1.0f;

    /** ����������� ������� �� ���������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float NegativeSensitivity = 1.0f;

    /** ��������� ����� NPC ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float ForgivenessRate = 1.0f;

    /** ��������� ������ NPC ���������� ����� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    float GrudgeStrength = 1.0f;

    //~ End Modifiers

    //~ Begin Memory

    /** ������� ������ ������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
    TArray<FRelationshipStimulus> EventHistory;

    /** �������� �������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 MaxEventHistory = 20;

    //~ End Memory

public:
    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    //~ Begin Public API

    /** ��������� ������ */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ApplyStimulus(const FRelationshipStimulus& Stimulus);

    /** �������� Affinity �������� */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyAffinity(float Delta);

    /** �������� Trust */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyTrust(float Delta);

    /** �������� Respect */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyRespect(float Delta);

    /** �������� Romance */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyRomance(float Delta);

    /** �������� Fear */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ModifyFear(float Delta);

    /** ���������� ���������� ������������� */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void SetMood(ENPCMoodState NewMood);

    /** �������� ������� ���� ��������� (������ ��� UI) */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    FText GetRelationshipRank() const;

    /** �������� Affinity */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetAffinity() const { return Affinity; }

    /** �������� Trust */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetTrust() const { return Trust; }

    /** �������� Romance */
    UFUNCTION(BlueprintPure, Category = "Relationship")
    float GetRomance() const { return Romance; }

    /** �������� ��� �������� */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void ResetRelationship();

    /** ������������ */
    UFUNCTION(BlueprintCallable, Category = "Relationship")
    void SaveToJson(FString& OutJson) const;

    UFUNCTION(BlueprintCallable, Category = "Relationship")
    bool LoadFromJson(const FString& Json);

    //~ End Public API

protected:
    /** ��������� decay */
    void ProcessDecay(float DeltaTime);

    /** ��������� ���������� */
    void ProcessMood(float DeltaTime);

    /** �������� ���������� �� ������ ��������� */
    void UpdateMoodFromAttributes();

    /** �������� threshold'�� */
    void CheckThresholds(float OldAffinity, float NewAffinity);

    /** Clamp �������� */
    void ClampAttribute(float& Value, float Min, float Max);
};
