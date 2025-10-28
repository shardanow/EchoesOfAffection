# Road Navigation - C++ Examples

## Создание NavArea классов в C++

### NavArea_Road.h

```cpp
// Copyright Your Company. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Road.generated.h"

/**
 * Navigation area for roads - low cost, preferred by AI
 */
UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Road : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Road();
};
```

### NavArea_Road.cpp

```cpp
#include "NavArea_Road.h"

UNavArea_Road::UNavArea_Road()
{
	// Low cost - AI prefers roads
	DefaultCost = 1.0f;
	FixedAreaEnteringCost = 0.0f;
	
	// Yellow color for debugging (visible with "P" key)
	DrawColor = FColor(255, 255, 0); // Yellow
	
	bIsMetaArea = false;
}
```

---

### NavArea_Grass.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Grass.generated.h"

/**
 * Navigation area for grass/off-road - high cost, AI avoids
 */
UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Grass : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Grass();
};
```

### NavArea_Grass.cpp

```cpp
#include "NavArea_Grass.h"

UNavArea_Grass::UNavArea_Grass()
{
	// High cost - AI avoids grass
	DefaultCost = 10.0f;
	FixedAreaEnteringCost = 0.0f;
	
	// Green color for debugging
	DrawColor = FColor(0, 255, 0); // Green
	
	bIsMetaArea = false;
}
```

---

### NavArea_Water.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Water.generated.h"

/**
 * Navigation area for water - excluded, AI cannot pass
 */
UCLASS()
class ECHOESOFAFFECTION_API UNavArea_Water : public UNavArea
{
	GENERATED_BODY()
	
public:
	UNavArea_Water();
};
```

### NavArea_Water.cpp

```cpp
#include "NavArea_Water.h"
#include "NavAreas/NavArea_Null.h"

UNavArea_Water::UNavArea_Water()
{
	// Very high cost (almost forbidden)
	DefaultCost = 1000.0f;
	FixedAreaEnteringCost = 0.0f;
	
	// Blue color for debugging
	DrawColor = FColor(0, 0, 255); // Blue
	
	// Or use meta area to completely exclude:
	// DefaultCost = BIG_NUMBER;
	// or inherit from UNavArea_Null
	
	bIsMetaArea = false;
}
```

---

## Программное создание Navigation Query Filter

### RoadNavigationFilter.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "RoadNavigationFilter.generated.h"

/**
 * Navigation filter for road-only movement
 */
UCLASS()
class ECHOESOFAFFECTION_API URoadNavigationFilter : public UNavigationQueryFilter
{
	GENERATED_BODY()
	
public:
	URoadNavigationFilter();
	
	// Helper: Create filter at runtime
	static URoadNavigationFilter* CreateRoadFilter(UObject* Outer);
};
```

### RoadNavigationFilter.cpp

```cpp
#include "RoadNavigationFilter.h"
#include "NavArea_Road.h"
#include "NavArea_Grass.h"
#include "NavAreas/NavArea_Default.h"

URoadNavigationFilter::URoadNavigationFilter()
{
	// Road area - cheap
	{
		FNavigationFilterArea RoadArea;
		RoadArea.AreaClass = UNavArea_Road::StaticClass();
		RoadArea.bOverrideTravelCost = true;
		RoadArea.TravelCostOverride = 0.5f; // Very cheap
		Areas.Add(RoadArea);
	}
	
	// Grass area - expensive
	{
		FNavigationFilterArea GrassArea;
		GrassArea.AreaClass = UNavArea_Grass::StaticClass();
		GrassArea.bOverrideTravelCost = true;
		GrassArea.TravelCostOverride = 100.0f; // Very expensive
		Areas.Add(GrassArea);
	}
	
	// Default area - expensive
	{
		FNavigationFilterArea DefaultArea;
		DefaultArea.AreaClass = UNavArea_Default::StaticClass();
		DefaultArea.bOverrideTravelCost = true;
		DefaultArea.TravelCostOverride = 50.0f; // Expensive
		Areas.Add(DefaultArea);
	}
}

URoadNavigationFilter* URoadNavigationFilter::CreateRoadFilter(UObject* Outer)
{
	URoadNavigationFilter* Filter = NewObject<URoadNavigationFilter>(Outer);
	return Filter;
}
```

---

## Применение фильтра в коде

### Пример 1: В Schedule Component

```cpp
// MyScheduleComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ScheduleComponent.h"
#include "MyScheduleComponent.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API UMyScheduleComponent : public UScheduleComponent
{
	GENERATED_BODY()
	
public:
	/** Filter class for road navigation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schedule|Navigation")
	TSubclassOf<UNavigationQueryFilter> RoadFilterClass;
	
	// Override to apply filter to move actions
	virtual void InitializeComponent() override;
};
```

```cpp
// MyScheduleComponent.cpp
#include "MyScheduleComponent.h"
#include "RoadNavigationFilter.h"
#include "Actions/ScheduleAction_MoveToLocation.h"

void UMyScheduleComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	// Set default filter class if not set
	if (!RoadFilterClass)
	{
		RoadFilterClass = URoadNavigationFilter::StaticClass();
	}
	
	// Apply filter to all MoveToLocation actions in schedule
	if (ScheduleData)
	{
		for (UScheduleEntryData* Entry : ScheduleData->Entries)
		{
			if (UScheduleAction_MoveToLocation* MoveAction = Cast<UScheduleAction_MoveToLocation>(Entry->Action))
			{
				if (!MoveAction->FilterClass)
				{
					MoveAction->FilterClass = RoadFilterClass;
					UE_LOG(LogTemp, Log, TEXT("Applied road filter to action: %s"), *Entry->GetDisplayText().ToString());
				}
			}
		}
	}
}
```

---

### Пример 2: Динамическая смена фильтра

```cpp
// NPCController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCController.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API ANPCController : public AAIController
{
	GENERATED_BODY()
	
public:
	/** Switch to road-only navigation */
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void EnableRoadOnlyMode();
	
	/** Allow off-road navigation */
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void EnableOffRoadMode();
	
private:
	UPROPERTY()
	TSubclassOf<UNavigationQueryFilter> RoadFilter;
	
	UPROPERTY()
	TSubclassOf<UNavigationQueryFilter> OffRoadFilter;
	
	void UpdateScheduleFilter(TSubclassOf<UNavigationQueryFilter> NewFilter);
};
```

```cpp
// NPCController.cpp
#include "NPCController.h"
#include "Components/ScheduleComponent.h"
#include "RoadNavigationFilter.h"
#include "Actions/ScheduleAction_MoveToLocation.h"

void ANPCController::EnableRoadOnlyMode()
{
	UE_LOG(LogTemp, Log, TEXT("Enabling road-only navigation mode"));
	
	if (!RoadFilter)
	{
		RoadFilter = URoadNavigationFilter::StaticClass();
	}
	
	UpdateScheduleFilter(RoadFilter);
}

void ANPCController::EnableOffRoadMode()
{
	UE_LOG(LogTemp, Log, TEXT("Enabling off-road navigation mode"));
	
	// Use nullptr to allow default navigation (or create custom off-road filter)
	UpdateScheduleFilter(nullptr);
}

void ANPCController::UpdateScheduleFilter(TSubclassOf<UNavigationQueryFilter> NewFilter)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
	
	UScheduleComponent* ScheduleComp = ControlledPawn->FindComponentByClass<UScheduleComponent>();
	if (!ScheduleComp || !ScheduleComp->ScheduleData) return;
	
	// Update all move actions
	for (UScheduleEntryData* Entry : ScheduleComp->ScheduleData->Entries)
	{
		if (UScheduleAction_MoveToLocation* MoveAction = Cast<UScheduleAction_MoveToLocation>(Entry->Action))
		{
			MoveAction->FilterClass = NewFilter;
			UE_LOG(LogTemp, Log, TEXT("Updated filter for action: %s"), *Entry->GetDisplayText().ToString());
		}
	}
	
	// Refresh schedule to apply changes
	ScheduleComp->RefreshSchedule();
}
```

---

### Пример 3: Множественные фильтры для разных ситуаций

```cpp
// NavigationFilterManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NavigationFilterManager.generated.h"

UENUM(BlueprintType)
enum class ENavigationMode : uint8
{
	RoadOnly UMETA(DisplayName = "Road Only"),
	RoadAndSidewalk UMETA(DisplayName = "Road and Sidewalk"),
	OffRoad UMETA(DisplayName = "Off-Road"),
	AvoidRoads UMETA(DisplayName = "Avoid Roads (Monster)"),
	NoRestriction UMETA(DisplayName = "No Restriction")
};

/**
 * Manager for navigation filters - singleton
 */
UCLASS(BlueprintType)
class ECHOESOFAFFECTION_API UNavigationFilterManager : public UObject
{
	GENERATED_BODY()
	
public:
	/** Get filter for specific navigation mode */
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	static TSubclassOf<UNavigationQueryFilter> GetFilterForMode(ENavigationMode Mode);
	
private:
	static TMap<ENavigationMode, TSubclassOf<UNavigationQueryFilter>> FilterCache;
};
```

```cpp
// NavigationFilterManager.cpp
#include "NavigationFilterManager.h"
#include "RoadNavigationFilter.h"
// Include other filter classes...

TMap<ENavigationMode, TSubclassOf<UNavigationQueryFilter>> UNavigationFilterManager::FilterCache;

TSubclassOf<UNavigationQueryFilter> UNavigationFilterManager::GetFilterForMode(ENavigationMode Mode)
{
	// Initialize cache if empty
	if (FilterCache.Num() == 0)
	{
		// TODO: Load from config or create filters
		FilterCache.Add(ENavigationMode::RoadOnly, URoadNavigationFilter::StaticClass());
		// FilterCache.Add(ENavigationMode::OffRoad, UOffRoadFilter::StaticClass());
		// etc...
	}
	
	TSubclassOf<UNavigationQueryFilter>* Filter = FilterCache.Find(Mode);
	return Filter ? *Filter : nullptr;
}
```

**Использование:**

```cpp
void AMyNPC::SwitchNavigationMode(ENavigationMode NewMode)
{
	TSubclassOf<UNavigationQueryFilter> Filter = UNavigationFilterManager::GetFilterForMode(NewMode);
	
	// Apply to schedule component
	if (UScheduleComponent* ScheduleComp = FindComponentByClass<UScheduleComponent>())
	{
		// ... apply filter to move actions
	}
}
```

---

## Программное создание Nav Modifier Volumes

### Пример: Динамическое создание дорог

```cpp
// RoadBuilder.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadBuilder.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API ARoadBuilder : public AActor
{
	GENERATED_BODY()
	
public:
	ARoadBuilder();
	
	/** Create road from point A to point B */
	UFUNCTION(BlueprintCallable, Category = "Road")
	void CreateRoad(FVector StartPoint, FVector EndPoint, float Width = 500.0f);
	
private:
	UPROPERTY()
	TArray<class ANavModifierVolume*> RoadVolumes;
};
```

```cpp
// RoadBuilder.cpp
#include "RoadBuilder.h"
#include "NavModifierVolume.h"
#include "NavArea_Road.h"
#include "Components/BrushComponent.h"

ARoadBuilder::ARoadBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARoadBuilder::CreateRoad(FVector StartPoint, FVector EndPoint, float Width)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	// Calculate road parameters
	FVector Direction = (EndPoint - StartPoint).GetSafeNormal();
	float Length = FVector::Dist(StartPoint, EndPoint);
	FVector Center = (StartPoint + EndPoint) * 0.5f;
	
	// Spawn Nav Modifier Volume
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	ANavModifierVolume* RoadVolume = World->SpawnActor<ANavModifierVolume>(
		ANavModifierVolume::StaticClass(),
		Center,
		Direction.Rotation(),
		SpawnParams
	);
	
	if (RoadVolume)
	{
		// Set area class
		RoadVolume->SetAreaClass(UNavArea_Road::StaticClass());
		
		// Scale to match road size
		FVector Scale(Length / 100.0f, Width / 100.0f, 2.0f); // Height = 200cm
		RoadVolume->SetActorScale3D(Scale);
		
		RoadVolumes.Add(RoadVolume);
		
		UE_LOG(LogTemp, Log, TEXT("Created road: Length=%.2f, Width=%.2f"), Length, Width);
	}
}
```

**Использование:**

```cpp
void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Create road builder
	ARoadBuilder* Builder = GetWorld()->SpawnActor<ARoadBuilder>();
	
	// Create road from point A to point B
	FVector PointA(0, 0, 0);
	FVector PointB(5000, 0, 0);
	Builder->CreateRoad(PointA, PointB, 500.0f);
}
```

---

## Отладка и визуализация

### Пример: Debug Draw для путей

```cpp
// NavigationDebugger.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationDebugger.generated.h"

UCLASS()
class ECHOESOFAFFECTION_API ANavigationDebugger : public AActor
{
	GENERATED_BODY()
	
public:
	/** Draw path for NPC */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawNPCPath(AActor* NPC);
	
	/** Show area costs */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void ShowAreaCosts();
};
```

```cpp
// NavigationDebugger.cpp
#include "NavigationDebugger.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

void ANavigationDebugger::DrawNPCPath(AActor* NPC)
{
	if (!NPC) return;
	
	AAIController* AIController = Cast<AAIController>(Cast<APawn>(NPC)->GetController());
	if (!AIController) return;
	
	UPathFollowingComponent* PathComp = AIController->GetPathFollowingComponent();
	if (!PathComp) return;
	
	FNavPathSharedPtr Path = PathComp->GetPath();
	if (!Path.IsValid()) return;
	
	// Draw path points
	const TArray<FNavPathPoint>& PathPoints = Path->GetPathPoints();
	for (int32 i = 0; i < PathPoints.Num() - 1; ++i)
	{
		FVector Start = PathPoints[i].Location;
		FVector End = PathPoints[i + 1].Location;
		
		// Draw line
		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Red,
			false,
			5.0f,
			0,
			5.0f
		);
		
		// Draw sphere at each point
		DrawDebugSphere(
			GetWorld(),
			Start,
			25.0f,
			12,
			FColor::Yellow,
			false,
			5.0f
		);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Drew path with %d points"), PathPoints.Num());
}

void ANavigationDebugger::ShowAreaCosts()
{
	// This would require access to NavMesh data
	// See UNavigationSystemV1::DrawDebugData for reference
	UE_LOG(LogTemp, Warning, TEXT("Area costs visualization not implemented yet"));
}
```

---

## Тесты

### Unit Test для Navigation Filter

```cpp
// NavigationFilterTest.cpp
#include "Misc/AutomationTest.h"
#include "RoadNavigationFilter.h"
#include "NavArea_Road.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRoadNavigationFilterTest,
	"Project.Navigation.RoadFilter",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FRoadNavigationFilterTest::RunTest(const FString& Parameters)
{
	// Create filter
	URoadNavigationFilter* Filter = NewObject<URoadNavigationFilter>();
	
	// Test that filter was created
	TestNotNull(TEXT("Filter should be created"), Filter);
	
	// Test that areas are configured
	TestTrue(TEXT("Filter should have areas"), Filter->Areas.Num() > 0);
	
	// Find road area
	FNavigationFilterArea* RoadArea = Filter->Areas.FindByPredicate([](const FNavigationFilterArea& Area) {
		return Area.AreaClass == UNavArea_Road::StaticClass();
	});
	
	TestNotNull(TEXT("Road area should exist"), RoadArea);
	
	if (RoadArea)
	{
		TestTrue(TEXT("Road area should override travel cost"), RoadArea->bOverrideTravelCost);
		TestEqual(TEXT("Road area cost should be 0.5"), RoadArea->TravelCostOverride, 0.5f);
	}
	
	return true;
}
```

---

## См. также

- [Руководство (RU)](ROAD_NAVIGATION_RU.md) - Полное руководство
- [Быстрый старт (RU)](ROAD_QUICK_START_RU.md) - Для новичков
- [Визуальное руководство (RU)](ROAD_VISUAL_GUIDE_RU.md) - Скриншоты и схемы
- [UE Documentation: Navigation System](https://docs.unrealengine.com/5.0/en-US/navigation-system-in-unreal-engine/)

---

**Автор**: EchoesOfAffection Team  
**Дата**: 2025-01-XX
