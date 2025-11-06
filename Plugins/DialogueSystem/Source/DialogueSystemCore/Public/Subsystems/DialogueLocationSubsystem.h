// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "DialogueLocationSubsystem.generated.h"

// Forward declarations
class ADialogueLocationMarker;

/**
 * Dialogue Location Subsystem
 * 
 * Централизованный менеджер для работы с LocationMarkers.
 * Кэширует маркеры для быстрого поиска по тегам.
 * 
 * v1.15: Dialogue Positioning System
 * 
 * Features:
 * - Fast tag-based marker lookup
 * - Marker caching и invalidation
 * - Debug visualization
 * - Query API для дизайнеров
 * 
 * AAA Best Practices:
 * - Subsystem для centralized management
 * - Caching для performance
 * - Blueprint-accessible API
 * - Editor integration
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueLocationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	/** Find location marker by GameplayTag */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Location")
	ADialogueLocationMarker* FindMarkerByTag(FGameplayTag MarkerTag) const;

	/** Find all markers with matching tag (supports partial matching) */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Location")
	TArray<ADialogueLocationMarker*> FindMarkersWithTag(FGameplayTag MarkerTag, bool bExactMatch = true) const;

	/** Get all registered markers */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Location")
	TArray<ADialogueLocationMarker*> GetAllMarkers() const;

	/** Rebuild marker cache (call after spawning/destroying markers) */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Location")
	void RebuildMarkerCache();

	/** Check if marker cache is valid */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Location")
	bool IsMarkerCacheValid() const { return bMarkerCacheValid; }

	/** Debug: Draw all markers in world */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Location|Debug")
	void DebugDrawAllMarkers(float Duration = 5.0f, float Thickness = 2.0f) const;

	/** Debug: Print marker info to log */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Location|Debug")
	void DebugPrintMarkerInfo() const;

protected:
	/** Build marker cache from world actors */
	void BuildMarkerCache();

	/** Clear marker cache */
	void ClearMarkerCache();

private:
	/** Cached markers by tag for fast lookup */
	UPROPERTY(Transient)
	TMap<FGameplayTag, TWeakObjectPtr<ADialogueLocationMarker>> MarkerCacheByTag;

	/** All cached markers */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ADialogueLocationMarker>> AllMarkers;

	/** Is marker cache valid */
	bool bMarkerCacheValid = false;
};
