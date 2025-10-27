// Copyright Epic Games, Inc. All Rights Reserved.

#include "Examples/TimeOfDayExampleActor.h"
#include "Components/TimeOfDayListenerComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystems/TimeOfDaySubsystem.h"

ATimeOfDayExampleActor::ATimeOfDayExampleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	// Create lamp mesh
	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LampMesh"));
	LampMesh->SetupAttachment(RootComp);

	// Create lamp light
	LampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LampLight"));
	LampLight->SetupAttachment(LampMesh);
	LampLight->SetIntensity(0.0f);
	LampLight->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f)); // Warm white
	LampLight->SetAttenuationRadius(1000.0f);

	// Create time listener
	TimeListener = CreateDefaultSubobject<UTimeOfDayListenerComponent>(TEXT("TimeListener"));
	TimeListener->bReceivePhaseEvents = true;
	TimeListener->bReceiveHourEvents = false;
	TimeListener->bReceiveMinuteEvents = false;

	// Default active phases (night and evening)
	ActivePhases.Add(ETimeOfDayPhase::Night);
	ActivePhases.Add(ETimeOfDayPhase::Evening);

	bIsCurrentlyOn = false;
	TargetIntensity = 0.0f;
	CurrentIntensity = 0.0f;
}

void ATimeOfDayExampleActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind to subsystem delegate
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSystem->OnDayPhaseChanged.AddDynamic(this, &ATimeOfDayExampleActor::HandlePhaseChanged);

			// Set initial state based on current phase using BP_ method
			HandlePhaseChanged(TimeSystem->BP_GetCurrentPhase(), TimeSystem->BP_GetCurrentTime());  // ? Using BP_ versions
		}
	}
}

void ATimeOfDayExampleActor::HandlePhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	// Check if lamp should be on in this phase
	const bool bShouldBeOn = ActivePhases.Contains(NewPhase);
	SetLampState(bShouldBeOn);

	UE_LOG(LogTemp, Log, TEXT("Street Lamp %s: Phase changed to %s, Lamp is %s"),
		*GetName(),
		*UEnum::GetDisplayValueAsText(NewPhase).ToString(),
		bShouldBeOn ? TEXT("ON") : TEXT("OFF"));
}

void ATimeOfDayExampleActor::SetLampState(bool bShouldBeOn)
{
	if (bIsCurrentlyOn == bShouldBeOn)
	{
		return; // Already in correct state
	}

	bIsCurrentlyOn = bShouldBeOn;
	TargetIntensity = bShouldBeOn ? LightIntensityOn : LightIntensityOff;

	// Immediate update (for smooth transitions, use Tick with interpolation)
	LampLight->SetIntensity(TargetIntensity);
	LampLight->SetVisibility(bShouldBeOn);

	// Call blueprint events
	if (bShouldBeOn)
	{
		OnLampTurnedOn();
	}
	else
	{
		OnLampTurnedOff();
	}
}

