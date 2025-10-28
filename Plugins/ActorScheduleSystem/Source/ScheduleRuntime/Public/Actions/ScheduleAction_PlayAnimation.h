// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ScheduleActionBase.h"
#include "ScheduleAction_PlayAnimation.generated.h"

class UAnimMontage;
class UAnimationAsset;

/**
 * Action that plays an animation or montage on the actor
 * Supports skeletal mesh animations
 */
UCLASS(BlueprintType, meta = (DisplayName = "Play Animation"))
class SCHEDULERUNTIME_API UScheduleAction_PlayAnimation : public UScheduleActionBase
{
	GENERATED_BODY()

public:
	/** Animation montage to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	TObjectPtr<UAnimMontage> AnimationMontage;

	/** Playback rate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float PlayRate = 1.0f;

	/** Whether to loop the animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bLooping = false;

	/** Blend in time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (ClampMin = "0"))
	float BlendInTime = 0.25f;

	/** Blend out time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (ClampMin = "0"))
	float BlendOutTime = 0.25f;

protected:
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const override;
	virtual FText GetActionDescription_Implementation() const override;

private:
	/** Track playing montages */
	UPROPERTY(Transient)
	TMap<FGuid, TWeakObjectPtr<class USkeletalMeshComponent>> PlayingMontages;
};
