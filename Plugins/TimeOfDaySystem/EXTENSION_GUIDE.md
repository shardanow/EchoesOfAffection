# Extending the Time of Day System

## Overview
This guide shows how to extend the Time of Day system with custom functionality while maintaining the modular architecture.

## Example 1: Adding Weather System Integration

### Step 1: Create Weather Interface

```cpp
// IWeatherSystemInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/TimeOfDayTypes.h"
#include "IWeatherSystemInterface.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
 Clear,
    Cloudy,
    Rainy,
    Stormy,
    Snowy,
    Foggy
};

UINTERFACE(MinimalAPI, Blueprintable)
class UWeatherSystemInterface : public UInterface
{
    GENERATED_BODY()
};

class IWeatherSystemInterface
{
    GENERATED_BODY()

public:
 UFUNCTION(BlueprintNativeEvent, Category = "Weather")
    void SetWeather(EWeatherType Weather, float Intensity);
    
    UFUNCTION(BlueprintNativeEvent, Category = "Weather")
    EWeatherType GetCurrentWeather() const;
};
```

### Step 2: Create Weather Subsystem

```cpp
// WeatherSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayInterface.h"
#include "IWeatherSystemInterface.h"
#include "WeatherSubsystem.generated.h"

UCLASS()
class UWeatherSubsystem : public UWorldSubsystem, 
    public ITimeOfDayListener, 
    public IWeatherSystemInterface
{
 GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // ITimeOfDayListener
    virtual void OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FGameTime& CurrentTime) override;
    virtual void OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FGameTime& CurrentTime) override;

    // IWeatherSystemInterface
    virtual void SetWeather_Implementation(EWeatherType Weather, float Intensity) override;
    virtual EWeatherType GetCurrentWeather_Implementation() const override;

protected:
    void UpdateSeasonalWeather(ETimeOfDaySeason Season);
    void UpdateTimeBasedWeather(ETimeOfDayPhase Phase);

    UPROPERTY()
    EWeatherType CurrentWeather;
    
    UPROPERTY()
    float WeatherIntensity;
};
```

### Step 3: Implementation

```cpp
// WeatherSubsystem.cpp
#include "WeatherSubsystem.h"
#include "Subsystems/TimeOfDaySubsystem.h"

void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Register with TimeOfDaySubsystem
    if (UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>())
 {
        TimeSystem->RegisterListener(this);
    }
}

void UWeatherSubsystem::OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FGameTime& CurrentTime)
{
    UpdateSeasonalWeather(NewSeason);
}

void UWeatherSubsystem::UpdateSeasonalWeather(ETimeOfDaySeason Season)
{
    switch(Season)
    {
        case ETimeOfDaySeason::Winter:
    SetWeather(EWeatherType::Snowy, 0.7f);
      break;
     case ETimeOfDaySeason::Spring:
            SetWeather(EWeatherType::Rainy, 0.5f);
   break;
    case ETimeOfDaySeason::Summer:
SetWeather(EWeatherType::Clear, 0.0f);
            break;
        case ETimeOfDaySeason::Autumn:
    SetWeather(EWeatherType::Foggy, 0.4f);
   break;
    }
}
```

## Example 2: Adding Moon Phase System

### Lunar Cycle Component

```cpp
// LunarCycleComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/TimeOfDayInterface.h"
#include "LunarCycleComponent.generated.h"

UENUM(BlueprintType)
enum class EMoonPhase : uint8
{
    NewMoon,
  WaxingCrescent,
    FirstQuarter,
    WaxingGibbous,
 FullMoon,
    WaningGibbous,
    LastQuarter,
  WaningCrescent
};

UCLASS(ClassGroup=(TimeOfDay), meta=(BlueprintSpawnableComponent))
class ULunarCycleComponent : public UActorComponent, public ITimeOfDayListener
{
    GENERATED_BODY()

public:
    ULunarCycleComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ITimeOfDayListener
    virtual void OnDayChanged_Implementation(const FGameTime& CurrentTime) override;

    UFUNCTION(BlueprintCallable, Category = "Lunar Cycle")
    EMoonPhase GetCurrentMoonPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintCallable, Category = "Lunar Cycle")
    float GetMoonIllumination() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lunar Cycle")
    int32 DaysPerLunarCycle = 28;

protected:
    void CalculateMoonPhase(int32 TotalDays);

    UPROPERTY()
  EMoonPhase CurrentPhase;
    
    UPROPERTY()
    int32 DayInCycle;
};
```

## Example 3: Adding Tidal System (Ocean Tides)

### Tidal System

```cpp
// TidalSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayInterface.h"
#include "TidalSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTideChanged, float, TideHeight);

UCLASS()
class UTidalSubsystem : public UTickableWorldSubsystem, public ITimeOfDayListener
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

// ITimeOfDayListener
    virtual void OnHourChanged_Implementation(const FGameTime& CurrentTime) override;

    UFUNCTION(BlueprintCallable, Category = "Tides")
    float GetCurrentTideHeight() const { return CurrentTideHeight; }

    UFUNCTION(BlueprintCallable, Category = "Tides")
    bool IsHighTide() const;

    UPROPERTY(BlueprintAssignable, Category = "Tides")
    FOnTideChanged OnTideChanged;

protected:
    void CalculateTideHeight(float NormalizedTimeOfDay);

    UPROPERTY(EditDefaultsOnly, Category = "Tides")
    float MaxTideHeight = 200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Tides")
    float MinTideHeight = -200.0f;

    UPROPERTY()
    float CurrentTideHeight;
};
```

## Example 4: Adding Event Calendar System

### Calendar Event System

```cpp
// CalendarEventSystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayInterface.h"
#include "CalendarEventSystem.generated.h"

USTRUCT(BlueprintType)
struct FCalendarEvent
{
 GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText EventDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Year;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETimeOfDaySeason Season;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 DayOfSeason;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Hour;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRecurringYearly = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bTriggered = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalendarEvent, const FCalendarEvent&, Event);

UCLASS()
class UCalendarEventSystem : public UWorldSubsystem, public ITimeOfDayListener
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ITimeOfDayListener
    virtual void OnHourChanged_Implementation(const FGameTime& CurrentTime) override;
    virtual void OnDayChanged_Implementation(const FGameTime& CurrentTime) override;

    UFUNCTION(BlueprintCallable, Category = "Calendar")
    void AddEvent(const FCalendarEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "Calendar")
    void RemoveEvent(const FCalendarEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "Calendar")
    TArray<FCalendarEvent> GetUpcomingEvents(int32 DaysAhead = 7) const;

    UPROPERTY(BlueprintAssignable, Category = "Calendar")
    FOnCalendarEvent OnEventTriggered;

protected:
    void CheckForEvents(const FGameTime& CurrentTime);
    bool DoesEventMatch(const FCalendarEvent& Event, const FGameTime& Time) const;

    UPROPERTY()
    TArray<FCalendarEvent> ScheduledEvents;
};
```

## Example 5: Adding Time-Based Asset Streaming

### Time-Based Streaming Manager

```cpp
// TimeBasedStreamingManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayInterface.h"
#include "TimeBasedStreamingManager.generated.h"

USTRUCT(BlueprintType)
struct FTimeBasedStreamingZone
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ZoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ETimeOfDayPhase> ActivePhases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ETimeOfDaySeason> ActiveSeasons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UWorld> LevelToStream;
};

UCLASS()
class UTimeBasedStreamingManager : public UWorldSubsystem, public ITimeOfDayListener
{
 GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ITimeOfDayListener
    virtual void OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FGameTime& CurrentTime) override;
    virtual void OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FGameTime& CurrentTime) override;

    UFUNCTION(BlueprintCallable, Category = "Streaming")
    void RegisterStreamingZone(const FTimeBasedStreamingZone& Zone);

protected:
    void UpdateStreamedLevels(ETimeOfDayPhase Phase, ETimeOfDaySeason Season);

    UPROPERTY()
    TArray<FTimeBasedStreamingZone> StreamingZones;
};
```

## Example 6: Adding Astronomical System (Sun/Stars)

### Astronomical Calculator

```cpp
// AstronomicalSystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayInterface.h"
#include "AstronomicalSystem.generated.h"

USTRUCT(BlueprintType)
struct FAstronomicalData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FRotator SunRotation;

    UPROPERTY(BlueprintReadOnly)
    FRotator MoonRotation;

    UPROPERTY(BlueprintReadOnly)
    float SunIntensity;

    UPROPERTY(BlueprintReadOnly)
    float MoonIntensity;

    UPROPERTY(BlueprintReadOnly)
    FLinearColor SkyColor;

    UPROPERTY(BlueprintReadOnly)
    float StarVisibility;
};

UCLASS()
class UAstronomicalSystem : public UTickableWorldSubsystem, public ITimeOfDayListener
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    UFUNCTION(BlueprintCallable, Category = "Astronomy")
    FAstronomicalData GetAstronomicalData() const;

    UFUNCTION(BlueprintCallable, Category = "Astronomy")
    FRotator CalculateSunRotation(float NormalizedTime) const;

    UFUNCTION(BlueprintCallable, Category = "Astronomy")
    FRotator CalculateMoonRotation(float NormalizedTime, int32 DayOfYear) const;

protected:
    void UpdateAstronomicalData();

    UPROPERTY()
    FAstronomicalData CurrentData;

    UPROPERTY(EditDefaultsOnly, Category = "Astronomy")
    float Latitude = 45.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Astronomy")
    float AxisTilt = 23.5f;
};
```

## Blueprint Integration Examples

### Custom Time Event Node (Blueprint Function Library)

```cpp
// TimeOfDayExtensionLibrary.h
UCLASS()
class UTimeOfDayExtensionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Check if current time is within a specific range */
    UFUNCTION(BlueprintPure, Category = "Time of Day|Extensions", meta = (WorldContext = "WorldContextObject"))
    static bool IsTimeBetween(const UObject* WorldContextObject, int32 StartHour, int32 EndHour);

    /** Get minutes until next phase */
    UFUNCTION(BlueprintPure, Category = "Time of Day|Extensions", meta = (WorldContext = "WorldContextObject"))
    static int32 GetMinutesUntilNextPhase(const UObject* WorldContextObject);

 /** Calculate sunrise time for current day */
    UFUNCTION(BlueprintPure, Category = "Time of Day|Extensions", meta = (WorldContext = "WorldContextObject"))
    static FGameTime GetSunriseTime(const UObject* WorldContextObject);

    /** Calculate sunset time for current day */
    UFUNCTION(BlueprintPure, Category = "Time of Day|Extensions", meta = (WorldContext = "WorldContextObject"))
    static FGameTime GetSunsetTime(const UObject* WorldContextObject);

    /** Get day length in real-world seconds */
    UFUNCTION(BlueprintPure, Category = "Time of Day|Extensions", meta = (WorldContext = "WorldContextObject"))
    static float GetRealSecondsDayLength(const UObject* WorldContextObject);
};
```

## Performance Optimization Tips

### 1. Event Filtering
Only subscribe to events you need:

```cpp
TimeListener->bReceiveMinuteEvents = false; // Disable if not needed
TimeListener->bReceiveHourEvents = true;    // Only enable required events
```

### 2. Batch Updates
Group related updates together:

```cpp
void OnHourChanged_Implementation(const FGameTime& CurrentTime) override
{
    // Batch all updates that should happen hourly
    UpdateNPCSchedules(CurrentTime);
    UpdateLighting(CurrentTime);
    UpdateAmbientSounds(CurrentTime);
}
```

### 3. LOD-Based Updates
Update distant objects less frequently:

```cpp
void UMyComponent::Tick(float DeltaTime)
{
    float Distance = GetDistanceToPlayer();
    if (Distance < NearDistance)
    {
        UpdateEveryFrame();
    }
  else if (Distance < FarDistance && ShouldUpdateThisFrame())
    {
  UpdateReduced();
  }
}
```

## Testing Extensions

### Unit Test Example

```cpp
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimeOfDayWeatherTest, 
    "TimeOfDay.Extensions.Weather", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTimeOfDayWeatherTest::RunTest(const FString& Parameters)
{
    // Create test world
    UWorld* TestWorld = CreateTestWorld();
  
    // Get subsystems
    UTimeOfDaySubsystem* TimeSystem = TestWorld->GetSubsystem<UTimeOfDaySubsystem>();
    UWeatherSubsystem* WeatherSystem = TestWorld->GetSubsystem<UWeatherSubsystem>();
    
    // Test seasonal weather changes
  FGameTime WinterTime;
    WinterTime.Season = ETimeOfDaySeason::Winter;
    TimeSystem->SetCurrentTime(WinterTime);
    
    TestEqual("Winter should have snowy weather", 
 WeatherSystem->GetCurrentWeather(), 
EWeatherType::Snowy);
    
    return true;
}
```

## Documentation Template for Extensions

When creating extensions, document them using this template:

```markdown
# [Extension Name]

## Purpose
Brief description of what this extension adds

## Dependencies
- TimeOfDayCore
- TimeOfDayRuntime
- [Other dependencies]

## Installation
1. Step-by-step installation instructions
2. Configuration steps
3. Setup in project

## API Reference
### Classes
- Class name and purpose
- Key methods
- Properties

### Events
- Event name
- When it fires
- Parameters

## Usage Examples
### C++ Example
```cpp
// Code example
```

### Blueprint Example
Description and screenshots

## Performance Impact
- Tick cost: X ms
- Memory usage: Y KB
- Recommended max instances: Z

## Known Limitations
- Limitation 1
- Limitation 2

## Future Improvements
- Planned feature 1
- Planned feature 2
```

## Conclusion

The Time of Day system is designed to be easily extensible. Follow these patterns:

1. **Use Interfaces** - Implement `ITimeOfDayListener` for time-aware systems
2. **Create Subsystems** - Use WorldSubsystem for global game systems
3. **Use Components** - Create ActorComponents for per-actor functionality
4. **Event-Driven** - Subscribe to relevant events only
5. **Data-Driven** - Use DataAssets for configuration
6. **Document** - Provide clear documentation and examples

This approach maintains the modularity and performance of the system while allowing unlimited extensibility.
