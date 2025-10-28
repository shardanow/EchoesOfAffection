// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScheduleTypes.generated.h"

class AActor;
class UWorld;
class UScheduleActionInterface;
class UScheduleConditionInterface;
class UScheduleLocationSelectorInterface;

/**
 * Enum representing the current state of a scheduled action
 */
UENUM(BlueprintType)
enum class EScheduleActionState : uint8
{
	/** Action is pending evaluation */
	Pending,
	/** Action is currently executing */
	Active,
	/** Action has been paused and can be resumed */
	Paused,
	/** Action has been cancelled */
	Cancelled,
	/** Action has completed successfully */
	Completed,
	/** Action failed during execution */
	Failed
};

/**
 * Enum for schedule recurrence patterns
 */
UENUM(BlueprintType)
enum class EScheduleRecurrence : uint8
{
	/** Execute once only */
	Once,
	/** Repeat daily */
	Daily,
	/** Repeat weekly */
	Weekly,
	/** Repeat at custom intervals */
	Custom
};

/**
 * Enum for time-based condition types
 */
UENUM(BlueprintType)
enum class EScheduleTimeType : uint8
{
	/** Specific time of day (e.g., 14:30) */
	TimeOfDay,
	/** Time range (e.g., 08:00 - 17:00) */
	TimeRange,
	/** Duration from schedule start */
	Duration,
	/** Game time elapsed */
	GameTime
};

/**
 * Handle to track a running action instance
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleActionHandle
{
	GENERATED_BODY()

	/** Unique identifier for this action instance */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FGuid HandleID;

	/** Actor executing this action */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TWeakObjectPtr<AActor> OwningActor;

	FScheduleActionHandle()
		: HandleID(FGuid::NewGuid())
		, OwningActor(nullptr)
	{}

	bool IsValid() const { return HandleID.IsValid() && OwningActor.IsValid(); }
	bool operator==(const FScheduleActionHandle& Other) const { return HandleID == Other.HandleID; }
	bool operator!=(const FScheduleActionHandle& Other) const { return !(*this == Other); }
};

FORCEINLINE uint32 GetTypeHash(const FScheduleActionHandle& Handle)
{
	return GetTypeHash(Handle.HandleID);
}

/**
 * Time-based condition data
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleTimeCondition
{
	GENERATED_BODY()

	/** Type of time condition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	EScheduleTimeType TimeType = EScheduleTimeType::TimeOfDay;

	/** Start hour (0-23) for TimeOfDay or TimeRange */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "23"))
	int32 StartHour = 8;

	/** Start minute (0-59) for TimeOfDay or TimeRange */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "59"))
	int32 StartMinute = 0;

	/** End hour (0-23) for TimeRange */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "23"))
	int32 EndHour = 17;

	/** End minute (0-59) for TimeRange */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "59"))
	int32 EndMinute = 0;

	/** Duration in seconds for Duration type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0"))
	float DurationSeconds = 3600.0f;

	/** Days of week this applies to (1=Monday, 7=Sunday, 0=All) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	TArray<int32> DaysOfWeek;

	/** Check if current time satisfies this condition */
	bool EvaluateTime(int32 CurrentHour, int32 CurrentMinute, int32 CurrentDayOfWeek) const;
};

/**
 * Location data for scheduled actions
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleLocation
{
	GENERATED_BODY()

	/** World location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FVector Location = FVector::ZeroVector;

	/** Optional rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Optional reference actor at this location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	TWeakObjectPtr<AActor> ReferenceActor;

	/** Gameplay tags describing this location (e.g., Location.Type.Shop, Location.Zone.Downtown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FGameplayTagContainer LocationTags;

	/** Custom user data for location-specific information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	TMap<FName, FString> CustomData;

	bool IsValid() const { return !Location.IsNearlyZero() || ReferenceActor.IsValid(); }
};

/**
 * Context for condition evaluation
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleEvaluationContext
{
	GENERATED_BODY()

	/** World context */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TObjectPtr<UWorld> World = nullptr;

	/** Actor being scheduled */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TWeakObjectPtr<AActor> ScheduledActor;

	/** Current game time in seconds */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	float GameTime = 0.0f;

	/** Current in-game time (hour, minute, day, etc.) - will be set by external time system */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	int32 CurrentHour = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	int32 CurrentMinute = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	int32 CurrentDayOfWeek = 0;

	/** Gameplay tags active on the actor */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FGameplayTagContainer ActiveTags;

	/** Custom evaluation data */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TMap<FName, FString> CustomData;
};

/**
 * Context for action execution
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleExecutionContext
{
	GENERATED_BODY()

	/** World context */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TObjectPtr<UWorld> World = nullptr;

	/** Actor executing the action */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TWeakObjectPtr<AActor> ExecutingActor;

	/** Target location for the action (if applicable) */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FScheduleLocation TargetLocation;

	/** Delta time since last execution update */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	float DeltaTime = 0.0f;

	/** Custom execution data */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TMap<FName, FString> CustomData;
};

/**
 * Context for location selection
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleLocationContext
{
	GENERATED_BODY()

	/** World context */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TObjectPtr<UWorld> World = nullptr;

	/** Actor requesting location */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TWeakObjectPtr<AActor> RequestingActor;

	/** Required location tags */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FGameplayTagContainer RequiredTags;

	/** Optional location tags (preferred but not required) */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FGameplayTagContainer OptionalTags;

	/** Maximum distance from actor (0 = unlimited) */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	float MaxDistance = 0.0f;

	/** Custom selection criteria */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TMap<FName, FString> CustomData;
};

/**
 * Schedule event data passed to listeners
 */
USTRUCT(BlueprintType)
struct SCHEDULECORE_API FScheduleEventData
{
	GENERATED_BODY()

	/** Type of event (e.g., Schedule.Event.Started, Schedule.Event.Completed) */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FGameplayTag EventType;

	/** Actor related to this event */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TWeakObjectPtr<AActor> RelatedActor;

	/** Action handle if action-related */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	FScheduleActionHandle ActionHandle;

	/** Additional event data */
	UPROPERTY(BlueprintReadOnly, Category = "Schedule")
	TMap<FName, FString> EventData;
};
