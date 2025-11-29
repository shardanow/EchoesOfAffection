// TurnToFaceComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurnToFaceComponent.generated.h"

// Forward declarations
struct FGameEventPayload;

/**
 * Turn-To-Face Component v1.18
 * 
 * Automatically subscribes to GameEvent.Dialogue.TurnToFace and rotates actor.
 * Just add to NPC blueprint - no additional setup needed!
 */
UCLASS(BlueprintType, ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UTurnToFaceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnToFaceComponent();

	/** Enable/disable turn-to-face for this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn To Face")
	bool bEnableTurnToFace = true;

	/** Rotation speed (higher = faster) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn To Face", meta = (ClampMin = "0.5", ClampMax = "10.0"))
	float RotationSpeed = 5.0f;
	
	/** Use smooth interpolation (false = instant snap) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn To Face")
	bool bSmoothRotation = true;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void OnTurnToFaceEvent(const FGameEventPayload& Payload);
	void BeginRotationTo(AActor* Target, float Duration);
	
	FDelegateHandle EventHandle;
	bool bIsRotating = false;
	FRotator InitialRotation;   // RENAMED from StartRotation
	FRotator TargetRotation;
	float RotationProgress = 0.0f;
	float RotationDuration = 0.5f;
};
