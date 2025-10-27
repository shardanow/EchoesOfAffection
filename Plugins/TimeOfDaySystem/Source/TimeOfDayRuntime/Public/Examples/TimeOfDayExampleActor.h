// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/TimeOfDayInterface.h"
#include "TimeOfDayExampleActor.generated.h"

class UTimeOfDayListenerComponent;
class UPointLightComponent;
class UStaticMeshComponent;

/**
 * Example actor demonstrating Time of Day System integration
 * This actor represents a street lamp that turns on/off based on time of day
 */
UCLASS()
class TIMEOFDAYRUNTIME_API ATimeOfDayExampleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeOfDayExampleActor();

protected:
	virtual void BeginPlay() override;

public:
	// ========== Components ==========
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LampMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> LampLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimeOfDayListenerComponent> TimeListener;

	// ========== Configuration ==========

	/** Phases when the lamp should be on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp Settings")
	TArray<ETimeOfDayPhase> ActivePhases;

	/** Light intensity when on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp Settings")
	float LightIntensityOn = 3000.0f;

	/** Light intensity when off */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp Settings")
	float LightIntensityOff = 0.0f;

	/** Transition speed for smooth on/off */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp Settings")
	float TransitionSpeed = 2.0f;

	// ========== Blueprint Events ==========

	/** Called when the lamp turns on */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lamp")
	void OnLampTurnedOn();

	/** Called when the lamp turns off */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lamp")
	void OnLampTurnedOff();

protected:
	UFUNCTION()
	void HandlePhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime);

	void SetLampState(bool bShouldBeOn);

	bool bIsCurrentlyOn;
	float TargetIntensity;
	float CurrentIntensity;
};

