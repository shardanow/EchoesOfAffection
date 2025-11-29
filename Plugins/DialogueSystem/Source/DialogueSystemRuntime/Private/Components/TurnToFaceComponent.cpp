// TurnToFaceComponent.cpp
#include "Components/TurnToFaceComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"  // Contains FGameEventPayload
#endif

DEFINE_LOG_CATEGORY_STATIC(LogTurnToFaceComponent, Log, All);

UTurnToFaceComponent::UTurnToFaceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false; // Enable only when rotating
}

void UTurnToFaceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnableTurnToFace)
	{
		UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] TurnToFace disabled"), *GetOwner()->GetName());
		return;
	}

#if WITH_GAMEEVENTBUS
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTurnToFaceComponent, Error, TEXT("[%s] World is null"), *GetOwner()->GetName());
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogTurnToFaceComponent, Error, TEXT("[%s] GameEventBus not available"), *GetOwner()->GetName());
		return;
	}

	FGameplayTag TurnToFaceTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.TurnToFace"));
	
	auto Lambda = [this](const FGameEventPayload& Payload)
	{
		this->OnTurnToFaceEvent(Payload);
	};
	
	EventHandle = EventBus->SubscribeToEventNative(
		TurnToFaceTag,
		FGameEventNativeDelegate::FDelegate::CreateLambda(Lambda)
	);
	
	UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Subscribed to TurnToFace events"), *GetOwner()->GetName());
#else
	UE_LOG(LogTurnToFaceComponent, Warning, TEXT("[%s] GameEventBus not enabled (WITH_GAMEEVENTBUS=0)"), *GetOwner()->GetName());
#endif
}

void UTurnToFaceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_GAMEEVENTBUS
	UWorld* World = GetWorld();
	if (World)
	{
		UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
		if (EventBus && EventHandle.IsValid())
		{
			FGameplayTag TurnToFaceTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.TurnToFace"));
			// Note: EventBus doesn't have UnsubscribeFromEventNative with handle
			// Handle will be automatically cleaned up when component is destroyed
			UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Component destroyed"), *GetOwner()->GetName());
		}
	}
#endif

	Super::EndPlay(EndPlayReason);
}

void UTurnToFaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsRotating)
	{
		SetComponentTickEnabled(false);
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Update rotation progress
	RotationProgress += DeltaTime;
	float Alpha = FMath::Clamp(RotationProgress / RotationDuration, 0.0f, 1.0f);

	// FIXED v1.18.1: Use linear interpolation with Alpha (not RInterpTo!)
	// RInterpTo ignores Duration and uses spring-like behavior
	
	// Optional: Apply ease-in-out for smoother start/end
	// This makes rotation feel more natural (slow start, fast middle, slow end)
	float EasedAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha); // Smoothstep
	
	FRotator CurrentRotation = FMath::Lerp(InitialRotation, TargetRotation, EasedAlpha);
	Owner->SetActorRotation(CurrentRotation);

	// Check if completed
	if (Alpha >= 1.0f)
	{
		Owner->SetActorRotation(TargetRotation);
		bIsRotating = false;
		SetComponentTickEnabled(false);
		
		UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Turn completed"), *Owner->GetName());
	}
}

void UTurnToFaceComponent::OnTurnToFaceEvent(const FGameEventPayload& Payload)
{
	AActor* Owner = GetOwner();
	if (!Owner || !bEnableTurnToFace)
	{
		return;
	}

	// Check if I should turn
	AActor* InstigatorActor = Payload.InstigatorActor.Get();
	if (InstigatorActor != Owner)
	{
		return; // Not me
	}

	AActor* Target = Payload.TargetActor.Get();
	if (!Target)
	{
		UE_LOG(LogTurnToFaceComponent, Warning, TEXT("[%s] No target actor in payload"), *Owner->GetName());
		return;
	}

	UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Turn-to-face event received: Target=%s, Mode=%s, Duration=%.2f"),
		*Owner->GetName(),
		*Target->GetName(),
		*Payload.StringParam.ToString(),
		Payload.FloatParam);

	// Calculate look-at rotation
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

	// Preserve pitch and roll (yaw only)
	FRotator CurrentRotation = Owner->GetActorRotation();
	FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

	// Check mode
	FString Mode = Payload.StringParam.ToString();
	if (Mode == "Instant" || !bSmoothRotation)
	{
		// Instant snap
		Owner->SetActorRotation(NewRotation);
		UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Instant rotation applied"), *Owner->GetName());
	}
	else
	{
		// Smooth rotation
		float Duration = Payload.FloatParam > 0.0f ? Payload.FloatParam : 0.5f;
		BeginRotationTo(Target, Duration);
	}
}

void UTurnToFaceComponent::BeginRotationTo(AActor* Target, float Duration)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Target) return;

	// Calculate target rotation
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

	// Store rotation data
	InitialRotation = Owner->GetActorRotation();
	TargetRotation = FRotator(InitialRotation.Pitch, LookAtRotation.Yaw, InitialRotation.Roll);
	RotationDuration = Duration;
	RotationProgress = 0.0f;
	bIsRotating = true;

	// Enable tick for interpolation
	SetComponentTickEnabled(true);

	UE_LOG(LogTurnToFaceComponent, Log, TEXT("[%s] Starting smooth rotation: %.1f deg -> %.1f deg over %.2fs"),
		*Owner->GetName(),
		InitialRotation.Yaw,
		TargetRotation.Yaw,
		Duration);
}
