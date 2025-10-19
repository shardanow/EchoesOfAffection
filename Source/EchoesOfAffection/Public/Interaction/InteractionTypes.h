// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.generated.h"

/**
 * Enum defining different types of interactions
 */
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None			UMETA(DisplayName = "None"),
	Pickup			UMETA(DisplayName = "Pickup"),
	Examine			UMETA(DisplayName = "Examine"),
	Talk			UMETA(DisplayName = "Talk"),
	Open			UMETA(DisplayName = "Open"),
	Close			UMETA(DisplayName = "Close"),
	Toggle			UMETA(DisplayName = "Toggle"),
	Use				UMETA(DisplayName = "Use"),
	Activate		UMETA(DisplayName = "Activate"),
	Custom			UMETA(DisplayName = "Custom")
};

/**
 * Struct containing data about an interaction
 */
USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

	/** Display name shown to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionName;

	/** Type of interaction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	EInteractionType InteractionType;

	/** Action text (e.g., "Press E to Open") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionText;

	/** Duration required to complete interaction (0 = instant) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDuration;

	/** Maximum distance from which interaction is possible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxInteractionDistance;

	/** Whether this interaction can be performed multiple times */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteractMultipleTimes;

	/** Whether this interaction requires line of sight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRequiresLineOfSight;

	/** Priority for interaction (higher = more important if multiple are available) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 InteractionPriority;

	FInteractionData()
		: InteractionName(FText::FromString("Interact"))
		, InteractionType(EInteractionType::Use)
		, ActionText(FText::FromString("Press E to Interact"))
		, InteractionDuration(0.0f)
		, MaxInteractionDistance(200.0f)
		, bCanInteractMultipleTimes(true)
		, bRequiresLineOfSight(true)
		, InteractionPriority(0)
	{
	}
};

/**
 * Result of an interaction attempt
 */
UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	Success				UMETA(DisplayName = "Success"),
	Failed				UMETA(DisplayName = "Failed"),
	TooFar				UMETA(DisplayName = "Too Far"),
	NoLineOfSight		UMETA(DisplayName = "No Line of Sight"),
	AlreadyInteracting	UMETA(DisplayName = "Already Interacting"),
	NotInteractable		UMETA(DisplayName = "Not Interactable"),
	RequirementsNotMet	UMETA(DisplayName = "Requirements Not Met")
};
