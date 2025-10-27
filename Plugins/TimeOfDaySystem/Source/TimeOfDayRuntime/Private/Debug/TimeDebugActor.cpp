// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/TimeDebugActor.h"
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Components/TextRenderComponent.h"

ATimeDebugActor::ATimeDebugActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root scene component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Create time text render
	TimeTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TimeText"));
	TimeTextRender->SetupAttachment(Root);
	TimeTextRender->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TimeTextRender->SetHorizontalAlignment(EHTA_Center);
	TimeTextRender->SetVerticalAlignment(EVRTA_TextCenter);
	TimeTextRender->SetWorldSize(50.0f);
	TimeTextRender->SetTextRenderColor(FColor::White);

	// Create date text render
	DateTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DateText"));
	DateTextRender->SetupAttachment(Root);
	DateTextRender->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	DateTextRender->SetHorizontalAlignment(EHTA_Center);
	DateTextRender->SetVerticalAlignment(EVRTA_TextCenter);
	DateTextRender->SetWorldSize(30.0f);
	DateTextRender->SetTextRenderColor(FColor::Yellow);

	// Create phase text render
	PhaseTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PhaseText"));
	PhaseTextRender->SetupAttachment(Root);
	PhaseTextRender->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	PhaseTextRender->SetHorizontalAlignment(EHTA_Center);
	PhaseTextRender->SetVerticalAlignment(EVRTA_TextCenter);
	PhaseTextRender->SetWorldSize(30.0f);
	PhaseTextRender->SetTextRenderColor(FColor::Cyan);

	// Create time listener component
	TimeListener = CreateDefaultSubobject<UTimeOfDayListenerComponent>(TEXT("TimeListener"));
	TimeListener->bReceiveMinuteEvents = true;
	TimeListener->bReceivePhaseEvents = true;
}

void ATimeDebugActor::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateDisplay();
}

void ATimeDebugActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateEveryFrame)
	{
		UpdateDisplay();
	}
}

void ATimeDebugActor::UpdateDisplay()
{
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			// Update time text
			const FString TimeString = TimeSystem->GetFormattedTime(true);
			TimeTextRender->SetText(FText::FromString(TimeString));

			// Update date text
			const FString DateString = TimeSystem->GetFormattedDate();
			DateTextRender->SetText(FText::FromString(DateString));

			// Update phase text using BP_ method
			const ETimeOfDayPhase CurrentPhase = TimeSystem->BP_GetCurrentPhase();  // ? Using BP_ version
			const FString PhaseString = UEnum::GetDisplayValueAsText(CurrentPhase).ToString();
			PhaseTextRender->SetText(FText::FromString(PhaseString));

			// Color phase text based on time of day
			if (TimeSystem->IsDaytime())
			{
				PhaseTextRender->SetTextRenderColor(FColor::Orange);
			}
			else
			{
				PhaseTextRender->SetTextRenderColor(FColor::Blue);
			}
		}
	}
}

