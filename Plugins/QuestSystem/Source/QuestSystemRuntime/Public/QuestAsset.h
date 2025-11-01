// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestTypes.h"
#include "QuestStructures.h"
#include "QuestObjective.h"
#include "QuestAsset.generated.h"

/**
 * Quest Asset - Primary Data Asset defining a complete quest
 * Data-driven quest definition with phases, objectives, dependencies, and rewards
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UQuestAsset();

	//~ Begin UPrimaryDataAsset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	//~ End UPrimaryDataAsset Interface

	/** Unique quest identifier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", AssetRegistrySearchable)
	FName QuestId;

	/** Display title */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText Title;

	/** Detailed description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (MultiLine = true))
	FText Description;

	/** Short summary for UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (MultiLine = true))
	FText Summary;

	/** Quest metadata */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FQuestMeta Meta;

	/** Quest phases/stages */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FQuestPhase> Phases;

	/** Dependencies required to start this quest */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FQuestDependency> Dependencies;

	/** How quest is started */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FQuestStartPolicyConfig StartPolicy;

	/** Quest failure configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FQuestFailurePolicy FailurePolicy;

	/** Rewards given on quest completion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FQuestRewardSet RewardsOnComplete;

	/** Icon for UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> QuestIcon;

	/** Banner image for journal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> BannerImage;

	/** Color tint for UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor UIColor = FLinearColor::White;

	/** Quest giver NPC ID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestGiverId;

	/** Location where quest is given */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestLocation;

	// Helper methods

	/** Get phase by ID */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	FQuestPhase GetPhase(FName PhaseId) const;

	/** Check if quest has phase */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool HasPhase(FName PhaseId) const;

	/** Get first phase */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	FQuestPhase GetFirstPhase() const;

	/** Get all objective IDs */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FName> GetAllObjectiveIds() const;

	/** Validate quest data */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool ValidateQuestData(TArray<FText>& OutErrors) const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
