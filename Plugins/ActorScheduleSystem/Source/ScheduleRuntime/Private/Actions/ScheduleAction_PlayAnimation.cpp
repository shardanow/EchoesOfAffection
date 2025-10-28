// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleAction_PlayAnimation.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

FScheduleActionHandle UScheduleAction_PlayAnimation::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);

	if (!AnimationMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimation: No animation montage assigned"));
		return Handle;
	}

	AActor* Actor = Context.ExecutingActor.Get();
	if (!Actor)
	{
		return Handle;
	}

	// Find skeletal mesh component
	USkeletalMeshComponent* MeshComp = nullptr;
	
	if (ACharacter* Character = Cast<ACharacter>(Actor))
	{
		MeshComp = Character->GetMesh();
	}
	else
	{
		MeshComp = Actor->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!MeshComp || !MeshComp->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimation: Actor %s has no skeletal mesh or anim instance"), *Actor->GetName());
		return Handle;
	}

	// Play montage
	float Duration = MeshComp->GetAnimInstance()->Montage_Play(
		AnimationMontage,
		PlayRate,
		EMontagePlayReturnType::MontageLength,
		0.0f,
		!bLooping
	);

	if (Duration > 0.0f)
	{
		PlayingMontages.Add(Handle.HandleID, MeshComp);
	}

	return Handle;
}

void UScheduleAction_PlayAnimation::CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (TWeakObjectPtr<USkeletalMeshComponent>* MeshPtr = PlayingMontages.Find(ActionHandle.HandleID))
	{
		if (USkeletalMeshComponent* Mesh = MeshPtr->Get())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(BlendOutTime, AnimationMontage);
			}
		}

		PlayingMontages.Remove(ActionHandle.HandleID);
	}
}

bool UScheduleAction_PlayAnimation::IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const
{
	if (const TWeakObjectPtr<USkeletalMeshComponent>* MeshPtr = PlayingMontages.Find(ActionHandle.HandleID))
	{
		if (USkeletalMeshComponent* Mesh = MeshPtr->Get())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				return AnimInstance->Montage_IsPlaying(AnimationMontage);
			}
		}
	}

	return false;
}

FText UScheduleAction_PlayAnimation::GetActionDescription_Implementation() const
{
	if (!ActionName.IsEmpty())
	{
		return Super::GetActionDescription_Implementation();
	}

	if (AnimationMontage)
	{
		return FText::Format(
			NSLOCTEXT("ScheduleSystem", "PlayAnimationAction", "Play Animation: {0}"),
			FText::FromString(AnimationMontage->GetName())
		);
	}

	return NSLOCTEXT("ScheduleSystem", "PlayAnimationActionNoAnim", "Play Animation (None)");
}
