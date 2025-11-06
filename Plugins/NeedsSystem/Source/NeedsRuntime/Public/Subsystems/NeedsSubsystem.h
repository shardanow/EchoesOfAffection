// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/NeedsInterface.h"
#include "Core/NeedsTypes.h"
#include "Data/NeedsSettings.h"
#include "NeedsSubsystem.generated.h"

// Forward declarations
class UCharacterNeedsComponent;
struct FTimeOfDayTime;

/**
 * Needs Subsystem
 * 
 * Central orchestrator for the needs system.
 * Responsibilities:
 * - Batch update all CharacterNeedsComponents
 * - LOD management (distance-based update frequency)
 * - Time integration (game time or real time)
 * - Global settings management
 * 
 * NO per-actor tick - 
 */
UCLASS()
class NEEDSRUNTIME_API UNeedsSubsystem : public UTickableWorldSubsystem, public INeedsManager
{
	GENERATED_BODY()

public:
	UNeedsSubsystem();

	// ========================================
	// USubsystem Interface
	// ========================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========================================
	// UTickableWorldSubsystem Interface
	// ========================================

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	// ========================================
	// INeedsManager Interface
	// ========================================

	virtual float GetCurrentGameTimeHours_Implementation() const override;
	virtual void RegisterNeedsComponent_Implementation(UActorComponent* Component) override;
	virtual void UnregisterNeedsComponent_Implementation(UActorComponent* Component) override;
	virtual FVector GetCameraLocation_Implementation() const override;

	// ========================================
	// CONFIGURATION
	// ========================================

	/** Initialize with settings asset */
	UFUNCTION(BlueprintCallable, Category = "Needs")
	void InitializeWithSettings(UNeedsSettings* InSettings);

	/** Get current settings */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs")
	UNeedsSettings* GetSettings() const { return Settings; }

	// ========================================
	// TIME INTEGRATION
	// ========================================

	/** Subscribe to Time System events (called automatically) */
	UFUNCTION(BlueprintCallable, Category = "Needs|Time Integration")
	void SubscribeToTimeSystem();

	/** Unsubscribe from Time System events */
	UFUNCTION(BlueprintCallable, Category = "Needs|Time Integration")
	void UnsubscribeFromTimeSystem();

	// ========================================
	// DEBUG
	// ========================================

	/** Enable/disable debug visualization */
	UFUNCTION(BlueprintCallable, Category = "Needs|Debug")
	void SetDebugVisualization(bool bEnable);

	/** Get debug stats */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs|Debug")
	void GetDebugStats(int32& OutRegisteredComponents, int32& OutNearLOD, int32& OutFarLOD, int32& OutParkedLOD) const;

protected:
	// ========================================
	// INTERNAL STATE
	// ========================================

	/** Settings asset */
	UPROPERTY()
	TObjectPtr<UNeedsSettings> Settings;

	/** Registered components */
	UPROPERTY()
	TArray<TObjectPtr<UCharacterNeedsComponent>> RegisteredComponents;

	/** Cached Time System (optional integration) */
	UPROPERTY()
	TObjectPtr<UWorldSubsystem> TimeSystem;

	/** Current game time (hours since start) */
	UPROPERTY()
	double CurrentGameTimeHours = 0.0;

	/** Accumulated time for update frequency control */
	float AccumulatedTime = 0.0f;

	/** Current batch index */
	int32 CurrentBatchIndex = 0;

	/** Debug visualization enabled */
	bool bDebugVisualization = false;

	/** Debug stats */
	int32 DebugNearCount = 0;
	int32 DebugFarCount = 0;
	int32 DebugParkedCount = 0;

	// ========================================
	// INTERNAL METHODS
	// ========================================

	/** Update batch of components */
	void UpdateComponentBatch(float DeltaTime);

	/** Calculate LOD for a component */
	ENeedLODLevel CalculateLOD(UCharacterNeedsComponent* Component) const;

	/** Update time (from Time System or real time) */
	void UpdateGameTime(float DeltaTime);

	/** Draw debug info */
	void DrawDebugInfo();

	/** Find Time System */
	UWorldSubsystem* FindTimeSystem() const;

	/** Time System event handler - called every game hour */
	UFUNCTION()
	void OnTimeSystemHourChanged(const FTimeOfDayTime& CurrentTime);
	
	/** Get delta game hours from Time System settings */
	float GetTimeSystemDeltaHours(float DeltaTime) const;
};
