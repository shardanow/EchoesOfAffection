// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Logging/LogVerbosity.h"
#include "ScheduleSystemSettings.generated.h"

/**
 * Project-wide settings for the Schedule System
 * Configure in Project Settings ? Plugins ? Actor Schedule System
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Actor Schedule System"))
class SCHEDULECORE_API UScheduleSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UScheduleSystemSettings();

	/** Whether to enable debug visualization */
	UPROPERTY(config, EditAnywhere, Category = "Debug", meta = (ConfigRestartRequired = false))
	bool bEnableDebugVisualization = false;

	/** Debug text color for schedule display */
	UPROPERTY(config, EditAnywhere, Category = "Debug")
	FColor DebugTextColor = FColor::Green;

	/** Duration to display debug info */
	UPROPERTY(config, EditAnywhere, Category = "Debug", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float DebugDisplayDuration = 0.1f;

	/** Maximum number of concurrent scheduled actors to optimize performance */
	UPROPERTY(config, EditAnywhere, Category = "Performance", meta = (ClampMin = "1", ClampMax = "1000"))
	int32 MaxConcurrentScheduledActors = 500;

	/** Whether to automatically disable schedules for actors beyond this distance */
	UPROPERTY(config, EditAnywhere, Category = "Performance")
	bool bUseDistanceBasedCulling = true;

	/** Distance beyond which schedules are disabled (0 = unlimited) */
	UPROPERTY(config, EditAnywhere, Category = "Performance", meta = (EditCondition = "bUseDistanceBasedCulling", ClampMin = "0"))
	float CullingDistance = 5000.0f;

	/** Update interval for distance checks (seconds) */
	UPROPERTY(config, EditAnywhere, Category = "Performance", meta = (EditCondition = "bUseDistanceBasedCulling", ClampMin = "0.1"))
	float CullingUpdateInterval = 1.0f;

	/** Default acceptance radius for movement actions */
	UPROPERTY(config, EditAnywhere, Category = "Defaults")
	float DefaultAcceptanceRadius = 100.0f;

	/** Default movement speed multiplier */
	UPROPERTY(config, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float DefaultMovementSpeedMultiplier = 1.0f;

	/** Gameplay tags that mark an actor as non-schedulable */
	UPROPERTY(config, EditAnywhere, Category = "Filtering")
	FGameplayTagContainer ExcludedActorTags;

	/** Actor classes to never schedule (e.g., projectiles, effects) */
	UPROPERTY(config, EditAnywhere, Category = "Filtering")
	TArray<TSoftClassPtr<AActor>> ExcludedActorClasses;

	/** Whether to log schedule events */
	UPROPERTY(config, EditAnywhere, Category = "Logging")
	bool bLogScheduleEvents = false;

	/** Log verbosity level (0=Fatal, 1=Error, 2=Warning, 3=Display, 4=Log, 5=Verbose) */
	UPROPERTY(config, EditAnywhere, Category = "Logging", meta = (ClampMin = "0", ClampMax = "5"))
	uint8 LogVerbosityLevel = 4;

public:
	/** Get the settings instance */
	static const UScheduleSystemSettings* Get()
	{
		return GetDefault<UScheduleSystemSettings>();
	}

	/** Get log verbosity as ELogVerbosity enum */
	ELogVerbosity::Type GetLogVerbosity() const
	{
		switch (LogVerbosityLevel)
		{
		case 0: return ELogVerbosity::Fatal;
		case 1: return ELogVerbosity::Error;
		case 2: return ELogVerbosity::Warning;
		case 3: return ELogVerbosity::Display;
		case 4: return ELogVerbosity::Log;
		case 5: return ELogVerbosity::Verbose;
		default: return ELogVerbosity::Log;
		}
	}

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
	virtual FText GetSectionText() const override { return NSLOCTEXT("ScheduleSystem", "ScheduleSystemSettingsSection", "Actor Schedule System"); }

#if WITH_EDITOR
	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("ScheduleSystem", "ScheduleSystemSettingsDescription", "Configure the Actor Schedule System behavior and performance settings");
	}
#endif
};
