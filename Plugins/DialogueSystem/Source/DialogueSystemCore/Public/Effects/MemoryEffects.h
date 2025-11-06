// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Effects/DialogueEffectExecutor.h"
#include "MemoryEffects.generated.h"

// Forward declarations
class UDialogueSessionContext;
class AActor;

/**
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UMemoryEffectBase : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    void Execute(AActor* NPCActor, AActor* PlayerActor) const;
    virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const {}
};

/**
 */
UCLASS(DisplayName = "Add Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UAddMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MemoryStrength = 1.0f;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 */
UCLASS(DisplayName = "Modify Memory Strength", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UModifyMemoryStrengthEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float StrengthModifier = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bAdditive = true;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 */
UCLASS(DisplayName = "Remove Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API URemoveMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 */
UCLASS(DisplayName = "Refresh Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API URefreshMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};
