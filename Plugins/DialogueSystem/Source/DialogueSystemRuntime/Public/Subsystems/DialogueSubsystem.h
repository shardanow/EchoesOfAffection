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
 * Глобальный subsystem для управления диалогами
 * 
 * Отвечает за:
 * - Централизованное управление активными диалогами
 * - Кэширование загруженных DialogueAssets
 * - Глобальные настройки (скорость текста, автопропуск, и т.д.)
 * - Save/Load состояния всех диалогов
 * - Регистрация Condition/Effect фабрик
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

    /** Текущий активный диалог (singleton) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> ActiveDialogue;

    /** История диалогов (для статистики) */
    UPROPERTY(Transient)
    TArray<FName> DialogueHistory;

    //~ End Active Dialogues

    //~ Begin Cache

    /** Кэш загруженных DialogueAssets */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueDataAsset>> LoadedDialogues;

    //~ End Cache

    //~ Begin Settings

    /** Скорость печати текста (символов в секунду) */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float TypewriterSpeed = 30.0f;

    /** Включен ли автопропуск */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bAutoAdvanceEnabled = false;

    /** Задержка автопропуска */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    float AutoAdvanceDelay = 3.0f;

    /** Показывать историю диалогов */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    bool bShowHistory = true;

    /** Максимум истории */
    UPROPERTY(Config, BlueprintReadWrite, Category = "Settings")
    int32 MaxHistoryLines = 50;

    //~ End Settings

public:
    //~ Begin Public API

    /** Начать диалог */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);

    /** Завершить текущий диалог */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void EndActiveDialogue();

    /** Получить активный диалог */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    UDialogueRunner* GetActiveDialogue() const { return ActiveDialogue; }

    /** Диалог активен? */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    bool IsDialogueActive() const { return ActiveDialogue != nullptr; }

    /** Загрузить DialogueAsset (с кэшированием) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    UDialogueDataAsset* LoadDialogue(FName DialogueId);

    /** Предзагрузка диалогов (для оптимизации) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void PreloadDialogues(const TArray<FName>& DialogueIds);

    /** Очистить кэш */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    void ClearCache();

    /** Получить историю диалогов */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Subsystem")
    const TArray<FName>& GetDialogueHistory() const { return DialogueHistory; }

    /** Сохранить состояние в JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Subsystem")
    FString SaveStateToJson() const;

    /** Загрузить состояние из JSON */
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
    /** Callback: диалог завершён */
    UFUNCTION()
    void HandleDialogueEnded();
};
