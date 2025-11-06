// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/DialogueLocationMarker.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/Texture2D.h"
#include "DrawDebugHelpers.h"

ADialogueLocationMarker::ADialogueLocationMarker()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // Only tick if debug enabled

	// Root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Billboard для визуализации в редакторе
#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
	BillboardComponent->bHiddenInGame = true;
	BillboardComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
#endif

	// Arrow для показа направления
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->ArrowColor = FColor::Green;
	ArrowComponent->bTreatAsASprite = true;
	ArrowComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
#if WITH_EDITORONLY_DATA
	ArrowComponent->bIsScreenSizeScaled = true;
#endif

	// Default settings
	bShowDebugInPIE = false;
	DebugColor = FColor::Green;
	DebugRadius = 50.0f;
}

void ADialogueLocationMarker::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Update arrow color based on tag
	if (ArrowComponent)
	{
		if (LocationTag.IsValid())
		{
			ArrowComponent->ArrowColor = FColor::Green;
		}
		else
		{
			ArrowComponent->ArrowColor = FColor::Red; // No tag set
		}
	}
}

void ADialogueLocationMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowDebugInPIE && GetWorld() && GetWorld()->IsGameWorld())
	{
		DrawDebugVisualization();
	}
}

#if WITH_EDITOR
void ADialogueLocationMarker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ADialogueLocationMarker, bShowDebugInPIE))
	{
		// Enable/disable ticking based on debug setting
		SetActorTickEnabled(bShowDebugInPIE);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ADialogueLocationMarker, LocationTag))
	{
		// Update arrow color when tag changes
		OnConstruction(GetActorTransform());
	}
}
#endif

FTransform ADialogueLocationMarker::GetTargetTransform() const
{
	return GetActorTransform();
}

void ADialogueLocationMarker::DrawDebugVisualization() const
{
	if (!GetWorld())
	{
		return;
	}

	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	const FVector ForwardVector = Rotation.Vector();

	// Draw sphere at marker location
	DrawDebugSphere(GetWorld(), Location, DebugRadius, 16, DebugColor, false, -1.0f, 0, 2.0f);

	// Draw arrow showing direction
	DrawDebugDirectionalArrow(
		GetWorld(),
		Location,
		Location + ForwardVector * 100.0f,
		50.0f,
		DebugColor,
		false,
		-1.0f,
		0,
		3.0f
	);

	// Draw tag name if valid
	if (LocationTag.IsValid())
	{
		DrawDebugString(
			GetWorld(),
			Location + FVector(0, 0, DebugRadius + 20.0f),
			LocationTag.ToString(),
			nullptr,
			DebugColor,
			0.0f,
			true
		);
	}
	else
	{
		DrawDebugString(
			GetWorld(),
			Location + FVector(0, 0, DebugRadius + 20.0f),
			TEXT("NO TAG SET!"),
			nullptr,
			FColor::Red,
			0.0f,
			true
		);
	}
}
