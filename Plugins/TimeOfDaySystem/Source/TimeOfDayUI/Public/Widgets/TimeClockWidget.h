// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/TimeOfDayInterface.h"
#include "Core/TimeOfDayTypes.h"
#include "TimeClockWidget.generated.h"

class UTextBlock;

/**
 * Compact clock widget showing only time
 * Perfect for HUD overlays
 */
UCLASS()
class TIMEOFDAYUI_API UTimeClockWidget : public UUserWidget, public ITimeOfDayListener
{
	GENERATED_BODY()

public:
	UTimeClockWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// ========== ITimeOfDayListener Interface ==========
	virtual void OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnTimeScaleChanged_Implementation(float NewTimeScale) override;

	// ========== Widget Bindings ==========

	/** Text block for displaying the time */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> ClockText;

	// ========== Configuration ==========

	/** Time display format string (e.g., "{Hour}:{Minute}" or "HH:MM") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clock")
	FString TimeFormat = TEXT("{Hour:02}:{Minute:02}");

	/** Text color for daytime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clock")
	FLinearColor DayColor = FLinearColor::White;

	/** Text color for nighttime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clock")
	FLinearColor NightColor = FLinearColor(0.7f, 0.7f, 1.0f);

	/** Whether to change color based on day/night */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clock")
	bool bColorChangesWithDayPhase = true;

protected:
	void UpdateClock();
	FString FormatTime(const FTimeOfDayTime& Time) const;
	void UpdateColor(ETimeOfDayPhase Phase);
};

