// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "TimeSystemGameEventEmitter.generated.h"

/**
 * Time System Game Event Emitter
 * 
 * Автоматически эмитирует события времени в GameEventBus
 * Подключается к системе времени и транслирует все изменения
 * 
 * События времени:
 * - Time.Event.HourChanged - изменился час (IntParam = новый час)
 * - Time.Event.DayChanged - изменился день (IntParam = новый день)
 * - Time.Event.TimeOfDay - достигнуто определённое время (IntParam = час, FloatParam = минуты)
 * - Time.Event.Season.Changed - изменился сезон
 * - Weather.Event.Changed - изменилась погода (AdditionalTags = теги погоды)
 * 
 * Использование:
 * 1) Добавьте этот компонент к вашему Time Manager'у
 * 2) События будут автоматически эмитироваться
 * 3) Квесты и другие системы подхватят их
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
};
