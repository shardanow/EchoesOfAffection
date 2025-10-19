// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueConnection.generated.h"

// Forward declarations
class UDialogueCondition;

/**
 * Соединение между узлами диалога
 * Представляет связь (ребро графа) между двумя узлами
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueConnection
{
    GENERATED_BODY()

    /** ID целевого узла */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    FName TargetNodeId;

    /** Условие перехода */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Connection")
    TObjectPtr<UDialogueCondition> Condition;

    /** Приоритет (для сортировки) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    int32 Priority;

    FDialogueConnection()
        : Priority(0)
    {
    }
};
