// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RelationshipSystemSettings.generated.h"

class URelationshipDatabase;

/**
 * Project settings for Relationship System
 * Configure main database and global behavior
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Relationship System"))
class RELATIONSHIPSYSTEMCORE_API URelationshipSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URelationshipSystemSettings();

	/** Main relationship database asset */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Database",
		meta = (AllowedClasses = "/Script/RelationshipSystemCore.RelationshipDatabase"))
	TSoftObjectPtr<URelationshipDatabase> Database;

	/** Enable automatic decay processing (requires timer setup) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Decay")
	bool bEnableAutomaticDecay;

	/** Decay processing interval in real-time seconds (default: 60 = every minute) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Decay",
		meta = (EditCondition = "bEnableAutomaticDecay", ClampMin = "1.0"))
	float DecayProcessingInterval;

	/** Game hours per real second (1.0 = real-time, 24.0 = 1 day per second) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Decay",
		meta = (EditCondition = "bEnableAutomaticDecay", ClampMin = "0.001"))
	float GameHoursPerRealSecond;

	/** Enable debug logging */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bEnableDebugLogging;

	/** Show relationship debug info in PIE */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowDebugInfo;

	/** UDeveloperSettings interface */
	virtual FName GetCategoryName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
};
