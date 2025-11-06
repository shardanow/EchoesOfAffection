// Copyright Epic Games, Inc. All Rights Reserved.

#include "Audio/MusicManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "AudioDevice.h"

DEFINE_LOG_CATEGORY_STATIC(LogMusicManager, Log, All);

void UMusicManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CurrentMusic = nullptr;
    CurrentPriority = -1;
    MasterVolume = 1.0f;
    bIsPaused = false;
    bIsCrossfading = false;
    CrossfadeProgress = 0.0f;
    CrossfadeDuration = 0.0f;
    TargetVolume = 1.0f;
    SourceVolume = 1.0f;


    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        UE_LOG(LogMusicManager, Error, TEXT("Failed to get GameInstance!"));
        return;
    }

    UWorld* World = GameInstance->GetWorld();
    if (!World)
    {
        UE_LOG(LogMusicManager, Error, TEXT("Failed to get World from GameInstance!"));
        return;
    }


    PrimaryAudioComponent = NewObject<UAudioComponent>(GameInstance, UAudioComponent::StaticClass(), TEXT("PrimaryMusicComponent"));
    if (PrimaryAudioComponent)
    {
     PrimaryAudioComponent->bAutoActivate = false;
     PrimaryAudioComponent->bIsUISound = true;
    PrimaryAudioComponent->bAllowSpatialization = false;
        PrimaryAudioComponent->bAutoDestroy = false;
        PrimaryAudioComponent->SetVolumeMultiplier(1.0f);
 

        PrimaryAudioComponent->RegisterComponentWithWorld(World);
        PrimaryAudioComponent->InitializeComponent();
        
    UE_LOG(LogMusicManager, Log, TEXT("? Created and registered PrimaryAudioComponent"));
        UE_LOG(LogMusicManager, Log, TEXT("   - IsRegistered: %s"), PrimaryAudioComponent->IsRegistered() ? TEXT("Yes") : TEXT("No"));
    }
    else
    {
        UE_LOG(LogMusicManager, Error, TEXT("Failed to create PrimaryAudioComponent!"));
    }


    SecondaryAudioComponent = NewObject<UAudioComponent>(GameInstance, UAudioComponent::StaticClass(), TEXT("SecondaryMusicComponent"));
    if (SecondaryAudioComponent)
  {
        SecondaryAudioComponent->bAutoActivate = false;
        SecondaryAudioComponent->bIsUISound = true;
        SecondaryAudioComponent->bAllowSpatialization = false;
        SecondaryAudioComponent->bAutoDestroy = false;
        SecondaryAudioComponent->SetVolumeMultiplier(1.0f);
  

   SecondaryAudioComponent->RegisterComponentWithWorld(World);
        SecondaryAudioComponent->InitializeComponent();
        
   UE_LOG(LogMusicManager, Log, TEXT("? Created and registered SecondaryAudioComponent"));
        UE_LOG(LogMusicManager, Log, TEXT("   - IsRegistered: %s"), SecondaryAudioComponent->IsRegistered() ? TEXT("Yes") : TEXT("No"));
    }
    else
    {
        UE_LOG(LogMusicManager, Error, TEXT("Failed to create SecondaryAudioComponent!"));
    }

    UE_LOG(LogMusicManager, Log, TEXT("MusicManager initialized successfully"));
}

void UMusicManager::Deinitialize()
{
    // Stop all music
    if (PrimaryAudioComponent)
    {
        PrimaryAudioComponent->Stop();
        PrimaryAudioComponent->UnregisterComponent();
      PrimaryAudioComponent = nullptr;
    }

    if (SecondaryAudioComponent)
    {
        SecondaryAudioComponent->Stop();
        SecondaryAudioComponent->UnregisterComponent();
        SecondaryAudioComponent = nullptr;
 }

    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(CrossfadeTimerHandle);
    }

    ActiveZones.Empty();
    CurrentMusic = nullptr;

    UE_LOG(LogMusicManager, Log, TEXT("MusicManager deinitialized"));

    Super::Deinitialize();
}

UMusicManager* UMusicManager::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
 {
        return nullptr;
  }

    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
   return GameInstance->GetSubsystem<UMusicManager>();
        }
    }

    return nullptr;
}


int32 UMusicManager::RegisterMusicZone(
    USoundBase* NewMusic,
    int32 Priority,
    EMusicTransitionType TransitionType,
  float CrossfadeDurationParam,
    float VolumeMultiplier)
{
    if (!NewMusic)
    {
        UE_LOG(LogMusicManager, Warning, TEXT("RegisterMusicZone called with null music!"));
        return -1;
    }


    int32 ZoneID = NextZoneID++;

    // Create request
    FMusicRequest Request;
    Request.Music = NewMusic;
    Request.Priority = Priority;
    Request.TransitionType = TransitionType;
    Request.CrossfadeDuration = CrossfadeDurationParam;
    Request.VolumeMultiplier = VolumeMultiplier;
    Request.RequestTime = FPlatformTime::Seconds();


    ActiveZones.Add(ZoneID, Request);

    UE_LOG(LogMusicManager, Log, TEXT("?? Registered zone ID=%d: %s (Priority: %d, Type: %d)"),
        ZoneID, *NewMusic->GetName(), Priority, (int32)TransitionType);

    // Find highest priority zone
    FMusicRequest HighestRequest;
    int32 HighestPriority = -1;

    if (FindHighestPriorityZone(HighestRequest, HighestPriority))
    {
        if (HighestPriority > CurrentPriority || CurrentMusic != HighestRequest.Music)
        {
            ProcessMusicChange(HighestRequest);
   CurrentPriority = HighestPriority;
        }
    }

    return ZoneID; // 
}


void UMusicManager::RequestMusicChange(
    USoundBase* NewMusic,
    int32 Priority,
  EMusicTransitionType TransitionType,
    float CrossfadeDurationParam,
    float VolumeMultiplier)
{

    RegisterMusicZone(NewMusic, Priority, TransitionType, CrossfadeDurationParam, VolumeMultiplier);
}


void UMusicManager::ReleaseZone(int32 ZoneID)
{
    if (!ActiveZones.Contains(ZoneID))
    {
   UE_LOG(LogMusicManager, Warning, TEXT("?? ReleaseZone: Zone ID=%d not found!"), ZoneID);
        return;
    }


    int32 ReleasedPriority = ActiveZones[ZoneID].Priority;


    ActiveZones.Remove(ZoneID);
    UE_LOG(LogMusicManager, Log, TEXT("??? Released zone ID=%d (Priority: %d)"), ZoneID, ReleasedPriority);



    FMusicRequest NextRequest;
    int32 NextPriority = -1;

    if (FindHighestPriorityZone(NextRequest, NextPriority))
    {

        if (CurrentMusic != NextRequest.Music || CurrentPriority != NextPriority)
{
         ProcessMusicChange(NextRequest);
       CurrentPriority = NextPriority;
        }
    }
    else
    {
        // No more zones - stop music
        StopMusic(2.0f);
        CurrentPriority = -1;
    }
}

void UMusicManager::StopMusic(float FadeOutDuration)
{
    if (PrimaryAudioComponent && PrimaryAudioComponent->IsPlaying())
    {
        PrimaryAudioComponent->FadeOut(FadeOutDuration, 0.0f);
        UE_LOG(LogMusicManager, Log, TEXT("Stopping music with fade: %.2f"), FadeOutDuration);
    }

    if (SecondaryAudioComponent && SecondaryAudioComponent->IsPlaying())
    {
        SecondaryAudioComponent->FadeOut(FadeOutDuration, 0.0f);
    }

    CurrentMusic = nullptr;
    bIsCrossfading = false;

    // Clear timer
 if (UWorld* World = GetWorld())
    {
 World->GetTimerManager().ClearTimer(CrossfadeTimerHandle);
    }
}

void UMusicManager::SetMasterMusicVolume(float Volume)
{
  MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

    if (PrimaryAudioComponent)
    {
        ApplyVolume(PrimaryAudioComponent, MasterVolume * TargetVolume);
    }

    if (SecondaryAudioComponent)
    {
    ApplyVolume(SecondaryAudioComponent, MasterVolume * SourceVolume);
    }

    UE_LOG(LogMusicManager, Log, TEXT("Master music volume set to: %.2f"), MasterVolume);
}

void UMusicManager::PauseMusic()
{
    if (bIsPaused)
    {
    return;
    }

    bIsPaused = true;

    if (PrimaryAudioComponent)
    {
        PrimaryAudioComponent->SetPaused(true);
    }

    if (SecondaryAudioComponent)
    {
        SecondaryAudioComponent->SetPaused(true);
    }

    UE_LOG(LogMusicManager, Log, TEXT("Music paused"));
}

void UMusicManager::ResumeMusic()
{
    if (!bIsPaused)
    {
        return;
    }

    bIsPaused = false;

    if (PrimaryAudioComponent)
    {
    PrimaryAudioComponent->SetPaused(false);
    }

 if (SecondaryAudioComponent)
    {
SecondaryAudioComponent->SetPaused(false);
    }

    UE_LOG(LogMusicManager, Log, TEXT("Music resumed"));
}

bool UMusicManager::IsMusicPlaying() const
{
    if (PrimaryAudioComponent && PrimaryAudioComponent->IsPlaying())
    {
        return true;
    }

    if (SecondaryAudioComponent && SecondaryAudioComponent->IsPlaying())
    {
        return true;
    }

    return false;
}

void UMusicManager::ProcessMusicChange(const FMusicRequest& Request)
{
    if (!PrimaryAudioComponent)
    {
        UE_LOG(LogMusicManager, Error, TEXT("PrimaryAudioComponent is null!"));
   return;
    }

    // Same music - just adjust volume
    if (Request.Music == CurrentMusic)
  {
        TargetVolume = Request.VolumeMultiplier;
   ApplyVolume(PrimaryAudioComponent, MasterVolume * TargetVolume);
        UE_LOG(LogMusicManager, Log, TEXT("?? Same music - adjusted volume to: %.2f"), TargetVolume);
        return;
 }

    switch (Request.TransitionType)
    {
    case EMusicTransitionType::Immediate:
        {
   // ? Stop old track first
   if (PrimaryAudioComponent->IsPlaying())
   {
                PrimaryAudioComponent->Stop();
  UE_LOG(LogMusicManager, Log, TEXT("?? Stopped old track"));
          }
            
     // Also stop secondary if playing
 if (SecondaryAudioComponent && SecondaryAudioComponent->IsPlaying())
   {
    SecondaryAudioComponent->Stop();
            }

         PrimaryAudioComponent->SetSound(Request.Music);
ApplyVolume(PrimaryAudioComponent, MasterVolume * Request.VolumeMultiplier);
       PrimaryAudioComponent->Play();

 if (PrimaryAudioComponent->IsPlaying())
      {
    UE_LOG(LogMusicManager, Log, TEXT("? Immediate transition to: %s (Volume: %.2f)"), 
           *Request.Music->GetName(), Request.VolumeMultiplier);
   }
          else
 {
    UE_LOG(LogMusicManager, Error, TEXT("? Immediate transition FAILED!"));
         }
 break;
        }

    case EMusicTransitionType::Crossfade:
  {
       StartCrossfade(Request.Music, Request.CrossfadeDuration, Request.VolumeMultiplier);
       UE_LOG(LogMusicManager, Log, TEXT("?? Crossfade transition to: %s (%.2fs)"),
    *Request.Music->GetName(), Request.CrossfadeDuration);
   break;
        }

    case EMusicTransitionType::WaitForEnd:
  {
            // TODO: Implement wait for end logic
  StartCrossfade(Request.Music, Request.CrossfadeDuration, Request.VolumeMultiplier);
     UE_LOG(LogMusicManager, Log, TEXT("? WaitForEnd transition (using crossfade): %s"),
    *Request.Music->GetName());
     break;
}

    case EMusicTransitionType::Layer:
   {
      // TODO: Implement layering logic
   StartCrossfade(Request.Music, Request.CrossfadeDuration, Request.VolumeMultiplier);
 UE_LOG(LogMusicManager, Log, TEXT("?? Layer transition (using crossfade): %s"),
     *Request.Music->GetName());
            break;
        }
    }

 CurrentMusic = Request.Music;
}

void UMusicManager::StartCrossfade(USoundBase* NewMusic, float Duration, float TargetVol)
{
    if (!PrimaryAudioComponent || !SecondaryAudioComponent)
    {
        UE_LOG(LogMusicManager, Error, TEXT("? StartCrossfade: Audio components are null!"));
        return;
    }

    if (!NewMusic)
  {
        UE_LOG(LogMusicManager, Error, TEXT("? StartCrossfade: NewMusic is null!"));
     return;
    }

    UE_LOG(LogMusicManager, Log, TEXT("?? StartCrossfade: NewMusic=%s, Duration=%.2f, TargetVol=%.2f"), 
      *NewMusic->GetName(), Duration, TargetVol);


    float CurrentVolume = 0.0f;
    bool bHasOldMusic = false;

    if (PrimaryAudioComponent->IsPlaying())
    {
   CurrentVolume = PrimaryAudioComponent->VolumeMultiplier;
        bHasOldMusic = true;
UE_LOG(LogMusicManager, Log, TEXT("? Old music: %s, Volume: %.2f"), 
            PrimaryAudioComponent->Sound ? *PrimaryAudioComponent->Sound->GetName() : TEXT("None"), CurrentVolume);
    }
 else
    {
        UE_LOG(LogMusicManager, Log, TEXT("?? No old music playing - starting fresh"));
    }


  

    if (SecondaryAudioComponent->IsPlaying())
    {
        UE_LOG(LogMusicManager, Warning, TEXT("Secondary component was playing - stopping it"));
        SecondaryAudioComponent->Stop();
    }


    UE_LOG(LogMusicManager, Log, TEXT("?? Secondary Component Diagnostics:"));
    UE_LOG(LogMusicManager, Log, TEXT("   - IsValid: %s"), IsValid(SecondaryAudioComponent) ? TEXT("Yes") : TEXT("No"));
    UE_LOG(LogMusicManager, Log, TEXT("   - IsRegistered: %s"), SecondaryAudioComponent->IsRegistered() ? TEXT("Yes") : TEXT("No"));
    UE_LOG(LogMusicManager, Log, TEXT("   - GetWorld(): %s"), SecondaryAudioComponent->GetWorld() ? TEXT("Valid") : TEXT("NULL"));
    
    if (!SecondaryAudioComponent->IsRegistered())
  {
        UE_LOG(LogMusicManager, Error, TEXT("? SecondaryAudioComponent is NOT registered!"));
        
        if (UWorld* World = GetWorld())
     {
      UE_LOG(LogMusicManager, Warning, TEXT("?? Attempting to re-register SecondaryAudioComponent..."));
       SecondaryAudioComponent->RegisterComponentWithWorld(World);
   
   if (SecondaryAudioComponent->IsRegistered())
    {
   UE_LOG(LogMusicManager, Log, TEXT("? Re-registration SUCCESS!"));
   }
        else
     {
         UE_LOG(LogMusicManager, Error, TEXT("? Re-registration FAILED!"));
return;
       }
        }
     else
{
   UE_LOG(LogMusicManager, Error, TEXT("? Cannot re-register: GetWorld() returned null!"));
   return;
        }
    }



    if (!SecondaryAudioComponent->IsActive())
    {


    }


    SecondaryAudioComponent->SetSound(NewMusic);
    
    if (!SecondaryAudioComponent->Sound)
    {
        UE_LOG(LogMusicManager, Error, TEXT("? Failed to set Sound on Secondary!"));
        return;
    }


    UE_LOG(LogMusicManager, Log, TEXT("? Sound set on Secondary: %s"), *SecondaryAudioComponent->Sound->GetName());
    UE_LOG(LogMusicManager, Log, TEXT("?? Sound Asset Diagnostics:"));
    UE_LOG(LogMusicManager, Log, TEXT("   - Sound Class: %s"), 
    SecondaryAudioComponent->Sound->GetClass() ? *SecondaryAudioComponent->Sound->GetClass()->GetName() : TEXT("None"));
    UE_LOG(LogMusicManager, Log, TEXT("   - Duration: %.2f seconds"), SecondaryAudioComponent->Sound->GetDuration());
    UE_LOG(LogMusicManager, Log, TEXT("   - Is Valid Asset: %s"), SecondaryAudioComponent->Sound->IsValidLowLevel() ? TEXT("Yes") : TEXT("No"));
    

    if (USoundWave* SoundWave = Cast<USoundWave>(SecondaryAudioComponent->Sound))
    {
        UE_LOG(LogMusicManager, Log, TEXT("   - Sound Wave: Yes"));
  UE_LOG(LogMusicManager, Log, TEXT("   - NumChannels: %d"), SoundWave->NumChannels);
   // UE_LOG(LogMusicManager, Log, TEXT("   - SampleRate: %d"), SoundWave->GetSampleRateForCurrentPlatform());
   //UE_LOG(LogMusicManager, Log, TEXT("   - Compressed Data Size: %d"), SoundWave->GetCompressedDataSize("");
    }
    else if (USoundCue* SoundCue = Cast<USoundCue>(SecondaryAudioComponent->Sound))
    {
        UE_LOG(LogMusicManager, Log, TEXT("   - Sound Cue: Yes"));
    UE_LOG(LogMusicManager, Log, TEXT("   - Max Distance: %.2f"), SoundCue->GetMaxDistance());
    }
    else
    {
      UE_LOG(LogMusicManager, Warning, TEXT("   - Unknown Sound Type!"));
    }


    UE_LOG(LogMusicManager, Log, TEXT("?? Calling Play() on SecondaryAudioComponent..."));
    

    if (UWorld* World = GetWorld())
    {
  if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
    {
      UE_LOG(LogMusicManager, Log, TEXT("?? Audio Device Diagnostics:"));
       UE_LOG(LogMusicManager, Log, TEXT("   - Audio Device: Valid"));
   UE_LOG(LogMusicManager, Log, TEXT("   - Device ID: %d"), AudioDevice->DeviceID);
         UE_LOG(LogMusicManager, Log, TEXT("   - Max Channels: %d"), AudioDevice->GetMaxChannels());
        }
        else
      {
    UE_LOG(LogMusicManager, Error, TEXT("? Audio Device is NULL! Sound won't play!"));
  }
    }
  
    SecondaryAudioComponent->Play();
    

    if (UWorld* World = GetWorld())
    {
      FTimerHandle DummyHandle;
     World->GetTimerManager().SetTimer(DummyHandle, [](){}, 0.001f, false);
    }
  
    if (!SecondaryAudioComponent->IsPlaying())
    {
   UE_LOG(LogMusicManager, Error, TEXT("? SecondaryAudioComponent FAILED to play!"));
        UE_LOG(LogMusicManager, Error, TEXT("   - IsRegistered: %s"), SecondaryAudioComponent->IsRegistered() ? TEXT("Yes") : TEXT("No"));
        UE_LOG(LogMusicManager, Error, TEXT("   - IsActive: %s"), SecondaryAudioComponent->IsActive() ? TEXT("Yes") : TEXT("No"));
        UE_LOG(LogMusicManager, Error, TEXT("   - Sound: %s"), SecondaryAudioComponent->Sound ? *SecondaryAudioComponent->Sound->GetName() : TEXT("None"));
        UE_LOG(LogMusicManager, Error, TEXT("   - bAutoActivate: %s"), SecondaryAudioComponent->bAutoActivate ? TEXT("Yes") : TEXT("No"));
        

     UE_LOG(LogMusicManager, Warning, TEXT("?? Trying alternative Play(0.0f) method..."));
        SecondaryAudioComponent->Play(0.0f);
        
        if (!SecondaryAudioComponent->IsPlaying())
        {
          UE_LOG(LogMusicManager, Error, TEXT("? Alternative Play() also FAILED! Aborting crossfade."));
     return;
}
        else
        {
     UE_LOG(LogMusicManager, Log, TEXT("? Alternative Play() SUCCESS!"));
    }
    }
    else
    {
        UE_LOG(LogMusicManager, Log, TEXT("? SecondaryAudioComponent is now playing!"));
    }



    const float MinAudibleVolume = 0.001f;  // 
    SecondaryAudioComponent->SetVolumeMultiplier(MinAudibleVolume);
    UE_LOG(LogMusicManager, Log, TEXT("Set initial volume to %.3f (min audible)"), MinAudibleVolume);

    // 6. Setup crossfade state
    bIsCrossfading = true;
    CrossfadeProgress = 0.0f;
    CrossfadeDuration = Duration;
    TargetVolume = TargetVol;
    SourceVolume = CurrentVolume;

    UE_LOG(LogMusicManager, Log, TEXT("?? Crossfade setup: TargetVol=%.2f, SourceVol=%.2f, MasterVol=%.2f, HasOldMusic=%s"),
        TargetVolume, SourceVolume, MasterVolume, bHasOldMusic ? TEXT("YES") : TEXT("NO"));

    // 7. Start timer for crossfade updates
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            CrossfadeTimerHandle,
        this,
&UMusicManager::UpdateCrossfadeTimer,
            0.016f, // ~60 FPS
         true
        );
        UE_LOG(LogMusicManager, Log, TEXT("? Crossfade timer started"));
    }
    else
 {
  UE_LOG(LogMusicManager, Error, TEXT("? Failed to get World for timer!"));
    }
}

void UMusicManager::UpdateCrossfadeTimer()
{
    if (!bIsCrossfading || CrossfadeDuration <= 0.0f)
 {
        return;
    }

    CrossfadeProgress += 0.016f;
    float Alpha = FMath::Clamp(CrossfadeProgress / CrossfadeDuration, 0.0f, 1.0f);

    // Smooth curve for better audio fade
    float SmoothAlpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);


    const float MinAudibleVolume = 0.001f;  // 


    float NewTrackVolume = FMath::Lerp(MinAudibleVolume, MasterVolume * TargetVolume, SmoothAlpha);
    

    float OldTrackVolume = FMath::Lerp(MasterVolume * SourceVolume, MinAudibleVolume, SmoothAlpha);


  static float LogTimer = 0.0f;
    LogTimer += 0.016f;
if (LogTimer >= 0.5f)
    {
     UE_LOG(LogMusicManager, Log, TEXT("Crossfade: Progress=%.0f%%, NewVol=%.2f, OldVol=%.2f"),
     Alpha * 100.0f, NewTrackVolume, OldTrackVolume);
        

        if (SecondaryAudioComponent)
{
  UE_LOG(LogMusicManager, Log, TEXT("  Secondary ACTUAL volume: %.2f, IsPlaying: %s"),
       SecondaryAudioComponent->VolumeMultiplier,
       SecondaryAudioComponent->IsPlaying() ? TEXT("Yes") : TEXT("No"));
        }
        if (PrimaryAudioComponent)
        {
         UE_LOG(LogMusicManager, Log, TEXT("  Primary ACTUAL volume: %.2f, IsPlaying: %s"),
          PrimaryAudioComponent->VolumeMultiplier,
         PrimaryAudioComponent->IsPlaying() ? TEXT("Yes") : TEXT("No"));
  }
        
        LogTimer = 0.0f;
    }

    // Fade in NEW track (Secondary)
    if (SecondaryAudioComponent && SecondaryAudioComponent->IsPlaying())
    {
        ApplyVolume(SecondaryAudioComponent, NewTrackVolume);
 }
    else
    {
        UE_LOG(LogMusicManager, Warning, TEXT("Secondary NOT playing during crossfade! Stopping crossfade."));
        bIsCrossfading = false;
    return;
    }

    // Fade out OLD track (Primary)
    if (PrimaryAudioComponent && PrimaryAudioComponent->IsPlaying())
    {
        ApplyVolume(PrimaryAudioComponent, OldTrackVolume);
}

    // Complete crossfade
    if (Alpha >= 1.0f)
    {
     bIsCrossfading = false;

 // Stop old track (Primary)
      if (PrimaryAudioComponent && PrimaryAudioComponent->IsPlaying())
        {
       PrimaryAudioComponent->Stop();
            UE_LOG(LogMusicManager, Log, TEXT("Stopped old track on Primary"));
    }


        UAudioComponent* Temp = PrimaryAudioComponent;
    PrimaryAudioComponent = SecondaryAudioComponent;
        SecondaryAudioComponent = Temp;

    UE_LOG(LogMusicManager, Log, TEXT("Swapped components after crossfade - new track now on Primary"));

        // Clear timer
      if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(CrossfadeTimerHandle);
        }

        UE_LOG(LogMusicManager, Log, TEXT("Crossfade completed - Final volume: %.2f"), NewTrackVolume);
 
        // Reset static timer
        LogTimer = 0.0f;
    }
}

bool UMusicManager::FindHighestPriorityZone(FMusicRequest& OutRequest, int32& OutPriority)
{
    OutPriority = -1;
    bool bFound = false;
    int32 SelectedZoneID = -1;



    for (const auto& Pair : ActiveZones)
    {
        const FMusicRequest& Request = Pair.Value;
        const int32 ZoneID = Pair.Key;
    

  if (Request.Priority > OutPriority)
        {

            OutPriority = Request.Priority;
            OutRequest = Request;
   SelectedZoneID = ZoneID;
   bFound = true;
    }
   else if (Request.Priority == OutPriority && ZoneID > SelectedZoneID)
        {

            OutRequest = Request;
SelectedZoneID = ZoneID;
        UE_LOG(LogMusicManager, Log, TEXT("?? Same priority %d - selected newer zone ID=%d (%s)"),
                OutPriority, ZoneID, *Request.Music->GetName());
    }
    }

 if (bFound)
 {
        UE_LOG(LogMusicManager, Log, TEXT("?? Highest priority zone: ID=%d, Priority=%d, Music=%s"),
          SelectedZoneID, OutPriority, *OutRequest.Music->GetName());
  }

    return bFound;
}

void UMusicManager::ApplyVolume(UAudioComponent* AudioComp, float Volume)
{
    if (AudioComp)
    {
        float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
        AudioComp->SetVolumeMultiplier(ClampedVolume);
        
        // Debug logging
      UE_LOG(LogMusicManager, Verbose, TEXT("ApplyVolume: %s -> %.2f (was %.2f)"), 
     *AudioComp->GetName(), ClampedVolume, AudioComp->VolumeMultiplier);
    }
}

// Console commands for debugging
#if !UE_BUILD_SHIPPING
static FAutoConsoleCommand CCmdMusicDebug(
    TEXT("Music.Debug"),
    TEXT("Shows current music state"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
    if (GEngine && GEngine->GetWorld())
        {
        if (UMusicManager* Manager = UMusicManager::Get(GEngine->GetWorld()))
      {
UE_LOG(LogMusicManager, Display, TEXT("=== MUSIC MANAGER DEBUG ==="));
         UE_LOG(LogMusicManager, Display, TEXT("Current Music: %s"),
     Manager->GetCurrentMusic() ? *Manager->GetCurrentMusic()->GetName() : TEXT("None"));
    UE_LOG(LogMusicManager, Display, TEXT("Master Volume: %.2f"), Manager->GetMasterMusicVolume());
     UE_LOG(LogMusicManager, Display, TEXT("Is Playing: %s"), Manager->IsMusicPlaying() ? TEXT("Yes") : TEXT("No"));
           UE_LOG(LogMusicManager, Display, TEXT("Active Zones: %d"), Manager->GetActiveZonesCount());
  }
        }
    })
);

static FAutoConsoleCommand CCmdMusicStop(
 TEXT("Music.Stop"),
    TEXT("Stops all music"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (GEngine && GEngine->GetWorld())
        {
            if (UMusicManager* Manager = UMusicManager::Get(GEngine->GetWorld()))
        {
     Manager->StopMusic(1.0f);
     UE_LOG(LogMusicManager, Display, TEXT("Music stopped"));
    }
        }
    })
);
#endif
