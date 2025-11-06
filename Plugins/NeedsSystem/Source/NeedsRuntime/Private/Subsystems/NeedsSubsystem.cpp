// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/NeedsSubsystem.h"
#include "Components/CharacterNeedsComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

#if WITH_TIME_SYSTEM
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Core/TimeOfDayTypes.h"
#endif

UNeedsSubsystem::UNeedsSubsystem()
{
}

void UNeedsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Try to load default settings
	const FString SettingsPath = TEXT("/NeedsSystem/DA_NeedsSettings.DA_NeedsSettings");
	Settings = Cast<UNeedsSettings>(StaticLoadObject(UNeedsSettings::StaticClass(), nullptr, *SettingsPath));

	if (Settings)
	{
		UE_LOG(LogTemp, Log, TEXT("? NeedsSubsystem: Loaded settings from %s"), *SettingsPath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("?? NeedsSubsystem: Settings not found at %s, using defaults"), *SettingsPath);
		
		// Create default settings
		Settings = NewObject<UNeedsSettings>(this);
	}

	// Try to subscribe to Time System
	SubscribeToTimeSystem();

	UE_LOG(LogTemp, Log, TEXT("NeedsSubsystem initialized"));
}

void UNeedsSubsystem::Deinitialize()
{
	UnsubscribeFromTimeSystem();
	RegisteredComponents.Empty();

	Super::Deinitialize();
}

void UNeedsSubsystem::Tick(float DeltaTime)
{
	if (!Settings)
	{
		return;
	}

	// Update game time
	UpdateGameTime(DeltaTime);

	// Update components in batches
	if (Settings->UpdateFrequency > 0.0f)
	{
		AccumulatedTime += DeltaTime;
		const float UpdateInterval = 1.0f / Settings->UpdateFrequency;

		if (AccumulatedTime >= UpdateInterval)
		{
			UpdateComponentBatch(AccumulatedTime);
			AccumulatedTime = 0.0f;
		}
	}
	else
	{
		// Every frame
		UpdateComponentBatch(DeltaTime);
	}

	// Debug visualization
	if (bDebugVisualization || Settings->bEnableDebugVisualization)
	{
		DrawDebugInfo();
	}
}

TStatId UNeedsSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNeedsSubsystem, STATGROUP_Tickables);
}

bool UNeedsSubsystem::IsTickable() const
{
	return !IsTemplate() && Settings != nullptr;
}

// ========================================
// INeedsManager Interface
// ========================================

float UNeedsSubsystem::GetCurrentGameTimeHours_Implementation() const
{
	return static_cast<float>(CurrentGameTimeHours);
}

void UNeedsSubsystem::RegisterNeedsComponent_Implementation(UActorComponent* Component)
{
	UCharacterNeedsComponent* NeedsComp = Cast<UCharacterNeedsComponent>(Component);
	if (NeedsComp && !RegisteredComponents.Contains(NeedsComp))
	{
		RegisteredComponents.Add(NeedsComp);
		UE_LOG(LogTemp, Log, TEXT("NeedsSubsystem: Registered component from %s (Total: %d)"),
			*NeedsComp->GetOwner()->GetName(), RegisteredComponents.Num());
	}
}

void UNeedsSubsystem::UnregisterNeedsComponent_Implementation(UActorComponent* Component)
{
	UCharacterNeedsComponent* NeedsComp = Cast<UCharacterNeedsComponent>(Component);
	if (NeedsComp)
	{
		RegisteredComponents.Remove(NeedsComp);
		UE_LOG(LogTemp, Log, TEXT("NeedsSubsystem: Unregistered component from %s (Total: %d)"),
			*NeedsComp->GetOwner()->GetName(), RegisteredComponents.Num());
	}
}

FVector UNeedsSubsystem::GetCameraLocation_Implementation() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return FVector::ZeroVector;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return FVector::ZeroVector;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	return CameraLocation;
}

// ========================================
// CONFIGURATION
// ========================================

void UNeedsSubsystem::InitializeWithSettings(UNeedsSettings* InSettings)
{
	if (!InSettings)
	{
		return;
	}

	Settings = InSettings;
	UE_LOG(LogTemp, Log, TEXT("NeedsSubsystem: Initialized with custom settings"));
}

// ========================================
// TIME INTEGRATION
// ========================================

void UNeedsSubsystem::SubscribeToTimeSystem()
{
#if WITH_TIME_SYSTEM
	// Try to find TimeOfDaySubsystem
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>();
	TimeSystem = TimeSubsystem;
	
	if (TimeSubsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("? NeedsSubsystem: Found Time System, subscribing to OnHourChanged event"));
		
		// Subscribe to OnHourChanged delegate
		TimeSubsystem->OnHourChanged.AddDynamic(this, &UNeedsSubsystem::OnTimeSystemHourChanged);
		
		UE_LOG(LogTemp, Log, TEXT("? NeedsSubsystem: Successfully subscribed to Time System events"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("?? NeedsSubsystem: Time System not found, using real time"));
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("?? NeedsSubsystem: Compiled without Time System support, using real time"));
#endif
}

void UNeedsSubsystem::UnsubscribeFromTimeSystem()
{
#if WITH_TIME_SYSTEM
	if (TimeSystem)
	{
		UTimeOfDaySubsystem* TimeSubsystem = Cast<UTimeOfDaySubsystem>(TimeSystem);
		if (TimeSubsystem)
		{
			// Unsubscribe from events
			TimeSubsystem->OnHourChanged.RemoveDynamic(this, &UNeedsSubsystem::OnTimeSystemHourChanged);
			UE_LOG(LogTemp, Log, TEXT("NeedsSubsystem: Unsubscribed from Time System"));
		}
		TimeSystem = nullptr;
	}
#endif
}

void UNeedsSubsystem::OnTimeSystemHourChanged(const FTimeOfDayTime& CurrentTime)
{
	// Called by Time System every game hour
	// Update all needs by 1 game hour
	
	const float DeltaGameHours = 1.0f; // 1 game hour passed
	
	// Update all components
	for (UCharacterNeedsComponent* Component : RegisteredComponents)
	{
		if (!Component || !Component->GetOwner())
		{
			continue;
		}

		// Calculate LOD
		const ENeedLODLevel LOD = CalculateLOD(Component);

		// Get LOD multiplier
		const float LODMultiplier = Component->GetLODConfig().GetUpdateMultiplier(LOD);
		const float AdjustedDelta = DeltaGameHours * LODMultiplier;

		// Update component
		Component->UpdateNeeds(AdjustedDelta, LOD);
	}
	
	// Increment our game time counter
	CurrentGameTimeHours += 1.0;
	
#if WITH_TIME_SYSTEM
	UE_LOG(LogTemp, Verbose, TEXT("NeedsSubsystem: Updated needs for 1 game hour (Total: %.2f hours, Time: %s)"), 
		CurrentGameTimeHours, *CurrentTime.ToString());
#else
	UE_LOG(LogTemp, Verbose, TEXT("NeedsSubsystem: Updated needs for 1 game hour (Total: %.2f hours)"), 
		CurrentGameTimeHours);
#endif
}

float UNeedsSubsystem::GetTimeSystemDeltaHours(float DeltaTime) const
{
#if WITH_TIME_SYSTEM
	if (!TimeSystem)
	{
		return 0.0f;
	}

	// Cast to TimeOfDaySubsystem to access settings
	UTimeOfDaySubsystem* TimeSubsystem = Cast<UTimeOfDaySubsystem>(TimeSystem);
	if (!TimeSubsystem)
	{
		return 0.0f;
	}

	// Get settings from Time System
	const UTimeOfDaySettings* TimeSettings = TimeSubsystem->GetSettings();
	if (!TimeSettings)
	{
		// Fallback to default (1200s per day)
		UE_LOG(LogTemp, Warning, TEXT("NeedsSubsystem: Time System has no settings, using default (1200s per day)"));
		
		const float GameSecondsPerRealSecond = 86400.0f / 1200.0f; // 72
		const float DeltaGameSeconds = DeltaTime * GameSecondsPerRealSecond;
		return DeltaGameSeconds / 3600.0f;
	}

	// Read actual time conversion settings from Time System
	const float RealSecondsPerGameDay = TimeSettings->TimeConversion.RealSecondsPerGameDay;
	const float TimeScale = TimeSettings->TimeConversion.TimeScale;
	
	// Calculate game seconds per real second
	// Example: 1200s per day, TimeScale 1.0 ? 86400 / 1200 * 1.0 = 72 game seconds per real second
	const float GameSecondsPerRealSecond = (86400.0f / RealSecondsPerGameDay) * TimeScale;
	
	// Convert to game hours
	const float DeltaGameSeconds = DeltaTime * GameSecondsPerRealSecond;
	const float DeltaGameHours = DeltaGameSeconds / 3600.0f;
	
	return DeltaGameHours;
#else
	return 0.0f;
#endif
}

// ========================================
// DEBUG
// ========================================

void UNeedsSubsystem::SetDebugVisualization(bool bEnable)
{
	bDebugVisualization = bEnable;
}

void UNeedsSubsystem::GetDebugStats(int32& OutRegisteredComponents, int32& OutNearLOD, int32& OutFarLOD, int32& OutParkedLOD) const
{
	OutRegisteredComponents = RegisteredComponents.Num();
	OutNearLOD = DebugNearCount;
	OutFarLOD = DebugFarCount;
	OutParkedLOD = DebugParkedCount;
}

// ========================================
// INTERNAL METHODS
// ========================================

void UNeedsSubsystem::UpdateComponentBatch(float DeltaTime)
{
	if (RegisteredComponents.Num() == 0 || !Settings)
	{
		return;
	}

#if WITH_TIME_SYSTEM
	// If using Time System and it's available, updates are event-driven via OnHourChanged
	// We don't need to update here - events will trigger updates
	if (Settings->bUseGameTime && TimeSystem)
	{
		// Time System handles updates via OnHourChanged event
		// No need to update here
		return;
	}
#endif

	// Real time mode or Time System not available - manual updates
	
	// Reset debug counters
	DebugNearCount = 0;
	DebugFarCount = 0;
	DebugParkedCount = 0;

	const int32 BatchSize = Settings->BatchSize;
	const int32 TotalComponents = RegisteredComponents.Num();
	const int32 ComponentsToUpdate = FMath::Min(BatchSize, TotalComponents);

	// Calculate delta game hours for real time mode
	// RealTimeScale is "how many game hours pass per real hour"
	// Example: RealTimeScale = 1.0 means 1 game hour = 1 real hour
	// So: DeltaTime (real seconds) * (RealTimeScale game hours / 3600 real seconds)
	const float DeltaGameHours = DeltaTime * (Settings->RealTimeScale / 3600.0f);

	// Update batch
	FVector CameraLocation = FVector::ZeroVector;
	if (this->Implements<UNeedsManager>())
	{
		CameraLocation = INeedsManager::Execute_GetCameraLocation(this);
	}

	for (int32 i = 0; i < ComponentsToUpdate; ++i)
	{
		const int32 ComponentIndex = (CurrentBatchIndex + i) % TotalComponents;
		UCharacterNeedsComponent* Component = RegisteredComponents[ComponentIndex];

		if (!Component || !Component->GetOwner())
		{
			continue;
		}

		// Calculate LOD
		const ENeedLODLevel LOD = CalculateLOD(Component);

		// Update debug stats
		switch (LOD)
		{
			case ENeedLODLevel::Near: DebugNearCount++; break;
			case ENeedLODLevel::Far: DebugFarCount++; break;
			case ENeedLODLevel::Parked: DebugParkedCount++; break;
		}

		// Get LOD multiplier
		const float LODMultiplier = Component->GetLODConfig().GetUpdateMultiplier(LOD);
		const float AdjustedDelta = DeltaGameHours * LODMultiplier;

		// Update component
		Component->UpdateNeeds(AdjustedDelta, LOD);
	}

	// Advance batch index
	CurrentBatchIndex = (CurrentBatchIndex + ComponentsToUpdate) % TotalComponents;
	
	// Update our game time counter
	CurrentGameTimeHours += DeltaGameHours;
}

ENeedLODLevel UNeedsSubsystem::CalculateLOD(UCharacterNeedsComponent* Component) const
{
	if (!Component || !Component->GetOwner())
	{
		return ENeedLODLevel::Parked;
	}

	FVector CameraLocation = FVector::ZeroVector;
	if (this->Implements<UNeedsManager>())
	{
		CameraLocation = INeedsManager::Execute_GetCameraLocation(this);
	}
	
	const FVector ComponentLocation = Component->GetOwner()->GetActorLocation();
	const float Distance = FVector::Dist(CameraLocation, ComponentLocation);

	return Component->GetLODConfig().GetLODForDistance(Distance);
}

void UNeedsSubsystem::UpdateGameTime(float DeltaTime)
{
	if (!Settings->bUseGameTime || !TimeSystem)
	{

		CurrentGameTimeHours += DeltaTime * Settings->RealTimeScale;
	}

}

void UNeedsSubsystem::DrawDebugInfo()
{
	if (!Settings)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector CameraLocation = FVector::ZeroVector;
	if (this->Implements<UNeedsManager>())
	{
		CameraLocation = INeedsManager::Execute_GetCameraLocation(this);
	}

	// Draw component info
	for (UCharacterNeedsComponent* Component : RegisteredComponents)
	{
		if (!Component || !Component->GetOwner())
		{
			continue;
		}

		const FVector ComponentLocation = Component->GetOwner()->GetActorLocation();
		const float Distance = FVector::Dist(CameraLocation, ComponentLocation);
		const ENeedLODLevel LOD = Component->GetCurrentLOD();

		// LOD color
		FColor LODColor = FColor::Green;
		switch (LOD)
		{
			case ENeedLODLevel::Near: LODColor = FColor::Green; break;
			case ENeedLODLevel::Far: LODColor = FColor::Yellow; break;
			case ENeedLODLevel::Parked: LODColor = FColor::Red; break;
		}

		// Draw sphere
		DrawDebugSphere(World, ComponentLocation, 100.0f, 8, LODColor, false, -1.0f, 0, 2.0f);

		// Draw text
		float CurrentMood = 0.0f;
		if (Component->Implements<UNeedsAccessor>())
		{
			CurrentMood = INeedsAccessor::Execute_GetCurrentMood(Component);
		}
		
		const FString DebugText = FString::Printf(TEXT("%s\nDist: %.0f\nLOD: %s\nMood: %.1f"),
			*Component->GetOwner()->GetName(),
			Distance,
			*StaticEnum<ENeedLODLevel>()->GetDisplayNameTextByValue((int64)LOD).ToString(),
			CurrentMood);

		DrawDebugString(World, ComponentLocation + FVector(0, 0, 150), DebugText, nullptr, 
			FColor::White, 0.0f, true, Settings->DebugTextSize);

		// Draw needs
		FVector TextOffset(0, 0, 120);
		const TArray<FGameplayTag> NeedTags = Component->GetAllNeedTags();
		
		for (const FGameplayTag& NeedTag : NeedTags)
		{
			float Value = 0.0f;
			ENeedThresholdLevel Threshold = ENeedThresholdLevel::Normal;
			
			if (Component->Implements<UNeedsAccessor>())
			{
				Value = INeedsAccessor::Execute_GetNeedValue(Component, NeedTag);
				Threshold = INeedsAccessor::Execute_GetNeedThreshold(Component, NeedTag);
			}

			FColor ValueColor = FColor::Green;
			if (Threshold == ENeedThresholdLevel::Warning) ValueColor = FColor::Yellow;
			else if (Threshold == ENeedThresholdLevel::Critical) ValueColor = FColor::Red;

			const FString NeedText = FString::Printf(TEXT("%s: %.0f"), *NeedTag.GetTagName().ToString(), Value);
			DrawDebugString(World, ComponentLocation + TextOffset, NeedText, nullptr, 
				ValueColor, 0.0f, true, Settings->DebugTextSize * 0.8f);

			TextOffset.Z -= 15.0f * Settings->DebugTextSize;
		}
	}

	// Draw global stats
	const FString StatsText = FString::Printf(TEXT("Needs System Stats:\nComponents: %d\nNear: %d | Far: %d | Parked: %d\nGame Time: %.2f hours"),
		RegisteredComponents.Num(),
		DebugNearCount,
		DebugFarCount,
		DebugParkedCount,
		CurrentGameTimeHours);

	DrawDebugString(World, CameraLocation + FVector(500, 0, 0), StatsText, nullptr, 
		FColor::Cyan, 0.0f, true, Settings->DebugTextSize);
}

UWorldSubsystem* UNeedsSubsystem::FindTimeSystem() const
{
#if WITH_TIME_SYSTEM
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return World->GetSubsystem<UTimeOfDaySubsystem>();
#else
	return nullptr;
#endif
}
