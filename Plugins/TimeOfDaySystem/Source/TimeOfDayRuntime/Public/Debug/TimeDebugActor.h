// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimeOfDayListenerComponent.h"
#include "TimeDebugActor.generated.h"

class UTextRenderComponent;

/**
 * Debug actor that displays current time information in the world
 * Useful for testing and debugging the Time of Day system
 */
UCLASS()
class TIMEOFDAYRUNTIME_API ATimeDebugActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeDebugActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	/** Text render component for displaying time */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> TimeTextRender;

	/** Text render component for displaying date */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> DateTextRender;

	/** Text render component for displaying phase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> PhaseTextRender;

	/** Time of day listener component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimeOfDayListenerComponent> TimeListener;

	/** Whether to update every frame or only on time events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bUpdateEveryFrame = false;

	/** Text size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float TextSize = 50.0f;

protected:
	void UpdateDisplay();
};

