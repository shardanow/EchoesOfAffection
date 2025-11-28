// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "Data/DialoguePositioningTypes.h"
#include "DialogueEffect_PositionParticipant.generated.h"

// Forward declarations
class ADialogueLocationMarker;
class AAIController;
class UDialogueLocationSubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogDialoguePositioning, Log, All);

/**
 * Base class for participant positioning effects
 * 
 * Handles finding target location and dispatching to specific positioning implementations.
 * 
 * v1.15: Dialogue Positioning System
 * Architecture: Factory + Strategy pattern
 * - Base class handles location resolution
 * - Derived classes implement specific positioning modes
 * 
 * AAA Best Practices:
 * - Async movement support (delegates for completion)
 * - Soft coupling через GameplayTags для поиска маркеров
 * - Error handling и fallback логика
 * - Debug logging для troubleshooting
 */
UCLASS(Abstract, BlueprintType, meta = (DisplayName = "Position Participant Base"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_PositionParticipant : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_PositionParticipant();

	/** Positioning configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FDialogueParticipantPositioning PositioningConfig;

	//~ Begin UDialogueEffect Interface
	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
	//~ End UDialogueEffect Interface

protected:
	/** Create appropriate positioning effect based on mode */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Positioning")
	static UDialogueEffect_PositionParticipant* CreatePositioningEffect(
		UObject* Outer,
		const FDialogueParticipantPositioning& Config
	);

	/** Resolve target transform from configuration */
	bool ResolveTargetTransform(
		UDialogueSessionContext* Context,
		FTransform& OutTransform,
		FString& OutErrorMessage
	) const;

	/** Find participant actor by ID */
	AActor* FindParticipantActor(
		UDialogueSessionContext* Context,
		FName ParticipantId
	) const;

	/** Find location marker by GameplayTag */
	ADialogueLocationMarker* FindLocationMarkerByTag(
		UWorld* World,
		FGameplayTag MarkerTag
	) const;

	/** Get AI controller from actor */
	AAIController* GetAIController(AActor* Actor) const;

	/** Execute specific positioning implementation (override in derived classes) */
	virtual void ExecutePositioning(
		AActor* ParticipantActor,
		const FTransform& TargetTransform,
		UDialogueSessionContext* Context
	);

	/** Check if positioning should wait for completion */
	virtual bool ShouldWaitForCompletion() const { return PositioningConfig.bWaitForCompletion; }

	/** Log positioning debug info */
	void LogPositioningInfo(
		AActor* ParticipantActor,
		const FTransform& TargetTransform,
		const FString& AdditionalInfo = TEXT("")
	) const;

	/** NEW v1.17.1: Emit GameEventBus event after positioning completes */
	void EmitParticipantPositionedEvent(UDialogueSessionContext* Context, FName ParticipantId);
};

/**
 * Factory for creating positioning effects
 * 
 * v1.15: Centralized effect creation
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialoguePositioningEffectFactory : public UObject
{
	GENERATED_BODY()

public:
	/** Create positioning effect from configuration */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Positioning")
	static UDialogueEffect_PositionParticipant* CreateEffect(
		UObject* Outer,
		const FDialogueParticipantPositioning& Config
	);

	/** Create effects for all participants in node positioning config */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Positioning")
	static TArray<UDialogueEffect_PositionParticipant*> CreateEffectsForNode(
		UObject* Outer,
		const FDialogueNodePositioning& NodePositioning,
		FName PrimaryPersonaId
	);
};
