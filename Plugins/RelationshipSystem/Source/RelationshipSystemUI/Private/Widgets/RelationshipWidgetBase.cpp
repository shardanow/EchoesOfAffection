// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/RelationshipWidgetBase.h"
#include "Components/NPCRelationshipComponent.h"

void URelationshipWidgetBase::SetActors(AActor* InSubject, AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("RelationshipWidgetBase::SetActors - Called"));
	UE_LOG(LogTemp, Log, TEXT("  InSubject: %s"), *GetNameSafe(InSubject));
	UE_LOG(LogTemp, Log, TEXT("  InTarget: %s"), *GetNameSafe(InTarget));

	Subject = InSubject;
	Target = InTarget;

	// Cache components
	SubjectComponent = Subject ? Subject->FindComponentByClass<UNPCRelationshipComponent>() : nullptr;
	TargetComponent = Target ? Target->FindComponentByClass<UNPCRelationshipComponent>() : nullptr;

	UE_LOG(LogTemp, Log, TEXT("  SubjectComponent: %s"), SubjectComponent ? TEXT("Found") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  TargetComponent: %s"), TargetComponent ? TEXT("Found") : TEXT("NULL"));

	RefreshDisplay();
	
	UE_LOG(LogTemp, Log, TEXT("  RefreshDisplay() called"));
}

void URelationshipWidgetBase::RefreshDisplay()
{
	NativeRefreshDisplay();
	BP_OnRefreshDisplay();
}

void URelationshipWidgetBase::NativeRefreshDisplay()
{
	// Override in child classes
}

void URelationshipWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshTimer = 0.0f;
}

void URelationshipWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
}

void URelationshipWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bAutoRefresh)
	{
		RefreshTimer += InDeltaTime;
		
		if (RefreshInterval <= 0.0f || RefreshTimer >= RefreshInterval)
		{
			RefreshDisplay();
			RefreshTimer = 0.0f;
		}
	}
}
