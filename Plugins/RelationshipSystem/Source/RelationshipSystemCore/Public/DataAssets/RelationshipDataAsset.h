// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipDataAsset.generated.h"

/**
 * Base class for all Relationship System Data Assets
 * Provides common functionality: validation, indexing, hot-reload support
 */
UCLASS(Abstract, BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URelationshipDataAsset();

	/** Unique identifier for this asset (used for lookups) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (Categories = "Relationship"))
	FGameplayTag AssetTag;

	/** Display name for editor and UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	/** Description for designers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText Description;

	/** Icon for UI representation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Color tint for UI elements */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	FLinearColor DisplayColor;

	/** Validate this asset's configuration */
	UFUNCTION(BlueprintCallable, Category = "Validation")
	virtual bool Validate(TArray<FText>& OutErrors) const;

	/** Get unique identifier */
	FORCEINLINE FGameplayTag GetAssetTag() const { return AssetTag; }

	/** Get display name (fallback to tag if empty) */
	FText GetDisplayName() const;

#if WITH_EDITOR
	/** Called when property changes in editor */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	/** Editor-time validation */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

protected:
	/** Override in child classes for specific validation */
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const { return true; }
};
