// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RelationshipWidgetBase.generated.h"

class UNPCRelationshipComponent;

/**
 * Base class for all Relationship UI widgets
 * Provides common functionality for displaying relationship data
 */
UCLASS(Abstract, Blueprintable)
class RELATIONSHIPSYSTEMUI_API URelationshipWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Set subject and target actors */
	UFUNCTION(BlueprintCallable, Category = "Relationship Widget")
	virtual void SetActors(AActor* InSubject, AActor* InTarget);

	/** Refresh widget display */
	UFUNCTION(BlueprintCallable, Category = "Relationship Widget")
	virtual void RefreshDisplay();

	/** Get subject actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship Widget")
	AActor* GetSubject() const { return Subject; }

	/** Get target actor */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship Widget")
	AActor* GetTarget() const { return Target; }

	/** Auto-refresh when relationship changes? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Widget")
	bool bAutoRefresh;

	/** Refresh interval (seconds, 0 = every frame) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Widget",
		meta = (EditCondition = "bAutoRefresh", ClampMin = "0.0"))
	float RefreshInterval;

protected:
	/** Subject actor */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship Widget")
	TObjectPtr<AActor> Subject;

	/** Target actor */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship Widget")
	TObjectPtr<AActor> Target;

	/** Cached subject component */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship Widget")
	TObjectPtr<UNPCRelationshipComponent> SubjectComponent;

	/** Cached target component */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship Widget")
	TObjectPtr<UNPCRelationshipComponent> TargetComponent;

	/** Blueprint event for refresh */
	UFUNCTION(BlueprintImplementableEvent, Category = "Relationship Widget", meta = (DisplayName = "On Refresh Display"))
	void BP_OnRefreshDisplay();

	/** Native refresh implementation (override in child classes) */
	virtual void NativeRefreshDisplay();

	/** UUserWidget interface */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	/** Timer for auto-refresh */
	float RefreshTimer;
};
