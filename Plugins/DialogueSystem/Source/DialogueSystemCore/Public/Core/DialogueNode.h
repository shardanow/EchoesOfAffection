// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialogueConnection.h"
#include "DialogueNode.generated.h"

// Forward declarations
class UDialogueCondition;
class UDialogueEffect;
class UDialogueSessionContext;

/**
 * ��� ���� �������
 */
UENUM(BlueprintType)
enum class EDialogueNodeType : uint8
{
    /** ����������� ������� NPC */
    NpcLine,
    
    /** ����� ������ (branch point) */
    PlayerChoice,
    
    /** �������� ������� (invisible node) */
    Condition,
    
    /** ��������� ����� (weighted random) */
    Random,
    
    /** ������� (trigger only) */
    Event,
    
    /** ���������� ������� */
    End,
    
    /** AI-������������ ������� */
    AIGenerated
};

/**
 * ��������� ������ ��� ��������
 */
UENUM(BlueprintType)
enum class EEmotionIntensity : uint8
{
    Neutral,
    Mild,
    Moderate,
    Strong,
    Extreme
};

/**
 * ���������� ��� ������ � ��������
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialoguePresentationData
{
    GENERATED_BODY()

    /** ��� ������ (GameplayTag: Emotion.Happy, Emotion.Angry, etc) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FGameplayTag EmotionTag;

    /** ������������� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    EEmotionIntensity Intensity = EEmotionIntensity::Neutral;

    /** �������� �������� ����/���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FName AnimationName;

    /** ��������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    TSoftObjectPtr<class USoundBase> VoiceAudio;

    /** ����� ����� ����� ��������� �������� (���) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    float PauseAfter = 0.5f;

    /** ������ ������ ��������������� �� ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    bool bFocusCamera = true;

    /** ��������� ������ (������/������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    float CameraDistance = 200.0f;
};

/**
 * ������� ��������� ���� �������
 * ������������ � DataTable � runtime ����
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueNodeData
{
    GENERATED_BODY()

    /** ���������� ID ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName NodeId;

    /** ��� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    EDialogueNodeType NodeType = EDialogueNodeType::NpcLine;

    /** ID ���������-���������� (��� NpcLine) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName SpeakerId;

    /** ����� ������� (������������ {placeholders}) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = true))
    FText DialogueText;

    /** ����� ������ ������ (��� PlayerChoice) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    FText ChoiceText;

    /** ������ ����������� (�����������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
    TSoftObjectPtr<class UTexture2D> ChoiceIcon;

    /** Presentation ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    FDialoguePresentationData Presentation;

    /** ������ ������� (DSL) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
    FString ConditionExpression;

    /** ������ �������� (DSL) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
    FString EffectExpression;

    /** ���� ��� ����������/������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    FGameplayTagContainer NodeTags;

    /** ��� ��� Random ����� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    float RandomWeight = 1.0f;

    /** ����� ���������� ������ 1 ���? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    bool bOneTimeOnly = false;

    /** �������������� ������� (��� �������� input) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
    bool bAutoAdvance = false;

    /** ����� �������� ������������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta", meta = (EditCondition = "bAutoAdvance"))
    float AutoAdvanceDelay = 2.0f;

    /** ��������� ��� AI (���� AIGenerated) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MultiLine = true))
    FString AIPromptHint;
};

/**
 * ����� ����� � ������� ����� ������
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueEdgeData
{
    GENERATED_BODY()

    /** ���� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FName FromNodeId;

    /** ���� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FName ToNodeId;

    /** ������� �������� (�����������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    FString EdgeCondition;

    /** ��������� (��� ����������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edge")
    int32 Priority = 0;
};

/**
 * Runtime ������������� ���� (�������� �� FDialogueNodeData)
 * ��������� ��������� � ������������ ������
 */
//UCLASS(BlueprintType)
UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType, Blueprintable)
class DIALOGUESYSTEMCORE_API UDialogueNode : public UObject
{
    GENERATED_BODY()

public:
    UDialogueNode();

    /** ���������� ID ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NodeId;

    /** ��� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    EDialogueNodeType NodeType;

    /** ID ���������-���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName SpeakerId;

    /** ����� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = true))
    FText DialogueText;

    /** ����� ������ ������ (��� PlayerChoice) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    /** ������ ���������� (�����������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UTexture2D> SpeakerPortrait;

    /** Presentation ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FDialoguePresentationData Presentation;

    /** ��� ������ (GameplayTag: Emotion.Happy, Emotion.Angry, etc) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTag EmotionTag;

    /** �������������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bAutoAdvance;

    /** ����� �������� ������������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "bAutoAdvance"))
    float AutoAdvanceDelay;

    /** ���� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTagContainer Tags;

    /** ��� ��� Random ����� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float RandomWeight;

    /** ����� ���������� ������ 1 ���? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bOneTimeOnly;

    /** ������� (������) */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TObjectPtr<UDialogueCondition> Condition;

    /** ������� ���� */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TArray<TObjectPtr<UDialogueEffect>> Effects;

    /** ���������� � ������� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueConnection> Connections;

    /** ��� �� ���� ������� � ������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bVisitedThisSession;

    /** ���������� ��������� (��� ����������) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    int32 VisitCount;

    /** ����� ��� ������������� */
    void Initialize(const FDialogueNodeData& InData);

    /** �������� ��� ���������� */
    void MarkVisited()
    {
        bVisitedThisSession = true;
        VisitCount++;
    }

    /** �������� ��������� ����� � ������������ ������������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FText GetResolvedText(const UDialogueSessionContext* Context) const;

    /** ��������� ����������� ���� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool IsAvailable(const UDialogueSessionContext* Context) const;

    /** Get dialogue text (wrapper for compatibility) */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetDialogueText() const { return DialogueText; }

    /** Get choice text (wrapper for compatibility) */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetChoiceText() const { return ChoiceText; }

    /** Get speaker name */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FText GetSpeakerName() const { return FText::FromName(SpeakerId); }

    /** Get emotion tag */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FGameplayTag GetEmotionTag() const { return EmotionTag; }

    /** Get speaker portrait */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    UTexture2D* GetSpeakerPortrait() const { return SpeakerPortrait.LoadSynchronous(); }
};
