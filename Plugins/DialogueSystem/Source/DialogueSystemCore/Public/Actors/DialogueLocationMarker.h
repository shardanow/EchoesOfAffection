// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DialogueLocationMarker.generated.h"

/**
 * Location Marker for Dialogue Positioning
 * 
 * Используется для определения позиций участников диалога.
 * Актор размещается в мире и может быть найден по GameplayTag.
 * 
 * Usage:
 * 1. Place in level at desired position
 * 2. Set LocationTag (e.g., "Location.Dialogue.Lianne.Front")
 * 3. Reference in DialogueNode effects: "PositionActor(Lianne, Location.Dialogue.Lianne.Front, MoveTo)"
 * 
 * AAA Best Practice:
 * - Soft coupling через GameplayTags
 * - Визуализация в редакторе (Billboard + Arrow)
 * - Debug draw в PIE для проверки позиций
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Dialogue Location Marker"))
class DIALOGUESYSTEMCORE_API ADialogueLocationMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	ADialogueLocationMarker();

	/** GameplayTag для поиска этого маркера (например, "Location.Dialogue.Lianne.Front") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Marker", meta = (Categories = "Location"))
	FGameplayTag LocationTag;

	/** Описание маркера (для дизайнеров) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Marker", meta = (MultiLine = true))
	FText Description;

	/** Показывать ли debug визуализацию в PIE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Marker|Debug")
	bool bShowDebugInPIE = false;

	/** Цвет debug визуализации */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Marker|Debug")
	FColor DebugColor = FColor::Green;

	/** Радиус debug сферы */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Marker|Debug")
	float DebugRadius = 50.0f;

	//~ Begin AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End AActor Interface

	/** Get target transform for positioning */
	UFUNCTION(BlueprintPure, Category = "Location Marker")
	FTransform GetTargetTransform() const;

	/** Get marker location */
	UFUNCTION(BlueprintPure, Category = "Location Marker")
	FVector GetMarkerLocation() const { return GetActorLocation(); }

	/** Get marker rotation */
	UFUNCTION(BlueprintPure, Category = "Location Marker")
	FRotator GetMarkerRotation() const { return GetActorRotation(); }

protected:
	/** Billboard component для визуализации в редакторе */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBillboardComponent> BillboardComponent;

	/** Arrow component для показа направления */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UArrowComponent> ArrowComponent;

#if WITH_EDITORONLY_DATA
	/** Sprite для billboard (иконка маркера) */
	UPROPERTY()
	TObjectPtr<class UTexture2D> MarkerSprite;
#endif

private:
	/** Draw debug визуализацию */
	void DrawDebugVisualization() const;
};
