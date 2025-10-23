// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DialogueEffectExecutor.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * Ѕазовый класс эффекта диалога
 * v1.3: Added Reverse() support for full undo
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UDialogueEffect : public UObject
{
    GENERATED_BODY()

public:
    /** ѕрименить эффект */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    void Execute(UDialogueSessionContext* Context);
    virtual void Execute_Implementation(UDialogueSessionContext* Context) {}

    /** NEW v1.3: Reverse effect (for undo) */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    void Reverse(UDialogueSessionContext* Context);
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) {}

    /** NEW v1.3: Check if this effect supports reversal */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Effect")
    virtual bool SupportsReverse() const { return bSupportsReverse; }

    /** ќписание эффекта дл€ UI */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    FText GetDisplayText() const;
    virtual FText GetDisplayText_Implementation() const { return FText::GetEmpty(); }

    /** »конка дл€ UI (опционально) */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    UTexture2D* GetIcon() const;
    virtual UTexture2D* GetIcon_Implementation() const { return nullptr; }

protected:
    /** NEW v1.3: Does this effect support reversal? */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
    bool bSupportsReverse = false;
};

/**
 * Ёффект: изменение Affinity
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyAffinity : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_ModifyAffinity() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float DeltaValue = 0.0f;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: изменение Trust
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyTrust : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_ModifyTrust() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float DeltaValue = 0.0f;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: добавить/удалить предмет
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyInventory : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_ModifyInventory() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 DeltaCount = 1; // отрицательное значение = удалить

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: изменить валюту
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyGold : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_ModifyGold() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 DeltaGold = 0;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: установить флаг пам€ти
 * v1.3: Supports reversal (stores old value)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_SetMemory : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_SetMemory() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName MemoryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    bool Value = true;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;

private:
    /** Stored for undo */
    UPROPERTY(Transient)
    bool OldValue = false;
    
    UPROPERTY(Transient)
    bool bHadOldValue = false;
};

/**
 * Ёффект: добавить WorldState тег
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_AddWorldStateTag : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_AddWorldStateTag() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag TagToAdd;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: удалить WorldState тег
 * v1.3: Supports reversal
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_RemoveWorldStateTag : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_RemoveWorldStateTag() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag TagToRemove;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: начать квест
 * v1.3: Does NOT support reversal (one-way operation)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_StartQuest : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_StartQuest() { bSupportsReverse = false; } // Cannot undo quest start

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
 FName QuestId;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: завершить квест
 * v1.3: Does NOT support reversal (one-way operation)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_CompleteQuest : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_CompleteQuest() { bSupportsReverse = false; } // Cannot undo quest completion

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName QuestId;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Ёффект: установить кастомную переменную
 * v1.3: Supports reversal (stores old value)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_SetVariable : public UDialogueEffect
{
  GENERATED_BODY()

public:
    UDialogueEffect_SetVariable() { bSupportsReverse = true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
 FName VariableKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FString Value;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;

private:
    /** Stored for undo */
    UPROPERTY(Transient)
    FString OldValue;
};

/**
 *  омпозитный эффект (последовательность)
 * v1.3: Supports reversal if all sub-effects support it
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_Composite : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UDialogueEffect_Composite();

    UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
    TArray<TObjectPtr<UDialogueEffect>> Effects;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual void Reverse_Implementation(UDialogueSessionContext* Context) override;
    virtual bool SupportsReverse() const override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * √лавный класс дл€ выполнени€ эффектов
 * 
 * v1.3: Added effect reversal support
 * v1.3.1: Parsing moved to UDialogueEffectParser (SRP principle)
 * 
 * Now focuses only on EXECUTION:
 * - ExecuteEffect() / ExecuteEffects()
 * - ReverseEffect() / ReverseEffects()
 * 
 * For PARSING, use UDialogueEffectParser instead.
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffectExecutor : public UObject
{
    GENERATED_BODY()

public:
    /** ¬ыполнить список эффектов */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ExecuteEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context);

    /** ¬ыполнить единичный эффект */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ExecuteEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context);

    /** NEW v1.3: Reverse single effect */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ReverseEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context);

    /** NEW v1.3: Reverse list of effects (in reverse order) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ReverseEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context);

    /** NEW v1.3: Check if effect supports reversal */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Effects")
    bool CanReverseEffect(UDialogueEffect* Effect) const;
};
