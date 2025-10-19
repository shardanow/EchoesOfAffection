// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueSubsystem.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueDataAsset;
class UDialogueSessionContext;

/**
 * ���������� subsystem ��� ���������� ���������
 * 
 * �������� ��:
 * - ���������������� ���������� ��������� ���������
 * - ����������� ����������� DialogueAssets
 * - ���������� ��������� (�������� ������, �����������, � �.�.)
 * - Save/Load ��������� ���� ��������
 * - ����������� Condition/Effect ������
 */
UCLASS(Config=Game)
class DIALOGUESYSTEMRUNTIME_API UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    //~ Begin USubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

protected:
    //~ Begin Active Dialogues

    /** ������� �������� ������ (singleton) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> ActiveDialogue;

    /** ������� �������� (��� ����������) */
    UPROPERTY(Transient)
    TArray<FName> DialogueHistory;

    //~ End Active Dialogues

    //~ Begin Cache

    /** ��� ����������� DialogueAssets */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueDataAsset>> LoadedDialogues;

    //~ End Cache

    //~ Begin Settings

    /** �������� ������ ������ (�������� � �������) */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float TypewriterSpeed = 30.0f;

    /** ������� �� ����������� */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bAutoAdvanceEnabled = false;

    /** �������� ������������ */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float AutoAdvanceDelay = 3.0f;

    /** ���������� ������� �������� */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bShowHistory = true;

    /** �������� ������� */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    int32 MaxHistoryLines = 50;

    //~ End Settings

public:
    //~ Begin Public API

    /** ������ ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);

    /** ��������� ������� ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void EndActiveDialogue();

    /** �������� �������� ������ */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    UDialogueRunner* GetActiveDialogue() const { return ActiveDialogue; }

    /** ������ �������? */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    bool IsDialogueActive() const { return ActiveDialogue != nullptr; }

    /** ��������� DialogueAsset (� ������������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    UDialogueDataAsset* LoadDialogue(FName DialogueId);

    /** ������������ �������� (��� �����������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void PreloadDialogues(const TArray<FName>& DialogueIds);

    /** �������� ��� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void ClearCache();

    /** �������� ������� �������� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    const TArray<FName>& GetDialogueHistory() const { return DialogueHistory; }

    /** ��������� ��������� � JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    FString SaveStateToJson() const;

    /** ��������� ��������� �� JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    bool LoadStateFromJson(const FString& Json);

    //~ End Public API

    //~ Begin Settings API

    UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
    float GetTypewriterSpeed() const { return TypewriterSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
    void SetTypewriterSpeed(float NewSpeed) { TypewriterSpeed = FMath::Max(1.0f, NewSpeed); }

    UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
    bool IsAutoAdvanceEnabled() const { return bAutoAdvanceEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
    void SetAutoAdvanceEnabled(bool bEnabled) { bAutoAdvanceEnabled = bEnabled; }

    //~ End Settings API

protected:
    /** Callback: ������ �������� */
    UFUNCTION()
    void HandleDialogueEnded();
};
