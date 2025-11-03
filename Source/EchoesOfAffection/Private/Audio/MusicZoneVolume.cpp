// Copyright Epic Games, Inc. All Rights Reserved.

#include "Audio/MusicZoneVolume.h"
#include "Audio/MusicManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/ShapeComponent.h"

AMusicZoneVolume::AMusicZoneVolume()
{
    // TriggerBox is already configured for overlap events
  // Just set up visualization
    
    if (UShapeComponent* ShapeComp = GetCollisionComponent())
    {
        // Color depends on priority (blue by default)
        ShapeComp->ShapeColor = FColor(100, 200, 255, 255);
        ShapeComp->bHiddenInGame = true; // Hide in game
    }

#if WITH_EDITORONLY_DATA
    bActorLabelEditable = true;
#endif
}

void AMusicZoneVolume::BeginPlay()
{
Super::BeginPlay();
    
    UE_LOG(LogTemp, Log, TEXT("MusicZoneVolume '%s' initialized - Priority: %d, Music: %s"),
        *GetName(), Priority, ZoneMusic ? *ZoneMusic->GetName() : TEXT("None"));
}

void AMusicZoneVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!bIsActive || !ZoneMusic)
    {
   return;
    }

    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn)
    {
return;
  }

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC)
    {
        return;
    }

    if (bPlayerOnly && !PC->IsLocalPlayerController())
    {
        return;
    }

    HandlePlayerEntered(PC);
}

void AMusicZoneVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);

    if (!bIsActive)
    {
        return;
    }

    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn)
    {
   return;
    }

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC)
    {
        return;
    }

    if (bPlayerOnly && !PC->IsLocalPlayerController())
    {
        return;
    }

    HandlePlayerExited(PC);
}

void AMusicZoneVolume::HandlePlayerEntered(APlayerController* PlayerController)
{
    if (!PlayerController || !ZoneMusic)
    {
  return;
    }

    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
        // CHANGED: Use RegisterMusicZone and save ID
        RegisteredZoneID = Manager->RegisterMusicZone(
        ZoneMusic,
            Priority,
     TransitionType,
          CrossfadeDuration,
         VolumeMultiplier
        );

        if (RegisteredZoneID >= 0)
    {
      UE_LOG(LogTemp, Log, TEXT("[OK] Player entered MusicZone '%s' - Registered ID: %d, Priority: %d"),
    *GetName(), RegisteredZoneID, Priority);
        }
     else
        {
            UE_LOG(LogTemp, Error, TEXT("[ERROR] Failed to register MusicZone '%s'!"), *GetName());
      }

 if (bShowDebugInfo)
        {
            ShowDebugInfo(
   FString::Printf(TEXT("Entered: %s (Priority: %d, ID: %d)"), *GetName(), Priority, RegisteredZoneID),
FColor::Green
            );
        }
    }

    OnPlayerEntered(PlayerController);
}

void AMusicZoneVolume::HandlePlayerExited(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }

    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
   // CHANGED: Release by unique ID
        if (RegisteredZoneID >= 0)
   {
     Manager->ReleaseZone(RegisteredZoneID);

      UE_LOG(LogTemp, Log, TEXT("[OK] Player exited MusicZone '%s' - Released ID: %d"),
      *GetName(), RegisteredZoneID);

            RegisteredZoneID = -1; // Reset ID
        }
        else
{
   UE_LOG(LogTemp, Warning, TEXT("[WARN] Player exited MusicZone '%s' but zone was not registered!"), *GetName());
        }

        if (bShowDebugInfo)
   {
       ShowDebugInfo(
  FString::Printf(TEXT("Exited: %s"), *GetName()),
     FColor::Yellow
       );
        }
    }

    OnPlayerExited(PlayerController);
}

void AMusicZoneVolume::OnPlayerEntered_Implementation(APlayerController* PlayerController)
{
    // Empty - override in Blueprint
}

void AMusicZoneVolume::OnPlayerExited_Implementation(APlayerController* PlayerController)
{
    // Empty - override in Blueprint
}

void AMusicZoneVolume::ShowDebugInfo(const FString& Message, const FColor& Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, Color, Message);
    }
}

#if WITH_EDITOR
void AMusicZoneVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = (PropertyChangedEvent.Property != nullptr)
   ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    // Update color based on priority or active status
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AMusicZoneVolume, Priority)
        || PropertyName == GET_MEMBER_NAME_CHECKED(AMusicZoneVolume, bIsActive))
    {
        if (UShapeComponent* ShapeComp = GetCollisionComponent())
        {
            if (bIsActive)
        {
  // Active - color depends on priority
           const float NormalizedPriority = FMath::Clamp(Priority / 100.0f, 0.0f, 1.0f);
    ShapeComp->ShapeColor = FColor(
        uint8(100 + NormalizedPriority * 155),  // R: 100-255
     uint8(200 - NormalizedPriority * 150),  // G: 50-200
         255,     // B: 255
    255      // A: opaque
 );
      }
    else
        {
       // Inactive - red
     ShapeComp->ShapeColor = FColor(255, 50, 50, 255);
    }
      
            ShapeComp->MarkRenderStateDirty();
  }
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AMusicZoneVolume, ZoneMusic))
    {
UE_LOG(LogTemp, Log, TEXT("MusicZoneVolume '%s' - Music changed to: %s"),
            *GetName(), ZoneMusic ? *ZoneMusic->GetName() : TEXT("None"));
    }
}
#endif
