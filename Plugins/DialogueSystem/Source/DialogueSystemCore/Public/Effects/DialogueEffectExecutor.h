// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DialogueEffectExecutor.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * ������� ����� ������� �������
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UDialogueEffect : public UObject
{
    GENERATED_BODY()

public:
    /** ��������� ������ */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    void Execute(UDialogueSessionContext* Context);
    virtual void Execute_Implementation(UDialogueSessionContext* Context) {}

    /** �������� ������� ��� UI */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    FText GetDisplayText() const;
    virtual FText GetDisplayText_Implementation() const { return FText::GetEmpty(); }

    /** ������ ��� UI (�����������) */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    UTexture2D* GetIcon() const;
    virtual UTexture2D* GetIcon_Implementation() const { return nullptr; }
};

/**
 * ������: ��������� Affinity
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyAffinity : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float DeltaValue = 0.0f;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ��������� Trust
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyTrust : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float DeltaValue = 0.0f;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ��������/������� �������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyInventory : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 DeltaCount = 1; // ������������� �������� = �������

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: �������� ������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyGold : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 DeltaGold = 0;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ���������� ���� ������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_SetMemory : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName MemoryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    bool Value = true;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: �������� WorldState ���
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_AddWorldStateTag : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag TagToAdd;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ������� WorldState ���
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_RemoveWorldStateTag : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag TagToRemove;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ������ �����
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_StartQuest : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName QuestId;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ��������� �����
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_CompleteQuest : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName QuestId;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������: ���������� ��������� ����������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_SetVariable : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName VariableKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FString Value;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ����������� ������ (������������������)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffect_Composite : public UDialogueEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
    TArray<TObjectPtr<UDialogueEffect>> Effects;

    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
    virtual FText GetDisplayText_Implementation() const override;
};

/**
 * ������� ����� ��� �������� � ���������� ��������
 * 
 * ������� DSL:
 * - "affinity[Lianne] += 10"
 * - "giveItem(Rose, 1); gold -= 50"
 * - "setMemory(GaveGift); startQuest(HelpLianne)"
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffectExecutor : public UObject
{
    GENERATED_BODY()

public:
    /** ������� ������ ������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    UDialogueEffect* ParseEffect(const FString& EffectString);

    /** ��������� ������ ������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ExecuteString(const FString& EffectString, UDialogueSessionContext* Context);

    /** ��������� ������ �������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ExecuteEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context);

    /** ��������� ������ ������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    void ExecuteEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context);

    /** �������� ������ ���� �������� �� ������ (��� UI preview) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
    TArray<UDialogueEffect*> ParseEffectList(const FString& EffectString);

protected:
    /** Tokenizer */
    TArray<FString> TokenizeExpression(const FString& Expression);

    /** ������� ������ ������� */
    UDialogueEffect* ParseSingleEffect(const TArray<FString>& Tokens, int32& Index);

    /** ��� �������� */
    UPROPERTY(Transient)
    TMap<FString, TObjectPtr<UDialogueEffect>> EffectCache;
};
