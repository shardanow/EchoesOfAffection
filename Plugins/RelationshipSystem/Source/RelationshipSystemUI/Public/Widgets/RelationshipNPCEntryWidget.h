// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RelationshipNPCEntryWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UProgressBar;

/** Delegate for NPC entry click */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCClickedSignature, AActor*, ClickedNPC);

/**
 * Widget representing a single NPC entry in the relationship list
 * Shows basic info: name, state, key dimension value
 * Clickable to show detailed relationship panel
 */
UCLASS()
class RELATIONSHIPSYSTEMUI_API URelationshipNPCEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Main button for selection (bind in Blueprint) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	/** NPC name text (bind in Blueprint) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NPCNameText;

	/** State text (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StateText;

	/** State icon (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> StateIcon;

	/** Preview dimension bar (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PreviewDimensionBar;

	/** Preview dimension value text (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PreviewDimensionText;

	/** Which dimension to show in preview? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Entry")
	FGameplayTag PreviewDimension;

	/** Set NPC actor */
	UFUNCTION(BlueprintCallable, Category = "NPC Entry")
	void SetNPCActor(AActor* InNPC);

	/** Set player actor (for relationship lookup) */
	UFUNCTION(BlueprintCallable, Category = "NPC Entry")
	void SetPlayerActor(AActor* InPlayer);

	/** Refresh display data */
	UFUNCTION(BlueprintCallable, Category = "NPC Entry")
	void RefreshDisplay();

	/** Get NPC name */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Entry")
	FText GetNPCName() const;

	/** Get current state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Entry")
	FGameplayTag GetCurrentState() const;

	/** Get state display name */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Entry")
	FText GetStateDisplayName() const;

	/** Get preview dimension value */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Entry")
	float GetPreviewDimensionValue() const;

	/** Get preview dimension normalized (0-1) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC Entry")
	float GetPreviewDimensionNormalized() const;

	/** Delegate for click event */
	UPROPERTY(BlueprintAssignable, Category = "NPC Entry|Events")
	FOnNPCClickedSignature OnNPCClicked;

protected:
	virtual void NativeConstruct() override;

private:
	/** NPC actor */
	UPROPERTY(Transient)
	TObjectPtr<AActor> NPCActor;

	/** Player actor (Subject) */
	UPROPERTY(Transient)
	TObjectPtr<AActor> PlayerActor;

	/** Handle button click */
	UFUNCTION()
	void OnButtonClicked();
};
