// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RelationshipWidgetBase.h"
#include "GameplayTagContainer.h"
#include "RelationshipDimensionBar.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * Widget for displaying a single dimension value as a progress bar
 * Designer can create Blueprint child and bind UMG controls
 */
UCLASS()
class RELATIONSHIPSYSTEMUI_API URelationshipDimensionBar : public URelationshipWidgetBase
{
	GENERATED_BODY()

public:
	/** Dimension to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension Bar", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Show numeric value? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension Bar")
	bool bShowNumericValue = true;

	/** Show dimension name? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension Bar")
	bool bShowDimensionName = true;

	/** Normalize to 0-1 range? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension Bar")
	bool bNormalizeValue = true;

	/** Get current dimension value */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dimension Bar")
	float GetDimensionValue() const;

	/** Get normalized dimension value (0-1) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dimension Bar")
	float GetNormalizedValue() const;

	/** Get dimension display name */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dimension Bar")
	FText GetDimensionDisplayName() const;

	/** Progress bar widget (bind in Blueprint) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	/** Value text widget (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ValueText;

	/** Name text widget (bind in Blueprint, optional) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameText;

protected:
	virtual void NativeRefreshDisplay() override;
};
