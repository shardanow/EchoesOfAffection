// Copyright Epic Games, Inc. All Rights Reserved.

#include "LocationMarker.h"
#include "Components/BillboardComponent.h"

ALocationMarker::ALocationMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create sprite component for editor visualization
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		RootComponent = SpriteComponent;
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->bIsScreenSizeScaled = true;
	}
#endif
}

void ALocationMarker::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = LocationTags;
}
