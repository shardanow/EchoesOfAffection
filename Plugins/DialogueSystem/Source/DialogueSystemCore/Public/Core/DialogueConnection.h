// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueConnection.generated.h"

// Forward declarations
class UDialogueCondition;

/**
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    FName TargetNodeId;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Connection")
    TObjectPtr<UDialogueCondition> Condition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    int32 Priority;

    FDialogueConnection()
        : Priority(0)
    {
    }
};
