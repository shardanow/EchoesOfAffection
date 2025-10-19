// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/DialogueNode.h"
#include "DialogueDataAsset.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * ������ ������� (��������) NPC
 * ������������ ��� ����������� ������ ������� � AI ��������
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialoguePersonaData
{
    GENERATED_BODY()

    /** ID ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FName PersonaId;

    /** ������������ ��� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FText DisplayName;

    /** ������� �������� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona", meta = (MultiLine = true))
    FText Description;

    /** �������� ����� ��������� (����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FGameplayTagContainer PersonalityTraits; // Trait.Shy, Trait.Confident, etc

    /** AI Prompt (system message) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MultiLine = true))
    FString AISystemPrompt;

    /** ������� ������ ��� Few-Shot Learning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FString> ExamplePhrases;

    /** ������������ ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float PositiveSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float NegativeSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float ForgivenessRate = 1.0f;
};

/**
 * ������� Asset �������
 * �������� ���� ���� �����, ���� � ����������
 * 
 * Data-driven ������: �� ������������� � DataTable ��� Editor,
 * � runtime ������ ������ � ���������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    //~ Begin Metadata

    /** ���������� ID ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FName DialogueId;

    /** ������������ �������� (��� ���������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FText DisplayName;

    /** �������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata", meta = (MultiLine = true))
    FText Description;

    /** ���� ��� ����������/������ */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FGameplayTagContainer DialogueTags;

    /** ����������� ������� ��������� ��� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    float MinAffinityRequired = -100.0f;

    /** ������ ������� (��� ��������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    int32 Version = 1;

    //~ End Metadata

    //~ Begin Graph Data

    /** ��� ���� ������� */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Graph")
    TArray<TObjectPtr<UDialogueNode>> Nodes;

    /** ID ���������� ���� (�� ���������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph")
    FName StartNode;

    //~ End Graph Data

    //~ Begin Persona Data

    /** ������ ������� �������� ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    FDialoguePersonaData PrimaryPersona;

    /** �������������� ������� (��� ��������� ��������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    TMap<FName, FDialoguePersonaData> AdditionalPersonas;

    //~ End Persona Data

    //~ Begin Localization

    /** Namespace ��� ����������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    FString LocalizationNamespace;

    /** �������������� ����� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    TArray<FString> SupportedCultures;

    //~ End Localization

    //~ Begin AI Integration

    /** ��������� AI ��������� ��� ����� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bAllowAIGeneration = false;

    /** ����������� ��� AI (������������) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float AITemperature = 0.7f;

    /** Max ������� ��� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 AIMaxTokens = 150;

    //~ End AI Integration

public:
    //~ Begin API

    /** ����� ���� �� ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    UDialogueNode* FindNode(FName NodeId) const;

    /** �������� ��������� ���� ���� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    TArray<FDialogueEdgeData> GetOutgoingEdges(FName FromNodeId) const;

    /** �������� ��������� ���� � ������ ��������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FName GetStartNode(const UDialogueSessionContext* Context) const;

    /** ��������� ����� (��� ���������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ValidateGraph(TArray<FString>& OutErrors) const;

    /** ������� � JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FString ExportToJson() const;

    /** ������ �� JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ImportFromJson(const FString& Json);

    //~ End API

protected:
    //~ Begin UObject Interface
    virtual void PostLoad() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    //~ End UObject Interface

    /** ��������� ������ ��� �������� ������ */
    void RebuildIndex();

    /** ������ ����� */
    UPROPERTY(Transient)
    TMap<FName, int32> NodeIndex;
};
