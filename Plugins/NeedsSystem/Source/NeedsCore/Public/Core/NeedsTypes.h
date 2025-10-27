// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "NeedsTypes.generated.h"

// Forward declarations
class UNeedDefinitionAsset;
class UNeedModifierAsset;
class UNeedEffectAsset;

/**
 * Handle дл€ безопасной ссылки на Need (без пр€мого указател€)
 * »спользуетс€ дл€ внешнего доступа к потребност€м
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedHandle
{
	GENERATED_BODY()

	/** Unique identifier for this need */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Handle")
	FGameplayTag NeedTag;

	/** Owner component ID (дл€ валидации) */
	UPROPERTY()
	int32 OwnerID = 0;

	FNeedHandle() = default;
	
	FNeedHandle(const FGameplayTag& InTag, int32 InOwnerID)
		: NeedTag(InTag), OwnerID(InOwnerID)
	{
	}

	bool IsValid() const { return NeedTag.IsValid(); }
	
	bool operator==(const FNeedHandle& Other) const
	{
		return NeedTag == Other.NeedTag && OwnerID == Other.OwnerID;
	}

	friend uint32 GetTypeHash(const FNeedHandle& Handle)
	{
		return HashCombine(GetTypeHash(Handle.NeedTag), GetTypeHash(Handle.OwnerID));
	}
};

/**
 * Threshold levels for need warnings
 */
UENUM(BlueprintType)
enum class ENeedThresholdLevel : uint8
{
	/** All is good */
	Normal UMETA(DisplayName = "Normal"),
	
	/** Need is approaching critical level */
	Warning UMETA(DisplayName = "Warning"),
	
	/** Need is at critical level */
	Critical UMETA(DisplayName = "Critical"),
	
	/** Need has recovered from warning/critical */
	Recovered UMETA(DisplayName = "Recovered")
};

/**
 * LOD Level дл€ симул€ции потребностей
 * ќпредел€ет частоту обновлени€ в зависимости от рассто€ни€ до камеры
 */
UENUM(BlueprintType)
enum class ENeedLODLevel : uint8
{
	/** Full simulation - player character or very close NPCs */
	Near UMETA(DisplayName = "Near (Full)"),
	
	/** Reduced update frequency - medium distance */
	Far UMETA(DisplayName = "Far (Reduced)"),
	
	/** Minimal simulation - far away or not visible */
	Parked UMETA(DisplayName = "Parked (Minimal)")
};

/**
 * Effect application type
 */
UENUM(BlueprintType)
enum class ENeedEffectType : uint8
{
	/** Apply effect instantly */
	Instant UMETA(DisplayName = "Instant"),
	
	/** Apply effect over time */
	OverTime UMETA(DisplayName = "Over Time")
};

/**
 * Modifier operation type
 */
UENUM(BlueprintType)
enum class ENeedModifierOperation : uint8
{
	/** Add value */
	Add UMETA(DisplayName = "Add"),
	
	/** Multiply value */
	Multiply UMETA(DisplayName = "Multiply"),
	
	/** Override value */
	Override UMETA(DisplayName = "Override")
};

/**
 * Runtime state of a single Need
 * ’ранит текущее значение, флаги, и runtime данные
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedRuntimeState
{
	GENERATED_BODY()

	/** Current value (0..100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need State")
	float CurrentValue = 100.0f;

	/** Is this need currently suppressed (not decaying) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need State")
	bool bIsSuppressed = false;

	/** Is this need frozen (not changing at all) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need State")
	bool bIsFrozen = false;

	/** Current threshold level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need State")
	ENeedThresholdLevel CurrentThreshold = ENeedThresholdLevel::Normal;

	/** Previous threshold (дл€ детекции изменений) */
	UPROPERTY()
	ENeedThresholdLevel PreviousThreshold = ENeedThresholdLevel::Normal;

	/** Time accumulated for decay (дл€ lerp) */
	UPROPERTY()
	float AccumulatedDecayTime = 0.0f;

	FNeedRuntimeState() = default;

	/** Clamp value to valid range */
	void ClampValue()
	{
		CurrentValue = FMath::Clamp(CurrentValue, 0.0f, 100.0f);
	}

	/** Check if threshold changed */
	bool HasThresholdChanged() const
	{
		return CurrentThreshold != PreviousThreshold;
	}

	/** Update previous threshold */
	void UpdatePreviousThreshold()
	{
		PreviousThreshold = CurrentThreshold;
	}
};

/**
 * Threshold configuration
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedThresholdConfig
{
	GENERATED_BODY()

	/** Warning threshold (value below this triggers warning) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float WarningThreshold = 30.0f;

	/** Critical threshold (value below this triggers critical) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float CriticalThreshold = 15.0f;

	/** Recovered threshold (value above this exits warning/critical) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float RecoveredThreshold = 50.0f;

	FNeedThresholdConfig()
		: WarningThreshold(30.0f)
		, CriticalThreshold(15.0f)
		, RecoveredThreshold(50.0f)
	{
	}

	/** Evaluate threshold level for a value */
	ENeedThresholdLevel EvaluateThreshold(float Value, ENeedThresholdLevel CurrentLevel) const
	{
		// ≈сли уже в Warning/Critical, нужен RecoveredThreshold дл€ выхода
		if (CurrentLevel == ENeedThresholdLevel::Warning || CurrentLevel == ENeedThresholdLevel::Critical)
		{
			if (Value >= RecoveredThreshold)
			{
				return ENeedThresholdLevel::Recovered;
			}
		}

		// Check critical first
		if (Value <= CriticalThreshold)
		{
			return ENeedThresholdLevel::Critical;
		}

		// Check warning
		if (Value <= WarningThreshold)
		{
			return ENeedThresholdLevel::Warning;
		}

		// Normal
		return ENeedThresholdLevel::Normal;
	}
};

/**
 * Decay/Recovery curve configuration
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedDecayConfig
{
	GENERATED_BODY()

	/** Base decay rate (points per game hour, can be negative for auto-recovery) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay", meta = (ClampMin = "-100.0", ClampMax = "100.0"))
	float BaseDecayRate = 5.0f;

	/** Optional decay curve (if set, overrides linear decay) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	FRuntimeFloatCurve DecayCurve;

	/** Use curve instead of linear decay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decay")
	bool bUseCurve = false;

	FNeedDecayConfig()
		: BaseDecayRate(5.0f)
		, bUseCurve(false)
	{
	}

	/** Calculate decay amount for given delta time (in game hours) */
	float CalculateDecay(float DeltaGameHours, float CurrentValue) const
	{
		if (bUseCurve && DecayCurve.GetRichCurveConst() && DecayCurve.GetRichCurveConst()->GetNumKeys() > 0)
		{
			// Use curve: X = normalized value (0..1), Y = decay rate multiplier
			const float NormalizedValue = CurrentValue / 100.0f;
			const float CurveMultiplier = DecayCurve.GetRichCurveConst()->Eval(NormalizedValue);
			return BaseDecayRate * CurveMultiplier * DeltaGameHours;
		}

		// Linear decay
		return BaseDecayRate * DeltaGameHours;
	}
};

/**
 * Mood calculation weight for a single need
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FMoodNeedWeight
{
	GENERATED_BODY()

	/** Need tag to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
	FGameplayTag NeedTag;

	/** Weight of this need in mood calculation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Weight = 1.0f;

	FMoodNeedWeight()
		: Weight(1.0f)
	{
	}
};

/**
 * Mood state
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FMoodState
{
	GENERATED_BODY()

	/** Current mood value (-100..100) - negative is bad, positive is good */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
	float MoodValue = 0.0f;

	/** Previous mood value (дл€ детекции изменений) */
	UPROPERTY()
	float PreviousMoodValue = 0.0f;

	FMoodState()
		: MoodValue(0.0f)
		, PreviousMoodValue(0.0f)
	{
	}

	/** Clamp mood to valid range */
	void ClampMood()
	{
		MoodValue = FMath::Clamp(MoodValue, -100.0f, 100.0f);
	}

	/** Check if mood changed significantly */
	bool HasChangedSignificantly(float Threshold = 5.0f) const
	{
		return FMath::Abs(MoodValue - PreviousMoodValue) >= Threshold;
	}

	/** Update previous value */
	void UpdatePrevious()
	{
		PreviousMoodValue = MoodValue;
	}
};

/**
 * Active effect instance (runtime)
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FActiveNeedEffect
{
	GENERATED_BODY()

	/** Effect definition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<UNeedEffectAsset> EffectAsset;

	/** Unique instance ID */
	UPROPERTY()
	int32 InstanceID = 0;

	/** Time remaining (for OverTime effects) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float RemainingDuration = 0.0f;

	/** Stack count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int32 StackCount = 1;

	/** Application time (game time) */
	UPROPERTY()
	double ApplicationTime = 0.0;

	FActiveNeedEffect()
		: InstanceID(0)
		, RemainingDuration(0.0f)
		, StackCount(1)
		, ApplicationTime(0.0)
	{
	}
};

/**
 * Active modifier instance (runtime)
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FActiveNeedModifier
{
	GENERATED_BODY()

	/** Modifier definition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	TObjectPtr<UNeedModifierAsset> ModifierAsset;

	/** Unique instance ID */
	UPROPERTY()
	int32 InstanceID = 0;

	/** Priority (higher = applied later) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	int32 Priority = 0;

	/** Is this modifier currently active (conditions met) */
	UPROPERTY()
	bool bIsActive = true;

	FActiveNeedModifier()
		: InstanceID(0)
		, Priority(0)
		, bIsActive(true)
	{
	}
};

/**
 * LOD Configuration
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedLODConfig
{
	GENERATED_BODY()

	/** Distance threshold for Far LOD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (ClampMin = "0.0"))
	float FarDistance = 2000.0f;

	/** Distance threshold for Parked LOD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (ClampMin = "0.0"))
	float ParkedDistance = 5000.0f;

	/** Update frequency multiplier for Far LOD (1.0 = normal, 0.5 = half speed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float FarUpdateMultiplier = 0.5f;

	/** Update frequency multiplier for Parked LOD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float ParkedUpdateMultiplier = 0.1f;

	FNeedLODConfig()
		: FarDistance(2000.0f)
		, ParkedDistance(5000.0f)
		, FarUpdateMultiplier(0.5f)
		, ParkedUpdateMultiplier(0.1f)
	{
	}

	/** Determine LOD level based on distance */
	ENeedLODLevel GetLODForDistance(float Distance) const
	{
		if (Distance >= ParkedDistance)
		{
			return ENeedLODLevel::Parked;
		}
		if (Distance >= FarDistance)
		{
			return ENeedLODLevel::Far;
		}
		return ENeedLODLevel::Near;
	}

	/** Get update multiplier for LOD level */
	float GetUpdateMultiplier(ENeedLODLevel LOD) const
	{
		switch (LOD)
		{
			case ENeedLODLevel::Far:
				return FarUpdateMultiplier;
			case ENeedLODLevel::Parked:
				return ParkedUpdateMultiplier;
			default:
				return 1.0f;
		}
	}
};
