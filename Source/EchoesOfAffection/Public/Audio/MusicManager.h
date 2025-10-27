// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "MusicManager.generated.h"

UENUM(BlueprintType)
enum class EMusicTransitionType : uint8
{
    Immediate UMETA(DisplayName = "Immediate"),
    Crossfade UMETA(DisplayName = "Crossfade"),
    WaitForEnd UMETA(DisplayName = "Wait For End"),
    Layer UMETA(DisplayName = "Layer")
};

USTRUCT()
struct FMusicRequest
{
    GENERATED_BODY()

    UPROPERTY()
    USoundBase* Music;

    UPROPERTY()
    int32 Priority;

    UPROPERTY()
    EMusicTransitionType TransitionType;

    UPROPERTY()
    float CrossfadeDuration;

    UPROPERTY()
    float VolumeMultiplier;

    UPROPERTY()
    double RequestTime;

    FMusicRequest()
        : Music(nullptr), Priority(0), TransitionType(EMusicTransitionType::Crossfade)
        , CrossfadeDuration(2.0f), VolumeMultiplier(1.0f), RequestTime(0.0)
    {
    }
};

UCLASS()
class ECHOESOFAFFECTION_API UMusicManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintPure, Category = "Music", meta = (WorldContext = "WorldContextObject"))
    static UMusicManager* Get(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "Music")
    void RequestMusicChange(USoundBase* NewMusic, int32 Priority, EMusicTransitionType TransitionType = EMusicTransitionType::Crossfade, float CrossfadeDuration = 2.0f, float VolumeMultiplier = 1.0f);

    // ? НОВАЯ функция: регистрация зоны с уникальным ID
    UFUNCTION(BlueprintCallable, Category = "Music")
    int32 RegisterMusicZone(USoundBase* NewMusic, int32 Priority, EMusicTransitionType TransitionType = EMusicTransitionType::Crossfade, float CrossfadeDuration = 2.0f, float VolumeMultiplier = 1.0f);

    // ? ИЗМЕНЕНО: Release по уникальному ID, а не по Priority
    UFUNCTION(BlueprintCallable, Category = "Music")
    void ReleaseZone(int32 ZoneID);

    UFUNCTION(BlueprintCallable, Category = "Music")
    void StopMusic(float FadeOutDuration = 2.0f);

    UFUNCTION(BlueprintPure, Category = "Music")
    USoundBase* GetCurrentMusic() const { return CurrentMusic; }

    UFUNCTION(BlueprintCallable, Category = "Music")
    void SetMasterMusicVolume(float Volume);

    UFUNCTION(BlueprintPure, Category = "Music")
    float GetMasterMusicVolume() const { return MasterVolume; }

    UFUNCTION(BlueprintCallable, Category = "Music")
    void PauseMusic();

    UFUNCTION(BlueprintCallable, Category = "Music")
    void ResumeMusic();

    UFUNCTION(BlueprintPure, Category = "Music")
    bool IsMusicPlaying() const;

    UFUNCTION(BlueprintPure, Category = "Music|Debug")
    int32 GetActiveZonesCount() const { return ActiveZones.Num(); }

protected:
    UPROPERTY()
    UAudioComponent* PrimaryAudioComponent;

    UPROPERTY()
    UAudioComponent* SecondaryAudioComponent;

    UPROPERTY()
    USoundBase* CurrentMusic;

    // ? ИЗМЕНЕНО: TMap<UniqueID, FMusicRequest> вместо TMap<Priority, FMusicRequest>
    UPROPERTY()
    TMap<int32, FMusicRequest> ActiveZones;

    // ? НОВОЕ: Счётчик для генерации уникальных ID
    int32 NextZoneID = 0;

    UPROPERTY()
    int32 CurrentPriority;

    UPROPERTY()
    float MasterVolume;

    UPROPERTY()
    bool bIsPaused;

    bool bIsCrossfading;
    float CrossfadeProgress;
    float CrossfadeDuration;
    float TargetVolume;
    float SourceVolume;

    FTimerHandle CrossfadeTimerHandle;

    void ProcessMusicChange(const FMusicRequest& Request);
    void StartCrossfade(USoundBase* NewMusic, float Duration, float TargetVol);
    void UpdateCrossfadeTimer();
    bool FindHighestPriorityZone(FMusicRequest& OutRequest, int32& OutPriority);
    void ApplyVolume(UAudioComponent* AudioComp, float Volume);
};