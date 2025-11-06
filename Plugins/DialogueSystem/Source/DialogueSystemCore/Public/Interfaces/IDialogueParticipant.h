// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDialogueParticipant.generated.h"

// Forward declarations
class UDialogueRunner;

/**
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UDialogueParticipant : public UInterface
{
    GENERATED_BODY()
};

class DIALOGUESYSTEMCORE_API IDialogueParticipant
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    FName GetParticipantId() const;
    virtual FName GetParticipantId_Implementation() const { return NAME_None; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    FText GetParticipantDisplayName() const;
    virtual FText GetParticipantDisplayName_Implementation() const { return FText::GetEmpty(); }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    UTexture2D* GetParticipantPortrait() const;
    virtual UTexture2D* GetParticipantPortrait_Implementation() const { return nullptr; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    bool CanStartDialogue(AActor* Initiator) const;
    virtual bool CanStartDialogue_Implementation(AActor* Initiator) const { return true; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    void OnDialogueStarted(UDialogueRunner* Runner);
    virtual void OnDialogueStarted_Implementation(UDialogueRunner* Runner) {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    void OnDialogueEnded();
    virtual void OnDialogueEnded_Implementation() {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Participant")
    void OnLineSaid(const FText& Line, const FGameplayTag& Emotion);
    virtual void OnLineSaid_Implementation(const FText& Line, const FGameplayTag& Emotion) {}
};
