// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "TimeSystemGameEventEmitter.generated.h"

// Forward declarations
#if __has_include("Core/TimeOfDayTypes.h")
	#include "Core/TimeOfDayTypes.h"
#else
	enum class ETimeOfDayPhase : uint8;
	enum class ETimeOfDaySeason : uint8;
	struct FTimeOfDayTime;
#endif

/**
 * Time System Game Event Emitter
 * 
 * Интегрируется с системой времени и транслирует её в GameEventBus
 * Используется в системе времени и календаря для оповещения
 * 
 * Базовые события:
 * - Time.Event.HourChanged - изменение часа (IntParam = номер часа)
 * - Time.Event.DayChanged - изменение дня (IntParam = номер дня)
 * - Time.Event.TimeOfDay - изменение нормализованного дня (IntParam = час, FloatParam = минута)
 * - Time.Event.Season.Changed - изменение сезона
 * - Weather.Event.Changed - изменение погоды (AdditionalTags = теги погоды)
 * 
 * Использование:
 * 1) Добавьте этот компонент к актору Time Manager'а
 * 2) События будут автоматически транслироваться
 * 3) Другие системы слушают их через GameEventBus
 */
UCLASS(BlueprintType, ClassGroup = (Time), meta = (BlueprintSpawnableComponent))
class UTimeSystemGameEventEmitter : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimeSystemGameEventEmitter();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	/**
	 * Enable/disable event emission
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	void SetEventEmissionEnabled(bool bEnabled);

	/**
	 * Check if event emission is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Time|Events")
	bool IsEventEmissionEnabled() const { return bEmitEvents; }

protected:
	/** Should emit events? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time|Events")
	bool bEmitEvents = true;

	/** Emit events for every hour change? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time|Events")
	bool bEmitHourlyEvents = true;

	/** Emit events for day change? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time|Events")
	bool bEmitDailyEvents = true;

	/** Emit events for weather changes? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time|Events")
	bool bEmitWeatherEvents = true;

	/** Log emitted events? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time|Events|Debug")
	bool bLogEvents = false;

	/**
	 * Called when hour changes
	 * Override this in Blueprint or C++ to customize behavior
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Time|Events")
	void OnHourChanged(int32 NewHour);
	virtual void OnHourChanged_Implementation(int32 NewHour);

	/**
	 * Called when day changes
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Time|Events")
	void OnDayChanged(int32 NewDay);
	virtual void OnDayChanged_Implementation(int32 NewDay);

	/**
	 * Called when weather changes
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Time|Events")
	void OnWeatherChanged(const FGameplayTagContainer& WeatherTags);
	virtual void OnWeatherChanged_Implementation(const FGameplayTagContainer& WeatherTags);

private:
	/** Last known hour (for change detection) */
	int32 LastHour = -1;

	/** Last known day (for change detection) */
	int32 LastDay = -1;

	/** Timer handle for periodic checks */
	FTimerHandle CheckTimerHandle;

	/** Check for time changes (called by timer) */
	void CheckForTimeChanges();

	/** Emit game event helper */
	void EmitTimeEvent(const FGameplayTag& EventTag, int32 Hour, int32 Day) const;

	//~ Begin TimeOfDaySubsystem Integration
	// Note: These methods don't use UFUNCTION because they use forward-declared types
	// They are bound dynamically at runtime when TimeOfDaySubsystem is available

	/** Called when TimeOfDaySubsystem hour changes */
	void OnTimeOfDayHourChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when TimeOfDaySubsystem day changes */
	void OnTimeOfDayDayChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when TimeOfDaySubsystem minute changes */
	void OnTimeOfDayMinuteChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when TimeOfDaySubsystem phase changes */
	void OnTimeOfDayPhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime);

	/** Called when TimeOfDaySubsystem season changes */
	void OnTimeOfDaySeasonChanged(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime);

	//~ End TimeOfDaySubsystem Integration
};
