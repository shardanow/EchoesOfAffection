// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "LocationMarker.generated.h"

/**
 * Simple location marker actor with Gameplay Tags support
 * Used by Schedule System to mark locations in the world
 * 
 * Usage:
 * 1. Place this actor in your level
 * 2. Add tags like "Location.Home", "Location.Shop", "Location.Fields" in LocationTags property
 * 3. Schedule System will find these markers by tags
 */
UCLASS(Blueprintable, ClassGroup=(Schedule), meta=(BlueprintSpawnableComponent))
class ECHOESOFAFFECTION_API ALocationMarker : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	ALocationMarker();

	/** Tags describing this location (e.g., Location.Home, Location.Shop, Location.Fields) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FGameplayTagContainer LocationTags;

	/** Optional display name for this location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FText LocationName;

	/** Optional description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location", meta = (MultiLine = true))
	FText LocationDescription;

	// IGameplayTagAssetInterface implementation
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

protected:
	/** Visual representation in editor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBillboardComponent> SpriteComponent;
};
